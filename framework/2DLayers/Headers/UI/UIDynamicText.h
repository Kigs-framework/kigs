#pragma once

#include "UI/UITexturedItem.h"

#include "stb/stb_truetype.h"
#include "maUSString.h"

#include "AttributePacking.h"
#include "TouchInputEventManager.h"

namespace Kigs
{

	namespace Draw2D
	{
		/*
		DynamicText Tags:

		note: respect spacing!

		<color>RRGGBBAA</color> everything after this tag will be colored according to the hexadecimal color code
		<color/> revert to the previous color

			exemple: "This is <color>FF0000FF</color>RED</color>"

		<image>filename.png</image> insert an inline image in the text

		<item>UIButton:button</item> insert an inline uiitem in the text. MUST BE a son of the UIDynamicText

		<align xxx/> every image or item after this tag will be aligned according to xxx :
			center halfchar
			center baseline
			baseline

			exemple: "This is a duck : <align center baseline/><image>duck.png</image>"

		<link>UIItem:Test@TestMethod</link>...<link/>

		<marker>CoreModifiable:CM@MyMethod</marker> calls CM->MyMethod(this, IterationState& state)

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
			SP<Draw::Texture>			mFontTexture = nullptr;
			std::vector<UnicodeRange> mRanges;
			stbtt_fontinfo mFontInfo = {};
			int mFontMapSize;
			float font_size;
			std::string font_id;

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

			unordered_map<Core::KigsID, FontMap> mFontMap;
			FontMap* PrecacheFont(const std::string& fontname, float fontsize);

		private:
			void OnFontTextureDestroy(CoreModifiable* tex)
			{
				auto it = mFontMap.find(tex->getName());
				if (it != mFontMap.end())
				{
					for (auto& range : it->second.mRanges)
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

			CoreModifiable* item;
			ItemAlign align;
			KigsID clickID;
			usString clickParam;

			u32 start_index;
		};

		usString TextTagProcessor(const usString& text, std::vector<TextTag>* output_tags = nullptr, std::vector<CMSP>* inline_items = nullptr, CoreModifiable* obj = nullptr);

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
				setOwnerNotification("Text", true);
				setOwnerNotification("FontSize", true);
				setOwnerNotification("Font", true);
				setOwnerNotification("TextAlign", true);
				setOwnerNotification("SelectedCharacter", true);
				setOwnerNotification("MaxWidth", true);
				setOwnerNotification("FontScaleFactor", true);
				setOwnerNotification("IgnoreTags", true);
				setOwnerNotification("IgnoreColorTags", true);
				setOwnerNotification("ShowCursor", true);
				setOwnerNotification("ExtraLineSpacing", true);
				setOwnerNotification("Opacity", true);
				setOwnerNotification("Color", true);
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
			void ProtectedDraw(Scene::TravState* state) override;

			void SetVertexArray(UIVerticesInfo* aQI) override {};
			void SetColor(UIVerticesInfo* aQI) override {};
			void SetTexUV(UIVerticesInfo* aQI) override {};

			virtual bool ManageClickTouchEvent(Input::ClickEvent& click_event);


			float MeasureWord(const unsigned short* str);

			bool							mResizeToMaxWidth = true;
			bool							mIgnoreTags = false;
			bool							mIgnoreColorTags = false;
			bool							mShowCursor = false;
			bool							mPickable = false;
			s32								mSelectedCharacter = -1;
			s32								mTextAlignment = TextAlign_Left;
			s32								mMaxLines = 0;
			float							mFontSize = 36;
			float							mMaxWidth = 0.0f;
			float							mFontScaleFactor = 1.0f;
			float							mExtraLineSpacing = 0.0f;
			usString						mText;
			std::string						mFont = "PT-serif.ttf";

			WRAP_ATTRIBUTES(	mResizeToMaxWidth, mIgnoreTags, mIgnoreColorTags, mShowCursor, mPickable, mSelectedCharacter, mTextAlignment, mMaxLines,
								mFontSize, mMaxWidth, mFontScaleFactor, mExtraLineSpacing, mText, mFont);

			FontMap* mFontMap = nullptr;
			int mLastMaxIndex = -1;

			bool mTextChanged = true;
			bool mFontChanged = true;
			bool mChanged = false;
			Input::TouchEventState* mEventState = nullptr;

		private:

			void LoadFont();
			void BuildVertexArray();
			void PreprocessTags();

			v2f mTextSize = { 0,0 };

			v2f mLastCursorPos = { 0,0 };

			usString mCurrentString;

			std::vector<CMSP> mInlineItems;
			std::vector<TextTag> mPreprocessedTags;


			std::vector<UnicodeRange> mRanges;

			WRAP_METHODS(ForceSetupText, ManageClickTouchEvent);
		};

	}
}