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
		class Material;
		class Mesh;
		class MeshItemGroup;
	}
	namespace Utils
	{
		using namespace Kigs::Core;
		/**
				Parse an obj file
		*/

		class STLImport : public CoreModifiable
		{
		public:
			DECLARE_CLASS_INFO(STLImport, CoreModifiable, Renderer);

			//! builds an list of kmesh
			STLImport(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			std::vector<CMSP>& GetMeshes()
			{
				return m_MeshList;
			}
#ifdef WIN32
			void ExportResult(); // used by edit
#endif //WIN32
			virtual ~STLImport();
		protected:

			struct readFacet
			{
				Vector3D	mNormal;
				Point3D		mVertex[3];
			};

			bool	CheckAscii(unsigned char* buffer, unsigned int l);

			void	createObjectFromReadedData();

			virtual void	InitModifiable();

			maString								m_FileName;

			std::vector<CMSP>			m_MeshList;

			void	ReinitReadBuffers();

			DynamicGrowingBuffer<readFacet>*	m_ReadTriangleBuffer;
			unsigned int						m_ReadTriangleIndex;


			std::string							m_CurrentObjectName;
		};
	}
}