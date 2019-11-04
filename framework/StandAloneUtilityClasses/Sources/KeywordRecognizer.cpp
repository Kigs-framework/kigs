#include "KeywordRecognizer.h"

#include "KigsTools.h"
#include "NotificationCenter.h"

#include <functional>

IMPLEMENT_CLASS_INFO(KeywordRecognizer);

#ifdef WUP

using namespace winrt::Windows::Media::SpeechRecognition;
void KeywordRecognizer::InitModifiable()
{
	ParentClassType::InitModifiable();

#ifdef KIGS_TOOLS
	mSpeechCommands.push_back(L"debug mode");
#endif

	KigsCore::GetNotificationCenter()->addObserver(this, "WUPWindowVisible", "WUPWindowVisible");
	InsertFunction("WUPWindowVisible", [this]()
	{
		mNeedRestart = true;
	});
}

void KeywordRecognizer::StartRecognition()
{
	StartRecognizeSpeechCommands();
}

void KeywordRecognizer::StopRecognition()
{
	StopCurrentRecognizerIfExists();
}


std::future<void> KeywordRecognizer::StopCurrentRecognizerIfExists()
{
	if (mSpeechRecognizer)
	{
		co_await mSpeechRecognizer.StopRecognitionAsync();
		mSpeechRecognizer.RecognitionQualityDegrading(mQualityDegradedToken);
		if (mSpeechRecognizer.ContinuousRecognitionSession())
		{
			mSpeechRecognizer.ContinuousRecognitionSession().ResultGenerated(mResultEventToken);
		}
	}
}

bool KeywordRecognizer::InitializeSpeechRecognizer()
{
	//auto language = ref new Windows::Globalization::Language("fr - FR");
	try
	{
		mSpeechRecognizer = SpeechRecognizer();
	}
	catch (...)
	{
		mSpeechRecognizer = nullptr;
		return false;
	}
	
	if (!mSpeechRecognizer)
	{
		return false;
	}

	mQualityDegradedToken = mSpeechRecognizer.RecognitionQualityDegrading([this](SpeechRecognizer const& sr, SpeechRecognitionQualityDegradingEventArgs args)
	{
		switch (args.Problem())
		{
		case SpeechRecognitionAudioProblem::TooFast:
			kigsprintf("The user spoke too quickly.\n");
			break;

		case SpeechRecognitionAudioProblem::TooSlow:
			kigsprintf("The user spoke too slowly.\n");
			break;

		case SpeechRecognitionAudioProblem::TooQuiet:
			kigsprintf("The user spoke too softly.\n");
			break;

		case SpeechRecognitionAudioProblem::TooLoud:
			kigsprintf("The user spoke too loudly.\n");
			break;

		case SpeechRecognitionAudioProblem::TooNoisy:
			kigsprintf("There is too much noise in the signal.\n");
			break;

		case SpeechRecognitionAudioProblem::NoSignal:
			kigsprintf("There is no signal.\n");
			break;

		case SpeechRecognitionAudioProblem::None:
		default:
			kigsprintf("An error was reported with no information.\n");
			break;
		}
	});

	mResultEventToken = mSpeechRecognizer.ContinuousRecognitionSession().ResultGenerated([this](SpeechContinuousRecognitionSession const& scrs, SpeechContinuousRecognitionResultGeneratedEventArgs args)
	{
		// For our list of commands, medium confidence is good enough. 
		// We also accept results that have high confidence.
		if (args.Result().Confidence() < SpeechRecognitionConfidence::Rejected)
		{
			std::lock_guard<std::mutex> lk{ mLastCommandMutex };
			mLastCommand = args.Result().Text();
		}
		else
		{
			kigsprintf("Recognition confidence not high enough.\n");
		}
	});
	return true;
}

void KeywordRecognizer::Update(const Timer& timer, void* addParam)
{
	ParentClassType::Update(timer, addParam);

	winrt::hstring cmd;
	{
		std::lock_guard<std::mutex> lk{ mLastCommandMutex };
		cmd = mLastCommand;
		mLastCommand.clear();
	}
	
	if (mNeedRestart)
	{
		bool exp = false;
		if (mSpeechRecognizerStarting.compare_exchange_strong(exp, true))
		{
			mNeedRestart = false;
			mSpeechRecognizer = nullptr;
			std::thread([this]()
			{
				if(InitializeSpeechRecognizer())
					StartRecognition();
				if (!mSpeechRecognizer) mNeedRestart = true;
				else kigsprintf("Speech Recognizer Initialized");
				mSpeechRecognizerStarting = false;
			}
			).detach();
		}
	}

	if (!cmd.empty())
	{
#ifdef KIGS_TOOLS
		if (cmd == L"debug mode")
		{

		}
		else
#endif
		{
			EmitSignal(Signals::OnKeyword, this, cmd.data());
		}
	}
}

std::future<void> KeywordRecognizer::StartRecognizeSpeechCommands()
{
	//return StopCurrentRecognizerIfExists().then([this]()
	{
		if (!mSpeechRecognizer)
		{
			return;
		}

		SpeechRecognitionListConstraint spConstraint = SpeechRecognitionListConstraint(mSpeechCommands);
		mSpeechRecognizer.Constraints().Clear();
		mSpeechRecognizer.Constraints().Append(spConstraint);
		SpeechRecognitionCompilationResult compilationResult = co_await mSpeechRecognizer.CompileConstraintsAsync();
		if (compilationResult.Status() == SpeechRecognitionResultStatus::Success)
		{
			co_await mSpeechRecognizer.ContinuousRecognitionSession().StartAsync();
			return;
		}

		kigsprintf("Could not initialize constraint-based speech engine!\n");
	}
}

#endif