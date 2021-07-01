#include "KeywordRecognizer.h"

#include "KigsTools.h"
#include "NotificationCenter.h"

#include <functional>

#include <winrt/Windows.Media.Capture.h>

#include "winrt_helpers.h"

IMPLEMENT_CLASS_INFO(KeywordRecognizer);

#if 0
void DEBUG_OUTPUT(const std::string&);
#else
#define DEBUG_OUTPUT(a) 
#endif

#ifdef WUP

using namespace winrt::Windows::Media::SpeechRecognition;
using namespace winrt::Windows::Media::Capture;


bool KeywordRecognizer::IsAllowed()
{
	return mSpeechRecognizer && mRecognitionEnabled;
}

void KeywordRecognizer::InitModifiable()
{
	ParentClassType::InitModifiable();
	mLastStartTry = TimePoint::clock::now();
}

void KeywordRecognizer::Update(const Timer& timer, void* addParam)
{
	ParentClassType::Update(timer, addParam);

	if (!IsInit() || !CanStart()) return;
	
	auto t = TimePoint::clock::now();
	if (!IsAllowed() && t - mLastStartTry > std::chrono::seconds(10))
	{
		mLastStartTry = t;
		no_await(StartRecognizeKeywords());
	}

	winrt::hstring cmd;
	{
		std::lock_guard<std::mutex> lk{ mLastCommandMutex };
		cmd = mLastCommand;
		mLastCommand.clear();
	}

	if (!cmd.empty())
	{
		EmitSignal(Signals::OnKeyword, this, cmd.data());
	}
}


winrt::Windows::Foundation::IAsyncAction KeywordRecognizer::StartRecognizeKeywords()
{
#ifndef KIGS_HOLOLENS2
	co_return;
#endif
	mRecognitionEnabled = true;

	try
	{
		MediaCapture mediaCapture = MediaCapture();
		auto settings = MediaCaptureInitializationSettings();
		settings.StreamingCaptureMode(StreamingCaptureMode::Audio);
		co_await mediaCapture.InitializeAsync(settings);
	}
	catch (winrt::hresult_error const& ex)
	{
		mSpeechRecognizer = nullptr;
		mRecognitionEnabled = false;
		co_return;
	}
	catch (...)
	{
		mSpeechRecognizer = nullptr;
		mRecognitionEnabled = false;
		co_return;
	}

	if(!mSpeechRecognizer) 
		mSpeechRecognizer = SpeechRecognizer();

	SpeechRecognitionListConstraint spConstraint = SpeechRecognitionListConstraint(mSpeechCommands);
	mSpeechRecognizer.Constraints().Clear();
	mSpeechRecognizer.Constraints().Append(spConstraint);

	DEBUG_OUTPUT(std::to_string(mSpeechCommands.size()) + " commands in constraints");

	SpeechRecognitionCompilationResult compilationResult = co_await mSpeechRecognizer.CompileConstraintsAsync();

	if (compilationResult.Status() != SpeechRecognitionResultStatus::Success)
	{
		mSpeechRecognizer = nullptr;
		mRecognitionEnabled = false;
		co_return;
	}

	while (mRecognitionEnabled)
	{
		DEBUG_OUTPUT("Waiting for result");
		auto result = co_await mSpeechRecognizer.RecognizeAsync();
		auto status = result.Status();
		if (status == SpeechRecognitionResultStatus::Success)
		{
			auto confidence = result.Confidence();
			DEBUG_OUTPUT("Text ("+ std::to_string((int)confidence) +")= " + to_utf8(result.Text().c_str()));
			//if (confidence <= SpeechRecognitionConfidence::Medium)
			{
				std::lock_guard<std::mutex> lk{ mLastCommandMutex };
				mLastCommand = result.Text();
			}
			//else if (confidence == SpeechRecognitionConfidence::Low)
			{
				// NOTE(antoine) do something ?
			}
		}
		else
		{
			DEBUG_OUTPUT(std::to_string((int)status));
		}
	}
}


void KeywordRecognizer::StopRecognizeKeywords()
{
	if (mSpeechRecognizer)
	{
		mRecognitionEnabled = false;
		mSpeechRecognizer.StopRecognitionAsync();
		mSpeechRecognizer = nullptr;
	}
}

#endif