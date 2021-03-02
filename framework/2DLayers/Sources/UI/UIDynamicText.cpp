#include "UI/UIDynamicText.h"

#include "TecLibs/2D/BBox2D.h"
#include "TecLibs/Math/Algorithm.h"
#include "maReference.h"
#include "ModuleFileManager.h"
#include "LocalizationManager.h"

#include "TextureFileManager.h"

#include "Abstract2DLayer.h"
#include "RenderingScreen.h"
#include "ModuleInput.h"
#include "TouchInputEventManager.h"
#include "NotificationCenter.h"
#define STBTT_STATIC
#define STBRP_STATIC



#ifndef _MSC_VER
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb/stb_rect_pack.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

#ifndef _MSC_VER
#pragma clang diagnostic pop
#endif

#include "utf8.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIDynamicText, UIDynamicText, 2DLayers);
IMPLEMENT_CLASS_INFO(UIDynamicText)
//IMPLEMENT_AND_REGISTER_CLASS_INFO(FontMapManager, FontMapManager, 2DLayers);
IMPLEMENT_CLASS_INFO(FontMapManager)

//#define DYNAMIC_TEXT_DEBUG_DRAW

IMPLEMENT_CONSTRUCTOR(FontMapManager)
{
	KigsCore::GetNotificationCenter()->addObserver(this, "ReloadTextures", "ResetContext");
}

FontMap* FontMapManager::PrecacheFont(const kstl::string& fontname, float fontsize)
{
	auto& tfm = KigsCore::Singleton<TextureFileManager>();
	SmartPointer<CoreRawBuffer> crb = nullptr;

	char str[1024];
	snprintf(str, 1024, "%s:%.2f", fontname.c_str(), fontsize);

	const std::string str2 = str;

	auto font = &mFontMap[str2];
	if (tfm->HasTexture(str)) return font;

	font->font_id = str;
	u64 len = 0;
	font->mFontBuffer = OwningRawPtrToSmartPtr(ModuleFileManager::LoadFile(fontname.c_str(), len));

	if (font->mFontBuffer == nullptr)
	{
		mFontMap.erase(str2);
		return nullptr;
	}

	font->mFontTexture = tfm->GetTexture(font->font_id, false);
	font->mFontTexture->setValue("FileName", "");

	KigsCore::Connect(font->mFontTexture.get(), "Destroy", this, "OnFontTextureDestroy");

	

	KIGS_ASSERT(!font->mFontBuffer.isNil());

	stbtt_InitFont(&font->mFontInfo, (unsigned char*)font->mFontBuffer->buffer(), 0);

	UnicodeRange latin;
	latin.codepoint_start = 0x20;
	latin.num_chars = 0x24F - latin.codepoint_start;

	latin.mBakedChars = new stbtt_packedchar[latin.num_chars];
	memset(latin.mBakedChars, 0, sizeof(stbtt_packedchar)*latin.num_chars);

	UnicodeRange geometric_shapes;
	geometric_shapes.codepoint_start = 0x25A0;
	geometric_shapes.num_chars = 0x25FF - geometric_shapes.codepoint_start;
	geometric_shapes.mBakedChars = new stbtt_packedchar[geometric_shapes.num_chars];
	memset(geometric_shapes.mBakedChars, 0, sizeof(stbtt_packedchar)*geometric_shapes.num_chars);

	font->mRanges.push_back(latin);
	font->mRanges.push_back(geometric_shapes);
	
	font->font_size = fontsize;
	font->mFontMapSize = 256;
	int result = 0;
	u8* bitmap_alpha = nullptr;
	while (result == 0)
	{
		font->mFontMapSize *= 2;
		delete[] bitmap_alpha;
		bitmap_alpha = new u8[font->mFontMapSize * font->mFontMapSize];
		stbtt_pack_context ctx;
	
		stbtt_PackBegin(&ctx, bitmap_alpha, font->mFontMapSize, font->mFontMapSize, 0, 1, nullptr);
	
		for (auto& range : font->mRanges)
		{
			result = stbtt_PackFontRange(&ctx, (unsigned char*)font->mFontBuffer->buffer(), 0, fontsize,
				range.codepoint_start, range.num_chars, range.mBakedChars);
			if (result == 0)
				break;
		}
		
		stbtt_PackEnd(&ctx);
		//result = stbtt_BakeFontBitmap((unsigned char*)font->mFontBuffer->buffer(), 0, fontsize, bitmap_alpha, font->mFontMapSize, font->mFontMapSize, 0, nGlyphs, font->mBakedChars);
	}

	u8* bitmap_AI8 = new u8[font->mFontMapSize * font->mFontMapSize * 2];
	for (int i = 0; i < font->mFontMapSize * font->mFontMapSize; ++i)
	{
		bitmap_AI8[2 * i + 0] = 255;
		bitmap_AI8[2 * i + 1] = bitmap_alpha[i];
	}
	font->mFontTexture->Init();

	SmartPointer<TinyImage>	img = OwningRawPtrToSmartPtr(TinyImage::CreateImage(bitmap_AI8, font->mFontMapSize, font->mFontMapSize, TinyImage::ImageFormat::AI88));
	font->mFontTexture->CreateFromImage(img);

	delete[] bitmap_AI8;
	delete[] bitmap_alpha;

	return font; 
}



void FontMapManager::ReloadTextures()
{
	auto& tfm = KigsCore::Singleton<TextureFileManager>();

	kstl::vector<u8> bitmap_alpha;
	kstl::vector<u8> bitmap_AI8;
	for (auto& font : mFontMap)
	{
		bitmap_alpha.resize(font.second.mFontMapSize*font.second.mFontMapSize);
		bitmap_AI8.resize(font.second.mFontMapSize*font.second.mFontMapSize * 2);
		stbtt_pack_context ctx;
		stbtt_PackBegin(&ctx, bitmap_alpha.data(), font.second.mFontMapSize, font.second.mFontMapSize, 0, 1, nullptr);
		for (auto& range : font.second.mRanges)
		{
			stbtt_PackFontRange(&ctx, (unsigned char*)font.second.mFontBuffer->buffer(), 0, font.second.font_size,
				range.codepoint_start, range.num_chars, range.mBakedChars);
		}
		stbtt_PackEnd(&ctx);


		for (int i = 0; i < font.second.mFontMapSize * font.second.mFontMapSize; ++i)
		{
			*(bitmap_AI8.data() + (2 * i + 0)) = 255;
			*(bitmap_AI8.data() + (2 * i + 1)) = *(bitmap_alpha.data() + i);
		}
		
		SmartPointer<TinyImage>	img = OwningRawPtrToSmartPtr(TinyImage::CreateImage(bitmap_AI8.data(), font.second.mFontMapSize, font.second.mFontMapSize, TinyImage::ImageFormat::AI88));

		SP<Texture> tex = tfm->GetTexture(font.second.font_id, false);
		tex->CreateFromImage(img);
	}
}

void UIDynamicText::LoadFont()
{

	auto& tfm = KigsCore::Singleton<TextureFileManager>();
	auto& font_map_manager = KigsCore::Singleton<FontMapManager>();

	auto& theLocalizationManager = KigsCore::Singleton<LocalizationManager>();
	float LanguageScale = 1.0f;
	theLocalizationManager->getValue("LanguageScale", LanguageScale);

	mFontMap = font_map_manager->PrecacheFont(mFont, mFontSize* LanguageScale);

	if (mFontMap)
	{
		mTexturePointer->setTexture(tfm->GetTexture(mFontMap->font_id));
		// make sure mFontMap is destoyed when texture is destoyed
		mFontMap->mFontTexture = nullptr;
	}
	else
		mTexturePointer = nullptr;
}

static_assert(sizeof(VInfo2D::Data) == 20, "");

void UIDynamicText::InitModifiable()
{
	ParentClassType::InitModifiable();
	if (IsInit())
	{
		ModuleInput* theInputModule = KigsCore::GetModule<ModuleInput>();
		if(mPickable)
			if(theInputModule) mEventState = theInputModule->getTouchManager()->registerEvent(this, "ManageClickTouchEvent", Click, EmptyFlag);
		if (theInputModule)
			mPickable.changeNotificationLevel(Owner);
	}
}


bool UIDynamicText::ManageClickTouchEvent(ClickEvent& click_event)
{
	if (click_event.state == StateRecognized)
	{
		// Left click only
		if (click_event.button_state_mask & ClickEvent::LeftButton)
		{
			auto result = PickCharacter(click_event.position.xy);
			if (result.current_clickable_tag)
			{
				result.current_clickable_tag->item->SimpleCall(result.current_clickable_tag->clickID, this, result.current_clickable_tag->clickParam);
			}
			if (result.current_character_index != -1 && result.current_character_index != mSelectedCharacter && mPickable)
			{
				mSelectedCharacter = result.current_character_index;
				mTextChanged = true;
			}
		}
		return true;
	}
	return CanInteract(click_event.position.xy);
}

// func need the following signature bool func(unsigned short character, v2f position, stbtt_aligned_quad quad, int current_character_index, int current_line)
// return true to stop the iteration

float UIDynamicText::MeasureWord(const unsigned short* str)
{
	v2f position(0, 0);
	while (*str && *str != (unsigned short)' ' && *str != 13 && *str != 10)
	{
		if (*str < mFontMap->mFontInfo.numGlyphs)
		{
			stbtt_aligned_quad q;

			auto range = mFontMap->GetRangeForCodePoint(*str);
			if (range)
			{
				stbtt_GetPackedQuadScaled(range->mBakedChars, mFontMap->mFontMapSize, mFontMap->mFontMapSize, *str - range->codepoint_start, &position.x, &position.y, &q, mFontScaleFactor);
			}
			
			//stbtt_GetBakedQuadScaled(mFontMap->mBakedChars, mFontMap->mFontMapSize, mFontMap->mFontMapSize, *str, &position.x, &position.y, &q, mFontScaleFactor, 1);
		}
		++str;
	}
	return position.x;
}


#include "GLSLDebugDraw.h"

//@TODO replace with template when done maybe?
void UIDynamicText::IterateCharacters(std::function<bool(IterationState&)> func, bool is_draw) 
{
	auto current_character = mCurrentString.us_str();
	auto first_character = current_character;
	float max_width = std::max<float>(mMaxWidth, 0.0f);
	
	v2f fsize(0,0);
	auto parent = getFather();
	if (parent)
	{
		fsize = parent->GetSize();
	}
	else
	{
		auto layer = getLayerFather();
		if (layer)
		{
			layer->GetRenderingScreen()->GetDesignSize(fsize.x, fsize.y);
		}
	}
	switch ((int)mSizeModeX)
	{
	case 0: // Default
		if (mMaxWidth < 0)
			max_width = fsize.x;
		else
			max_width = mMaxWidth;
		break;
	case 1: // Multiply
		max_width = fsize.x*mMaxWidth;
		break;
	case 2: // Add
		max_width = fsize.x+mMaxWidth;
		break;
	}

	auto& theLocalizationManager = KigsCore::Singleton<LocalizationManager>();
	float LanguageScale = 1.0f;
	theLocalizationManager->getValue("LanguageScale", LanguageScale);

	float font_scale = mFontScaleFactor;
	float scale = stbtt_ScaleForPixelHeight(&mFontMap->mFontInfo, mFontSize* LanguageScale)*font_scale;
	
	int ascent_i;
	stbtt_GetFontVMetrics(&mFontMap->mFontInfo, &ascent_i, 0, 0);
	float ascent = mExtraLineSpacing/scale + ascent_i;
	v2f position{ 2, ascent*scale };
	int current_line = 0;
	int current_character_index = 0;
	int mapsize = mFontMap->mFontMapSize;
	std::vector<TextTag*> color_tag_stack; color_tag_stack.push_back(nullptr);
	std::vector<TextTag*> clickable_tag_stack; clickable_tag_stack.push_back(nullptr);

	v2f space_size(0, 0); stbtt_aligned_quad space_q;
	//stbtt_GetBakedQuadScaled(mFontMap->mBakedChars, mapsize, mapsize, (u16)' ', &space_size.x, &space_size.y, &space_q, font_scale, 1);
	
	auto range = mFontMap->GetRangeForCodePoint((u16)' ');
	KIGS_ASSERT(range);
	stbtt_GetPackedQuadScaled(range->mBakedChars, mapsize, mapsize, (u16)' ' - range->codepoint_start, &space_size.x, &space_size.y, &space_q, mFontScaleFactor);

	bool break_out = false;
	bool line_test_pass = false;
	auto line_start = current_character;
	int line_start_index = current_character_index;
	std::vector<TextTag*> line_start_tags;
	v2f line_position = position;
	bool new_line_character = false;
	int nb_spaces = 0;
	float extra_w_per_space = 0.0f;
	int test_spaces = 0;
	bool break_word = false;

	u32 current_tag_index = 0;
	TextTag* next_tag = mPreprocessedTags.size() ? &mPreprocessedTags[0] : nullptr;

	int line_start_tag_index = 0;
	TextTag* line_start_next_tag = nullptr;
	float line_y_extra_offset_above = 0.0f;
	float line_y_extra_offset_under = 0.0f;

	IterationState iteration_state;
	
	for (; (*current_character && !break_out) || line_test_pass; )
	{
		line_test_pass = !line_test_pass;

		if (line_test_pass)
		{
			line_start = current_character;
			line_start_index = current_character_index;

			line_start_tags = color_tag_stack;

			line_start_next_tag = next_tag;
			line_start_tag_index = current_tag_index;

			line_position = position;
			new_line_character = false;
			nb_spaces = 0;
			extra_w_per_space = 0.0f;
			test_spaces = 0;

			line_y_extra_offset_under = line_y_extra_offset_above = 0.0f;

			float first_word_width = MeasureWord(current_character);

			break_word = (max_width != 0.0f && position.x + first_word_width > max_width);
		}
		else
		{
			current_character = line_start;
			current_character_index = line_start_index;
			std::swap(color_tag_stack, line_start_tags);

			next_tag = line_start_next_tag;
			current_tag_index = line_start_tag_index;

			test_spaces = nb_spaces;
			nb_spaces = 0;
			if (max_width != 0.0f)
			{
				if (mTextAlign == UIDynamicText::TextAlign_Right)
				{
					float remaining = max_width - position.x;
					position = line_position;
					position.x += remaining;
				}
				else if (mTextAlign == UIDynamicText::TextAlign_Center)
				{
					float remaining = max_width - position.x;
					position = line_position;
					position.x += remaining / 2.0f;
				}
				else if (mTextAlign == UIDynamicText::TextAlign_Justify && !new_line_character)
				{
					float remaining = max_width - position.x;
					extra_w_per_space = remaining / test_spaces;
					position = line_position;
				}
				else
				{
					position = line_position;
				}
			}
			else
			{
				position = line_position;
			}

			position.y += line_y_extra_offset_above;
		}

		bool at_least_one_char = false;
		for (; true; ++current_character)
		{
			bool force_new_line = false;
			while (next_tag && first_character + next_tag->start_index == current_character)
			{
				if (next_tag->type == TextTag::Type::ColorStart)
					color_tag_stack.push_back(next_tag);
				else if (next_tag->type == TextTag::Type::ColorEnd)
				{
					color_tag_stack.pop_back();
					KIGS_ASSERT(color_tag_stack.size() != 0);
				}	
				else if (next_tag->type == TextTag::Type::ClickableStart)
					clickable_tag_stack.push_back(next_tag);
				else if (next_tag->type == TextTag::Type::ClickableEnd)
				{
					clickable_tag_stack.pop_back();
					KIGS_ASSERT(clickable_tag_stack.size() != 0);
				}
					
				else if (next_tag->type == TextTag::Type::ExternalItem || next_tag->type == TextTag::Type::InlineItem)
				{
					v2f itemsize = ((Node2D*)next_tag->item)->GetSize();
					v2f pos = position;

					if (max_width != 0.0f && itemsize.x + position.x > max_width && at_least_one_char)
					{
						force_new_line = true;
						break;
					}

					v2f itemanchor = next_tag->item->getValue<v2f>("Anchor");

					float offset_y = 0.0f; 

					switch (next_tag->align)
					{
						// bottom is on baseline
					case TextTag::ItemAlign::BottomOnBaseline: offset_y += itemsize.y * (1.0f-itemanchor.y) ; break;
						// anchor is on baseline
					case TextTag::ItemAlign::CenteredAroundBaseLine: offset_y += 0.0f; break;
						// anchor is on halfchar
					case TextTag::ItemAlign::CenteredAroundHalfChar: offset_y += ascent*scale*0.5f; break;
					}

					float offsetUp = offset_y + itemsize.y * itemanchor.y;
					float offsetDown = offset_y - itemsize.y * (1.0f-itemanchor.y);

					line_y_extra_offset_above = std::max(line_y_extra_offset_above, std::max(offsetUp - ascent*scale, 2.0f));
					line_y_extra_offset_under = std::max(line_y_extra_offset_under, std::max(- offsetDown, 2.0f));
					pos.y -= offset_y;
					pos.x += itemsize.x * itemanchor.x;
					if (is_draw)
						next_tag->item->setArrayValue("Position", &pos.x, 2);

					position.x += itemsize.x;
				}
				++current_tag_index;
				if (current_tag_index < mPreprocessedTags.size())
					next_tag++;
				else
					next_tag = nullptr;
			}
			if (force_new_line)
				break;

			u16 utf16_value = *current_character;

			if (utf16_value == 0)
			{
				break;
			}
			if (utf16_value == (u16)'\r') continue; // Ignore

			bool break_line_after_space = false;
			if (utf16_value == (u16)'\t')
			{
				nb_spaces+=4;
				if (line_test_pass)
				{
					float next_word_width = MeasureWord(current_character + 1);
					if (max_width != 0.0f && position.x + next_word_width + space_size.x*4 > max_width)
					{
						break_line_after_space = true;
					}
				}
				else
				{
					if (nb_spaces > test_spaces)
					{
						break_line_after_space = true;
					}
				}
			}
			if (utf16_value == (u16)' ')
			{
				nb_spaces++;
				if (line_test_pass)
				{
					float next_word_width = MeasureWord(current_character + 1);
					if (max_width != 0.0f && position.x + next_word_width + space_size.x > max_width)
					{
						break_line_after_space = true;
					}
				}
				else
				{
					if (nb_spaces > test_spaces)
					{
						break_line_after_space = true;
					}
				}

			}
			if (utf16_value == (u16)'\n')
			{
				++current_character;
				new_line_character = true;
				break;
			}
			int glyph_index = utf16_value; // stbtt_FindGlyphIndex(&mFontMap->mFontInfo, utf16_value);

			auto range = mFontMap->GetRangeForCodePoint(glyph_index);

			if (range || *current_character == (u16)'\t')//mFontMap->mFontInfo.numGlyphs)
			{
				stbtt_aligned_quad q;
				float pos_x_before = position.x;

				if (*current_character == (u16)'\t')
				{
					range = mFontMap->GetRangeForCodePoint((u16)' ');

					for (int tab = 0; tab < 4; ++tab)
					{
						//stbtt_GetBakedQuadScaled(mFontMap->mBakedChars, mapsize, mapsize, ' ', &position.x, &position.y, &q, font_scale, 1);
						stbtt_GetPackedQuadScaled(range->mBakedChars, mapsize, mapsize, (u16)' ' - range->codepoint_start, &position.x, &position.y, &q, font_scale);
					}
						
					position.x += extra_w_per_space * 4;
				}
				else
				{
					//stbtt_GetBakedQuadScaled(mFontMap->mBakedChars, mapsize, mapsize, glyph_index, &position.x, &position.y, &q, font_scale, 1);
					stbtt_GetPackedQuadScaled(range->mBakedChars, mapsize, mapsize, glyph_index - range->codepoint_start, &position.x, &position.y, &q, font_scale);
				}
					

				if (*current_character == (u16)' ')
					position.x += extra_w_per_space;

				if (break_word && position.x > max_width && at_least_one_char)
				{
					position.x = pos_x_before;
					break;
				}

				at_least_one_char = true;

				iteration_state.character = *current_character;
				iteration_state.current_character_index = current_character_index;
				iteration_state.pos = position;
				iteration_state.quad = q;
				iteration_state.current_line = current_line;
				iteration_state.current_color_tag = color_tag_stack.back();
				iteration_state.current_clickable_tag = clickable_tag_stack.back();
				iteration_state.char_width = position.x - pos_x_before;
				iteration_state.line_height = ascent*scale + line_y_extra_offset_under;

				if (!line_test_pass && func(iteration_state))
				{
					break_out = true;
					break;
				}
				current_character_index++;

				if (break_line_after_space)
				{
					++current_character;
					--nb_spaces;
					break;
				}
			}
		}

		if (!line_test_pass)
		{	
#ifdef DYNAMIC_TEXT_DEBUG_DRAW
			if (is_draw)
			{
				v2f pts[2] =
				{
					position, v2f{ 2, position.y }
				};
				TransformPoints(pts, 2);
				dd::line2D(pts[0], pts[1], v3f{ 0,0,255 });
			}
#endif
			position.x = 2;
			position.y += ascent*scale + line_y_extra_offset_under;
			++current_line;

			if (mMaxLines != 0 && current_line >= mMaxLines)
				break;
		}
	}



}

UIDynamicText::IterationState UIDynamicText::PickCharacter(v2f pos)
{
	Point2D pt[4];
	GetTransformedPoints(pt);

	float sx = ProjectOnLineScalar(pos, pt[0], pt[3]);
	float sy = ProjectOnLineScalar(pos, pt[0], pt[1]);

	auto size = GetSize();
	sx *= size.x;
	sy *= size.y;

	IterationState result = {}; result.current_character_index = -1;
	IterationState last_state = {};
	IterateCharacters([&](IterationState& state)
	{		
		last_state = state;
		if (state.pos.x - state.char_width/2 > sx && state.pos.y > sy)
		{
			result = state;
			return true;
		}
		return false;
	});

	if (result.current_character_index == -1)
	{
		//if (last_state.pos.x > sx && last_state.pos.y > sy)
			result.current_character_index = last_state.current_character_index + 1;
	}
	return result;
}

void UIDynamicText::BuildVertexArray()
{
	if (!mFontMap) return;

	auto& theLocalizationManager = KigsCore::Singleton<LocalizationManager>();
	float LanguageScale = 1.0f;
	theLocalizationManager->getValue("LanguageScale", LanguageScale);

	float scale = stbtt_ScaleForPixelHeight(&mFontMap->mFontInfo, mFontSize* LanguageScale);
	int ascent;
	stbtt_GetFontVMetrics(&mFontMap->mFontInfo, &ascent, 0, 0);
	v2f position{ 0, ascent*scale };

	BBox2D size(v2f{ mResizeToMaxWidth ? std::max(0.0f, (float)mMaxWidth) : 0.0f, ascent*scale });

	u32 nb_chars = mCurrentString.length();

	auto vi = GetVerticesInfo();

	vi->Resize((nb_chars+1) * 6);
	memset(vi->Buffer(), 0, vi->Offset*vi->vertexCount);
	
	vi->SetFlag(UIVerticesInfo_Vertex | UIVerticesInfo_Color | UIVerticesInfo_Texture | UIVerticesInfo_UseModelMatrix);
	u32 max_index = 0;

	
	v2f cursor_pos;
	bool draw_cursor = false;

	v2f last_cursor_pos;

	IterateCharacters([&](IterationState& state)
	{
		size.Update(v2f{ state.quad.x1, state.quad.y1 });
		
		auto data = reinterpret_cast<VInfo2D::Data*>(vi->Buffer());
		auto offset = 6 * state.current_character_index;
		data[offset + 0].setVertex(state.quad.x0, state.quad.y0);
		data[offset + 1].setVertex(state.quad.x1, state.quad.y0);
		data[offset + 2].setVertex(state.quad.x1, state.quad.y1);

		data[offset + 3].setVertex(state.quad.x0, state.quad.y0);
		data[offset + 4].setVertex(state.quad.x1, state.quad.y1);
		data[offset + 5].setVertex(state.quad.x0, state.quad.y1);

		data[offset + 0].setTexUV(state.quad.s0, state.quad.t0);
		data[offset + 1].setTexUV(state.quad.s1, state.quad.t0);
		data[offset + 2].setTexUV(state.quad.s1, state.quad.t1);

		data[offset + 3].setTexUV(state.quad.s0, state.quad.t0);
		data[offset + 4].setTexUV(state.quad.s1, state.quad.t1);
		data[offset + 5].setTexUV(state.quad.s0, state.quad.t1);
		
#ifdef DYNAMIC_TEXT_DEBUG_DRAW
		v2f pts[4];
		pts[0] = { state.quad.x0, state.quad.y0 };
		pts[1] = { state.quad.x0, state.quad.y1 };
		pts[2] = { state.quad.x1, state.quad.y1 };
		pts[3] = { state.quad.x1, state.quad.y0 };
		
		TransformPoints(pts, 4);

		dd::line2D(pts[0], pts[1], v3f{ 0,0,255 });
		dd::line2D(pts[1], pts[2], v3f{ 0,0,255 });
		dd::line2D(pts[2], pts[3], v3f{ 0,0,255 });
		dd::line2D(pts[3], pts[0], v3f{ 0,0,255 });
#endif

		if (state.current_color_tag && !mIgnoreColorTags)
		{
			data[offset + 0].setColorF(state.current_color_tag->color.r, state.current_color_tag->color.g, state.current_color_tag->color.b, state.current_color_tag->color.a);
			data[offset + 1].setColorF(state.current_color_tag->color.r, state.current_color_tag->color.g, state.current_color_tag->color.b, state.current_color_tag->color.a);
			data[offset + 2].setColorF(state.current_color_tag->color.r, state.current_color_tag->color.g, state.current_color_tag->color.b, state.current_color_tag->color.a);
			data[offset + 3].setColorF(state.current_color_tag->color.r, state.current_color_tag->color.g, state.current_color_tag->color.b, state.current_color_tag->color.a);
			data[offset + 4].setColorF(state.current_color_tag->color.r, state.current_color_tag->color.g, state.current_color_tag->color.b, state.current_color_tag->color.a);
			data[offset + 5].setColorF(state.current_color_tag->color.r, state.current_color_tag->color.g, state.current_color_tag->color.b, state.current_color_tag->color.a);
		}
		else
		{
			float opacity = GetOpacity();
			data[offset + 0].setColorF(mColor[0], mColor[1], mColor[2], opacity);
			data[offset + 1].setColorF(mColor[0], mColor[1], mColor[2], opacity);
			data[offset + 2].setColorF(mColor[0], mColor[1], mColor[2], opacity);
			data[offset + 3].setColorF(mColor[0], mColor[1], mColor[2], opacity);
			data[offset + 4].setColorF(mColor[0], mColor[1], mColor[2], opacity);
			data[offset + 5].setColorF(mColor[0], mColor[1], mColor[2], opacity);
		}

		last_cursor_pos = state.pos;
		
		if (state.current_character_index == mSelectedCharacter)
		{
			//float w = state.quad.x1 - state.quad.x0;
			cursor_pos = state.pos;
			cursor_pos.x -= state.char_width;

			mLastCursorPos = cursor_pos;

			draw_cursor = true;
		}

		max_index = state.current_character_index + 1;
		return false;
	}, true);
	

	if (mSelectedCharacter == max_index)
	{
		draw_cursor = true;
		cursor_pos = last_cursor_pos;
		mLastCursorPos = last_cursor_pos;
	}

	mLastMaxIndex = max_index;

	if (mShowCursor && draw_cursor)
	{
		auto data = reinterpret_cast<VInfo2D::Data*>(vi->Buffer());
		u32 offset = 6 * max_index;

		stbtt_aligned_quad space_q;
		v2f pos{ 0,0 };
		//stbtt_GetBakedQuadScaled(mFontMap->mBakedChars, mFontMap->mFontMapSize, mFontMap->mFontMapSize, (u16)'|', &pos.x, &pos.y, &space_q, mFontScaleFactor, 1);

		auto range = mFontMap->GetRangeForCodePoint((u16)'|');
		if (range)
		{
			stbtt_GetPackedQuadScaled(range->mBakedChars, mFontMap->mFontMapSize, mFontMap->mFontMapSize, (u16)'|' - range->codepoint_start, &pos.x, &pos.y, &space_q, mFontScaleFactor);
	
			float h = space_q.y1 - space_q.y0;
			pos.y = h;
			data[offset + 0].setVertex(cursor_pos + v2f{ 0, 0});
			data[offset + 1].setVertex(cursor_pos + v2f{ pos.x*0.2f, 0});
			data[offset + 2].setVertex(cursor_pos + v2f{ pos.x*0.2f, -pos.y});

			data[offset + 3].setVertex(cursor_pos + v2f{ 0, 0});
			data[offset + 4].setVertex(cursor_pos + v2f{ pos.x*0.2f, -pos.y });
			data[offset + 5].setVertex(cursor_pos + v2f{ 0, -pos.y });

		
			data[offset + 0].setTexUV(space_q.s0, space_q.t0);
			data[offset + 1].setTexUV(space_q.s1, space_q.t0);
			data[offset + 2].setTexUV(space_q.s1, space_q.t1);

			data[offset + 3].setTexUV(space_q.s0, space_q.t0);
			data[offset + 4].setTexUV(space_q.s1, space_q.t1);
			data[offset + 5].setTexUV(space_q.s0, space_q.t1);

			float opacity = GetOpacity();

			data[offset + 0].setColorF(mColor[0], mColor[1], mColor[2], opacity);
			data[offset + 1].setColorF(mColor[0], mColor[1], mColor[2], opacity);
			data[offset + 2].setColorF(mColor[0], mColor[1], mColor[2], opacity);
			data[offset + 3].setColorF(mColor[0], mColor[1], mColor[2], opacity);
			data[offset + 4].setColorF(mColor[0], mColor[1], mColor[2], opacity);
			data[offset + 5].setColorF(mColor[0], mColor[1], mColor[2], opacity);

			max_index++;
		}
	}

	vi->Resize(max_index * 6);

	if (mSizeModeX == 0)
	{
		setValue("SizeX", size.m_Max.x);
		mRealSize.x = size.m_Max.x;
	}
	if (mSizeModeY == 0)
	{
		setValue("SizeY", size.m_Max.y);
		mRealSize.y = size.m_Max.y;
	}
}

v4f HexaToV4(const kstl::string& hexa)
{
	v4f result(0, 0, 0, 0);
	for (int i = 0; i < 8; ++i)
	{
		int value = 0;
		switch (hexa[i])
		{
		case '0': value = 0; break;
		case '1': value = 1; break;
		case '2': value = 2; break;
		case '3': value = 3; break;
		case '4': value = 4; break;
		case '5': value = 5; break;
		case '6': value = 6; break;
		case '7': value = 7; break;
		case '8': value = 8; break;
		case '9': value = 9; break;
		case 'A': value = 10; break;
		case 'B': value = 11; break;
		case 'C': value = 12; break;
		case 'D': value = 13; break;
		case 'E': value = 14; break;
		case 'F': value = 15; break;
		}
		if (i % 2 == 0) value *= 16;
		result.data[i / 2] += value;
	}
	result /= 255;
	return result;
}

void UIDynamicText::PreprocessTags()
{
	for (auto item : mInlineItems)
	{
		CMSP toDel(item, StealRefTag{});
		removeItem(toDel);
	}
	mPreprocessedTags.clear();
	mInlineItems.clear();

	if (mIgnoreTags)
	{
		mCurrentString = mText;
		return;
	}
	mCurrentString = TextTagProcessor(mText.const_ref(), &mPreprocessedTags, &mInlineItems, this);
}

void UIDynamicText::SetUpNodeIfNeeded()
{	
	if(GetNodeFlag(Node2D_NeedUpdatePosition) && GetNodeFlag(Node2D_SizeChanged))
		mChanged = true;
	ForceSetupText();
	ParentClassType::SetUpNodeIfNeeded();
}

void UIDynamicText::NotifyUpdate(const unsigned int labelID)
{
	if (labelID == mText.getID() || labelID == mIgnoreTags.getID())
	{
		mTextChanged = true;
	}
	else if (labelID == mSizeX.getID()
		|| labelID == mSizeY.getID()
		|| labelID == mTextAlign.getID()
		|| labelID == mMaxWidth.getID()
		|| labelID == mFontScaleFactor.getID()
		|| labelID == mSizeModeX.getID()
		|| labelID == mSizeModeY.getID()
		|| labelID == mSelectedCharacter.getID()
		|| labelID == mShowCursor.getID()
		|| labelID == mIgnoreColorTags.getID()
		|| labelID == mExtraLineSpacing.getID()
		|| labelID == mOpacity.getID()
		|| labelID == mColor.getID()
		)
	{
		mChanged = true;
	}
	else if (labelID == mFont.getID() || labelID == mFontSize.getID())
	{
		mFontChanged = true;
	}
	else if (labelID == mPickable.getID())
	{
		ModuleInput* theInputModule = KigsCore::GetModule<ModuleInput>();
		if (mPickable)
			mEventState = theInputModule->getTouchManager()->registerEvent(this, "ManageClickTouchEvent", Click, EmptyFlag);
		else
			theInputModule->getTouchManager()->unregisterEvent(this, Click);
	}
	UITexturedItem::NotifyUpdate(labelID);
}

void UIDynamicText::ForceSetupText()
{
	if (mFontChanged)
	{
		LoadFont();
		mFontChanged = false;
		mChanged = true;
	}
	if (mTextChanged)
	{
		PreprocessTags();
		mTextChanged = false;
		mChanged = true;
	}
	if (mChanged)
	{
		BuildVertexArray();
		SetNodeFlag(Node2D_NeedUpdatePosition);
		mChanged = false;
		EmitSignal(Signals::TextRebuilt, this);
	}
	
	// Call super class method to avoid a loop
	ParentClassType::SetUpNodeIfNeeded();
}


usString TextTagProcessor(const usString& text, kstl::vector<TextTag>* output_tags, kstl::vector<CoreModifiable*>* inline_items, CoreModifiable* obj)
{
	auto current_character = text.us_str();
	if (text.length() > 0 && text[0] == (unsigned short)'#')
	{
		kstl::string reftext = text.ToString();
		kstl::string key = reftext.substr(1, reftext.length() - 1);
		auto& theLocalizationManager = KigsCore::Singleton<LocalizationManager>();
		current_character = (PLATFORM_WCHAR*)theLocalizationManager->getLocalizedString(key.c_str());

		if (current_character == nullptr)
			return usString("");
	}
	usString result = current_character;

	usString copystr = result;
	usString parsestr = result;

	US16ParserUtils parser((u16*)parsestr.us_str(), parsestr.length());

	US16ParserUtils block(parser);
	TextTag current_tag;

	int removed_chars = 0;
	int item_start = 0;
	TextTag::ItemAlign current_align_mode = TextTag::ItemAlign::CenteredAroundHalfChar;

	int start_count = 0;

	while (parser.GetBlock(block, (u16)'<', (u16)'>'))
	{
		usString blockname = block;
		auto end = parser.GetPosition();
		auto start = end - block.length() - 2;
		bool remove_block = false;
		if (blockname == "color")
		{
			current_tag.type = TextTag::Type::ColorStart;
			current_tag.start_index = start - removed_chars;
			US16ParserUtils colorcode(block);
			parser.GetChars(colorcode, 8);

			auto hexacolor = (kstl::string)colorcode;
			current_tag.color = HexaToV4(hexacolor);

			remove_block = true;
		}
		else if (blockname == "/color")
		{
			if (output_tags)
				output_tags->push_back(current_tag);
			start -= 8;
			remove_block = true;
		}
		else if (blockname == "link")
		{
			current_tag.type = TextTag::Type::ClickableStart;
			item_start = end;
			current_tag.start_index = start - removed_chars;
			remove_block = true;
			start_count++;
		}
		else if (blockname == "/link")
		{
			US16ParserUtils reference_parser(parser);
			int oldpos = parser.GetPosition();
			parser.GetPart(reference_parser, item_start, start - item_start);

			usString item_ref = reference_parser;
			auto vec = item_ref.SplitByCharacter('@');
			if (vec.size() >= 2)
			{
				maReference ref{ "", vec[0].ToString() };
				CoreModifiable* cm = ref;
				KIGS_ASSERT(cm != nullptr);
				if (cm)
				{
					current_tag.item = cm;
					current_tag.align = current_align_mode;
					current_tag.clickID = vec[1].ToString();
					if (vec.size() == 3)
						current_tag.clickParam = vec[2];
					if (output_tags)
						output_tags->push_back(current_tag);
				}
			}
			parser.SetPosition(oldpos);
			remove_block = true;
			start = item_start;
			item_start = end;
		}
		else if (blockname == "link/")
		{
			current_tag.type = TextTag::Type::ClickableEnd;
			current_tag.start_index = start - removed_chars;
			
			KIGS_ASSERT(start_count != 0);
			start_count--;

			if (output_tags)
				output_tags->push_back(current_tag);

			remove_block = true;
		}


		else if (blockname == "marker")
		{
			current_tag.type = TextTag::Type::Marker;
			item_start = end;
			current_tag.start_index = start - removed_chars;
			remove_block = true;
			start_count++;
		}
		else if (blockname == "/marker")
		{
			US16ParserUtils reference_parser(parser);
			int oldpos = parser.GetPosition();
			parser.GetPart(reference_parser, item_start, start - item_start);

			usString item_ref = reference_parser;
			auto vec = item_ref.SplitByCharacter('@');
			if (vec.size() >= 2)
			{
				maReference ref{ "", vec[0].ToString() };
				CoreModifiable* cm = ref;
				KIGS_ASSERT(cm != nullptr);
				if (cm)
				{
					current_tag.item = cm;
					current_tag.align = current_align_mode;
					current_tag.clickID = vec[1].ToString();
					if (vec.size() == 3)
						current_tag.clickParam = vec[2];
					if (output_tags)
						output_tags->push_back(current_tag);
				}
			}
			parser.SetPosition(oldpos);
			remove_block = true;
			start = item_start;
			item_start = end;
		}
		else if (blockname == "color/")
		{
			current_tag.type = TextTag::Type::ColorEnd;
			current_tag.start_index = start - removed_chars;
			if (output_tags)
				output_tags->push_back(current_tag);
			remove_block = true;
		}
		else if (blockname == "item")
		{
			current_tag.type = TextTag::Type::ExternalItem;
			item_start = end;
			current_tag.start_index = start - removed_chars;
			remove_block = true;
		}
		else if (blockname == "/item")
		{
			US16ParserUtils reference_parser(parser);
			int oldpos = parser.GetPosition();
			parser.GetPart(reference_parser, item_start, start - item_start);

			kstl::string item_ref = reference_parser;

			maReference ref{ "", item_ref };
			CoreModifiable* cm = ref;
			if (obj && cm && cm->isSubType("UIItem") && cm->getFirstParent("UIItem") == obj)
			{
				current_tag.item = static_cast<UIItem*>(cm);
				current_tag.align = current_align_mode;
				cm->setValue("Dock", "{0,0}");
				cm->setValue("Position", "{0,0}");
				if (output_tags)
					output_tags->push_back(current_tag);
			}
			parser.SetPosition(oldpos);
			remove_block = true;
			start = item_start;
		}
		else if (blockname == "image")
		{
			current_tag.type = TextTag::Type::InlineItem;
			item_start = end;
			current_tag.start_index = start - removed_chars;
			remove_block = true;
		}
		else if (blockname == "/image")
		{
			US16ParserUtils reference_parser(parser);
			int oldpos = parser.GetPosition();
			parser.GetPart(reference_parser, item_start, start - item_start);
			kstl::string image_path = reference_parser;

			if (inline_items)
			{
				CMSP cm = nullptr;
				if (image_path == "BLABLA")
				{
					cm = KigsCore::GetInstanceOf("inlineitem", "UIRenderingScreen");
					cm->setValue("RenderingScreen", "RenderingScreen:offscreen");
					cm->setValue("SizeX", 320);
					cm->setValue("SizeY", 240);
					cm->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::STRING, "Texture", image_path.c_str());
				}
				else
				{
					cm = KigsCore::GetInstanceOf("inlineitem", "UIImage");
					cm->setValue("TextureName", image_path);
				}
				
				if(obj)
					cm->setValue("Priority", obj->getValue<int>("Priority"));
				cm->setValue("IsTouchable", false);

				if(obj)
					obj->addItem(cm);

				cm->Init();
				
				current_tag.item = cm.get();
				inline_items->push_back(cm.get());
			}

			current_tag.align = current_align_mode;

			if (output_tags)
				output_tags->push_back(current_tag);

			parser.SetPosition(oldpos);

			remove_block = true;
			start = item_start;
		}
		else if (blockname.substr(0, 6) == "align ")
		{
			kstl::string aligntype = (kstl::string)blockname.substr(6, blockname.length() - 6);

			if (aligntype == "baseline/")
			{
				current_align_mode = TextTag::ItemAlign::BottomOnBaseline;
			}
			else if (aligntype == "center baseline/")
			{
				current_align_mode = TextTag::ItemAlign::CenteredAroundBaseLine;
			}
			else if (aligntype == "center halfchar/")
			{
				current_align_mode = TextTag::ItemAlign::CenteredAroundHalfChar;
			}

			remove_block = true;
		}

		if (remove_block)
		{
			memcpy((void*)(result.us_str() + start - removed_chars), copystr.us_str() + end, (usString::strlen(copystr.us_str() + end) + 1) * sizeof(u16));
			removed_chars += (end - start);
		}
	}
	
	return result;
}


void UIDynamicText::ProtectedDraw(TravState* state)
{
#ifdef DYNAMIC_TEXT_DEBUG_DRAW
	mChanged = true;
	ForceSetupText();
#endif
	UITexturedItem::ProtectedDraw(state);
}