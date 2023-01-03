#pragma once

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "maReference.h"
#include "AsciiParserUtils.h"
#include "DynamicGrowingBuffer.h"

namespace Kigs
{
	namespace Draw
	{
		class Mesh;
		class Material;
		class MeshItemGroup;
	}
	namespace Maths
	{
		struct Point3D;
	}
	namespace Utils
	{
		
		using namespace Kigs::Core;

		/**
				Parse an obj file
		*/


		class OBJImport : public CoreModifiable
		{
		public:
			DECLARE_CLASS_INFO(OBJImport, CoreModifiable, Renderer);

			//! builds an list of kmesh
			OBJImport(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			std::vector<CMSP>& GetMeshes()
			{
				return m_MeshList;
			}
#ifdef WIN32
			void ExportResult(); // used by edit
#endif //WIN32
			virtual ~OBJImport();
		protected:

			class ReadMaterial
			{
			public:
				ReadMaterial();
				~ReadMaterial();
				void	Init();
				float	Ka[3], Kd[3], Ks[3], d, Ns;
				std::string	name;
				int	illum;
				std::string	textureName;
				SP<Draw::Material>		m_Material;
			};

			class IndexTrio
			{
			public:
				IndexTrio() : iv(-1), it(-1), in(-1)
				{}
				int iv, it, in;
			};

			class Triangle
			{
			public:
				IndexTrio	indexes[3];
			};

			class FacetGroup
			{
			public:

				FacetGroup() :matName(""), m_ReadFacetBuffer(nullptr), m_ReadFacetCount(0), m_HasNormal(true), m_HasTextCoords(true)
				{

				}

				~FacetGroup()
				{
					if (m_ReadFacetBuffer)
					{
						delete m_ReadFacetBuffer;
					}
				}

				void	InitTriangle(Draw::MeshItemGroup* mig, int startTriangleIndex);
				void	InitSTriangle(Draw::MeshItemGroup* mig, int startTriangleIndex);
				void	InitSGTriangle(Draw::MeshItemGroup* mig, int startTriangleIndex);
				void	InitTTriangle(Draw::MeshItemGroup* mig, int startTriangleIndex);
				void	InitSTTriangle(Draw::MeshItemGroup* mig, int startTriangleIndex);

				std::string							matName;
				DynamicGrowingBuffer<Triangle>* m_ReadFacetBuffer;
				unsigned int							m_ReadFacetCount;
				bool									m_HasNormal;
				bool									m_HasTextCoords;

			};

			std::map<std::string, ReadMaterial>	m_materialList;
			ReadMaterial* m_currentMatRead;

			void	MTLImport(const std::string& name);

			// parse specific line
			void	StartObj(AsciiParserUtils& line);
			void	StartGroup(AsciiParserUtils& line);
			void	ParseVertex(AsciiParserUtils& line);
			void	ParseTextureCoord(AsciiParserUtils& line);
			void	ParseNormal(AsciiParserUtils& line);
			void	ParseFacet(AsciiParserUtils& line);

			void	RetreiveShortNameAndExt(const std::string& filename, std::string& shortname, std::string& fileext, std::string& filepath);

			void	createObjectFromReadedData();

			virtual void	InitModifiable();

			maString								m_FileName;
			maReference								m_FirstMesh;
			maBool									m_ModernMesh;

			std::vector<CMSP>			m_MeshList;

			void	ReinitReadBuffers();

			std::map<std::string, FacetGroup>		m_FacetGroupList;
			FacetGroup* m_CurrentFacetGroup;
			DynamicGrowingBuffer<Point3D>* m_ReadVertexBuffer;
			unsigned int							m_ReadVertexIndex;
			DynamicGrowingBuffer<Point3D>* m_ReadTextCoordsBuffer;
			unsigned int							m_ReadTextCoordIndex;
			DynamicGrowingBuffer<Point3D>* m_ReadNormalBuffer;
			unsigned int							m_ReadNormalIndex;
			DynamicGrowingBuffer<Vector4D>* m_ReadColorBuffer;
			unsigned int							m_ReadColorIndex;


			std::list<std::string> m_TextureList;
			maString								m_ExportPath;

			std::string							m_CurrentObjectName;
		};
	}
}