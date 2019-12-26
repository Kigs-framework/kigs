#include "UI/UIButtonText.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "NotificationCenter.h"
#include "AlphaMask.h"
#include "LocalizationManager.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIButtonText, UIButtonText, 2DLayers);;
IMPLEMENT_CLASS_INFO(UIButtonText)

IMPLEMENT_CONSTRUCTOR(UIButtonText)
	, myUpText(*this, false, "UpText", "")
	, myDownText(*this, false, "DownText", "")
	, myOverText(*this, false, "OverText", "")
	, myUpColor(*this, false, "UpColor", 0, 0, 0, 0)
	, myOverColor(*this, false, "OverColor", 0, 0, 0, 0)
	, myDownColor(*this, false, "DownColor", 0, 0, 0, 0)
	, myFont(*this, false, "Font", "arial.ttf")
	, myFontSize(*this, false, "FontSize", 12)
	, myLength(*this, false, "Length", 0)
	, myTextAlign(*this, false, "TextAlignment", 1)
{
	KigsCore::GetNotificationCenter()->addObserver(this, "ReloadTexture", "ResetContext");
}

void UIButtonText::InitModifiable()
{
	UIButton::InitModifiable();
	if (_isInit)
	{
		if (myUpText.const_ref() != "")
			ChangeTextTexture(myUpText.const_ref(), 0);
		if (myOverText.const_ref() != "")
			ChangeTextTexture(myOverText.const_ref(), 1);
		if (myDownText.const_ref() != "")
			ChangeTextTexture(myDownText.const_ref(), 2);

		AutoSize();

		ChangeState();


		myUpText.changeNotificationLevel(Owner);
		myDownText.changeNotificationLevel(Owner);
		myOverText.changeNotificationLevel(Owner);
		myFont.changeNotificationLevel(Owner);
		myFontSize.changeNotificationLevel(Owner);
	}
}

void UIButtonText::NotifyUpdate(const unsigned int labelid)
{
	UIButton::NotifyUpdate(labelid);

	if (labelid == myUpText.getLabelID())
		ChangeTextTexture(myUpText.c_str(), 0);
	if (labelid == myDownText.getLabelID())
		ChangeTextTexture(myDownText.c_str(), 2);
	if (labelid == myOverText.getLabelID())
		ChangeTextTexture(myOverText.c_str(), 1);

	if (labelid == myFont.getLabelID() ||
		labelid == myFontSize.getLabelID())
	{
		if (myUpText.const_ref() != "")
			ChangeTextTexture(myUpText.const_ref(), 0);
		if (myOverText.const_ref() != "")
			ChangeTextTexture(myOverText.const_ref(), 1);
		if (myDownText.const_ref() != "")
			ChangeTextTexture(myDownText.const_ref(), 2);
	}
}
void UIButtonText::ChangeTextureColor(Vector4D& UpColor, Vector4D& OverColor, Vector4D& DownColor)
{
	myUpColor[0] = UpColor.x;
	myUpColor[1] = UpColor.y;
	myUpColor[2] = UpColor.z;
	myUpColor[3] = UpColor.w;

	myOverColor[0] = OverColor.x;
	myOverColor[1] = OverColor.y;
	myOverColor[2] = OverColor.z;
	myOverColor[3] = OverColor.w;

	myDownColor[0] = DownColor.x;
	myDownColor[1] = DownColor.y;
	myDownColor[2] = DownColor.z;
	myDownColor[3] = DownColor.w;

	ChangeState();
}

void UIButtonText::GetUpColor(float& R, float& G, float& B)
{
	R = myUpColor[0];
	G = myUpColor[1];
	B = myUpColor[2];
}

void UIButtonText::GetOverColor(float& R, float& G, float& B)
{
	R = myOverColor[0];
	G = myOverColor[1];
	B = myOverColor[2];
}

void UIButtonText::GetDownColor(float& R, float& G, float& B)
{
	R = myDownColor[0];
	G = myDownColor[1];
	B = myDownColor[2];
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
	//TextureFileManager* textureManager = (TextureFileManager*)KigsCore::GetSingleton("TextureFileManager");

	kstl::string _text = a_text;
	if (_text != "")
	{
		switch (_texture)
		{
		case 0:
			//up texture
			if (!myUpTexture)
			{
				myUpTexture = KigsCore::CreateInstance(getName() + "_UPTEX", "Texture");
				myUpTexture->Init();
			}

			L_Texture = myUpTexture.get();
			myUpText = _text;
			break;
		case 1:
			//over texture
			if (!myOverTexture)
			{
				myOverTexture = KigsCore::CreateInstance(getName() + "_OVERTEX", "Texture");
				myOverTexture->Init();
			}

			L_Texture = myOverTexture.get();
			myOverText = _text;
			break;
		case 2:
			//down texture
			if (!myDownTexture)
			{
				myDownTexture = KigsCore::CreateInstance(getName() + "_DOWNTEX", "Texture");
				myDownTexture->Init();
			}

			L_Texture = myDownTexture.get();
			myDownText = _text;
			break;
		}

		// need localization ?
		if (_text[0] == '#')
		{
			kstl::string key = _text.substr(1, _text.length() - 1);
			LocalizationManager* theLocalizationManager = (LocalizationManager*)KigsCore::GetSingleton("LocalizationManager");
			PLATFORM_WCHAR* localized = (PLATFORM_WCHAR*)theLocalizationManager->getLocalizedString(key.c_str());

			bool modified = false;
			if (myLength > 0)
				localized = CutText(localized, modified);

			if (localized)
				L_Texture->CreateFromText(localized, myFontSize, myFont.c_str(), myTextAlign, 255.0f, 255.0f, 255.0f);
			if (modified)
				free(localized);
		}
		else
		{
			bool modified = false;
			if (myLength > 0)
				_text = CutText(_text.c_str(), modified);

			L_Texture->CreateFromText(_text.c_str(), myFontSize, myFont.const_ref().c_str(), myTextAlign, 255.0f, 255.0f, 255.0f);
		}

		float width, height;
		L_Texture->GetSize(width, height);

		if (mySizeX != width || mySizeY != height)
		{
			mySizeX = width;
			mySizeY = height;
			myNeedUpdatePosition = true;
		}
	}
	else
	{
		switch (_texture)
		{
		case 0:
			myUpTexture = NULL;
			break;
		case 1:
			myOverTexture = NULL;
			break;
		case 2:
			myDownTexture = NULL;
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
	if (myUpTexture && myAutoResize)
	{
		float width, height;
		myUpTexture->GetSize(width, height);
		if (width != mySizeX || height != mySizeY)
		{
			mySizeX = width;
			mySizeY = height;
			myNeedUpdatePosition = true;
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

	if (len > myLength)
	{
		unsigned short* newtext = (unsigned short*)malloc((myLength + 1)*sizeof(unsigned short));
		memset(newtext, 0, (myLength + 1)*sizeof(unsigned short));
		memcpy(newtext, _text, myLength*sizeof(unsigned short));
		_flag = true;
		return newtext;
	}

	return _text;
}

char*	UIButtonText::CutText(const char* _text, bool& _flag)
{
	if (strlen(_text) > myLength)
	{
		char* newtext = (char*)malloc((myLength + 1)*sizeof(char));
		memset(newtext, 0, (myLength + 1)*sizeof(char));
		memcpy(newtext, _text, myLength*sizeof(char));
		_flag = true;
		return newtext;
	}

	return (char*)_text;
}

DEFINE_METHOD(UIButtonText, ReloadTexture)
{
	if (myUpTexture)
	{
		myTexture->SetFlag(Texture::isDirtyContext);
		myTexture->ReInit();
		ChangeTextTexture(myUpText.c_str(), 0);
	}
	if (myOverTexture)
	{
		myOverTexture->SetFlag(Texture::isDirtyContext);
		myOverTexture->ReInit();
		ChangeTextTexture(myOverText.c_str(), 1);
	}
	if (myDownTexture)
	{
		myDownTexture->SetFlag(Texture::isDirtyContext);
		myDownTexture->ReInit();
		ChangeTextTexture(myDownText.c_str(), 2);
	}

	return false;
}

void UIButtonText::ChangeState()
{
	if (myIsEnabled) // down and mouse over only when enabled
	{
		if (myIsDown)
		{
			if (myDownTexture)
				myTexture = myDownTexture;
			if (myDownColor[3] != 0.0f)
			{
				myColor[0] = myDownColor[0];
				myColor[1] = myDownColor[1];
				myColor[2] = myDownColor[2];
				myOpacity = myDownColor[3];
			}
			return;
		}

		if (myIsMouseOver)
		{
			if (myOverTexture)
				myTexture = myOverTexture;
			if (myOverColor[3] != 0.0f)
			{
				myColor[0] = myOverColor[0];
				myColor[1] = myOverColor[1];
				myColor[2] = myOverColor[2];
				myOpacity = myOverColor[3];
			}
			else
			{
				myColor[0] = myUpColor[0];
				myColor[1] = myUpColor[1];
				myColor[2] = myUpColor[2];
				myOpacity = myUpColor[3];
			}
			return;
		}
	}

	// set default color and texture
	myTexture = myUpTexture;
	myColor[0] = myUpColor[0];
	myColor[1] = myUpColor[1];
	myColor[2] = myUpColor[2];
	myOpacity = myUpColor[3];
}
