#pragma once
#include "CoreModifiable.h"
#include "../GlobalEnum.h"
#include "../Node2D.h"

#include "AttributePacking.h"
#include "TouchInputEventManager.h"
#include "../AlphaMask.h"

namespace Kigs
{
	namespace Draw
	{
		class Texture;
	}
	namespace Scene
	{
		class TravState;
	}
	namespace Draw2D
	{

		class UIVerticesInfo;
		class BaseUI2DLayer;

		// ****************************************
		// * UIItem class
		// * --------------------------------------
		/**
		* \file	UIItem.h
		* \class	UIItem
		* \ingroup 2DLayers
		* \brief	Parent class for all UI types ( buttons, texts...)
		*
		*/
		// ****************************************

		class UIItem : public Node2D
		{
		public:
			DECLARE_CLASS_INFO(UIItem, Node2D, 2DLayers);
			DECLARE_CONSTRUCTOR(UIItem);

			static constexpr unsigned int UIItem_HasFocus = 1 << ParentClassType::usedUserFlags;
			static constexpr unsigned int usedUserFlags = ParentClassType::usedUserFlags + 1;


			/*Getter*/
			float												GetOpacity() override {
				if (mOpacity >= 0.0f) return mOpacity;
				if (mParent) return mParent->GetOpacity(); // if opacity is < 0.0 then check parent opacity
				return 0.0;
			}
			inline bool											Get_DisableBlend() const { return mDisableBlend; }
			inline bool                                         Get_IsHidden() const { return mIsHidden; }

			/*Setter*/
			inline void											Set_RotationAngle(float a_radAngle) { mRotationAngle = a_radAngle; SetNodeFlag(Node2D_NeedUpdatePosition); }
			inline void											Set_Position(float a_x, float a_y) { mPosition[0] = a_x; mPosition[1] = a_y; SetNodeFlag(Node2D_NeedUpdatePosition); }
			inline void											Set_Position(Point2D a_pos) { Set_Position(a_pos.x, a_pos.y); }
			inline void											IsTouchable(bool a_value) { mIsTouchable = a_value; }
			inline void											Set_Opacity(float a_value) { mOpacity = a_value; SetNodeFlag(Node2D_NeedVerticeInfoUpdate); }
			inline void											Set_PreScale(float a_valueX, float a_valueY) { mPreScale = v2f(a_valueX, a_valueY); SetNodeFlag(Node2D_NeedUpdatePosition); }
			inline void											Set_PostScale(float a_valueX, float a_valueY) { mPostScale = v2f(a_valueX, a_valueY); SetNodeFlag(Node2D_NeedUpdatePosition); }
			inline void											Set_DisableBlend(bool a_value) { mDisableBlend = a_value; SetNodeFlag(Node2D_NeedVerticeInfoUpdate); }

			bool												Draw(Scene::TravState* state) override;

			virtual bool										HasFocus() { return false; }
			virtual void										LoseFocus() {}
			virtual void										GetFocus() {}
			virtual bool										isAlpha(float X, float Y) { return false; };

			bool												ContainsPoint(float X, float Y);

			// utility method
			Point2D												GetCoordsInContainer(float X, float Y);

			inline void											TranslateWithOffSet(int a_offsetX, int a_offsetY) { mPosition[0] += a_offsetX; mPosition[1] += a_offsetY; SetNodeFlag(Node2D_NeedUpdatePosition); }

			// call just before opengl drawelement
			virtual void PreDraw(Scene::TravState* state) {} // use for texture predraw if needed
			virtual void PostDraw(Scene::TravState* state) {} // use for texture postdraw if needed

			virtual int GetTransparencyType() { return (GetOpacity() == 1.0f) ? 0 : 2; }

			virtual void SetTexUV(UIVerticesInfo* aQI) {}
			virtual void SetVertexArray(UIVerticesInfo* aQI) {}
			virtual void SetColor(UIVerticesInfo* aQI) {}
			void GetColor(float& R, float& G, float& B, float& A) { R = mColor[0]; G = mColor[1]; B = mColor[2]; A = GetOpacity(); }

			virtual UIVerticesInfo* GetVerticesInfo() { return NULL;/* mQI; */ }

			BaseUI2DLayer* getParentLayer();

			bool CanInteract(v2f pos, bool must_contain = true);

			virtual												~UIItem()
			{
				if (mAlphaMask)
					mAlphaMask = nullptr;
			}

		protected:

			void propagateOpacityChange()
			{
				SetNodeFlag(Node2D_NeedVerticeInfoUpdate);
				for (auto s : mSons)
				{
					if (s->isSubType(UIItem::mClassID))
					{
						UIItem* sonItem = static_cast<UIItem*>(s);
						if (sonItem->mOpacity < 0.0f)
						{
							sonItem->propagateOpacityChange();
						}
					}
				}
			}

			bool ManageInputSwallowEvent(Input::InputEvent& ev);

			/**
			* \brief	initialize modifiable
			* \fn 		void InitModifiable() override
			*/
			void	InitModifiable() override;

			void NotifyUpdate(const unsigned int labelid) override;

			// real drawing
			void										ProtectedDraw(Scene::TravState* state)  override {}



			maBool												mIsHidden;
			maBool												mIsTouchable;
			maBool												mDisableBlend;
			maBool												mIsEnabled;
			maVect3DF											mColor;  // USE [0,1] RANGE
			maFloat												mOpacity;
			SP<AlphaMask>										mAlphaMask;
			maBool												mSwallowInputs;

			WRAP_METHODS(ContainsPoint, ManageInputSwallowEvent);
		};
	}
}
