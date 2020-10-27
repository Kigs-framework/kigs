#ifndef __COLLADA_PARSER_H__
#define __COLLADA_PARSER_H__

#include "CoreModifiable.h"
#include "TecLibs\Tec3D.h"
#include "DynamicGrowingBuffer.h"
#include "Core.h"
#include "CoreModifiableAttribute.h"
#include "Material.h"
#include "Node3D.h"
#include "ASystem.h"
#include "Bones/APRSStream.h"
#include "Bones/APRSKeyStream.h"
#include "ModernMesh.h"
#include "Base3DImporter.h"


#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "Bones/APRSStream.h"

#include	<stack>
#include	<string>
#include	<sstream>

// ****************************************
// * ColladarParser class
// * --------------------------------------
/**
* \file	ColladaParser.h
* \class	ColladaParser
* \ingroup StandAloneUtility
* \brief	 Class used to parse collada files (.dae)
* \author	König Jolan
* \version ukn
* \date	25/05/16
* This class is used to parsed a collada file. To get a node3D who contains the entire scene, use GetRootNode,
if you just want object 3D without light or camera use getRootNodeJustWithMeshes
*/
// ****************************************
class Scene3D;

struct vertex_weights
{
	unsigned char index[4];
	unsigned char weight[4];
};
struct Controller
{
	std::string id;
	std::vector<std::string> sids;
	std::vector<Matrix3x4> joints_inv_bind_matrix;
	
	//Per vertex weights
	std::vector<vertex_weights> weights;
	std::vector<std::pair<int, XMLNodeBase*>> joints_table;
	
	XMLNodeBase* skin_node;
};

struct ChannelData
{
	unsigned int				keyCount;
	std::vector<std::string>	interpolations;
	std::vector<PositionKey>	positionKeys;
	std::vector<RotationKey>	rotationKeys;
	std::vector<ScaleKey>		scaleKeys;
	std::string				channelTarget;
	kfloat length;
};

struct AnimationData
{
	XMLNodeBase* source_node;
	std::vector<AnimationData*> sub_anims;
	std::vector<ChannelData> channels;
};



class SourceData
{
	public:
	
	SourceData(XMLNodeBase* source_node);
	~SourceData();
	
	SourceData(const SourceData&) = delete;
	SourceData(SourceData&& other) = delete;
	SourceData& operator=(const SourceData& other) = delete;
	SourceData& operator=(SourceData && other) = delete;
	
	enum SourceType
	{
		FloatArray,
		MatrixArray,
		Vector2DArray,
		Vector3DArray,
		Vector4DArray,
		NameArray,
	} type;
	
	
	template<typename T> T at(int index)
	{
		auto& buffer = *(DynamicGrowingBuffer<T>*)(dyn_buffer);
		return buffer[index];
	}
	
	template<> std::string at(int index)
	{
		auto& buffer = *(DynamicGrowingBuffer<char[128]>*)(dyn_buffer);
		return buffer[index];
	}
	
	
	XMLNodeBase* source_node;
	int item_count;
	
	private:
	void* dyn_buffer=nullptr;
};


class ColladaParser : public Base3DImporter
{
	public:
	DECLARE_CLASS_INFO(ColladaParser, Base3DImporter,Renderer);
	
	//! builds an list of kmesh
	ColladaParser(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	
	virtual ~ColladaParser();
	
	SP<Node3D>	getRootNode()
	{
		return RootNode;
	}
	
	//Entire scene, with lights and everything
	CMSP getScene(){return myScene;}
	
	
	//Textures name list, for copy texture if needed
	std::list<std::string> getTextureNameList() { return m_TextureList; }
		
protected:
	//functions CoreModifiable
	virtual void	InitModifiable();
	
	// specific parser
	void	ParseAsset();
	void	ParseScene();
	void	ParseVScene(XMLNodeBase* vscene);
	
	CMSP ParseNode(XMLNodeBase* scenenode, bool &isModernMeshRoot);
	CMSP ParseGeometry(XMLNodeBase* geomnode);
	CMSP ParseCamera(XMLNodeBase* cameranode);
	CMSP ParseLight(XMLNodeBase* cameranode);
	CMSP ParseMesh(XMLNodeBase* meshnode, const std::string& name, Controller* controller);
	CMSP ParseInstanceController(XMLNodeBase* controller_node);
	Controller* ParseController(std::string url);
	SourceData* ParseSource(XMLNodeBase* sourceNode);
	AnimationData* ParseAnimation(XMLNodeBase* currentAnimationNode);
	bool ParseAnimationClips();
	void ParseStandaloneAnimations();	
	
	void CreateChannel(AnimationData* anim, const std::string& target, SourceData* time_source, SourceData* matrix_source,SourceData* interpolation_source);
	
	CMSP CreateMeshFromMeshCollada(Controller* controller);

	// Parse 3x4 transform matrix 
	Matrix3x4 ParseTransform(XMLNodeBase* scenenode);
	// Parse skinning weights
	void ParseWeights(Controller* controller);
	
	void CalcTransformRec(Controller* controller, std::vector<std::pair<int, XMLNodeBase*>>& joints, XMLNodeBase* current, const Matrix3x4& parent_transform, const Matrix3x4& parent_inv);
	
	void ParseBindMaterial(XMLNodeBase* bindMaterialNode);
	void ParseEffect(XMLNodeBase* effectNode, std::string materialName);
	//Parse Faces
	void parseFacetPolylist(XMLNodeBase* vertexCountNode, XMLNodeBase* indexesNode, std::map<int, std::vector<char>> offsets);
	void parseFacetTriangle(XMLNodeBase* indexesNode, std::map<int, std::vector<char>> offsets);
	void parseFacetPolygons(XMLNodeBase* indexesNode, std::map<int, std::vector<char>> offsets);
	void parseFacetTrifans(XMLNodeBase* indexesNode, std::map<int, std::vector<char>> offsets);
	
	
	//Tools
	Point3D ReadVector3D(XMLNodeBase* sceneNode);
	float ReadFloat(XMLNodeBase* sceneNode);
	Vector4D ReadVector4D(XMLNodeBase* sceneNode);
	Quaternion ReadQuaternion(XMLNodeBase* sceneNode);
	Matrix3x4 ReadMatrix3x4(XMLNodeBase* sceneNode);
	
	void	RetrieveShortFileName(const std::string& filename, std::string& shortname);
	
	class IndexTrio
	{
		public:
		IndexTrio() : iv(-1), it(-1), in(-1), ic(-1), ig(-1), ib(-1)
		{}
		int iv, it, in, ic, ig, ib;
	};
	
	class Triangle
	{
		public:
		IndexTrio	indexes[3];
	};
	class FacetGroup
	{
		public:
		
		FacetGroup() :matName(""), m_ReadFacetBuffer(nullptr), m_ReadFacetCount(0), m_HasNormal(false), m_HasTextCoords(false), m_HasColor(false)
		{
			
		}
		
		~FacetGroup()
		{
			if (m_ReadFacetBuffer)
			{
				delete m_ReadFacetBuffer;
			}
		}
		
		std::string							matName;
		
		void setColorUrl(std::string url) { colorUrl = url.substr(1, url.length() - 1); }
		void setPositionUrl(std::string url) { positionUrl = url.substr(1, url.length() - 1); }
		void setNormalUrl(std::string url) { normalUrl = url.substr(1, url.length() - 1); }
		void setTextureCoordUrl(std::string url) { textureCoordUrl = url.substr(1, url.length() - 1); }
		void setTangentUrl(std::string url) { tangentUrl = url.substr(1, url.length() - 1); }
		void setBinormalUrl(std::string url) { binormalUrl = url.substr(1, url.length() - 1); }
		
		//To find the good source in the meshCollada parent
		DynamicGrowingBuffer<Triangle>*			m_ReadFacetBuffer;
		unsigned int							m_ReadFacetCount;
		
		bool									m_HasNormal;
		bool									m_HasTextCoords;
		bool									m_HasColor;
		bool									m_HasTangent;
		bool									m_hasBinormal;
		
		std::string positionUrl, normalUrl, textureCoordUrl, colorUrl, tangentUrl, binormalUrl;
	};
	

	class MeshCollada
	{
		public:
		
		MeshCollada() : m_ReadFacetGroupCount(0), m_CurrentFacetGroup(nullptr), m_ReadFacetGroupBuffer(nullptr)
		{
			m_ReadFacetGroupBuffer = new DynamicGrowingBuffer<FacetGroup*>(512);
		}

		void ClearThis()
		{
			if (m_ReadFacetGroupBuffer) {
				m_CurrentFacetGroup = nullptr;

				for (auto i = 0; i < m_ReadFacetGroupBuffer->size(); i++)
				{
					delete (*m_ReadFacetGroupBuffer)[i];
				}
				delete m_ReadFacetGroupBuffer;
			}
			/*std::map<std::string, SourceData*>::iterator itmat = m_SourceBufferList.begin();
			std::map<std::string, SourceData*>::iterator itmatend = m_SourceBufferList.end();

			while (itmat != itmatend)
			{
				delete (*itmat).second;
				++itmat;
			}*/

			m_SourceBufferList.clear();
		}

		~MeshCollada() {
			
			ClearThis();
			
		}
		
		//id of the source
		std::map<std::string, SourceData*>		m_SourceBufferList;
		
		DynamicGrowingBuffer<FacetGroup*>*			m_ReadFacetGroupBuffer;
		unsigned int m_ReadFacetGroupCount;
		FacetGroup* m_CurrentFacetGroup;
	};
	class ReadMaterial
	{
		public:
		ReadMaterial();
		~ReadMaterial();
		void	Init();
		float	ambiantColor[3], emissionColor[3], diffuseColor[3], specularColor[3], shininess, indexOfRefraction, transparency;
		std::string	name;
		std::vector<std::string>	texturesName;
		std::vector<std::string>	NormalMapName;
		
		SP<Material>		m_Material;
	};
	
	
	//parameters
	maString									m_FileName;
	
	//Object used to parse
	XMLBase*									myXMLFile;
	XMLNodeBase*								myRoot;
	DynamicGrowingBuffer<MeshCollada>*			m_MeshColladaList;
	MeshCollada* m_CurrentMeshCollada;
	std::string								m_CurrentObjectName;
	ReadMaterial*								m_currentMatRead;
	
	
	//Objects Created
	CMSP								myScene;
	
	std::map<std::string, ReadMaterial>		m_materialList;
	std::list<std::string>					m_TextureList;
	std::map<std::string, CMSP>	m_MeshList;
	std::vector<Controller>					m_ParsedControllers;
	
	std::map<XMLNodeBase*, AnimationData> m_animations;
	std::map<XMLNodeBase*, SourceData> m_sources;
	
	SP<Node3D>										RootNode;
	
	std::string								shortFileName;
	
};

#endif //__COLLADA_PARSER_H__

