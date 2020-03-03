#ifndef _KDMESH_H_
#define _KDMESH_H_

#include "TecLibs/Tec3D.h"
#include "HDrawable.h"
#include "TecLibs/3D/3DObject/BBox.h"
#include "BufferedFile.h"
#include "RefCountedClass.h"

class Material;

#ifdef _3DSMAX
namespace KigsFramework
{
#endif

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
	/*
	enum TriangleTypeOld
	{
	// F is flat S is smooth (1 normal or 3 normals per triangle)
	eF_Triangle=1,
	eS_Triangle,
	// C is Colored G is Gouraud (1 color or 3 colors per triangle)
	eFC_Triangle,
	eSC_Triangle,
	eFG_Triangle,
	eSG_Triangle,
	// T for textured
	eTF_Triangle,
	eTS_Triangle,
	eTFC_Triangle,
	eTSC_Triangle,
	eTFG_Triangle,
	eTSG_Triangle
	};*/

	/*! \defgroup Triangle Triangle group
	* \ingroup TriangleInternalStruct
	*  triangle class
	*/

	/*! \defgroup TriangleInternalStruct Internal Mesh
	* \ingroup Renderer
	*  internal structure of mesh
	*/

	// ****************************************
	// * MeshItemGroup class
	// * --------------------------------------
	/**
	* \class	MeshItemGroup
	* \ingroup Drawable
	* \ingroup RendererDrawable
	* \brief	
	* \author	ukn
	* \version ukn
	* \date	ukn
	*/
	// ****************************************
	class MeshItemGroup : public Drawable
	{
	public:
		DECLARE_ABSTRACT_CLASS_INFO(MeshItemGroup,Drawable,Renderer)

		/**
		* \brief	constructor
		* \fn 		MeshItemGroup(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
		* \param	name : instance name
		* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
		*/
		MeshItemGroup(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG) 
			: Drawable(name,PASS_CLASS_NAME_TREE_ARG),
			myCullMode(*this,false,LABEL_AND_ID(CullMode),1)
		{
			myFirstTriangle = 0;
			myTriangleCount = 0;
			myTriangleSize = 0;
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
		* \fn 		virtual bool	BBoxUpdate(kdouble)
		* \param	kdouble : world time
		* \return	TRUE because has a bounding box
		*/
		bool	BBoxUpdate(kdouble /* time*/) override {return true;}

		//! List of Triangles
		void*						myFirstTriangle;
		//! Triangle Count
		int							myTriangleCount;
		//! size of a triangle
		int							myTriangleSize;
		//! type of triangle
		TriangleType				myTriangleType;

		maInt						myCullMode;
	};

	// ****************************************
	// * Mesh class
	// * --------------------------------------
	/**
	* \file	KMesh.h
	* \class	Mesh
	* \ingroup Drawable
	* \ingroup RendererDrawable
	* \brief	Mesh object class
	* \author	ukn
	* \version ukn
	* \date	ukn
	* 
	* <dl class="dependency"><dt><b>Dependency:</b></dt><dd>ModuleRenderer</dd></dl>
	* <dl class="exemple"><dt><b>Exemple:</b></dt><dd>
	* <span class="comment"> Manage mesh file : </span><br>
	* <span class="code">
	* theFileManager=new ModuleFileManager(<i>instance_name</i>);<br>
	* theFileManager->Init(KigsCore::Instance(),0);<br>
	* theFileManager->AddToPath(<i>directory_path</i>,"mesh"); <span class="comment"> //manage .mesh file </span><br>
	* </span>
	* <span class="comment"> Load the module :</span><br>
	* <span class="code">
	* theRenderer=new ModuleRenderer(<i>instance_name</i>);<br>
	* theRenderer->Init(KigsCore::Instance(),0);<br>
	* </span>
	* <span class="comment"> Create the object :</span><br>
	* <span class="code">
	* CoreModifiable* MeshObject=(CoreModifiable*)(KigsCore::GetInstanceOf(<i>instance_name</i>,"Mesh"));<br>
	* </span>
	* <span class="comment"> Initialization :</span><br>
	* <span class="code">
	* MeshObject->setValue(LABEL_TO_ID(FileName),<i>file_name.mesh</i>);<br>
	* MeshObject->Init();<br>
	* </span>
	* </dd></dl>
	*
	* <dl class="exported"><dt><b>Exported parameters :</b></dt><dd>
	* <table>
	* <tr><td>string</td>	<td><strong>FileName</strong> :</td>			<td>name of the file to read in load method</td></tr>	
	* <tr><td>bool</td>	<td><strong>VertexNeedUpdate</strong> :</td>	<td>TRUE if myVertex need to be updated</td></tr>
	* <tr><td>bool</td>	<td><strong>ColorNeedUpdate</strong> :</td>		<td>TRUE if myColor need to be updated</td></tr>
	* <tr><td>bool</td>	<td><strong>TexCoordNeedUpdate</strong> :</td>	<td>TRUE if myTexCoord need to be updated</td></tr>
	* <tr><td>bool</td>	<td><strong>NormalNeedUpdate</strong> :</td>	<td>TRUE if myNormal need to be updated</td></tr>
	* <tr><td>bool</td>	<td><strong>ShareMaterial</strong> :</td></tr>
	* <tr><td>*</td>		<td><strong>GetVertexPointer</strong> :</td>	<td>function GetVertexPointer</td></tr>
	* <tr><td>*</td>		<td><strong>GetColorPointer</strong> :</td>		<td>function GetColorPointer</td></tr>
	* <tr><td>*</td>		<td><strong>GetNormalPointer</strong> :</td>	<td>function GetNormalPointer</td></tr>
	* <tr><td>*</td>		<td><strong>GetTexCoordPointer</strong> :</td>	<td>function GetTexCoordPointer</td></tr>
	* </table>
	* </dd></dl>
	*/
	// ****************************************
	class Mesh : public HDrawable
	{
	protected:
		inline void Set_FileName(const char* _value){myFileName=_value;}

	public:
		DECLARE_ABSTRACT_CLASS_INFO(Mesh,HDrawable,Renderer)

		/**
		* \brief	constructor
		* \fn 		Mesh(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
		* \param	name : instance name
		* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
		*/
		Mesh(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

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
		* \fn 		virtual void	GetBoundingBox(Point3D& pmin,Point3D& pmax) const {pmin=myBBoxMin; pmax=myBBoxMax;}
		* \param	pmin : point min of the bounding box (in/out param)
		* \param	pmax : point max of the bounding box (in/out param)
		*/
		void	GetNodeBoundingBox(Point3D& pmin,Point3D& pmax) const override
		{
			pmin = myBoundingBox.m_Min;
			pmax = myBoundingBox.m_Max;
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
		* \class	TexCoord
		* \ingroup TriangleInternalStruct
		* \brief	texture coordinate class
		* \author	ukn
		* \version ukn
		* \date	ukn
		*/
		// ****************************************
		class TexCoord
		{
		public:
			//! coordinate on x axis
			kfloat u;
			//! coordinate on y axis
			kfloat v;

			/**
			* \brief	read u and v value in a file
			* \fn 		void	Load(BufferedFile* currentfile);
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile);
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
		* \class	Triangle
		* \ingroup Triangle
		* \brief	Triangle alone (3 points)
		* \author	ukn
		* \version ukn
		* \date	ukn
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
			void	Load(BufferedFile* currentfile);

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
		* \class	TexturedTriangleStructBase
		* \ingroup TriangleInternalStruct
		* \brief	nothing here, just get needed texture coord count
		* \author	ukn
		* \version ukn
		* \date	ukn
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
		* \class	TexturedTriangleStruct
		* \ingroup TriangleInternalStruct
		* \brief	1 texture struct
		* \author	ukn
		* \version ukn
		* \date	ukn
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile)
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
		* \class	ThreeColorTriangleStruct
		* \ingroup TriangleInternalStruct
		* \brief	3 colors struct
		* \author	ukn
		* \version ukn
		* \date	ukn
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile);
		};

		// ****************************************
		// * OneColorTriangleStruct class
		// * --------------------------------------
		/**
		* \class	OneColorTriangleStruct
		* \ingroup TriangleInternalStruct
		* \brief	1 color struct
		* \author	ukn
		* \version ukn
		* \date	ukn
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile);
		};

		// ****************************************
		// * ThreeNormalTriangleStruct class
		// * --------------------------------------
		/**
		* \class	ThreeNormalTriangleStruct
		* \ingroup TriangleInternalStruct
		* \brief	3 normals struct
		* \author	ukn
		* \version ukn
		* \date	ukn
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile);
		};

		// ****************************************
		// * OneNormalTriangleStruct class
		// * --------------------------------------
		/**
		* \class	OneNormalTriangleStruct
		* \ingroup TriangleInternalStruct
		* \brief	1 Normal struct
		* \author	ukn
		* \version ukn
		* \date	ukn
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile);
		};

		// *******************************************************************
		// And now all possible triangle struct
		// (not all in fact but enougth for now)
		// *******************************************************************
		// ****************************************
		// * C_Triangle class
		// * --------------------------------------
		/**
		* \class	C_Triangle
		* \ingroup Triangle
		* \brief	triangle with 1 color
		* \author	ukn
		* \version ukn
		* \date	ukn
		*/
		// ****************************************
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile)
			{
				Triangle::Load(currentfile);
				OneColorTriangleStruct::Load(currentfile);
			}
		};


		// ****************************************
		// * G_Triangle class
		// * --------------------------------------
		/**
		* \class	G_Triangle
		* \ingroup Triangle
		* \brief	triangle with 3 colors
		* \author	ukn
		* \version ukn
		* \date	ukn
		*/
		// ****************************************
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile)
			{
				Triangle::Load(currentfile);
				ThreeColorTriangleStruct::Load(currentfile);
			}
		};

		// ****************************************
		// * F_Triangle class
		// * --------------------------------------
		/**
		* \class	F_Triangle
		* \ingroup Triangle
		* \brief	triangle with 1 normal
		* \author	ukn
		* \version ukn
		* \date	ukn
		*/
		// ****************************************
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile)
			{
				Triangle::Load(currentfile);
				OneNormalTriangleStruct::Load(currentfile);
			}
		};

		// ****************************************
		// * FC_Triangle class
		// * --------------------------------------
		/**
		* \class	FC_Triangle
		* \ingroup Triangle
		* \brief	triangle with 1 normal and 1 color
		* \author	ukn
		* \version ukn
		* \date	ukn
		*/
		// ****************************************
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile)
			{
				F_Triangle::Load(currentfile);
				OneColorTriangleStruct::Load(currentfile);
			}
		};

		// ****************************************
		// * FG_Triangle class
		// * --------------------------------------
		/**
		* \class	FG_Triangle
		* \ingroup Triangle
		* \brief	triangle with 1 normal and 3 colors
		* \author	ukn
		* \version ukn
		* \date	ukn
		*/
		// ****************************************
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile)
			{
				F_Triangle::Load(currentfile);
				ThreeColorTriangleStruct::Load(currentfile);
			}
		};

		// ****************************************
		// * S_Triangle class
		// * --------------------------------------
		/**
		* \class	S_Triangle
		* \ingroup Triangle
		* \brief	triangle with 3 normals
		* \author	ukn
		* \version ukn
		* \date	ukn
		*/
		// ****************************************
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile)
			{
				Triangle::Load(currentfile);
				ThreeNormalTriangleStruct::Load(currentfile);
			}
		};

		// ****************************************
		// * SC_Triangle class
		// * --------------------------------------
		/**
		* \class	SC_Triangle
		* \ingroup Triangle
		* \brief	triangle with 3 normals and 1 color
		* \author	ukn
		* \version ukn
		* \date	ukn
		*/
		// ****************************************
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile)
			{
				S_Triangle::Load(currentfile);
				OneColorTriangleStruct::Load(currentfile);
			}
		};

		// ****************************************
		// * SG_Triangle class
		// * --------------------------------------
		/**
		* \class	SG_Triangle
		* \ingroup Triangle
		* \brief	triangle with 3 normals and 3 colors
		* \author	ukn
		* \version ukn
		* \date	ukn
		*/
		// ****************************************
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile)
			{
				S_Triangle::Load(currentfile);
				ThreeColorTriangleStruct::Load(currentfile);
			}
		};

		template<unsigned int textureCount>
		// ****************************************
		// * T_Triangle class
		// * --------------------------------------
		/**
		* \class	T_Triangle
		* \ingroup Triangle
		* \brief	triangle with 1 texture
		* \author	ukn
		* \version ukn
		* \date	ukn
		*/
		// ****************************************
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile)
			{
				Triangle::Load(currentfile);
				TexturedTriangleStruct<textureCount>::Load(currentfile);
			}
		};

		template<unsigned int textureCount>
		// ****************************************
		// * TF_Triangle class
		// * --------------------------------------
		/**
		* \class	TF_Triangle
		* \ingroup Triangle
		* \brief	triangle with 1 texture and 1 normal
		* \author	ukn
		* \version ukn
		* \date	ukn
		*/
		// ****************************************
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile)
			{
				F_Triangle::Load(currentfile);
				TexturedTriangleStruct<textureCount>::Load(currentfile);
			}
		};

		template<unsigned int textureCount>
		// ****************************************
		// * TFC_Triangle class
		// * --------------------------------------
		/**
		* \class	TFC_Triangle
		* \ingroup Triangle
		* \brief	triangle with 1 texture and 1 normal and 1 color
		* \author	ukn
		* \version ukn
		* \date	ukn
		*/
		// ****************************************
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile)
			{
				FC_Triangle::Load(currentfile);
				TexturedTriangleStruct<textureCount>::Load(currentfile);
			}
		};

		template<unsigned int textureCount>
		// ****************************************
		// * TFG_Triangle class
		// * --------------------------------------
		/**
		* \class	TFG_Triangle
		* \ingroup Triangle
		* \brief	triangle with 1 texture and 1 normal and 3 colors
		* \author	ukn
		* \version ukn
		* \date	ukn
		*/
		// ****************************************
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile)
			{
				FG_Triangle::Load(currentfile);
				TexturedTriangleStruct<textureCount>::Load(currentfile);
			}
		};

		template<unsigned int textureCount>
		// ****************************************
		// * TS_Triangle class
		// * --------------------------------------
		/**
		* \class	TS_Triangle
		* \ingroup Triangle
		* \brief	triangle with 1 texture and 3 normals
		* \author	ukn
		* \version ukn
		* \date	ukn
		*/
		// ****************************************
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile)
			{
				S_Triangle::Load(currentfile);
				TexturedTriangleStruct<textureCount>::Load(currentfile);
			}
		};

		template<unsigned int textureCount>
		// ****************************************
		// * TSC_Triangle class
		// * --------------------------------------
		/**
		* \class	TSC_Triangle
		* \ingroup Triangle
		* \brief	triangle with 1 texture and 3 normals and 1 color
		* \author	ukn
		* \version ukn
		* \date	ukn
		*/
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile)
			{
				SC_Triangle::Load(currentfile);
				TexturedTriangleStruct<textureCount>::Load(currentfile);
			}
		};

		template<unsigned int textureCount>
		// ****************************************
		// * TSG_Triangle class
		// * --------------------------------------
		/**
		* \class	TSG_Triangle
		* \ingroup Triangle
		* \brief	triangle with 1 texture and 3 normals and 3 colors
		* \author	ukn
		* \version ukn
		* \date	ukn
		*/
		// ****************************************
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
			* \fn 		void	Load(BufferedFile* currentfile)
			* \param	currentfile : BufferedFile to read
			*/	
			void	Load(BufferedFile* currentfile)
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
		const Point3D&	GetVertex(unsigned int index){return VertexArray[index];}

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
		const Vector4D&	GetColor(unsigned int index){return ColorArray[index];}

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
		const Vector3D&	GetNormal(unsigned int index){return NormalArray[index];}

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
		const TexCoord&	GetTexCoord(unsigned int index){return TexArray[index];}

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
		bool		GetVertexPointer(kstl::vector<CoreModifiableAttribute*>& params);
		*/

		DECLARE_METHOD(GetVertexPointer);

		//! link to getColor method
		/*maMethod	myGetColorPointerMethod;
		//! getColor method
		bool		GetColorPointer(kstl::vector<CoreModifiableAttribute*>& params);
		*/
		DECLARE_METHOD(GetColorPointer);


		//! link to getNormal method
		/*maMethod	myGetNormalPointerMethod;
		//! getNormal method
		bool		GetNormalPointer(kstl::vector<CoreModifiableAttribute*>& params);
		*/
		DECLARE_METHOD(GetNormalPointer);

		//! link to getTexCoord method
		/*	maMethod	myGetTexCoordPointerMethod;
		//! getTexCoord method
		bool		GetTexCoordPointer(kstl::vector<CoreModifiableAttribute*>& params);
		*/

		DECLARE_METHOD(GetTexCoordPointer);

		COREMODIFIABLE_METHODS(GetVertexPointer, GetColorPointer, GetNormalPointer, GetTexCoordPointer);

		/**
		* \brief	update the mesh if one of its componants has changed
		* \fn 		virtual void	UpdateMesh(){;}
		*/
		virtual void	UpdateMesh(){;}

		//! TRUE if myVertex need to be updated
		maBool	myVertexNeedUpdate;
		//! TRUE if myColor need to be updated
		maBool	myColorNeedUpdate;
		//! TRUE if myTexCoord need to be updated
		maBool	myTexCoordNeedUpdate;
		//! TRUE if myNormal need to be updated
		maBool	myNormalNeedUpdate;
		//! ?
		maBool	myShareMaterial;

#ifdef _DEBUG
		maBool myShowVertex;
#endif
		maBool myWireMode;
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
		maString	myFileName;

		//! number of vertice
		unsigned int		VertexCount;
		//! List of All Vertice
		Point3D*			VertexArray;	

		//! number of normal
		unsigned int		NormalCount;
		//! List of All normals
		Vector3D*			NormalArray;	

		//! number of Texture coords
		unsigned int		TexCoordCount;
		//! List of all Texture coords
		TexCoord*			TexArray;		

		//! number of color
		unsigned int		ColorCount;
		//! List of All colors
		Vector4D*			ColorArray;

		maBool				myDynamicInit;

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
		BBox          myBoundingBox;

	public:
		friend class ASEMeshLoader;
		friend class BinMeshLoader;
//#ifdef WIN32
		friend class STLMeshLoader;
//#endif
		friend class AABBTreeNode;
		friend class Collision;
		friend class Distance;

		//Setters
		//! Set number of vertice
		inline void Set_VertexCount(unsigned int _value){VertexCount = _value;}

		//! Set List of All Vertice
		inline void Set_VertexArray(Point3D* _value){VertexArray = _value;}	

		//! Set number of normal
		inline void Set_NormalCount(unsigned int _value){NormalCount = _value;}

		//! Set List of All normals
		inline void Set_NormalArray(Vector3D* _value){NormalArray = _value;}	

		//! Set number of Texture coords
		inline void Set_TexCoordCount(unsigned int _value){TexCoordCount = _value;}

		//! Set List of all Texture coords
		inline void Set_TexArray(TexCoord* _value){TexArray = _value;}

		//! Set number of color
		inline void Set_ColorCount(unsigned int _value){ColorCount = _value;}

		//! Set List of All colors
		inline void Set_ColorArray(Vector4D* _value){ColorArray = _value;}

		inline unsigned int Get_NormalCount() const {return NormalCount;}
#ifdef WIN32
		void ExportMeshTo(kstl::string _pathDirectory, kstl::string _fileName);
#endif

		void	RecomputeNormals();
		void	RecomputeNormalsFast();
	};

#ifdef _3DSMAX

} // namespace KigsFramework

#endif

#endif // _KDMESH_H_
