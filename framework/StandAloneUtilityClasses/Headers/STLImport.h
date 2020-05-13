#ifndef _STLIMPORT_H_
#define _STLIMPORT_H_


#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "maReference.h"
#include "AsciiParserUtils.h"
#include "DynamicGrowingBuffer.h"
/**
		Parse an obj file
*/

class Mesh;
class Material;
class MeshItemGroup;

class STLImport : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(STLImport,CoreModifiable,Renderer);
	
	//! builds an list of kmesh
	STLImport(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	kstl::vector<CMSP>&	GetMeshes()
	{
		return m_MeshList;
	}
#ifdef WIN32
	void ExportResult(); // used by edit
#endif //WIN32
protected:

	struct readFacet
	{
		Vector3D	mNormal;
		Point3D		mVertex[3];
	};

	bool	CheckAscii(unsigned char* buffer, unsigned int l);

	void	createObjectFromReadedData();

	virtual void	InitModifiable();
	virtual ~STLImport();
	maString								m_FileName;

	kstl::vector<CMSP>			m_MeshList;

	void	ReinitReadBuffers();

	DynamicGrowingBuffer<readFacet>*		m_ReadTriangleBuffer;
	unsigned int							m_ReadTriangleIndex;


	kstl::string							m_CurrentObjectName;
};
#endif //_STLIMPORT_H_