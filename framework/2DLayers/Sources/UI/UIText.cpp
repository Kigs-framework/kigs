#include "UI/UIText.h"
#include "AlphaMask.h"
#include "Texture.h"
#include "LocalizationManager.h"
#include "NotificationCenter.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIText, UIText, 2DLayers);
IMPLEMENT_CLASS_INFO(UIText)

UIText::UIText(const kstl::string& name, CLASS_NAME_TREE_ARG) 
	: UITexturedItem(name, PASS_CLASS_NAME_TREE_ARG)
	, mText(*this, false, "Text", (kstl::string)"")
	, mFont(*this, false, "Font", "arial.ttf")
	, mFontSize(*this, false, "FontSize", 12)
	, mDirection(*this, false, "Direction", 0)
	, mLength(*this, false, "Length", 0)
	, mTextAlignment(*this, false, "TextAlignment", 1)
	, mMaxWidth(*this, false, "MaxWidth", 128)
	, mMaxLines(*this, false, "MaxLines", 0)
{
	// text don't have color array
	//unsetUserFlag(UserFlagUseColorArray);

	KigsCore::GetNotificationCenter()->addObserver(this, "ReloadTexture", "ResetContext");
}

void UIText::NotifyUpdate(const unsigned int labelid)
{
	if ((labelid == mText.getLabelID())||
		(labelid == mFontSize.getLabelID()) ||
		(labelid == mFont.getLabelID()) ||
		(labelid == mMaxWidth.getLabelID()) ||
		(labelid == mTextAlignment.getLabelID()) ||
		(labelid == mLength.getLabelID()))
	{
		ChangeText(mText.const_ref());
	}
	UITexturedItem::NotifyUpdate(labelid);
}

void UIText::InitModifiable()
{
	UITexturedItem::InitModifiable();
	if (IsInit())
	{
		mText.changeNotificationLevel(Owner);
		mFontSize.changeNotificationLevel(Owner);
		mFont.changeNotificationLevel(Owner);
		mLength.changeNotificationLevel(Owner);
		mOpacity.changeNotificationLevel(Owner);
		mMaxWidth.changeNotificationLevel(Owner);
		mTextAlignment.changeNotificationLevel(Owner);

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
		ChangeText(mText.const_ref());
	}
}

bool UIText::isAlpha(float X, float Y)
{
	//Try to get mask
	if (!mAlphaMask)
	{
		kstl::vector<ModifiableItemStruct> sons = getItems();

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

void	UIText::ChangeText(const kstl::string& _newText)
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
		kstl::string text = _newText.ToString();
		kstl::string key = text.substr(1, text.length() - 1);

		PLATFORM_WCHAR* localized = (PLATFORM_WCHAR*)theLocalizationManager->getLocalizedString(key);

		bool modified = false;
		if (localized && mLength > 0)
			localized = CutText(localized, modified);

		if (localized)
		{
			mTexturePointer->GetEmptyTexture()->CreateFromText(localized, mMaxLines, mMaxWidth, (unsigned int)((float)((unsigned int)mFontSize) * LanguageScale), (mFont.const_ref()).c_str(), mTextAlignment, 255, 255, 255, 255, TinyImage::RGBA_32_8888);
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

		mTexturePointer->GetEmptyTexture()->CreateFromText(L_returneValue, mMaxLines, mMaxWidth, (unsigned int)((float)((unsigned int)mFontSize) * LanguageScale), (mFont.const_ref()).c_str(), mTextAlignment, 255, 255, 255,255, TinyImage::RGBA_32_8888);
		mTexturePointer->refreshTextureInfos();
		if (modified)
			free(L_returneValue);
	}

	int width, height;
	mTexturePointer->GetEmptyTexture()->getValue(LABEL_TO_ID(Width), width);
	mTexturePointer->GetEmptyTexture()->getValue(LABEL_TO_ID(Height), height);

	if (mSizeX != width || mSizeY != height)
	{
		mSizeX = width;
		mSizeY = height;
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
	if (!mTexturePointer.isNil())
	{
		mTexturePointer->SetFlag(Texture::isDirtyContext);
		mTexturePointer->ReInit();
		ChangeText(mText.us_str());
	}

	return false;
}


