#include "UI/UIText.h"
#include "AlphaMask.h"
#include "Texture.h"
#include "LocalizationManager.h"
#include "NotificationCenter.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIText, UIText, 2DLayers);
IMPLEMENT_CLASS_INFO(UIText)

UIText::UIText(const kstl::string& name, CLASS_NAME_TREE_ARG) 
	: UITexturedItem(name, PASS_CLASS_NAME_TREE_ARG)
	, myText(*this, false, "Text", (kstl::string)"")
	, myFontName(*this, false, "FontName", "arial.ttf")
	, myFontSize(*this, false, "FontSize", 12)
	, myDirection(*this, false, "Direction", 0)
	, myLength(*this, false, "Length", 0)
	, myBold(*this, false, "Bold", false)
	, myStroke(*this, false, "Stroke", false)
	, myStrokeColor(*this, false, "StrokeColor", 0.0f, 0.0f, 0.0f, 255.0f)
	, myAlignment(*this, false, "Alignment", 1)
	, myMaxSize(*this, false, "MaxSize", 0)
	, myMaxLineNumber(*this, false, "MaxLineNumber", 0)
{
	// text don't have color array
	unsetUserFlag(UserFlagUseColorArray);

	KigsCore::GetNotificationCenter()->addObserver(this, "ReloadTexture", "ResetContext");
}

void UIText::NotifyUpdate(const unsigned int labelid)
{
	if ((labelid == myText.getLabelID())||
		(labelid == myFontSize.getLabelID()) ||
		(labelid == myBold.getLabelID()) ||
		(labelid == myColor.getLabelID()) ||
		(labelid == myFontName.getLabelID()) ||
		//(labelid == myOpacity.getLabelID()) ||
		(labelid == myMaxSize.getLabelID()) ||
		(labelid == myAlignment.getLabelID()) ||
		(labelid == myLength.getLabelID()))
	{
		ChangeText(myText.const_ref());
	}
	UITexturedItem::NotifyUpdate(labelid);
}

void UIText::InitModifiable()
{
	UITexturedItem::InitModifiable();
	if (IsInit())
	{
		myColor.changeNotificationLevel(Owner);
		myText.changeNotificationLevel(Owner);
		myFontSize.changeNotificationLevel(Owner);
		myBold.changeNotificationLevel(Owner);
		myFontName.changeNotificationLevel(Owner);
		myLength.changeNotificationLevel(Owner);
		myOpacity.changeNotificationLevel(Owner);
		myMaxSize.changeNotificationLevel(Owner);
		myAlignment.changeNotificationLevel(Owner);

		myTexture = KigsCore::CreateInstance(getName() + "_TEX", "Texture");
		myTexture->Init();
		myTexture->SetRepeatUV(false, false);
		ChangeText(myText.const_ref());
	}
}

bool UIText::isAlpha(float X, float Y)
{
	//Try to get my mask
	if (!myAlphaMask)
	{
		kstl::vector<ModifiableItemStruct> sons = getItems();

		for (unsigned int i = 0; i < sons.size(); i++)
		{
			if (sons[i].myItem->isSubType("AlphaMask"))
			{
				myAlphaMask = (AlphaMask*)sons[i].myItem;
				break;
			}
		}
	}

	if (myAlphaMask)
	{
		//Check on my mask the specified location
		return !myAlphaMask->CheckTo(X, Y);
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
	//Set Text Color
	unsigned char R = (unsigned char)(myColor[0] * 255.0f);
	unsigned char G = (unsigned char)(myColor[1] * 255.0f);
	unsigned char B = (unsigned char)(myColor[2] * 255.0f);
	unsigned char A = (unsigned char)(myOpacity * 255.0f);

	kfloat stR, stG, stB, st_A;
	GetStrokeColor(stR, stG, stB, st_A);

	// need localization ?
	if ((_newText.length()>0) && (_newText.us_str()[0] == ((unsigned short)'#')))
	{
		kstl::string text = _newText.ToString();
		kstl::string key = text.substr(1, text.length() - 1);
		LocalizationManager* theLocalizationManager = (LocalizationManager*)KigsCore::GetSingleton("LocalizationManager");
		PLATFORM_WCHAR* localized = (PLATFORM_WCHAR*)theLocalizationManager->getLocalizedString(key.c_str());

		bool modified = false;
		if (localized && myLength > 0)
			localized = CutText(localized, modified);

		if (localized)
			myTexture->CreateFromText(localized, myMaxLineNumber, myMaxSize, myFontSize, (myFontName.const_ref()).c_str(), myAlignment, R, G, B, A, TinyImage::RGBA_32_8888, myBold, myStroke, stR, stG, stB, st_A);

		if (modified)
			free(localized);
	}
	else
	{
		bool modified = false;
		unsigned short* L_returneValue = NULL;
		if (myLength > 0)
			L_returneValue = CutText(_newText.us_str(), modified);
		else
			L_returneValue = const_cast<unsigned short*>(_newText.us_str());

		myTexture->CreateFromText(L_returneValue, myMaxLineNumber, myMaxSize, myFontSize, (myFontName.const_ref()).c_str(), myAlignment, R, G, B, A, TinyImage::RGBA_32_8888, myBold, myStroke, stR, stG, stB, st_A);

		if (modified)
			free(L_returneValue);
	}

	int width, height;
	myTexture->getValue(LABEL_TO_ID(Width), width);
	myTexture->getValue(LABEL_TO_ID(Height), height);

	if (mySizeX != width || mySizeY != height)
	{
		mySizeX = width;
		mySizeY = height;
		myNeedUpdatePosition = true;
	}
	
}

bool UIText::TriggerMouseMove(bool over, float _MouseDeltaX, float _MouseDeltaY)
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
}

bool UIText::TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick)
{
	/* should be done by the myMouseOverList if false returned
	if (!this->GetParents().empty())
	{
		CoreModifiable* parent = this->GetParents()[0];
		if (parent->isSubType("UIItem"))
		{
			return ((UIItem*)parent)->TriggerMouseClick(idxButton, down, X, Y, catchClick);
		}
	}
	*/
	return false;
}

unsigned short*		UIText::CutText(const unsigned short* _text, bool& _flag)
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

	return (unsigned short*)_text;
}


DEFINE_METHOD(UIText, ReloadTexture)
{
	if (!myTexture.isNil())
	{
		myTexture->SetFlag(Texture::isDirtyContext);
		myTexture->ReInit();
		ChangeText(myText.us_str());
	}

	return false;
}


