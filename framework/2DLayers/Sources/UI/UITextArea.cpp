#include "UI/UITextArea.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "AlphaMask.h"
#include "LocalizationManager.h"
#include "NotificationCenter.h"
#include "InputIncludes.h"
#include "CoreBaseApplication.h"
#include "ModuleRenderer.h"
#include "Platform/Input/KeyDefine.h"
#include "TravState.h"
#include "UIVerticesInfo.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UITextArea, UITextArea, 2DLayers);
IMPLEMENT_CLASS_INFO(UITextArea)

UITextArea::UITextArea(const kstl::string& name, CLASS_NAME_TREE_ARG) :
	UIDrawableItem(name, PASS_CLASS_NAME_TREE_ARG),
	myTextColor(*this, false, "TextColor", KFLOAT_ZERO, KFLOAT_ZERO, KFLOAT_ZERO, 255.0f),
	myText(*this, false, "Text", (kstl::string)"DefaultText"),
	myFont(*this, false, "Font", ""),
	myReleaseAction(*this, false, "ReleaseAction", ""),
	myFontSize(*this, false, "FontSize", 12),
	myLength(*this, false, "Length", 0),
	myRow(*this, false, "Row", 1),
	myCol(*this, false, "Col", 1),
	myHasDefaultText(*this, false, "HasDefaultText", true),
	myBold(*this, false, "Bold", false),
	myTextAlign(*this, false, "TextAlignment", 1)
{
	CONSTRUCT_METHOD(UITextArea, UpdateKeyBoard);

	unsetUserFlag(UserFlagUseColorArray);

	myStayPressed = true;
	myIsDown = false;
	myIsDefaultText = true;
}

void	UITextArea::InitModifiable()
{
	UIDrawableItem::InitModifiable();

	SP<TextureFileManager> textureManager = KigsCore::GetSingleton("TextureFileManager");
	myTexture = textureManager->CreateTexture(getName());
	myTexture->Init();
	myTexture->SetRepeatUV(false, false);
}


void UITextArea::NotifyUpdate(const unsigned int labelid)
{
	if ((labelid == myText.getLabelID()))
	{
		ChangeText(NULL);
		KigsCore::GetNotificationCenter()->postNotificationName("TextChanged", this);
	}
	UIDrawableItem::NotifyUpdate(labelid);
}

//-----------------------------------------------------------------------------------------------------
//ReloadTexture

void	UITextArea::ReloadTexture()
{
	if (myTexture && myText != usString(""))
	{
		ChangeText(myText.us_str());
	}
}

UITextArea::~UITextArea()
{
	myStayPressed = false;
	myIsDown = false;

}

void UITextArea::CreateFirstText()
{
	myText.changeNotificationLevel(Owner);

	ChangeText(NULL);
}

bool UITextArea::isAlpha(float X, float Y)
{
	//Try to get my mask
	if (!myAlphaMask)
	{
		kstl::vector<ModifiableItemStruct> sons = getItems();

		for (unsigned int i = 0; i < sons.size(); i++)
		{
			if (sons[i].myItem->isSubType("AlphaMask"))
			{
				myAlphaMask = sons[i].myItem;
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

void UITextArea::GetFocus()
{
	myFocus = true;

	//Get KeyBoard
	ModuleInput* theInputModule = (ModuleInput*)CoreGetModule(ModuleInput);
	KeyboardDevice* theKeyboard = theInputModule->GetKeyboard();
	if (theKeyboard == NULL)
		return;

	CMSP toAdd(this, StealRefTag{});

	theKeyboard->addItem(toAdd);
	theKeyboard->Show();
}

void UITextArea::LoseFocus()
{
	myFocus = false;
	myIsDown = false;

	KigsCore::GetNotificationCenter()->removeObserver(this, "UpdateTextArea");

	if (myReleaseAction.const_ref() != "")
		KigsCore::GetNotificationCenter()->postNotificationName(myReleaseAction.const_ref(), this);


	//Get KeyBoard
	ModuleInput* theInputModule = (ModuleInput*)CoreGetModule(ModuleInput);
	KeyboardDevice* theKeyboard = theInputModule->GetKeyboard();
	if (theKeyboard == NULL)
		return;

	CMSP toDel(this, StealRefTag{});
	theKeyboard->removeItem(toDel);
	theKeyboard->Hide();
}

void UITextArea::UpdateText(kstl::vector<int> _keycodeList)
{
	//Get KeyBoard
	ModuleInput* theInputModule = (ModuleInput*)CoreGetModule(ModuleInput);
	KeyboardDevice* theKeyboard = theInputModule->GetKeyboard();

	char tmptxt[512] = { 0 };
	strcpywUtoC(tmptxt, (unsigned short*)myText.us_str());
	kstl::string newString = tmptxt;

	if (newString == "DefaultText" || myHasDefaultText)
	{
		newString = "";
		myHasDefaultText = false;
		myIsDefaultText = false;
	}

	for (unsigned int i = 0; i < _keycodeList.size(); i++)
	{
		//Touch pressed
		if (_keycodeList[i] == 14)//Return
		{
			if (!newString.empty())
				newString = newString.substr(0, newString.length() - 1);
		}
		else if (_keycodeList[i] > 0)
		{
			newString += theKeyboard->ScanToChar(_keycodeList[i]);
		}
	}
	ChangeText(newString);
}


void	UITextArea::ChangeText(const unsigned short* _newText)
{
	if (_newText)
		myText = _newText;
	else
		_newText = (unsigned short*)myText.us_str();

	char tmptxt[512] = { 0 };
	strcpywUtoC(tmptxt, _newText);

	if (((kstl::string)tmptxt) != "")
	{


		//Set Text Color
		kfloat R, G, B, A = 0.0f;
		GetColor(R, G, B, A);

		SP<LocalizationManager> theLocalizationManager = KigsCore::GetSingleton("LocalizationManager");
		float LanguageScale = 1.0f;
		theLocalizationManager->getValue("LanguageScale", LanguageScale);


		// need localization ?
		const kstl::string& text = tmptxt;
		if (text[0] == '#')
		{
			kstl::string key = text.substr(1, text.length() - 1);

			const PLATFORM_WCHAR* localized = theLocalizationManager->getLocalizedString(key.c_str());
			myTexture->CreateFromText(localized, (unsigned int)((float)((unsigned int)myFontSize) * LanguageScale), (myFont.const_ref()).c_str(), myTextAlign, R, G, B, A, TinyImage::RGBA_32_8888, myBold);
		}
		else
		{
			myTexture->CreateFromText(myText.us_str(), (unsigned int)((float)((unsigned int)myFontSize) * LanguageScale), (myFont.const_ref()).c_str(), myTextAlign, R, G, B, A, TinyImage::RGBA_32_8888, myBold);
		}
	}
}

bool UITextArea::TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick)
{
	if (buttonEvent&UIInputEvent::LEFT)
	{
		bool down = buttonState&UIInputEvent::LEFT;
		if (!this->GetParents().empty())
		{
			CoreModifiable* parent = this->GetParents()[0];
			if (parent->isSubType("UIItem"))
			{
				//Ask permission at parent
				if (!((UIItem*)parent)->PermissionToClicRequiredFromParent(down, this))
				{
					return false;
				}
			}
		}

		if (!myIsDown && down) // Button Pressed
		{
			myIsDown = down;
			if (!myFocus)
			{
				KigsCore::GetNotificationCenter()->postNotificationName("GetFocus", this);
				this->GetFocus();
			}
		}
		else if (myIsDown && !down && !myStayPressed) // Button Released
		{
			myIsDown = false;
			if (myFocus)
			{
				KigsCore::GetNotificationCenter()->postNotificationName("ReleaseFocus", this);
				this->LoseFocus();
			}
		}

		//Traitement
		if (!this->GetParents().empty())
		{
			CoreModifiable* parent = this->GetParents()[0];
			if (parent->isSubType("UIItem"))
			{
				//Ask permission at parent
				((UIItem*)parent)->TreatClick(down, this);

			}
		}
	}
	return true;
}


void UITextArea::ProtectedDraw(TravState* state)
{
	UIDrawableItem::ProtectedDraw(state);

	// draw text
	if (!myTexture)
		return;

	myTexture->DoPreDraw(state);

	//VERTEX
	float tx, ty;
	myTexture->GetSize(tx, ty);

	float posY = mySizeY - ty;

	Point2D pt[4];
	pt[0].Set(0.0f, posY);
	pt[1].Set(0.0f, posY + ty);
	pt[2].Set(tx, posY + ty);
	pt[3].Set(tx, posY);

	TransformPoints(pt, 4);

	VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(mTextureQI->Buffer());

	mTextureQI->Flag |= UIVerticesInfo_Vertex;
	buf[0].setVertex(pt[0].x, pt[0].y);
	buf[1].setVertex(pt[1].x, pt[1].y);
	buf[2].setVertex(pt[2].x, pt[2].y);
	buf[3].setVertex(pt[3].x, pt[3].y);

	// TEXTURE COORD
	kfloat ratioX, ratioY;
	myTexture->GetRatio(ratioX, ratioY);

	mTextureQI->Flag |= UIVerticesInfo_Texture;
	buf[0].setTexUV(0.0f, 0.0f);
	buf[1].setTexUV(0.0f, ratioY);
	buf[2].setTexUV(ratioX, ratioY);
	buf[3].setTexUV(ratioX, 0.0f);

	ModuleSpecificRenderer* renderer = state->GetRenderer();
	renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_ON);
	renderer->SetAlphaMode(RENDERER_ALPHA_GREATER, 0.01f);
	renderer->SetBlendMode(RENDERER_BLEND_ON);
	renderer->SetBlendFuncMode(RENDERER_BLEND_SRC_ALPHA, RENDERER_BLEND_ONE_MINUS_SRC_ALPHA);
	renderer->DrawUIQuad(state, mTextureQI);

	myTexture->DoPostDraw(state);
}

DEFINE_METHOD(UITextArea, UpdateKeyBoard)
{
	if (!myFocus)
		return false;

	kstl::vector<KeyEvent> * eventList = static_cast<kstl::vector<KeyEvent>*> (privateParams);

	kstl::vector<KeyEvent>::iterator ITStart = eventList->begin();
	kstl::vector<KeyEvent>::iterator ITEND = eventList->end();

	kstl::string newString = myText.ToString();

	bool terminated = false;
	while (ITStart != ITEND && !terminated)
	{
		KeyEvent temp = (*ITStart);
		++ITStart;

		if (temp.Action != KeyEvent::ACTION_UP)
			continue;

		//Touch pressed
		switch (temp.KeyCode)
		{
		case CM_KEY_BACK://Return
			if (!newString.empty())
			{
				newString = newString.substr(0, newString.length() - 1);
			}
			break;
		case CM_KEY_RETURN://Return
			LoseFocus();
			terminated = true;
			break;
		default:
			if (temp.flag & 1)
			{
				newString += temp.Unicode;
			}
			break;
		}
	}

	if (myText.ToString() != newString)
	{
		ChangeText(newString);
		myText = newString;

		KigsCore::GetNotificationCenter()->postNotificationName("TextChanged", this);
	}
	return false;
}