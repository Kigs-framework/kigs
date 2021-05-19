#include "UI/UIButtonText.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "NotificationCenter.h"
#include "AlphaMask.h"
#include "LocalizationManager.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIButtonText, UIButtonText, 2DLayers);;
IMPLEMENT_CLASS_INFO(UIButtonText)

IMPLEMENT_CONSTRUCTOR(UIButtonText)
	, mUpText(*this, false, "UpText", "")
	, mDownText(*this, false, "DownText", "")
	, mOverText(*this, false, "OverText", "")
	, mUpColor(*this, false, "UpColor", 0, 0, 0, 0)
	, mOverColor(*this, false, "OverColor", 0, 0, 0, 0)
	, mDownColor(*this, false, "DownColor", 0, 0, 0, 0)
	, mFont(*this, false, "Font", "arial.ttf")
	, mFontSize(*this, false, "FontSize", 12)
	, mLength(*this, false, "Length", 0)
	, mTextAlignment(*this, false, "TextAlignment", 1)
{
	KigsCore::GetNotificationCenter()->addObserver(this, "ReloadTexture", "ResetContext");
}

void UIButtonText::InitModifiable()
{
	UIButton::InitModifiable();
	if (_isInit)
	{
		if (mUpText.const_ref() != "")
			ChangeTextTexture(mUpText.const_ref(), 0);
		if (mOverText.const_ref() != "")
			ChangeTextTexture(mOverText.const_ref(), 1);
		if (mDownText.const_ref() != "")
			ChangeTextTexture(mDownText.const_ref(), 2);

		AutoSize();

		ChangeState();


		mUpText.changeNotificationLevel(Owner);
		mDownText.changeNotificationLevel(Owner);
		mOverText.changeNotificationLevel(Owner);
		mFont.changeNotificationLevel(Owner);
		mFontSize.changeNotificationLevel(Owner);
	}
}

void UIButtonText::NotifyUpdate(const unsigned int labelid)
{
	UIButton::NotifyUpdate(labelid);

	if (labelid == mUpText.getLabelID())
		ChangeTextTexture(mUpText.c_str(), 0);
	if (labelid == mDownText.getLabelID())
		ChangeTextTexture(mDownText.c_str(), 2);
	if (labelid == mOverText.getLabelID())
		ChangeTextTexture(mOverText.c_str(), 1);

	if (labelid == mFont.getLabelID() ||
		labelid == mFontSize.getLabelID())
	{
		if (mUpText.const_ref() != "")
			ChangeTextTexture(mUpText.const_ref(), 0);
		if (mOverText.const_ref() != "")
			ChangeTextTexture(mOverText.const_ref(), 1);
		if (mDownText.const_ref() != "")
			ChangeTextTexture(mDownText.const_ref(), 2);
	}
}
void UIButtonText::ChangeTextureColor(Vector4D& UpColor, Vector4D& OverColor, Vector4D& DownColor)
{
	mUpColor[0] = UpColor.x;
	mUpColor[1] = UpColor.y;
	mUpColor[2] = UpColor.z;
	mUpColor[3] = UpColor.w;

	mOverColor[0] = OverColor.x;
	mOverColor[1] = OverColor.y;
	mOverColor[2] = OverColor.z;
	mOverColor[3] = OverColor.w;

	mDownColor[0] = DownColor.x;
	mDownColor[1] = DownColor.y;
	mDownColor[2] = DownColor.z;
	mDownColor[3] = DownColor.w;

	ChangeState();
}

void UIButtonText::GetUpColor(float& R, float& G, float& B)
{
	R = mUpColor[0];
	G = mUpColor[1];
	B = mUpColor[2];
}

void UIButtonText::GetOverColor(float& R, float& G, float& B)
{
	R = mOverColor[0];
	G = mOverColor[1];
	B = mOverColor[2];
}

void UIButtonText::GetDownColor(float& R, float& G, float& B)
{
	R = mDownColor[0];
	G = mDownColor[1];
	B = mDownColor[2];
}

void	UIButtonText::ChangeTextTexture(const unsigned short* _text, unsigned int _texture)
{
	if (_text)
	{
		char tmptxt[1024] = { 0 };
		strcpywUtoC(tmptxt, _text);

		kstl::string L_text = tmptxt;

		ChangeTextTexture(L_text, _texture);
	}
}

void	UIButtonText::ChangeTextTexture(const kstl::string & a_text, unsigned int _texture)
{
	Texture* L_Texture = NULL;
	

	kstl::string _text = a_text;
	if (_text != "")
	{
		switch (_texture)
		{
		case 0:
			//up texture
			if (!mUpTexturePointer)
			{
				mUpTexturePointer = KigsCore::GetInstanceOf(getName() + "_UPTEX", "Texture");
				mUpTexturePointer->Init();
			}

			L_Texture = mUpTexturePointer.get();
			mUpText = _text;
			break;
		case 1:
			//over texture
			if (!mOverTexturePointer)
			{
				mOverTexturePointer = KigsCore::GetInstanceOf(getName() + "_OVERTEX", "Texture");
				mOverTexturePointer->Init();
			}

			L_Texture = mOverTexturePointer.get();
			mOverText = _text;
			break;
		case 2:
			//down texture
			if (!mDownTexturePointer)
			{
				mDownTexturePointer = KigsCore::GetInstanceOf(getName() + "_DOWNTEX", "Texture");
				mDownTexturePointer->Init();
			}

			L_Texture = mDownTexturePointer.get();
			mDownText = _text;
			break;
		}
		auto& theLocalizationManager = KigsCore::Singleton<LocalizationManager>();
		float LanguageScale = 1.0f;
		theLocalizationManager->getValue("LanguageScale", LanguageScale);
		// need localization ?
		if (_text[0] == '#')
		{
			kstl::string key = _text.substr(1, _text.length() - 1);

			PLATFORM_WCHAR* localized = (PLATFORM_WCHAR*)theLocalizationManager->getLocalizedString(key.c_str());

			bool modified = false;
			if (mLength > 0)
				localized = CutText(localized, modified);

			if (localized)
				L_Texture->CreateFromText(localized, (unsigned int)((float)((unsigned int)mFontSize) * LanguageScale), mFont.c_str(), mTextAlignment, 255.0f, 255.0f, 255.0f);
			if (modified)
				free(localized);
		}
		else
		{
			bool modified = false;
			if (mLength > 0)
				_text = CutText(_text.c_str(), modified);

			L_Texture->CreateFromText(_text.c_str(), (unsigned int)((float)((unsigned int)mFontSize) * LanguageScale), mFont.const_ref().c_str(), mTextAlignment, 255.0f, 255.0f, 255.0f);
		}

		float width, height;
		L_Texture->GetSize(width, height);

		if (width != mSize[0] || height != mSize[1])
		{
			mSize = v2f(width, height);
			SetNodeFlag(Node2D_SizeChanged);
		}
	}
	else
	{
		switch (_texture)
		{
		case 0:
			mUpTexturePointer = NULL;
			break;
		case 1:
			mOverTexturePointer = NULL;
			break;
		case 2:
			mDownTexturePointer = NULL;
			break;
		default:
			break;
		}
	}

	ChangeState();
}

void UIButtonText::ChangeTexture(kstl::string & _UpText, kstl::string & _overText, kstl::string & _downText)
{
	ChangeTextTexture(_UpText, 0);

	if (_overText != "")
		ChangeTextTexture(_overText, 1);
	if (_downText != "")
		ChangeTextTexture(_downText, 2);

	//auto Size
	if (mUpTexturePointer && mAutoResize)
	{
		float width, height;
		mUpTexturePointer->GetSize(width, height);

		if (width != mSize[0] || height != mSize[1])
		{
			mSize = v2f(width, height);
			SetNodeFlag(Node2D_SizeChanged);
		}
	}
}

unsigned short*		UIButtonText::CutText(unsigned short* _text, bool& _flag)
{
	unsigned short*	read = (unsigned short*)_text;

	unsigned int len = 0;

	while (*read)
	{
		++read;
		++len;
	}

	if (len > mLength)
	{
		unsigned short* newtext = (unsigned short*)malloc((mLength + 1)*sizeof(unsigned short));
		memset(newtext, 0, (mLength + 1)*sizeof(unsigned short));
		memcpy(newtext, _text, mLength*sizeof(unsigned short));
		_flag = true;
		return newtext;
	}

	return _text;
}

char*	UIButtonText::CutText(const char* _text, bool& _flag)
{
	if (strlen(_text) > mLength)
	{
		char* newtext = (char*)malloc((mLength + 1)*sizeof(char));
		memset(newtext, 0, (mLength + 1)*sizeof(char));
		memcpy(newtext, _text, mLength*sizeof(char));
		_flag = true;
		return newtext;
	}

	return (char*)_text;
}

DEFINE_METHOD(UIButtonText, ReloadTexture)
{
	if (mUpTexturePointer)
	{
		mTexturePointer->setUserFlag(Texture::isDirtyContext);
		mTexturePointer->ReInit();
		ChangeTextTexture(mUpText.c_str(), 0);
	}
	if (mOverTexturePointer)
	{
		mOverTexturePointer->setUserFlag(Texture::isDirtyContext);
		mOverTexturePointer->ReInit();
		ChangeTextTexture(mOverText.c_str(), 1);
	}
	if (mDownTexturePointer)
	{
		mDownTexturePointer->setUserFlag(Texture::isDirtyContext);
		mDownTexturePointer->ReInit();
		ChangeTextTexture(mDownText.c_str(), 2);
	}

	return false;
}

void UIButtonText::ChangeState()
{
	if (mIsEnabled) // down and mouse over only when enabled
	{
		if (mIsDown)
		{
			if (mDownTexturePointer)
				mTexturePointer = mDownTexturePointer;
			if (mDownColor[3] != 0.0f)
			{
				mColor[0] = mDownColor[0];
				mColor[1] = mDownColor[1];
				mColor[2] = mDownColor[2];
				mOpacity = mDownColor[3];
			}
			return;
		}

		if (mIsMouseOver)
		{
			if (mOverTexturePointer)
				mTexturePointer = mOverTexturePointer;
			if (mOverColor[3] != 0.0f)
			{
				mColor[0] = mOverColor[0];
				mColor[1] = mOverColor[1];
				mColor[2] = mOverColor[2];
				mOpacity = mOverColor[3];
			}
			else
			{
				mColor[0] = mUpColor[0];
				mColor[1] = mUpColor[1];
				mColor[2] = mUpColor[2];
				mOpacity = mUpColor[3];
			}
			return;
		}
	}

	// set default color and texture
	mTexturePointer = mUpTexturePointer;
	mColor[0] = mUpColor[0];
	mColor[1] = mUpColor[1];
	mColor[2] = mUpColor[2];
	mOpacity = mUpColor[3];
}
