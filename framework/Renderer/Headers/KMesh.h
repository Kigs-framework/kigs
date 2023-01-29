#pragma once

#include "TecLibs/Tec3D.h"
#include "HDrawable.h"
#include "TecLibs/3D/3DObject/BBox.h"
#include "BufferedFile.h"

namespace Kigs
{

	namespace Collide
	{
		class AABBTreeNode;
		class Collision;
		class Distance;
	}
	namespace Draw
	{



class Material;



	/**
	* \brief	triangle type
	* \enum 	TriangleType
	*/
	// new enum use a flag type so it will be easier to get multitexture
	enum TriangleType
	{
		// F is flat S is smooth (1 normal or 3 normals per triangle)
		eF_Triangle=0,
		eS_Triangle=1,
		// C is Colored G is Gouraud (1 color or 3 colors per triangle)
		eFC_Triangle=2,
		eSC_Triangle=3,
		eFG_Triangle=4,
		eSG_Triangle=5,
		// T for textured
		eTF_Triangle=8,
		eTS_Triangle=9,
		eTFC_Triangle=10,
		eTSC_Triangle=11,
		eTFG_Triangle=12,
		eTSG_Triangle=13,

		// T2 for 2 textures stage
		eT2F_Triangle=16,
		eT2S_Triangle=17,
		eT2FC_Triangle=18,
		eT2SC_Triangle=19,
		eT2FG_Triangle=20,
		eT2SG_Triangle=21,

		// T3 for 3 textures stage
		eT3F_Triangle=32,
		eT3S_Triangle=33,
		eT3FC_Triangle=34,
		eT3SC_Triangle=35,
		eT3FG_Triangle=36,
		eT3SG_Triangle=37

	};


// ****************************************
// * MeshItemGroup class
// * --------------------------------------
/**
 * \file	KMesh.h
 * \class	MeshItemGroup
 * \ingroup Renderer
 * \brief	Group used in old style Mesh.
 *
 * Obsolete
 */
 // ****************************************
	class MeshItemGroup : public Drawable
	{
	public:
		DECLARE_ABSTRACT_CLASS_INFO(MeshItemGroup,Drawable,Renderer)

		/**
		* \brief	constructor
		* \fn 		MeshItemGroup(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
		* \param	name : instance name
		* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
		*/
		MeshItemGroup(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG) 
			: Drawable(name,PASS_CLASS_NAME_TREE_ARG),
			mCullMode(*this,"CullMode",1)
		{
			mFirstTriangle = 0;
			mTriangleCount = 0;
			mTriangleSize = 0;
		};

		/**
		* \brief	destructor
		* \fn 		~MeshItemGroup();
		*/
		virtual ~MeshItemGroup();

		unsigned int	GetSelfDrawingNeeds() override
		{
			return ((unsigned int)Need_Predraw)|((unsigned int)Need_Draw);
		}


		/**
		* \brief	update the bounding box
		* \fn 		virtual bool	BBoxUpdate(double)
		* \param	double : world time
		* \return	TRUE because has a bounding box
		*/
		bool	BBoxUpdate(double /* time*/) override {return true;}

		//! List of Triangles
		void*						mFirstTriangle;
		//! Triangle Count
		int							mTriangleCount;
		//! size of a triangle
		int							mTriangleSize;
		//! type of triangle
		TriangleType				mTriangleType;

		maInt						mCullMode;
	};


	// ****************************************
	// * Mesh class
	// * --------------------------------------
	/**
	 * \file	KMesh.h
	 * \class	Mesh
	 * \ingroup Renderer
	 * \brief	Old style Mesh.
	 *
	 * Obsolete
	 */
	 // ****************************************
	class Mesh : public HDrawable
	{
	protected:
		inline void Set_FileName(const char* _value){mFileName=_value;}

	public:
		DECLARE_ABSTRACT_CLASS_INFO(Mesh,HDrawable,Renderer)

		/**
		* \brief	constructor
		* \fn 		Mesh(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
		* \param	name : instance name
		* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
		*/
		Mesh(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);

		/**
		* \brief	constructor from a mesh
		* \fn 		Mesh(const Mesh& pMesh);
		* \param	pMesh : mesh to copy
		*/
		Mesh(const Mesh& pMesh);


		/**
		* \brief	pre draw method
		* \fn 		virtual void	DoPreDraw(TravState*);
		* \param	TravState : camera state
		*/
		void	DoPreDraw(TravState*) override;

		/**
		* \brief	retreive the bounding box of the bitmap (point min and point max)
		* \fn 		virtual void	GetBoundingBox(Point3D& pmin,Point3D& pmax) const {pmin=myBBoxMin; pmax=mBBoxMax;}
		* \param	pmin : point min of the bounding box (in/out param)
		* \param	pmax : point max of the bounding box (in/out param)
		*/
		void	GetNodeBoundingBox(Point3D& pmin,Point3D& pmax) const override
		{
			pmin = mBoundingBox.m_Min;
			pmax = mBoundingBox.m_Max;
		}

		// RETRIEVING
		/**
		* \brief	get the number of vertice
		* \fn 		int getVertexCount();
		* \return	the number of vertice
		*/
		int getVertexCount();

		/**
		* \brief	get the number of triangles
		* \fn 		int getTriangleCount();
		* \return	the number of triangles
		*/
		int getTriangleCount();

		/**
		* \brief	get a vertex from its index
		* \fn 		bool getVertex(int index, Point3D &Pt);
		* \param	index : index of the vertex to get
		* \param	Pt : the asked vertex (in/out param)
		* \return	TRUE if the vertex exist, FALSE otherwise
		*/
		bool getVertex(unsigned int index, Point3D &Pt);

		/**
		* \brief	get a triangle from its index
		* \fn 		bool getTriangle(int index, int &a, int &b, int &c);
		* \param	index : index of the vertex to get
		* \param	a : first vertex's indice of the asked triangle (in/out param)
		* \param	b : second vertex's indice of the asked triangle (in/out param)
		* \param	c : third vertex's indice of the asked triangle (in/out param)
		* \return	TRUE if the triangle exist, FALSE otherwise
		*/
		bool getTriangle(int index, int &a, int &b, int &c);

		/**
		* \brief	copy the triangle list and the vertice list
		* \fn 		bool CopyVertexAndTriangle(int &VCount, int &TCount, Point3D *&VArray, int *&IndexArray);
		* \param	VCount : number of vertice (in/out param)
		* \param	TCount : number of triangles (in/out param)
		* \param	VArray : list of vertice (in/out param)
		* \param	IndexArray : list of triangle's vertice index (in/out param)
		* \return	TRUE if the copy has been done, FALSE otherwise
		*/
		bool CopyVertexAndTriangle(int &VCount, int &TCount, Point3D *&VArray, int *&IndexArray);

		// ****************************************
		// * TexCoord class
		// * --------------------------------------
		/**
		 * \file	KMesh.h
		 * \class	TexCoord
		 * \ingroup Renderer
		 * \brief	Texture Coordinates structure.
		 *
		 */
		 // ****************************************
		class TexCoord
		{
		public:
			//! coordinate on x axis
			float u;
			//! coordinate on y axis
			float v;

			/**
			* \brief	read u and v value in a file
			* \fn 		void	Load(BufferedFile* currentfile);
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile);
		};


		// define classes

		class C_Triangle;

		class G_Triangle;

		class F_Triangle;

		class FC_Triangle;

		class FG_Triangle;

		class S_Triangle;

		class SC_Triangle;

		class SG_Triangle;

		template<unsigned int textureCount>
		class TF_Triangle;

		template<unsigned int textureCount>
		class TS_Triangle;

		template<unsigned int textureCount>
		class TFC_Triangle;

		template<unsigned int textureCount>
		class TSC_Triangle;

		template<unsigned int textureCount>
		class TFG_Triangle;

		template<unsigned int textureCount>
		class TSG_Triangle;


		// ****************************************
		// * Triangle class
		// * --------------------------------------
		/**
		 * \file	KMesh.h
		 * \class	Triangle
		 * \ingroup Renderer
		 * \brief	Triangle structure.
		 *
		 */
		 // ****************************************
		class Triangle
		{
		public:
			/**
			* \brief	destructor
			* \fn 		~Triangle();
			*/
			virtual ~Triangle(){};

			//! first indice
			unsigned int a;	
			//! second indice
			unsigned int b;	
			//! third indice
			unsigned int c;	

			//! overload '=' operator for copy action
			const Triangle& operator =( Triangle& T )
			{
				CopyFrom(T);
				return *this;
			}

			/**
			* \brief	copy a triangle
			* \fn 		void CopyFrom(Triangle& T)
			* \param	T : triangle to copy
			*/	
			virtual void CopyFrom(Triangle& T)
			{
				T.CopyTo(this);
			}

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	

#define DECLARE_COPY_TO(triangleType)	virtual void CopyTo(triangleType* T) \
			{ \
			T->a = a; \
			T->b = b; \
			T->c = c; \
			}	

			DECLARE_COPY_TO(Triangle)
			DECLARE_COPY_TO(C_Triangle)
			DECLARE_COPY_TO(G_Triangle)
			DECLARE_COPY_TO(F_Triangle)
			DECLARE_COPY_TO(FC_Triangle)
			DECLARE_COPY_TO(FG_Triangle)
			DECLARE_COPY_TO(S_Triangle)
			DECLARE_COPY_TO(SC_Triangle)
			DECLARE_COPY_TO(SG_Triangle)
			DECLARE_COPY_TO(TF_Triangle<1>)
			DECLARE_COPY_TO(TS_Triangle<1>)
			DECLARE_COPY_TO(TFC_Triangle<1>)
			DECLARE_COPY_TO(TSC_Triangle<1>)
			DECLARE_COPY_TO(TFG_Triangle<1>)
			DECLARE_COPY_TO(TSG_Triangle<1>)

			/**
			* \brief	read a, b and c value in a file
			* \fn 		void	Load(BufferedFile* currentfile);
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile);

			/**
			* \brief	get the size of the triangle
			* \fn 		virtual unsigned int	getSizeOf()
			* \return	the size of this instance of triangle
			*/	
			virtual unsigned int	getSizeOf(){return sizeof(*this);}

			/**
			* \brief	Compute the normal at triangle vertices ponderated by the angle
			* \fn 		void NormalAngle(const Point3D* VertexArray, Vector3D &Na, Vector3D &Nb,Vector3D &Nc);
			* \param	VertexArray : list of vertice
			* \param	Na : normal vector at the first triangle's vertex (in/out param)
			* \param	Nb : normal vector at the second triangle's vertex (in/out param)
			* \param	Nc : normal vector at the third triangle's vertex (in/out param)
			*/
			void NormalAngle(const Point3D* VertexArray, Vector3D &Na, Vector3D &Nb,Vector3D &Nc);

			/**
			* \brief	Compute the normal at triangle 
			* \fn 		void Normal(const Point3D* VertexArray, Vector3D &Na);
			* \param	VertexArray : list of vertice
			* \param	Na : normal vector at the triangle (in/out param)
			*/
			void Normal(const Point3D* VertexArray, Vector3D &Na)
			{
				Vector3D u(VertexArray[a]);
				u-=VertexArray[b];
				
				Vector3D v(VertexArray[c]);
				v-=VertexArray[b];
			
				Na.CrossProduct(u,v);
				Na.Normalize();
			}
		};


		// ****************************************
		// * TexturedTriangleStructBase class
		// * --------------------------------------
		/**
		* \file	KMesh.h
		* \class	TexturedTriangleStructBase
		* \ingroup Renderer
		* \brief	Textured triangle base structure.
		*
		*/
		// ****************************************
		class TexturedTriangleStructBase
		{
		public:
			/**
			* \brief	destructor
			* \fn 		~TexturedTriangleStructBase();
			*/
			virtual ~TexturedTriangleStructBase(){};

			/**
			* \brief	get the texture count
			* \fn 		virtual unsigned int GetTextureCount()=0;
			* \return	the number of texture coordinate
			*/
			virtual unsigned int GetTextureCount()=0;
		};

		template<unsigned int textureCount>
		// ****************************************
		// * TexturedTriangleStruct class
		// * --------------------------------------
		/**
		* \file	KMesh.h
		* \class	TexturedTriangleStruct
		* \ingroup Renderer
		* \brief	Textured triangle structure, with templated texture count parameter.
		*
		*/
		// ****************************************
		class TexturedTriangleStruct : public TexturedTriangleStructBase
		{
		public:
			//! index of TextCoord for the triangle's first vertex
			unsigned int Ta[textureCount];		
			//! index of TextCoord for the triangle's second vertex
			unsigned int Tb[textureCount];		
			//! index of TextCoord for the triangle's third vertex
			unsigned int Tc[textureCount];		

			/**
			* \brief	get the texture count
			* \fn 		unsigned int GetTextureCount()
			* \return	the number of texture coordinate
			*/
			unsigned int GetTextureCount(){return textureCount;}

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(TexturedTriangleStruct* T)
			{
				T->Ta[0] = Ta[0];
				T->Tb[0] = Tb[0];
				T->Tc[0] = Tc[0];
			}
			/**
			* \brief	read Ta, Tb and Tc value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile)
			{
				int i;
				for(i=0;i<textureCount;i++)
				{
					fread(&Ta[i],sizeof(Ta[i]),1,currentfile);
					fread(&Tb[i],sizeof(Tb[i]),1,currentfile);
					fread(&Tc[i],sizeof(Tc[i]),1,currentfile);
				}
			}
		};

		// ****************************************
		// * ThreeColorTriangleStruct class
		// * --------------------------------------
		/**
		* \file	KMesh.h
		* \class	ThreeColorTriangleStruct
		* \ingroup Renderer
		* \brief	Triangle with a color at each vertex.
		*
		*/
		// ****************************************
		class ThreeColorTriangleStruct
		{
		public:
			/**
			* \brief	destructor
			* \fn 		~ThreeColorTriangleStruct();
			*/
			virtual ~ThreeColorTriangleStruct(){};
			//! index of colors a
			unsigned int Ca;	
			//! index of colors b
			unsigned int Cb;	
			//! index of colors c
			unsigned int Cc;	

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(ThreeColorTriangleStruct* T)
			{
				T->Ca = Ca;
				T->Cb = Cb;
				T->Cc = Cc;
			}

			/**
			* \brief	read Ca, Cb and Cc value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile);
		};

		// ****************************************
		// * OneColorTriangleStruct class
		// * --------------------------------------
		/**
		* \file	KMesh.h
		* \class	OneColorTriangleStruct
		* \ingroup Renderer
		* \brief	Triangle with a uniform color.
		*
		*/
		// ****************************************
		class OneColorTriangleStruct
		{
		public:
			/**
			* \brief	destructor
			* \fn 		~OneColorTriangleStruct();
			*/
			virtual ~OneColorTriangleStruct(){};
			//! index of color
			unsigned int C;		

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(OneColorTriangleStruct* T)
			{
				T->C = C;
			}

			/**
			* \brief	read C value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile);
		};

		// ****************************************
		// * ThreeNormalTriangleStruct class
		// * --------------------------------------
		/**
		* \file	KMesh.h
		* \class	ThreeNormalTriangleStruct
		* \ingroup Renderer
		* \brief	Triangle with normal at each vertex.
		*
		*/
		// ****************************************
		class ThreeNormalTriangleStruct
		{
		public:
			/**
			* \brief	destructor
			* \fn 		~ThreeNormalTriangleStruct();
			*/
			virtual ~ThreeNormalTriangleStruct(){};
			//! index of normals
			unsigned int Na;
			//! index of normals
			unsigned int Nb;
			//! index of normals
			unsigned int Nc;

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(ThreeNormalTriangleStruct* T)
			{
				T->Na = Na;
				T->Nb = Nb;
				T->Nc = Nc;
			}

			/**
			* \brief	read Na, Nb and Nc value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile);
		};

		// ****************************************
		// * OneNormalTriangleStruct class
		// * --------------------------------------
		/**
		* \file	KMesh.h
		* \class	OneNormalTriangleStruct
		* \ingroup Renderer
		* \brief	Triangle with a unique normal.
		*
		*/
		// ****************************************
		class OneNormalTriangleStruct
		{
		public:
			/**
			* \brief	destructor
			* \fn 		~OneNormalTriangleStruct();
			*/
			virtual ~OneNormalTriangleStruct(){};
			//! index of normal
			unsigned int N;

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(OneNormalTriangleStruct* T)
			{
				T->N = N;
			}

			/**
			* \brief	read N value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile);
		};

	
		class C_Triangle : public Triangle , public OneColorTriangleStruct
		{
		public:

			/**
			* \brief	get the size of the class
			* \fn 		virtual unsigned int	getSizeOf()
			* \return	the size of the class
			*/
			virtual unsigned int	getSizeOf(){return sizeof(*this);}

			/**
			* \brief	copy a triangle
			* \fn 		void CopyFrom(Triangle& T)
			* \param	T : triangle to copy
			*/	
			virtual void CopyFrom(Triangle& T)
			{
				T.CopyTo((C_Triangle*)this);
			}

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(C_Triangle* T)
			{
				Triangle::CopyTo((Triangle*)T);
				OneColorTriangleStruct::CopyTo((OneColorTriangleStruct*)T);
			}
			using Triangle::CopyTo;
			using OneColorTriangleStruct::CopyTo;

			/**
			* \brief	read a C-riangle value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile)
			{
				Triangle::Load(currentfile);
				OneColorTriangleStruct::Load(currentfile);
			}
		};


		class G_Triangle : public Triangle , public ThreeColorTriangleStruct
		{
		public:

			/**
			* \brief	get the size of the class
			* \fn 		virtual unsigned int	getSizeOf()
			* \return	the size of the class
			*/
			virtual unsigned int	getSizeOf(){return sizeof(*this);}

			/**
			* \brief	copy a triangle
			* \fn 		void CopyFrom(Triangle& T)
			* \param	T : triangle to copy
			*/	
			virtual void CopyFrom(Triangle& T)
			{
				T.CopyTo((G_Triangle*)this);
			}

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(G_Triangle* T)
			{
				Triangle::CopyTo((Triangle*)T);
				ThreeColorTriangleStruct::CopyTo((ThreeColorTriangleStruct*)T);
			}
			using Triangle::CopyTo;
			using ThreeColorTriangleStruct::CopyTo;
			/**
			* \brief	read a G_Triangle value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile)
			{
				Triangle::Load(currentfile);
				ThreeColorTriangleStruct::Load(currentfile);
			}
		};

		class F_Triangle : public Triangle , public OneNormalTriangleStruct
		{
		public:


			/**
			* \brief	get the size of the class
			* \fn 		virtual unsigned int	getSizeOf()
			* \return	the size of the class
			*/
			virtual unsigned int	getSizeOf(){return sizeof(*this);}

			/**
			* \brief	copy a triangle
			* \fn 		void CopyFrom(Triangle& T)
			* \param	T : triangle to copy
			*/	
			virtual void CopyFrom(Triangle& T)
			{
				T.CopyTo((F_Triangle*)this);
			}

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(F_Triangle* T)
			{
				Triangle::CopyTo((Triangle*)T);
				OneNormalTriangleStruct::CopyTo((OneNormalTriangleStruct*)T);
			}
			using Triangle::CopyTo;
			using OneNormalTriangleStruct::CopyTo;


			/**
			* \brief	read F_Traingle value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile)
			{
				Triangle::Load(currentfile);
				OneNormalTriangleStruct::Load(currentfile);
			}
		};


		class FC_Triangle : public F_Triangle ,  public OneColorTriangleStruct
		{
		public:


			/**
			* \brief	get the size of the class
			* \fn 		virtual unsigned int	getSizeOf()
			* \return	the size of the class
			*/
			virtual unsigned int	getSizeOf(){return sizeof(*this);}


			/**
			* \brief	copy a triangle
			* \fn 		void CopyFrom(Triangle& T)
			* \param	T : triangle to copy
			*/	
			virtual void CopyFrom(Triangle& T)
			{
				T.CopyTo((FC_Triangle*)this);
			}

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(FC_Triangle* T)
			{
				F_Triangle::CopyTo((F_Triangle*)T);
				OneColorTriangleStruct::CopyTo((OneColorTriangleStruct*)T);
			}
			using F_Triangle::CopyTo;
			using OneColorTriangleStruct::CopyTo;
			/**
			* \brief	read FC_Traingle value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile)
			{
				F_Triangle::Load(currentfile);
				OneColorTriangleStruct::Load(currentfile);
			}
		};

		class FG_Triangle : public F_Triangle , public ThreeColorTriangleStruct
		{
		public:

			/**
			* \brief	get the size of the class
			* \fn 		virtual unsigned int	getSizeOf()
			* \return	the size of the class
			*/
			virtual unsigned int	getSizeOf(){return sizeof(*this);}

			/**
			* \brief	copy a triangle
			* \fn 		void CopyFrom(Triangle& T)
			* \param	T : triangle to copy
			*/	
			virtual void CopyFrom(Triangle& T)
			{
				T.CopyTo((FG_Triangle*)this);
			}

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(FG_Triangle* T)
			{
				F_Triangle::CopyTo((F_Triangle*)T);
				ThreeColorTriangleStruct::CopyTo((ThreeColorTriangleStruct*)T);
			}
			using F_Triangle::CopyTo;
			using ThreeColorTriangleStruct::CopyTo;
			/**
			* \brief	read FG_Traingle value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile)
			{
				F_Triangle::Load(currentfile);
				ThreeColorTriangleStruct::Load(currentfile);
			}
		};


		class S_Triangle : public Triangle , public ThreeNormalTriangleStruct
		{
		public:

			/**
			* \brief	get the size of the class
			* \fn 		virtual unsigned int	getSizeOf()
			* \return	the size of the class
			*/
			virtual unsigned int	getSizeOf(){return sizeof(*this);}

			/**
			* \brief	copy a triangle
			* \fn 		void CopyFrom(Triangle& T)
			* \param	T : triangle to copy
			*/	
			virtual void CopyFrom(Triangle& T)
			{
				T.CopyTo((S_Triangle*)this);
			}

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(S_Triangle* T)
			{
				Triangle::CopyTo((Triangle*)T);
				ThreeNormalTriangleStruct::CopyTo((ThreeNormalTriangleStruct*)T);
			}
			using Triangle::CopyTo;
			using ThreeNormalTriangleStruct::CopyTo;
			/**
			* \brief	read S_Traingle value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile)
			{
				Triangle::Load(currentfile);
				ThreeNormalTriangleStruct::Load(currentfile);
			}
		};


		class SC_Triangle : public S_Triangle , public OneColorTriangleStruct
		{
		public:


			/**
			* \brief	get the size of the class
			* \fn 		virtual unsigned int	getSizeOf()
			* \return	the size of the class
			*/
			virtual unsigned int	getSizeOf(){return sizeof(*this);}

			/**
			* \brief	copy a triangle
			* \fn 		void CopyFrom(Triangle& T)
			* \param	T : triangle to copy
			*/	
			virtual void CopyFrom(Triangle& T)
			{
				T.CopyTo((SC_Triangle*)this);
			}

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(SC_Triangle* T)
			{
				S_Triangle::CopyTo((S_Triangle*)T);
				OneColorTriangleStruct::CopyTo((OneColorTriangleStruct*)T);
			}
			using S_Triangle::CopyTo;
			using OneColorTriangleStruct::CopyTo;
			/**
			* \brief	read SC_Traingle value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile)
			{
				S_Triangle::Load(currentfile);
				OneColorTriangleStruct::Load(currentfile);
			}
		};


		class SG_Triangle : public S_Triangle , public ThreeColorTriangleStruct
		{
		public:

			/**
			* \brief	get the size of the class
			* \fn 		virtual unsigned int	getSizeOf()
			* \return	the size of the class
			*/
			virtual unsigned int	getSizeOf(){return sizeof(*this);}

			/**
			* \brief	copy a triangle
			* \fn 		void CopyFrom(Triangle& T)
			* \param	T : triangle to copy
			*/	
			virtual void CopyFrom(Triangle& T)
			{
				T.CopyTo((SG_Triangle*)this);
			}

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(SG_Triangle* T)
			{
				S_Triangle::CopyTo((S_Triangle*)T);
				ThreeColorTriangleStruct::CopyTo((ThreeColorTriangleStruct*)T);
			}
			using S_Triangle::CopyTo;
			using ThreeColorTriangleStruct::CopyTo;
			/**
			* \brief	read SG_Traingle value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile)
			{
				S_Triangle::Load(currentfile);
				ThreeColorTriangleStruct::Load(currentfile);
			}
		};

		template<unsigned int textureCount>
		class T_Triangle : public Triangle, public TexturedTriangleStruct<textureCount>
		{
		public:


			/**
			* \brief	get the size of the class
			* \fn 		virtual unsigned int	getSizeOf()
			* \return	the size of the class
			*/
			virtual unsigned int	getSizeOf(){return sizeof(*this);}

			/**
			* \brief	copy a triangle
			* \fn 		void CopyFrom(Triangle& T)
			* \param	T : triangle to copy
			*/	
			virtual void CopyFrom(Triangle& T)
			{
				T.CopyTo((T_Triangle*)this);
			}

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(T_Triangle* T)
			{
				Triangle::CopyTo((Triangle*)T);
				TexturedTriangleStruct<textureCount>::CopyTo((TexturedTriangleStruct<textureCount>*)T);
			}
			using Triangle::CopyTo;
			using TexturedTriangleStruct<textureCount>::CopyTo;
			/**
			* \brief	read T_Traingle value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile)
			{
				Triangle::Load(currentfile);
				TexturedTriangleStruct<textureCount>::Load(currentfile);
			}
		};

		template<unsigned int textureCount>
		class TF_Triangle : public F_Triangle , public TexturedTriangleStruct<textureCount>
		{
		public:


			/**
			* \brief	get the size of the class
			* \fn 		virtual unsigned int	getSizeOf()
			* \return	the size of the class
			*/
			virtual unsigned int	getSizeOf(){return sizeof(*this);}

			/**
			* \brief	copy a triangle
			* \fn 		void CopyFrom(Triangle& T)
			* \param	T : triangle to copy
			*/	
			virtual void CopyFrom(Triangle& T)
			{
				T.CopyTo((TF_Triangle*)this);
			}

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(TF_Triangle* T)
			{
				F_Triangle::CopyTo((F_Triangle*)T);
				TexturedTriangleStruct<textureCount>::CopyTo((TexturedTriangleStruct<textureCount>*)T);
			}
			using F_Triangle::CopyTo;
			using TexturedTriangleStruct<textureCount>::CopyTo;
			/**
			* \brief	read TF_Traingle value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile)
			{
				F_Triangle::Load(currentfile);
				TexturedTriangleStruct<textureCount>::Load(currentfile);
			}
		};

		template<unsigned int textureCount>
		class TFC_Triangle : public FC_Triangle , public TexturedTriangleStruct<textureCount>
		{
		public:


			/**
			* \brief	get the size of the class
			* \fn 		virtual unsigned int	getSizeOf()
			* \return	the size of the class
			*/
			virtual unsigned int	getSizeOf(){return sizeof(*this);}

			/**
			* \brief	copy a triangle
			* \fn 		void CopyFrom(Triangle& T)
			* \param	T : triangle to copy
			*/	
			virtual void CopyFrom(Triangle& T)
			{
				T.CopyTo((TFC_Triangle*)this);
			}

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(TFC_Triangle* T)
			{
				FC_Triangle::CopyTo((FC_Triangle*)T);
				TexturedTriangleStruct<textureCount>::CopyTo((TexturedTriangleStruct<textureCount>*)T);
			}
			using FC_Triangle::CopyTo;
			using TexturedTriangleStruct<textureCount>::CopyTo;
			/**
			* \brief	read TFC_Traingle value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile)
			{
				FC_Triangle::Load(currentfile);
				TexturedTriangleStruct<textureCount>::Load(currentfile);
			}
		};

		template<unsigned int textureCount>
		class TFG_Triangle : public FG_Triangle , public TexturedTriangleStruct<textureCount>
		{
		public:


			/**
			* \brief	get the size of the class
			* \fn 		virtual unsigned int	getSizeOf()
			* \return	the size of the class
			*/
			virtual unsigned int	getSizeOf(){return sizeof(*this);}

			/**
			* \brief	copy a triangle
			* \fn 		void CopyFrom(Triangle& T)
			* \param	T : triangle to copy
			*/	
			virtual void CopyFrom(Triangle& T)
			{
				T.CopyTo((TFG_Triangle*)this);
			}

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(TFG_Triangle* T)
			{
				FG_Triangle::CopyTo((FG_Triangle*)T);
				TexturedTriangleStruct<textureCount>::CopyTo((TexturedTriangleStruct<textureCount>*)T);
			}
			using FG_Triangle::CopyTo;
			using TexturedTriangleStruct<textureCount>::CopyTo;
			/**
			* \brief	read TFG_Traingle value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile)
			{
				FG_Triangle::Load(currentfile);
				TexturedTriangleStruct<textureCount>::Load(currentfile);
			}
		};

		template<unsigned int textureCount>
		class TS_Triangle : public S_Triangle , public TexturedTriangleStruct<textureCount>
		{
		public:


			/**
			* \brief	get the size of the class
			* \fn 		virtual unsigned int	getSizeOf()
			* \return	the size of the class
			*/
			virtual unsigned int	getSizeOf(){return sizeof(*this);}

			/**
			* \brief	copy a triangle
			* \fn 		void CopyFrom(Triangle& T)
			* \param	T : triangle to copy
			*/	
			virtual void CopyFrom(Triangle& T)
			{
				T.CopyTo((TS_Triangle*)this);
			}

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(TS_Triangle* T)
			{
				S_Triangle::CopyTo((S_Triangle*)T);
				TexturedTriangleStruct<textureCount>::CopyTo((TexturedTriangleStruct<textureCount>*)T);
			}
			using S_Triangle::CopyTo;
			using TexturedTriangleStruct<textureCount>::CopyTo;
			/**
			* \brief	read TS_Traingle value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile)
			{
				S_Triangle::Load(currentfile);
				TexturedTriangleStruct<textureCount>::Load(currentfile);
			}
		};

		template<unsigned int textureCount>
		class TSC_Triangle : public SC_Triangle , public TexturedTriangleStruct<textureCount>
		{
		public:


			/**
			* \brief	get the size of the class
			* \fn 		virtual unsigned int	getSizeOf()
			* \return	the size of the class
			*/
			virtual unsigned int	getSizeOf(){return sizeof(*this);}

			/**
			* \brief	copy a triangle
			* \fn 		void CopyFrom(Triangle& T)
			* \param	T : triangle to copy
			*/	
			virtual void CopyFrom(Triangle& T)
			{
				T.CopyTo((TSC_Triangle*)this);
			}

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(TSC_Triangle* T)
			{
				SC_Triangle::CopyTo((SC_Triangle*)T);
				TexturedTriangleStruct<textureCount>::CopyTo((TexturedTriangleStruct<textureCount>*)T);
			}
			using SC_Triangle::CopyTo;
			using TexturedTriangleStruct<textureCount>::CopyTo;
			/**
			* \brief	read TSC_Traingle value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile)
			{
				SC_Triangle::Load(currentfile);
				TexturedTriangleStruct<textureCount>::Load(currentfile);
			}
		};

		template<unsigned int textureCount>
		class TSG_Triangle : public SG_Triangle , public TexturedTriangleStruct<textureCount>
		{
		public:


			/**
			* \brief	get the size of the class
			* \fn 		virtual unsigned int	getSizeOf()
			* \return	the size of the class
			*/
			virtual unsigned int	getSizeOf(){return sizeof(*this);}	
			/**
			* \brief	copy a triangle
			* \fn 		void CopyFrom(Triangle& T)
			* \param	T : triangle to copy
			*/

			virtual void CopyFrom(Triangle& T)
			{
				T.CopyTo((TSG_Triangle*)this);
			}

			/**
			* \brief	copy to triangle
			* \fn 		void CopyTo(Triangle* T)
			* \param	T : triangle to copy to
			*/	
			virtual void CopyTo(TSG_Triangle* T)
			{
				SG_Triangle::CopyTo((SG_Triangle*)T);
				TexturedTriangleStruct<textureCount>::CopyTo((TexturedTriangleStruct<textureCount>*)T);
			}
			using SG_Triangle::CopyTo;
			using TexturedTriangleStruct<textureCount>::CopyTo;
			/**
			* \brief	read TSG_Traingle value in a file
			* \fn 		void	Load(File::BufferedFile* currentfile)
			* \param	currentfile : File::BufferedFile to read
			*/	
			void	Load(File::BufferedFile* currentfile)
			{
				SG_Triangle::Load(currentfile);
				TexturedTriangleStruct<textureCount>::Load(currentfile);
			}
		};

		/**
		* \brief	get a vertex from its index
		* \fn 		const Point3D&	GetVertex(unsigned int index)
		* \param	index : index asked
		* \return	vertex asked (in/out param)
		*/	
		const Point3D&	GetVertex(unsigned int index){return mVertexArray[index];}

		/**
		* \brief	set a vertex to an index
		* \fn 		void SetVertex(unsigned int index, const Point3D &v);
		* \param	index : new vertex index
		* \param	v : new vertex
		*/	
		void SetVertex(unsigned int index, const Point3D &v);

		/**
		* \brief	get a color from its index
		* \fn 		const Vector4D&	GetColor(unsigned int index)
		* \param	index : index asked
		* \return	color asked (in/out param)
		*/	
		const Vector4D&	GetColor(unsigned int index){return mColorArray[index];}

		/**
		* \brief	set a color to an index
		* \fn 		void SetColor(unsigned int index, const Vector4D &v);
		* \param	index : new color index
		* \param	v : new color
		*/
		void SetColor(unsigned int index, const Vector4D &v);

		/**
		* \brief	get a normal from its index
		* \fn 		const Vector3D&	GetNormal(unsigned int index)
		* \param	index : index asked
		* \return	normal asked (in/out param)
		*/	
		const Vector3D&	GetNormal(unsigned int index){return mNormalArray[index];}

		/**
		* \brief	set a normal to an index
		* \fn 		void SetNormal(unsigned int index, const Vector3D &v);
		* \param	index : new normal index
		* \param	v : new normal
		*/
		void SetNormal(unsigned int index, const Vector3D &v);

		/**
		* \brief	get a texCoord from its index
		* \fn 		const TexCoord&	GetTexCoord(unsigned int index)
		* \param	index : index asked
		* \return	texCoord asked (in/out param)
		*/	
		const TexCoord&	GetTexCoord(unsigned int index){return mTexArray[index];}

		/**
		* \brief	set a texCoord to an index
		* \fn 		void SetTexCoord(unsigned int index, const TexCoord &v);
		* \param	index : new texCoord index
		* \param	v : new texCoord
		*/
		void SetTexCoord(unsigned int index, const TexCoord &v);

	protected:
		/*	//! link to getVertex method
		maMethod	myGetVertexPointerMethod;
		//! getVertex method
		bool		GetVertexPointer(std::vector<CoreModifiableAttribute*>& params);
		*/

		DECLARE_METHOD(GetVertexPointer);

		//! link to getColor method
		/*maMethod	myGetColorPointerMethod;
		//! getColor method
		bool		GetColorPointer(std::vector<CoreModifiableAttribute*>& params);
		*/
		DECLARE_METHOD(GetColorPointer);


		//! link to getNormal method
		/*maMethod	myGetNormalPointerMethod;
		//! getNormal method
		bool		GetNormalPointer(std::vector<CoreModifiableAttribute*>& params);
		*/
		DECLARE_METHOD(GetNormalPointer);

		//! link to getTexCoord method
		/*	maMethod	myGetTexCoordPointerMethod;
		//! getTexCoord method
		bool		GetTexCoordPointer(std::vector<CoreModifiableAttribute*>& params);
		*/

		DECLARE_METHOD(GetTexCoordPointer);

		COREMODIFIABLE_METHODS(GetVertexPointer, GetColorPointer, GetNormalPointer, GetTexCoordPointer);

		/**
		* \brief	update the mesh if one of its componants has changed
		* \fn 		virtual void	UpdateMesh(){;}
		*/
		virtual void	UpdateMesh(){;}

		//! TRUE if myVertex need to be updated
		maBool	mVertexNeedUpdate;
		//! TRUE if mColor need to be updated
		maBool	mColorNeedUpdate;
		//! TRUE if myTexCoord need to be updated
		maBool	mTexCoordNeedUpdate;
		//! TRUE if myNormal need to be updated
		maBool	mNormalNeedUpdate;
		//! ?
		maBool	mShareMaterial;

#ifdef _DEBUG
		maBool mShowVertex;
#endif
		maBool mWireMode;
		/**
		* \brief	protected initialization modifiable
		* \fn 		virtual void ProtectedInit()=0;
		*/ 
		virtual void ProtectedInit()=0;

		/**
		* \brief	initialize modifiable
		* \fn 		void InitModifiable() override
		*/ 
		void	InitModifiable() override;

		//! name of the file to read in load method
		maString	mFileName;

		//! number of vertice
		unsigned int		mVertexCount;
		//! List of All Vertice
		Point3D*			mVertexArray;	

		//! number of normal
		unsigned int		mNormalCount;
		//! List of All normals
		Vector3D*			mNormalArray;	

		//! number of Texture coords
		unsigned int		mTexCoordCount;
		//! List of all Texture coords
		TexCoord*			mTexArray;		

		//! number of color
		unsigned int		mColorCount;
		//! List of All colors
		Vector4D*			mColorArray;

		maBool				mDynamicInit;

		/**
		* \brief	destructor
		* \fn 		~Mesh();
		*/
		virtual		~Mesh();

		/**
		* \brief	bounding box initialization 
		* \fn 		virtual void  InitBoundingBox();
		*/ 
		virtual void  InitBoundingBox();

		//! link to the bounding box
		BBox          mBoundingBox;

	public:
		friend class ASEMeshLoader;
		friend class BinMeshLoader;
//#ifdef WIN32
		friend class STLMeshLoader;
//#endif
		friend class Collide::AABBTreeNode;
		friend class Collide::Collision;
		friend class Collide::Distance;

		//Setters
		//! Set number of vertice
		inline void Set_VertexCount(unsigned int _value){mVertexCount = _value;}

		//! Set List of All Vertice
		inline void Set_VertexArray(Point3D* _value){mVertexArray = _value;}	

		//! Set number of normal
		inline void Set_NormalCount(unsigned int _value){mNormalCount = _value;}

		//! Set List of All normals
		inline void Set_NormalArray(Vector3D* _value){mNormalArray = _value;}	

		//! Set number of Texture coords
		inline void Set_TexCoordCount(unsigned int _value){mTexCoordCount = _value;}

		//! Set List of all Texture coords
		inline void Set_TexArray(TexCoord* _value){mTexArray = _value;}

		//! Set number of color
		inline void Set_ColorCount(unsigned int _value){mColorCount = _value;}

		//! Set List of All colors
		inline void Set_ColorArray(Vector4D* _value){mColorArray = _value;}

		inline unsigned int Get_NormalCount() const {return mNormalCount;}
#ifdef WIN32
		void ExportMeshTo(std::string _pathDirectory, std::string _fileName);
#endif

		void	RecomputeNormals();
		void	RecomputeNormalsFast();
	};

	}
}
