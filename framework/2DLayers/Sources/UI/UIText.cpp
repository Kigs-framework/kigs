#include "UI/UIText.h"
#include "AlphaMask.h"
#include "Texture.h"
#include "LocalizationManager.h"
#include "NotificationCenter.h"

using namespace Kigs::Draw2D;
using namespace Kigs::Draw;

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIText, UIText, 2DLayers);
IMPLEMENT_CLASS_INFO(UIText)

UIText::UIText(const std::string& name, CLASS_NAME_TREE_ARG) 
	: UITexturedItem(name, PASS_CLASS_NAME_TREE_ARG)
{
	// text don't have color array
	//unsetUserFlag(UserFlagUseColorArray);

	KigsCore::GetNotificationCenter()->addObserver(this, "ReloadTexture", "ResetContext");
}

void UIText::NotifyUpdate(const unsigned int labelid)
{
	if ((labelid == KigsID("Text")._id)||
		(labelid == KigsID("FontSize")._id ) ||
		(labelid == KigsID("Font")._id ) ||
		(labelid == KigsID("MaxWidth")._id ) ||
		(labelid == KigsID("TextAlignment")._id ) ||
		(labelid == KigsID("Length")._id ))
	{
		ChangeText(mText);
	}
	UITexturedItem::NotifyUpdate(labelid);
}

void UIText::InitModifiable()
{
	UITexturedItem::InitModifiable();
	if (IsInit())
	{
		setOwnerNotification("Text", true);
		setOwnerNotification("FontSize", true);
		setOwnerNotification("Font", true);
		setOwnerNotification("Length", true);
		setOwnerNotification("Opacity", true);
		setOwnerNotification("MaxWidth", true);
		setOwnerNotification("TextAlignment", true);

		char	nameBuffer[256];
		std::string name = getName();
		if (name.length() > 128)
		{
			name = name.substr(0, 128);
		}

		sprintf(nameBuffer, "%s_%u_TEX", name.c_str(), getUID());

		mTexturePointer = KigsCore::GetInstanceOf(nameBuffer, "TextureHandler");
		mTexturePointer->Init();
		mTexturePointer->GetEmptyTexture(nameBuffer);
		mTexturePointer->SetRepeatUV(false, false);
		ChangeText(mText);
	}
}

bool UIText::isAlpha(float X, float Y)
{
	//Try to get mask
	if (!mAlphaMask)
	{
		std::vector<ModifiableItemStruct> sons = getItems();

		for (unsigned int i = 0; i < sons.size(); i++)
		{
			if (sons[i].mItem->isSubType("AlphaMask"))
			{
				mAlphaMask = sons[i].mItem;
				break;
			}
		}
	}

	if (mAlphaMask)
	{
		//Check on mask the specified location
		return !mAlphaMask->CheckTo(X, Y);
	}

	return false;
}

void	UIText::ChangeText(const std::string& _newText)
{
	usString toChange(_newText);
	ChangeText(toChange);

}

void	UIText::ChangeText(const usString& _newText)
{
	auto theLocalizationManager = KigsCore::Singleton<LocalizationManager>();
	float LanguageScale = 1.0f;
	theLocalizationManager->getValue("LanguageScale", LanguageScale);

	// need localization ?
	if ((_newText.length()>0) && (_newText.us_str()[0] == ((unsigned short)'#')))
	{
		std::string text = _newText.ToString();
		std::string key = text.substr(1, text.length() - 1);

		PLATFORM_WCHAR* localized = (PLATFORM_WCHAR*)theLocalizationManager->getLocalizedString(key);

		bool modified = false;
		if (localized && mLength > 0)
			localized = CutText(localized, modified);

		if (localized)
		{
			mTexturePointer->GetEmptyTexture()->CreateFromText(localized, mMaxLines, mMaxWidth, (unsigned int)((float)((unsigned int)mFontSize) * LanguageScale), mFont.c_str(), mTextAlignment, 255, 255, 255, 255, Pict::TinyImage::RGBA_32_8888);
			mTexturePointer->refreshTextureInfos();
		}
		if (modified)
			free(localized);
	}
	else
	{
		bool modified = false;
		unsigned short* L_returneValue = NULL;
		if (mLength > 0)
			L_returneValue = CutText(_newText.us_str(), modified);
		else
			L_returneValue = const_cast<unsigned short*>(_newText.us_str());

		mTexturePointer->GetEmptyTexture()->CreateFromText(L_returneValue, mMaxLines, mMaxWidth, (unsigned int)((float)((unsigned int)mFontSize) * LanguageScale), mFont.c_str(), mTextAlignment, 255, 255, 255,255, Pict::TinyImage::RGBA_32_8888);
		mTexturePointer->refreshTextureInfos();
		if (modified)
			free(L_returneValue);
	}

	int width, height;
	mTexturePointer->GetEmptyTexture()->getValue("Width", width);
	mTexturePointer->GetEmptyTexture()->getValue("Height", height);

	if (mSize[0] != width || mSize[1] != height)
	{
		mSize = v2f(width, height);
		SetNodeFlag(Node2D_SizeChanged);
	}
	SetUpNodeIfNeeded();
}

/*bool UIText::TriggerMouseMove(bool over, float _MouseDeltaX, float _MouseDeltaY)
{
	if (!this->GetParents().empty())
	{
		CoreModifiable* parent = this->GetParents()[0];
		if (parent->isSubType("UIItem"))
		{
			return ((UIItem*)parent)->TriggerMouseMove(over, _MouseDeltaX, _MouseDeltaY);
		}
	}
	return false;
}*/

/*
bool UIText::TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick)
{
	
	return false;
}*/

unsigned short*		UIText::CutText(const unsigned short* _text, bool& _flag)
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

	return (unsigned short*)_text;
}


DEFINE_METHOD(UIText, ReloadTexture)
{
	if (mTexturePointer)
	{
		mTexturePointer->setUserFlag(Texture::isDirtyContext);
		mTexturePointer->ReInit();
		ChangeText(mText);
	}

	return false;
}


