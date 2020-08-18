#pragma once

#include "UI/UITexturedItem.h"

#include "stb/stb_truetype.h"
#include "maUSString.h"

#include "AttributePacking.h"
#include "TouchInputEventManager.h"

/*
DynamicText Tags:

note: respect spacing!

<color>RRGGBBAA</color> everything after this tag will be colored according to the hexadecimal color code
<color/> revert to the previous color

	exemple: "This is <color>FF0000FF</color>RED</color>"

<image>filename.png</image> insert an inline image in the text

<item>UIButton:mybutton</item> insert an inline uiitem in the text. MUST BE a son of the UIDynamicText

<align xxx/> every image or item after this tag will be aligned according to xxx :
	center halfchar
	center baseline
	baseline
	
	exemple: "This is a duck : <align center baseline/><image>duck.png</image>"

<link>UIItem:Test@TestMethod</link>...<link/>

<marker>CoreModifiable:myCM@MyMethod</marker> calls myCM->MyMethod(this, IterationState& state)

*/

struct UnicodeRange
{
	int codepoint_start;
	int num_chars;
	stbtt_packedchar* mBakedChars = nullptr;
};

struct FontMap
{
	SmartPointer<CoreRawBuffer> mFontBuffer = nullptr;
	SP<Texture>					mFontTexture = nullptr;
	kstl::vector<UnicodeRange> mRanges;
	stbtt_fontinfo mFontInfo = {};
	int mFontMapSize;
	float font_size;
	kstl::string font_id;

	UnicodeRange* GetRangeForCodePoint(int codepoint)
	{
		for (auto& range : mRanges)
		{
			if (codepoint >= range.codepoint_start && codepoint < range.codepoint_start + range.num_chars)
				return &range;
		}
		return nullptr;
	}

};

// ****************************************
// * FontMapManager class
// * --------------------------------------
/**
* \file	UIDynamicText.h
* \class	FontMapManager
* \ingroup 2DLayers
* \brief	Manage font textures.
*/
// ****************************************

class FontMapManager : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(FontMapManager, CoreModifiable, ModuleName);
	DECLARE_CONSTRUCTOR(FontMapManager);

	kigs::unordered_map<KigsID, FontMap> mFontMap;
	FontMap* PrecacheFont(const kstl::string& fontname, float fontsize);

private:
	void OnFontTextureDestroy(CoreModifiable* tex)
	{
		auto it = mFontMap.find(tex->getName());
		if (it != mFontMap.end())
		{
			for(auto& range : it->second.mRanges)
				delete[] range.mBakedChars;

			mFontMap.erase(it);
		}
	}

	void ReloadTextures();

	WRAP_METHODS(OnFontTextureDestroy, PrecacheFont, ReloadTextures);
};


struct TextTag
{
	enum class Type : u32
	{
		Ignore = 0,
		ColorStart,
		ColorEnd,
		ExternalItem,
		InlineItem,
		Marker,
		ClickableStart,
		ClickableEnd,
	};
	enum class ItemAlign : u32
	{
		BottomOnBaseline = 0,
		CenteredAroundBaseLine,
		CenteredAroundHalfChar,
	};
	Type type;
	v4f color;

	CoreModifiable *item;
	ItemAlign align;
	KigsID clickID;
	usString clickParam;

	u32 start_index;
};

usString TextTagProcessor(const usString& text, kstl::vector<TextTag>* output_tags = nullptr, kstl::vector<CoreModifiable*>* inline_items = nullptr, CoreModifiable* obj = nullptr);

// ****************************************
// * UIDynamicText class
// * --------------------------------------
/**
* \file	UIDynamicText.h
* \class	UIDynamicText
* \ingroup 2DLayers
* \brief	Manage UI Texts with light html like tags.
*/
// ****************************************
class UIDynamicText : public UITexturedItem
{
public:
	DECLARE_CLASS_INFO(UIDynamicText, UITexturedItem, ModuleName);
	DECLARE_INLINE_CONSTRUCTOR(UIDynamicText)
	{
		mText.changeNotificationLevel(Owner);
		mFontSize.changeNotificationLevel(Owner);
		mFont.changeNotificationLevel(Owner);
		mTextAlign.changeNotificationLevel(Owner);
		mSelectedCharacter.changeNotificationLevel(Owner);
		mMaxWidth.changeNotificationLevel(Owner);
		mFontScaleFactor.changeNotificationLevel(Owner);
		mIgnoreTags.changeNotificationLevel(Owner);
		mIgnoreColorTags.changeNotificationLevel(Owner);
		mShowCursor.changeNotificationLevel(Owner);
		mExtraLineSpacing.changeNotificationLevel(Owner);
		mOpacity.changeNotificationLevel(Owner);
		mColor.changeNotificationLevel(Owner);
	}
	SIGNALS(TextRebuilt);

	enum TextAlignment
	{
		TextAlign_Left = 0,
		TextAlign_Right,
		TextAlign_Center,
		TextAlign_Justify,
	};

	void ForceSetupText();
	void SetUpNodeIfNeeded() override;
	v2f GetCursorPos() const { return mLastCursorPos; }


	struct IterationState
	{
		unsigned short character;
		v2f pos;
		stbtt_aligned_quad quad;
		int current_character_index;
		int current_line;
		float char_width;
		float line_height;
		TextTag* current_color_tag;
		TextTag* current_clickable_tag;
	};
	void IterateCharacters(std::function<bool(IterationState&)> func, bool is_draw = false);
	IterationState PickCharacter(v2f pos);

protected:

	void InitModifiable() override;
	void NotifyUpdate(const unsigned int labelID) override;
	void ProtectedDraw(TravState* state) override;

	void SetVertexArray(UIVerticesInfo * aQI) override {};
	void SetColor(UIVerticesInfo * aQI) override {};
	void SetTexUV(UIVerticesInfo * aQI) override {};

	virtual bool ManageClickTouchEvent(ClickEvent& click_event);


	float MeasureWord(const unsigned short* str);


	maUSString	mText = BASE_ATTRIBUTE(Text);
	maString	mFont = BASE_ATTRIBUTE(Font, "PT-serif.ttf");
	maFloat		mFontSize = BASE_ATTRIBUTE(FontSize, 36);
	maInt		mSelectedCharacter = BASE_ATTRIBUTE(SelectedCharacter, -1);
	maInt		mTextAlign = BASE_ATTRIBUTE(TextAlignment, TextAlign_Left);
	maFloat		mMaxWidth = BASE_ATTRIBUTE(MaxWidth, 0.0f);
	maBool      mResizeToMaxWidth = BASE_ATTRIBUTE(ResizeToMaxWidth, true);
	maInt		mMaxLines = BASE_ATTRIBUTE(MaxLines, 0);
	maFloat		mFontScaleFactor = BASE_ATTRIBUTE(FontScaleFactor, 1.0f);

	maFloat		mExtraLineSpacing = BASE_ATTRIBUTE(ExtraLineSpacing, 0.0f);

	maBool		mIgnoreTags = BASE_ATTRIBUTE(IgnoreTags, false);
	maBool		mIgnoreColorTags = BASE_ATTRIBUTE(IgnoreColorTags, false);
	
	maBool		mShowCursor = BASE_ATTRIBUTE(ShowCursor, false);
	maBool		mPickable = BASE_ATTRIBUTE(Pickable, false);

	FontMap* mFontMap = nullptr;
	int mLastMaxIndex = -1;

	

	bool mTextChanged = true;
	bool mFontChanged = true;
	bool mChanged = false;
	TouchEventState* mEventState = nullptr;

private:

	void LoadFont();
	void BuildVertexArray();
	void PreprocessTags();

	v2f mTextSize = { 0,0 };

	v2f mLastCursorPos = { 0,0 };

	usString mCurrentString;

	kstl::vector<CoreModifiable*> mInlineItems;
	kstl::vector<TextTag> mPreprocessedTags;


	kstl::vector<UnicodeRange> mRanges;

	WRAP_METHODS(ForceSetupText, ManageClickTouchEvent);
};

