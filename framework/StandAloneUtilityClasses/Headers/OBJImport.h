#ifndef _OBJIMPORT_H_
#define _OBJIMPORT_H_


#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "maReference.h"
#include "AsciiParserUtils.h"
#include "DynamicGrowingBuffer.h"
/**
		Parse an obj file
*/

class Mesh;
struct Point3D;
class Material;
class MeshItemGroup;



class OBJImport : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(OBJImport,CoreModifiable,Renderer);
	
	//! builds an list of kmesh
	OBJImport(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	kstl::vector<CMSP>&	GetMeshes()
	{
		return m_MeshList;
	}
#ifdef WIN32
	void ExportResult(); // used by edit
#endif //WIN32
protected:

	class ReadMaterial
	{
	public:
		ReadMaterial();
		~ReadMaterial();
		void	Init();
		float	Ka[3],Kd[3],Ks[3],d,Ns;
		kstl::string	name;
		int	illum;
		kstl::string	textureName;
		SP<Material>		m_Material;
	};

	class IndexTrio
	{
	public:
		IndexTrio() : iv(-1),it(-1),in(-1)
		{}
		int iv,it,in;
	};

	class Triangle
	{
	public:
		IndexTrio	indexes[3];
	};

	class FacetGroup
	{
	public:

		FacetGroup():matName(""),m_ReadFacetBuffer(nullptr),m_ReadFacetCount(0),m_HasNormal(true),m_HasTextCoords(true)
		{

		}

		~FacetGroup()
		{
			if(m_ReadFacetBuffer)
			{
				delete m_ReadFacetBuffer;
			}
		}

		void	InitTriangle(MeshItemGroup* mig,int startTriangleIndex);
		void	InitSTriangle(MeshItemGroup* mig,int startTriangleIndex);
		void	InitSGTriangle(MeshItemGroup* mig,int startTriangleIndex);
		void	InitTTriangle(MeshItemGroup* mig,int startTriangleIndex);
		void	InitSTTriangle(MeshItemGroup* mig,int startTriangleIndex);

		kstl::string							matName;
		DynamicGrowingBuffer<Triangle>*			m_ReadFacetBuffer;
		unsigned int							m_ReadFacetCount;
		bool									m_HasNormal;
		bool									m_HasTextCoords;
		
	};

	kstl::map<kstl::string,ReadMaterial>	m_materialList;
	ReadMaterial*							m_currentMatRead;

	void	MTLImport(const kstl::string& name);

	// parse specific line
	void	StartObj(AsciiParserUtils& line);
	void	StartGroup(AsciiParserUtils& line);
	void	ParseVertex(AsciiParserUtils& line);
	void	ParseTextureCoord(AsciiParserUtils& line);
	void	ParseNormal(AsciiParserUtils& line);
	void	ParseFacet(AsciiParserUtils& line);

	void	RetreiveShortNameAndExt(const kstl::string& filename, kstl::string& shortname, kstl::string& fileext, kstl::string& filepath);

	void	createObjectFromReadedData();

	virtual void	InitModifiable();
	virtual ~OBJImport();
	maString								m_FileName;
	maReference								m_FirstMesh;
	maBool									m_ModernMesh;

	kstl::vector<CMSP>			m_MeshList;

	void	ReinitReadBuffers();

	kstl::map<kstl::string,FacetGroup>		m_FacetGroupList;
	FacetGroup*								m_CurrentFacetGroup;
	DynamicGrowingBuffer<Point3D>*			m_ReadVertexBuffer;
	unsigned int							m_ReadVertexIndex;
	DynamicGrowingBuffer<Point3D>*			m_ReadTextCoordsBuffer;
	unsigned int							m_ReadTextCoordIndex;
	DynamicGrowingBuffer<Point3D>*			m_ReadNormalBuffer;
	unsigned int							m_ReadNormalIndex;
	DynamicGrowingBuffer<Vector4D>*			m_ReadColorBuffer;
	unsigned int							m_ReadColorIndex;


	kstl::list<kstl::string> m_TextureList;
	maString								m_ExportPath;

	kstl::string							m_CurrentObjectName;
};
#endif //_OBJIMPORT_H_