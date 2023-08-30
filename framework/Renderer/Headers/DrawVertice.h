#pragma once

#include "Drawable.h"
#include "SmartPointer.h"
#include "Texture.h"

#include "TecLibs/3D/3DObject/BBox.h"

#include "RendererDefines.h"

namespace Kigs
{

	namespace Draw
	{
		// ****************************************
		// * DrawVertice class
		// * --------------------------------------
		/**
		 * \file	DrawVertice.h
		 * \class	DrawVertice
		 * \ingroup Renderer
		 * \brief	Draw a dynamic mesh.
		 */
		 // ****************************************

		class DrawVertice : public Drawable
		{
		public:
			DECLARE_CLASS_INFO(DrawVertice, Drawable, Renderer)
				DECLARE_CONSTRUCTOR(DrawVertice)

				/**
				 * \brief	destructor
				 * \fn 		~SimpleDrawing();
				 */
				virtual ~DrawVertice();

			/**
			 * \brief	initialise draw method
			 * \fn 		virtual bool Draw(TravState* travstate) = 0;
			 * \param	travstate : camera state
			 * \return	TRUE if a could draw
			 */
			bool Draw(TravState* travstate) override;

			/**
			 * \brief	update the bounding box
			 * \fn 		virtual bool	BBoxUpdate(double)
			 * \param	double : world time
			 * \return	TRUE because has a bounding box
			 */
			bool BBoxUpdate(double/* time */) override { return true; }

			/**
			 * \brief	retreive the bounding box of the bitmap (point min and point max)
			 * \fn 		virtual void	GetBoundingBox(v3f& pmin,v3f& pmax) const {pmin=myBBoxMin; pmax=mBBoxMax;}
			 * \param	pmin : point min of the bounding box (in/out param)
			 * \param	pmax : point max of the bounding box (in/out param)
			 */
			void GetNodeBoundingBox(v3f& pmin, v3f& pmax) const override;

			/**
			 * \brief	set the Vertex list (cler the old Vertex list if not null)
			 * \fn 		void setVertexArray(v3f *aArray, int Count);
			 * \param	aArray : new Vertex list
			 * \param	Count :  number of the Vertex
			 */
			void SetVertexArray(void* aArray, int Count, int Size = 12/*default Size is 3 float*/, int decal = 0, int format = KIGS_FLOAT);
			void GetVertexArray(void** aArray, int& Count, int& Size, int& format)
			{
				*aArray = mVertices;
				Count = mVertexCount;
				Size = mVertexSize;
				format = mVertexFormat;
			}
			/**
			 * \brief	set the indice list
			 * \fn 		void setindiceArray(v3f *aArray, int Count);
			 * \param	aArray : new Vertex list
			 * \param	Count :  number of the Vertex
			 */
			void SetIndexArray(void* aArray, int Count, int Size = 4/*default Size is 1 uint*/, int format = KIGS_UNSIGNED_INT);
			void GetIndexArray(void** aArray, int& Count, int& Size, int& format)
			{
				*aArray = mIndices;
				Count = mIndiceCount;
				Size = mIndiceSize;
				format = mIndiceFormat;
			}

			// aArray = nullptr for interleaved array with vertex
			void SetTexCoordArray(void* aArray, int Count, int Size = 8/*default Size is 2 float*/, int decal = 0, int format = KIGS_FLOAT);
			// aArray = nullptr for interleaved array with vertex
			void SetNormalArray(void* aArray, int Count, int Size = 12/*default Size is 3 float*/, int decal = 0, int format = KIGS_FLOAT);
			// aArray = nullptr for interleaved array with vertex
			void SetColorArray(void* aArray, int Count, int Size = 4/*default Size is 4 char*/, int decal = 0, int format = KIGS_UNSIGNED_BYTE);

			void SetTexture(SmartPointer<Texture> tex) { mTexture = tex; SetDataFromPreset(); }
			SmartPointer<Texture> GetTexture() { return mTexture; }

		protected:
			void InitModifiable() override;
			void NotifyUpdate(const u32 labelid) override;

			void RecalculateBBox() const;

			void SetDataFromPreset();

			void clearArrays();

			//! list of normal
			void* mNormals = nullptr;
			//! number of normal
			int									mNormalCount = 0;
			//! Size of normal
			int									mNormalSize = 0;
			//! Decal of normal
			int									mNormalDecal = 0;
			//! format of normal
			int									mNormalFormat = KIGS_FLOAT;

			//! list of Vertex
			void* mVertices = nullptr;
			//! number of Vertex
			int									mVertexCount = 0;
			//! Size of Vertex
			int									mVertexSize = 0;
			//! Decal of Vertex
			int									mVertexDecal = 0;
			//! format of Vertex
			int									mVertexFormat = KIGS_FLOAT;

			//! list of color
			void* mColors = nullptr;
			//! number of color
			int									mColorCount = 0;
			//! Size of color
			int									mColorSize = 0;
			//! Decal of color
			int									mColorDecal = 0;
			//! format of color
			int									mColorFormat = KIGS_UNSIGNED_BYTE;

			//! list of indice
			void* mIndices = nullptr;
			//! number of indice
			int									mIndiceCount = 0;
			//! Size of indice
			int									mIndiceSize = 0;
			//! format of indice
			int									mIndiceFormat = KIGS_UNSIGNED_INT;

			//! list of tex coord
			void* mTexCoords = nullptr;
			//! number of tex coord
			unsigned int						mTexCoordCount = 0;
			//! Size of tex coord
			int									mTexCoordSize = 0;
			//! Decal of tex coord
			int									mTexCoordDecal = 0;
			//! format of tex coord
			int									mTexCoordFormat = KIGS_FLOAT;


			mutable BBox						mBBox;
			mutable bool						mNeedRecomputeBBox = true;

			bool needReloadData = false;

			SmartPointer<Texture>				mTexture = nullptr;

			bool								mIsStaticBuffer = false;
			s32									mDrawMode = KIGS_DRAW_MODE_TRIANGLES;

			s32									mCullMode = 0;
			//! 0 = no transparency, 1 = billboard (binary transparency), 2 = need blend, -1 = use texture transparency type if available otherwise no transparency
			s32									mTransparencyType = -1;

			v2f									mPresetUVSize = { 1.0f, 1.0f };
			v2f									mPresetUVOffset = { 0, 0 };
			v3f									mPresetSize = { 1.0f, 1.0f, 1.0f };
			v3f									mPresetOffset = { 0, 0, 0 };
			v4f									mPresetColor = { 1.0f, 1.0f, 1.0f, 1.0f };
			std::string							mTextureFileName = "";

			WRAP_ATTRIBUTES(mIsStaticBuffer, mDrawMode, mCullMode, mTransparencyType, mPresetUVSize, mPresetUVOffset, mPresetSize, mPresetOffset, mPresetColor, mTextureFileName);

			// preset should be done with an upgrador ?
			maEnum<5>							mPreset = BASE_ATTRIBUTE(Preset, "None", "TexturedQuad", "Quad", "TexturedBox", "Box");

#ifdef KIGS_TOOLS
			maBool mDrawNormals = BASE_ATTRIBUTE(DrawNormals, false);
			maBool mDrawUVs = BASE_ATTRIBUTE(DrawUVs, false);
			maBool mWireMode = BASE_ATTRIBUTE(WireMode, false);
#endif

			u32 mVertexBuffer = -1;
		};
	}
}