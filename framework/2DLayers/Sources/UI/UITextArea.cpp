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

using namespace Kigs::Draw2D;
using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(UITextArea)

UITextArea::UITextArea(const std::string& name, CLASS_NAME_TREE_ARG) :
	UIDrawableItem(name, PASS_CLASS_NAME_TREE_ARG),
	mText(*this, false, "Text", (std::string)"DefaultText"),
	mFont(*this, false, "Font", ""),
	mReleaseAction(*this, false, "ReleaseAction", ""),
	mFontSize(*this, false, "FontSize", 12),
	mLength(*this, false, "Length", 0),
	mRow(*this, false, "Row", 1),
	mCol(*this, false, "Col", 1),
	mHasDefaultText(*this, false, "HasDefaultText", true),
	mTextAlignment(*this, false, "TextAlignment", 1)
{

	//unsetUserFlag(UserFlagUseColorArray);

	mStayPressed = true;
	mIsDown = false;
	mIsDefaultText = true;
}

void	UITextArea::InitModifiable()
{
	UIDrawableItem::InitModifiable();

	auto textureManager = KigsCore::Singleton<TextureFileManager>();
	mTexturePointer = textureManager->CreateTexture(getName());
	mTexturePointer->Init();
	mTexturePointer->SetRepeatUV(false, false);
}


void UITextArea::NotifyUpdate(const unsigned int labelid)
{
	if ((labelid == mText.getLabelID()))
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
	if (mTexturePointer && mText != usString(""))
	{
		ChangeText(mText.us_str());
	}
}

UITextArea::~UITextArea()
{
	mStayPressed = false;
	mIsDown = false;

}

void UITextArea::CreateFirstText()
{
	mText.changeNotificationLevel(Owner);

	ChangeText(NULL);
}

bool UITextArea::isAlpha(float X, float Y)
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

void UITextArea::GetFocus()
{
	SetNodeFlag(UIItem_HasFocus);

	//Get KeyBoard
	auto theInputModule = KigsCore::GetModule<Input::ModuleInput>();
	Input::KeyboardDevice* theKeyboard = theInputModule->GetKeyboard();
	if (theKeyboard == NULL)
		return;

	
	theKeyboard->addItem(SharedFromThis());
	theKeyboard->Show();
}

void UITextArea::LoseFocus()
{
	ClearNodeFlag(UIItem_HasFocus);
	mIsDown = false;

	KigsCore::GetNotificationCenter()->removeObserver(this, "UpdateTextArea");

	if (mReleaseAction.const_ref() != "")
		KigsCore::GetNotificationCenter()->postNotificationName(mReleaseAction.const_ref(), this);


	//Get KeyBoard
	auto theInputModule = KigsCore::GetModule<Input::ModuleInput>();
	Input::KeyboardDevice* theKeyboard = theInputModule->GetKeyboard();
	if (theKeyboard == NULL)
		return;

	theKeyboard->removeItem(SharedFromThis());
	theKeyboard->Hide();
}

/*void UITextArea::UpdateText(std::vector<int> _keycodeList)
{
	//Get KeyBoard
	ModuleInput* theInputModule = (ModuleInput*)CoreGetModule(ModuleInput);
	KeyboardDevice* theKeyboard = theInputModule->GetKeyboard();

	char tmptxt[512] = { 0 };
	strcpywUtoC(tmptxt, (unsigned short*)mText.us_str());
	std::string newString = tmptxt;

	if (newString == "DefaultText" || mHasDefaultText)
	{
		newString = "";
		mHasDefaultText = false;
		mIsDefaultText = false;
	}

	for (unsigned int i = 0; i < _keycodeList.size(); i++)
	{
		//Touch pressed
		if (_keycodeList[i] == VK_RETURN)//Return
		{
			if (!newString.empty())
				newString = newString.substr(0, newString.length() - 1);
		}
		else if (_keycodeList[i] > 0)
		{
			newString += theKeyboard->ScanToChar(_keycodeList[i],nullptr);
		}
	}
	ChangeText(newString);
}
*/

void	UITextArea::ChangeText(const unsigned short* _newText)
{
	if (_newText)
		mText = _newText;
	else
		_newText = (unsigned short*)mText.us_str();

	char tmptxt[512] = { 0 };
	strcpywUtoC(tmptxt, _newText);

	if (((std::string)tmptxt) != "")
	{

		auto theLocalizationManager = KigsCore::Singleton<LocalizationManager>();
		float LanguageScale = 1.0f;
		theLocalizationManager->getValue("LanguageScale", LanguageScale);


		// need localization ?
		const std::string& text = tmptxt;
		if (text[0] == '#')
		{
			std::string key = text.substr(1, text.length() - 1);

			const PLATFORM_WCHAR* localized = theLocalizationManager->getLocalizedString(key.c_str());
			mTexturePointer->CreateFromText(localized, (unsigned int)((float)((unsigned int)mFontSize) * LanguageScale), (mFont.const_ref()).c_str(), mTextAlignment, 255,255, 255, 255, Pict::TinyImage::RGBA_32_8888);
		}
		else
		{
			mTexturePointer->CreateFromText(mText.us_str(), (unsigned int)((float)((unsigned int)mFontSize) * LanguageScale), (mFont.const_ref()).c_str(), mTextAlignment, 255, 255, 255, 255, Pict::TinyImage::RGBA_32_8888);
		}
	}
}

void UITextArea::ProtectedDraw(TravState* state)
{
	UIDrawableItem::ProtectedDraw(state);

	// draw text
	if (!mTexturePointer)
		return;

	mTexturePointer->DoPreDraw(state);

	//VERTEX
	float tx, ty;
	mTexturePointer->GetSize(tx, ty);

	float posY = mSize[1] - ty;

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
	float ratioX, ratioY;
	mTexturePointer->GetRatio(ratioX, ratioY);

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

	mTexturePointer->DoPostDraw(state);
}

DEFINE_METHOD(UITextArea, UpdateKeyBoard)
{
	if (!GetNodeFlag(UIItem_HasFocus))
		return false;

	std::vector<KeyEvent> * eventList = static_cast<std::vector<KeyEvent>*> (privateParams);

	std::vector<KeyEvent>::iterator ITStart = eventList->begin();
	std::vector<KeyEvent>::iterator ITEND = eventList->end();

	std::string newString = mText.ToString();

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
		case VK_BACK://Return
			if (!newString.empty())
			{
				newString = newString.substr(0, newString.length() - 1);
			}
			break;
		case VK_RETURN://Return
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

	if (mText.ToString() != newString)
	{
		ChangeText(newString);
		mText = newString;

		KigsCore::GetNotificationCenter()->postNotificationName("TextChanged", this);
	}
	return false;
}