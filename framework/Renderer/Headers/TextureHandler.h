#pragma once

#include "AttributePacking.h"
#include "Drawable.h"
#include "TinyImage.h"
#include "TecLibs/2D/BBox2DI.h"
#include "Upgrador.h"
#include "Texture.h"
#include "ModuleRenderer.h"

namespace Kigs
{
	namespace Draw
	{
		// ****************************************
		// * TextureHandler class
		// * --------------------------------------
		/**
		* \file	TextureHandler.h
		* \class	TextureHandler
		* \ingroup Renderer
		* \brief handle texture drawing. A TextureHandler is used to manipulate a texture or animated texture or sprite in a texture the same way
		*
		*/
		// ****************************************
		class TextureHandler : public CoreModifiable
		{

		public:


			friend class AnimationUpgrador;
			DECLARE_CLASS_INFO(TextureHandler, CoreModifiable, Renderer)
				static constexpr unsigned int pushUVMatrix = 1 << ParentClassType::usedUserFlags;
			static constexpr unsigned int usedUserFlags = ParentClassType::usedUserFlags + 1;

			/**
			* \brief	constructor
			* \fn 		TextureHandler(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
			* \param	name : instance name
			* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			*/
			TextureHandler(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			/**
			* \brief	destructor
			* \fn 		~TextureHandler();
			*/
			virtual ~TextureHandler();

			// access to texture methods
			void SetRepeatUV(bool RU, bool RV)
			{
				if (mTexture)
				{
					mTexture->SetRepeatUV(RU, RV);
				}
			}

			void	DoPreDraw(TravState* st)
			{
				if (mTexture)
				{
					mTexture->DoPreDraw(st);
					if (st && isUserFlagSet(pushUVMatrix))
					{
						st->GetRenderer()->PushAndLoadMatrix(MATRIX_MODE_UV, mUVTexture);
					}
				}
			}
			void	DoPostDraw(TravState* st)
			{
				if (mTexture)
				{
					if (st && isUserFlagSet(pushUVMatrix))
					{
						st->GetRenderer()->PopMatrix(MATRIX_MODE_UV);
					}
					mTexture->DoPostDraw(st);
				}
			}

			void GetSize(unsigned int& width, unsigned int& height)
			{
				width = mSize.x;
				height = mSize.y;
			}


			void GetSize(float& width, float& height)
			{
				width = mSize.x;
				height = mSize.y;
			}

			void GetPow2Size(unsigned int& width, unsigned int& height)
			{
				if (mTexture)
				{
					mTexture->GetPow2Size(width, height);
				}
			}
			void GetRatio(float& rX, float& rY)
			{
				if (mTexture)
				{
					mTexture->GetRatio(rX, rY);
				}
			}
			int GetTransparency()
			{
				if (mTexture)
				{
					return mTexture->GetTransparency();
				}
				return 0;
			}

			void setUserFlag(unsigned int flag)
			{
				if (mTexture)
				{
					mTexture->setUserFlag(flag);
				}
			}

			v2f getDrawablePos(const v2f& pos);

			bool HasTexture()
			{
				return mTexture;
			}

			SP<Texture>	GetEmptyTexture(const std::string& name = "");
			// use mTextureName to load texture
			bool	changeTexture();

			void	refreshTextureInfos();

			void	setTexture(SP<Texture> texture)
			{
				mCurrentFrame = nullptr;
				if (GetUpgrador("AnimationUpgrador"))
				{
					Downgrade("AnimationUpgrador");
				}
				// replace texture
				mTexture = texture;

				if (!mTexture)
				{
					mSize.Set(0.0f, 0.0f);
				}
			}

			SP<Texture> getTexture()
			{
				return mTexture;
			}

			const Matrix4x4& getUVTexture() const
			{
				return mUVTexture;
			}

		protected:

			/**
			* \brief	initialize modifiable
			* \fn		virtual	void	InitModifiable();
			*/
			void	InitModifiable() override;

			/**
			* \brief	this method is called to notify this that one of its attribute has changed.
			* \fn 		virtual void NotifyUpdate(const unsigned int);
			* \param	int : not used
			*/
			void NotifyUpdate(const unsigned int /* labelid */) override;



			INSERT_FORWARDSP(Texture, mTexture);

			maString	mTextureName = BASE_ATTRIBUTE(TextureName, "");
			maBool		mPushUVMatrix = BASE_ATTRIBUTE(PushUVMatrix, false);

			void	refreshSizeAndUVs(const SpriteSheetFrameData* ssf);

			// size of one pixel in uv coordinates
			v2f mOneOnPower2Size;

			v2f mSize = { 0.0f,0.0f };

			Matrix4x4	mUVTexture;

			// return true if something changed
			bool	initFromSpriteSheet(const std::string& jsonfilename);
			// return true if something changed
			bool	initFromPicture(const std::string& picfilename);
			void	setCurrentFrame(const SpriteSheetFrameData* ssf);

			const SpriteSheetFrameData* mCurrentFrame = nullptr;

			void	textureWasInit();

			void TextureNotifyUpdate(CoreModifiable* sender, const unsigned int /* labelid */);

			WRAP_METHODS(textureWasInit, TextureNotifyUpdate);

		};


		class 	AnimationUpgrador : public Upgrador<TextureHandler>
		{
		protected:
			// create and init Upgrador if needed and add dynamic attributes
			virtual void	Init(CoreModifiable* toUpgrade) override;

			// destroy UpgradorData and remove dynamic attributes 
			virtual void	Destroy(CoreModifiable* toDowngrade, bool toDowngradeDeleted) override;

			START_UPGRADOR(AnimationUpgrador);

			UPGRADOR_METHODS(Play, AnimationNotifyUpdate);

			void	Update(const Timer& _timer, TextureHandler* parent);
			void	NotifyUpdate(const unsigned int /* labelid */, TextureHandler* parent);

		public:

			maString* mCurrentAnimation = nullptr;
			maUInt* mFramePerSecond = nullptr;
			maBool* mLoop = nullptr;
			bool				mWasdAutoUpdate = false;
			unsigned int		mCurrentFrame = 0;
			double				mElpasedTime = 0.0;
			unsigned int		mFrameNumber = 0;

		protected:

		};

	}
}