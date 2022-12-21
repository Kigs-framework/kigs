#ifndef _PLYIMPORT_H_
#define _PLYIMPORT_H_


#include "Core.h"
#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "AsciiParserUtils.h"
#include "DynamicGrowingBuffer.h"
/**
Parse a binary ply file
*/
class Material;
class ModernMesh;
class ModernMeshItemGroup;

// only manage binary "classic" format at the moment:
// 3 float position
// 4 unsigned char color
// 3 float normal

class PLYImport : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(PLYImport, CoreModifiable, Renderer);

	//! builds an list of kmesh
	PLYImport(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	std::vector<CMSP>&	GetMeshes()
	{
		return m_MeshList;
	}
	virtual ~PLYImport();
protected:

	virtual void	InitModifiable();
	
	maString								m_FileName;
	
	enum propertyType
	{
		propertyUnknown = 0,
		propertyFloat = 1,
		propertyInt = 2,
		propertyChar = 3,
		propertyList = 4
	};

	struct propertyDesc
	{
		propertyType	m_type;
		std::string	m_propertyname;
		propertyType	m_ListIndexSize; // only for list desc
		propertyType	m_ListIndexType; // only for list desc
	};

	struct elementDesc
	{
		std::string							m_typename;
		std::vector<propertyDesc>				m_properties;
		unsigned int							m_count;

		void	addProperty(AsciiParserUtils& line);
	};

	std::vector<elementDesc>					m_ElementDescriptors;

	SP<ModernMeshItemGroup>	readBinData(unsigned char* binData,ModernMesh*);
	SP<ModernMeshItemGroup>	readAsciiData(AsciiParserUtils& asciiData, ModernMesh*);

	class ColoredPoint3D : public Point3D
	{
	public:
		unsigned int	m_color;
	};

	class TriangleStruct
	{
	public:
		unsigned int	a, b, c;
	};

	static propertyType getTypeFromString(const std::string w)
	{
		if ((w == "float") || (w == "float32"))
		{
			return propertyFloat;
		}
		
		if ((w == "int") || (w == "int32") || (w == "uint32"))
		{
			return propertyInt;
		}
		
		if ((w == "char") || (w == "int8") || (w == "uint8"))
		{
			return propertyChar;
		}

		if (w == "list")
		{
			return propertyList;
		}
		return propertyUnknown;
	}

	static int	getSizeOfType(const propertyType& ptype)
	{
		switch (ptype)
		{
		case propertyFloat:
			return 4;
		case propertyInt:
			return 4;
		case propertyChar:
			return 1;

		}

		return 0;
	}

	// return color pos (in byte) or -1 if no color
	static int	hasColor(const elementDesc& desc)
	{
		return getPropertyPos(desc, "red");
	}

	// return normal pos (in byte) or -1 if no color
	static int	hasNormal(const elementDesc& desc)
	{
		return getPropertyPos(desc, "nx");
	}

	// return pos (in byte) of given property or -1 if not found
	static int getPropertyPos(const elementDesc& desc, const std::string& pname)
	{
		int returnedPos = 0;

		std::vector<propertyDesc>::const_iterator	itpc = desc.m_properties.begin();
		std::vector<propertyDesc>::const_iterator	itpe = desc.m_properties.end();

		while (itpc != itpe)
		{
			if ((*itpc).m_propertyname == pname)
			{
				return returnedPos;
			}

			returnedPos += getSizeOfType((*itpc).m_type);

			++itpc;
		}
		

		return -1;
	}

	// sum all property size (must be float int or char, no list)
	static int getElementSize(const elementDesc& desc)
	{
		int returnedSize = 0;

		std::vector<propertyDesc>::const_iterator	itpc = desc.m_properties.begin();
		std::vector<propertyDesc>::const_iterator	itpe = desc.m_properties.end();

		while (itpc != itpe)
		{
			returnedSize += getSizeOfType((*itpc).m_type);
			++itpc;
		}

		return returnedSize;
	}

	bool						m_isASCII;

	std::vector<CMSP>			m_MeshList;
	std::string							m_CurrentObjectName;
};
#endif //_PLYIMPORT_H_
