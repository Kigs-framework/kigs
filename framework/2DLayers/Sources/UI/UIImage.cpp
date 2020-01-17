#include "UI/UIImage.h"

#include "AlphaMask.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIImage, UIImage, 2DLayers);
IMPLEMENT_CLASS_INFO(UIImage)

UIImage::UIImage(const kstl::string& name, CLASS_NAME_TREE_ARG) :
	UITexturedItem(name, PASS_CLASS_NAME_TREE_ARG)
	//, myAutoresizeTexture(*this, false, LABEL_AND_ID(AutoResize), false)
	, myTextureName(*this, false, LABEL_AND_ID(Texture), "")
{
	myCurrentTextureName = "";
}

void UIImage::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == myIsEnabled.getLabelID())
	{
		if (!GetSons().empty())
		{
			kstl::set<Node2D*, Node2D::PriorityCompare> sons = GetSons();
			kstl::set<Node2D*, Node2D::PriorityCompare>::iterator it = sons.begin();
			kstl::set<Node2D*, Node2D::PriorityCompare>::iterator end = sons.end();
			while (it != end)
			{
				(*it)->setValue("IsEnabled", myIsEnabled);
				it++;
			}
		}
	}
	else if (labelid == myTextureName.getLabelID() && IsInit())
	{
		ChangeTexture();
	}
	UITexturedItem::NotifyUpdate(labelid);
}

void UIImage::InitModifiable()
{
	UITexturedItem::InitModifiable();
	if (IsInit())
	{
		// load texture
		ChangeTexture();

		// auto size
		if ((int)myDisplayMode == AUTO)
		{
			if ((mySizeX.const_ref() == 0) && (mySizeY.const_ref() == 0))
				myDisplayMode.setValue(RESIZE);
			else
				myDisplayMode.setValue(SCALE);
		}

		myIsEnabled.changeNotificationLevel(Owner);
		myTextureName.changeNotificationLevel(Owner);


		ComputeRealSize();
	}
}

void UIImage::ComputeRealSize()
{
	switch ((int)myDisplayMode)
	{
	case RESIZE:
		if (!myTexture.isNil()) myTexture->GetSize(myRealSize.x, myRealSize.y);
		break;
	case SCALE:
		UITexturedItem::ComputeRealSize();
		if (myKeepRatio && !myTexture.isNil())
		{
			unsigned int tsX, tsY;
			myTexture->GetSize(tsX, tsY);

			float ratioTex = (float)tsX / (float)tsY;
			float ratioUI = myRealSize.x / myRealSize.y;

			int dirTex = (ratioTex == 1) ? 0 : (ratioTex < 1) ? -1 : 1;
			int dirUI = (ratioUI == 1) ? 0 : (ratioUI < 1) ? -1 : 1;

			if (dirUI == 0 && dirTex == 0) // both square
			{
				// keep realSize as it is
			}
			else if (dirUI == 0) // scale Tex to fit in a square
			{
				if (dirTex < 0) // x smaller than y
				{
					myRealSize.x *= ratioTex;
				}
				else // y smaller than x
				{
					myRealSize.y /= ratioTex;
				}
			}
			else if (dirTex == 0) // scale Tex to fit in a rectangle
			{
				if (dirUI < 0) // x smaller than y
				{
					myRealSize.y = myRealSize.x;
				}
				else // y smaller than x
				{
					myRealSize.x = myRealSize.y;
				}
			}
			else if (dirTex*dirUI < 0) //two rectangle with different dir
			{
				if (dirTex < 0) // x smaller than y
				{
					myRealSize.x = myRealSize.y*ratioTex;
				}
				else // y smaller than x
				{
					myRealSize.y = myRealSize.x/ratioTex;
				}
			}
			else // two rectangle in the same direction
			{
				if (dirTex < 0)
				{
					if(myRealSize.y*ratioTex<myRealSize.x)
						myRealSize.x = myRealSize.y*ratioTex;
					else
						myRealSize.y = myRealSize.x / ratioTex;
				}
				else
				{
					if (myRealSize.x/ratioTex<myRealSize.y)
						myRealSize.y = myRealSize.x / ratioTex;
					else
						myRealSize.x = myRealSize.y*ratioTex;
				}
			}

		}
		break;
	default:
		if (!myTexture.isNil())myTexture->GetSize(myRealSize.x, myRealSize.y);
		kigsprintf("Display Mode not supported, use resize\n");
		break;
	}
}

void UIImage::ChangeTexture()
{
	// no change
	if (myCurrentTextureName == myTextureName.const_ref())
		return;

	myTextureName.getValue(myCurrentTextureName);

	// empty texture name >> destroy texture
	if (myCurrentTextureName == "")
	{
		myTexture = nullptr;
		return;
	}

	auto arr = SplitStringByCharacter(myCurrentTextureName, ':');
	if (arr.size() > 1) // use spritesheet
	{
		auto& textureManager = KigsCore::Singleton<TextureFileManager>();
		mySpriteSheetTexture = textureManager->GetSpriteSheetTexture(arr[0]);
		SetTexture(mySpriteSheetTexture->Get_Texture());
		if (myTexture)
		{
			const SpriteSheetFrame* frame = mySpriteSheetTexture->Get_Frame(arr[1]);
			if (frame)
			{
				myAutoresizeValue.x = (float)frame->FrameSize_X;
				myAutoresizeValue.y = (float)frame->FrameSize_Y;


				Point2D s, r;
				myTexture->GetSize(s.x, s.y);
				myTexture->GetRatio(r.x, r.y);
				s /= r;

				mUVMin.Set((kfloat)frame->FramePos_X + 0.5f, (kfloat)frame->FramePos_Y + 0.5f);
				mUVMin /= s;

				mUVMax.Set((kfloat)(frame->FramePos_X + frame->FrameSize_X - 0.5f), (kfloat)(frame->FramePos_Y + frame->FrameSize_Y - 0.5f));
				mUVMax /= s;
			}
			else // frame not found
			{
				myTexture->GetSize(myAutoresizeValue.x, myAutoresizeValue.y);
				mUVMin.Set(FLT_MAX, FLT_MAX);
				mUVMax.Set(FLT_MAX, FLT_MAX);
			}
		}
	}
	else
	{
		auto& textureManager = KigsCore::Singleton<TextureFileManager>();
		SetTexture(textureManager->GetTexture(myCurrentTextureName).get());
		myTexture->GetSize(myAutoresizeValue.x, myAutoresizeValue.y);
		mUVMin.Set(FLT_MAX, FLT_MAX);
		mUVMax.Set(FLT_MAX, FLT_MAX);
	}

	if (!myTexture)
		return;

	myTexture->Init();
}

bool UIImage::isAlpha(float X, float Y)
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
		return !myAlphaMask->CheckTo(X, Y);
	}

	return false;
}
