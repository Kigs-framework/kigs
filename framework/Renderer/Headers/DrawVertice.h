#pragma once

#include "Drawable.h"
#include "SmartPointer.h"
#include "Texture.h"

#include "TecLibs/3D/3DObject/BBox.h"

#include "RendererDefines.h"

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
	 * \fn 		virtual bool	BBoxUpdate(kdouble)
	 * \param	kdouble : world time
	 * \return	TRUE because has a bounding box
	 */
	bool BBoxUpdate(kdouble/* time */) override { return true; }

	/**
	 * \brief	retreive the bounding box of the bitmap (point min and point max)
	 * \fn 		virtual void	GetBoundingBox(Point3D& pmin,Point3D& pmax) const {pmin=myBBoxMin; pmax=myBBoxMax;}
	 * \param	pmin : point min of the bounding box (in/out param)
	 * \param	pmax : point max of the bounding box (in/out param)
	 */
	void GetNodeBoundingBox(Point3D& pmin, Point3D& pmax) const override;

	/**
	 * \brief	set the Vertex list (cler the old Vertex list if not null)
	 * \fn 		void setVertexArray(Point3D *aArray, int Count);
	 * \param	aArray : new Vertex list
	 * \param	Count :  number of the Vertex
	 */
	void SetVertexArray(void *aArray, int Count, int Size = 12/*default Size is 3 float*/, int decal = 0, int format = KIGS_FLOAT);
	void GetVertexArray(void **aArray, int &Count, int &Size, int &format)
	{
		*aArray = myVertex;
		Count = myVertexCount;
		Size = myVertexSize;
		format = myVertexFormat;
	}
	/**
	 * \brief	set the indice list
	 * \fn 		void setindiceArray(Point3D *aArray, int Count);
	 * \param	aArray : new Vertex list
	 * \param	Count :  number of the Vertex
	 */
	void SetIndexArray(void *aArray, int Count, int Size = 4/*default Size is 1 uint*/, int format = KIGS_UNSIGNED_INT);
	void GetIndexArray(void **aArray, int &Count, int &Size, int &format)
	{
		*aArray = myIndex;
		Count = myIndiceCount;
		Size = myIndiceSize;
		format = myIndiceFormat;
	}

	// aArray = nullptr for interleaved array with vertex
	void SetTexCoordArray(void* aArray, int Count, int Size = 8/*default Size is 2 float*/, int decal = 0, int format = KIGS_FLOAT);
	// aArray = nullptr for interleaved array with vertex
	void SetNormalArray(void* aArray, int Count, int Size = 12/*default Size is 3 float*/, int decal = 0, int format = KIGS_FLOAT);
	// aArray = nullptr for interleaved array with vertex
	void SetColorArray(void* aArray, int Count, int Size = 4/*default Size is 4 char*/, int decal = 0, int format = KIGS_UNSIGNED_BYTE);

	void SetTexture(SmartPointer<Texture> tex) { mypTexture = tex; SetDataFromPreset(); }
	SmartPointer<Texture> GetTexture() { return mypTexture; }

protected:
	void InitModifiable() override;
	void NotifyUpdate(const u32 labelid) override;

	void RecalculateBBox() const;

	void SetDataFromPreset();

	void ProtectedDestroy() override;

	void clearArrays();

	//! list of normal
	void*								myNormals = nullptr;
	//! number of normal
	int									myNormalCount = 0;
	//! Size of normal
	int									myNormalSize = 0;
	//! Decal of normal
	int									myNormalDecal = 0;
	//! format of normal
	int									myNormalFormat = KIGS_FLOAT;

	//! list of Vertex
	void*								myVertex = nullptr;
	//! number of Vertex
	int									myVertexCount = 0;
	//! Size of Vertex
	int									myVertexSize = 0;
	//! Decal of Vertex
	int									myVertexDecal = 0;
	//! format of Vertex
	int									myVertexFormat = KIGS_FLOAT;

	//! list of color
	void *								myColor = nullptr;
	//! number of color
	int									myColorCount = 0;
	//! Size of color
	int									myColorSize = 0;
	//! Decal of color
	int									myColorDecal = 0;
	//! format of color
	int									myColorFormat = KIGS_UNSIGNED_BYTE;

	//! list of indice
	void*								myIndex = nullptr;
	//! number of indice
	int									myIndiceCount = 0;
	//! Size of indice
	int									myIndiceSize = 0;
	//! format of indice
	int									myIndiceFormat = KIGS_UNSIGNED_INT;

	//! list of tex coord
	void *								myTexCoords = nullptr;
	//! number of tex coord
	unsigned int						myTexCoordCount = 0;
	//! Size of tex coord
	int									myTexCoordSize = 0;
	//! Decal of tex coord
	int									myTexCoordDecal = 0;
	//! format of tex coord
	int									myTexCoordFormat = KIGS_FLOAT;


	mutable BBox								myBBox;
	mutable bool needRecomputeBBox = true;

	bool needReloadData = false;

	SmartPointer<Texture>				mypTexture = nullptr;

	maString							myTextureFileName = BASE_ATTRIBUTE(TextureFileName, "");
	
	maBool								myIsStaticBuffer = BASE_ATTRIBUTE(IsStaticBuffer, false);
	maInt								myDrawMode = BASE_ATTRIBUTE(DrawMode, KIGS_DRAW_MODE_TRIANGLES); 

	maInt								myCullMode = BASE_ATTRIBUTE(CullMode, 0);

	maEnum<5>							myPreset = BASE_ATTRIBUTE(Preset, "None", "TexturedQuad", "Quad", "TexturedBox", "Box");
	maVect3DF							myPresetSize = BASE_ATTRIBUTE(PresetSize, 1, 1, 1);
	maVect4DF							myPresetColor = BASE_ATTRIBUTE(PresetColor, 1, 1, 1, 1);
	maVect3DF							myPresetOffset = BASE_ATTRIBUTE(PresetOffset, 0, 0, 0);
	maVect2DF							myPresetUVSize = BASE_ATTRIBUTE(PresetUVSize, 1, 1);
	maVect2DF							myPresetUVOffset = BASE_ATTRIBUTE(PresetUVOffset, 0, 0);

#ifdef KIGS_TOOLS
	maBool myDrawNormals = BASE_ATTRIBUTE(DrawNormals, false);
	maBool myDrawUVs = BASE_ATTRIBUTE(DrawUVs, false);
	maBool myWireMode = BASE_ATTRIBUTE(WireMode, false);
#endif

	u32 myVertexBuffer = -1;
	//u32 myIndexBuffer = -1;
};


