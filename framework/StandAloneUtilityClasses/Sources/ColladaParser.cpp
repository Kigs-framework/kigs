#include "PrecompiledHeaders.h"

#include "ColladaParser.h"
#include "XML.h"
#include "XMLNode.h"
#include "XMLAttribute.h"
#include "Core.h"
#include "Node3D.h"
#include "AsciiParserUtils.h"
#include "Camera.h"
#include "Light.h"
#include "Texture.h"
#include "TextureFileManager.h"
#include "ModernMesh.h"
#include "CoreBaseApplication.h"
#include "Bones/AObjectSkeletonResource.h"
#include "Bones/APRSStream.h"
#include "AnimationResourceInfo.h"
#include "Bones/APRSKeyStream.h"

#include "XML.h"


#include <queue>
#include <numeric>

IMPLEMENT_CLASS_INFO(ColladaParser)

ColladaParser::ColladaParser(const std::string& name, CLASS_NAME_TREE_ARG) : Base3DImporter(name, PASS_CLASS_NAME_TREE_ARG)
, myRoot(0)
, myXMLFile(0)
, myScene(0)
, m_FileName(*this, true, "FileName")
{
}
void	ColladaParser::InitModifiable()
{
	Base3DImporter::InitModifiable();
	if (_isInit)
	{
		myXMLFile = XML::ReadFile(m_FileName, "xml");
		if (myXMLFile != nullptr)
		{
			RetrieveShortFileName(m_FileName, shortFileName);
			
			m_MeshColladaList = new DynamicGrowingBuffer<MeshCollada>(140);
			myRoot = myXMLFile->getRoot();
			
			ParseAsset();
			
			// Parse individual animations if no animation clips available
			if(!ParseAnimationClips())
			{
				ParseStandaloneAnimations();
			}
			
			ParseScene();

			return;
		}
	}
	Base3DImporter::UninitModifiable();
}
ColladaParser::~ColladaParser()
{
	if (myScene)
	{
		myScene = nullptr;
	}
	if (myXMLFile)
	{
		delete myXMLFile;
	}
	if (m_MeshColladaList)
	{
		delete m_MeshColladaList;
	}
	myRoot = nullptr;
	m_materialList.clear();
}




static std::string GetNodeName(XMLNodeBase* scenenode)
{
	std::string nodeName;
	XMLAttributeBase* name = scenenode->getAttribute("name");
	if (name != nullptr)
	{
		nodeName = name->getString();
	}
	else
	{
		XMLAttributeBase* id = scenenode->getAttribute("id");
		if (id != nullptr)
		{
			nodeName = id->XMLAttributeBase::getString();
		}
		else
		{
			nodeName = "node";
		}
	}
	return nodeName;
}


static std::string GetNodeUniqueID(XMLNodeBase* scenenode)
{
	std::string nodeUniqueID;
	XMLAttributeBase* id = scenenode->getAttribute("id");
	if (id != nullptr)
	{
		nodeUniqueID = id->getString();
	}
	else
	{
		XMLAttributeBase* name = scenenode->getAttribute("name");
		if (name != nullptr)
		{
			nodeUniqueID = name->getString();
		}
		else
		{
			nodeUniqueID = "node";
		}
	}
	return nodeUniqueID;
}


void	ColladaParser::RetrieveShortFileName(const std::string& filename, std::string& shortname)
{
	int pos = static_cast<int>(filename.rfind("/")) + 1;
	int pos1 = static_cast<int>(filename.rfind("\\")) + 1;
	
	if (pos1 > pos)
	{
		pos = pos1;
	}
	
	bool	filenameIsShortfilename = false;
	
	if (pos == 0)
	{
		shortname = filename;
		filenameIsShortfilename = true;
	}
	else
	{
		shortname = filename.substr(static_cast<unsigned int>(pos), filename.length() - pos);
	}
	
	std::string fileext = "";
	pos = static_cast<int>(shortname.rfind(".")) + 1;
	if (pos)
	{
		fileext.append(shortname, static_cast<unsigned int>(pos), shortname.length() - pos);
		shortname = shortname.substr(0, pos - 1);
	}
}



static XMLNodeBase* SearchNode(XMLNodeBase* father, const std::string& name, const std::string& param, const std::string& value, bool recursive=false)
{
	XMLNodeBase* result = nullptr;
	int ccount = father->getChildCount();
	for (auto i = 0; i < ccount; i++)
	{
		if (father->getChildElement(i) != nullptr)
		{
			auto check = father->getChildElement(i);
			if (check->getName() == name)
			{
				if (param != "")
				{
					if (check->getAttribute(param) != nullptr)
					{
						XMLAttributeBase* checkattr = check->getAttribute(param);
						if (checkattr->getString() == value)
						{
							result = check;
							break;
						}
					}
				}
				else
				{
					result = check;
					break;
				}
			}
			if(recursive)
			{
				result = SearchNode(check, name, param, value, true);
				if(result) break;
			}
		}			
	}
	
	return result;
}


void ColladaParser::ParseAsset()
{
	auto asset = myRoot->getChildElement("asset");
}

void ColladaParser::ParseScene()
{
	XMLNodeBase* scene = myRoot->getChildElement("scene");
	if (scene != nullptr)
	{
		// Parse visual scene
		auto vscene = scene->getChildElement("instance_visual_scene");
		if (vscene)
		{
			ParseVScene(vscene);
		}
		// Parse physics scene
		auto pscene = scene->getChildElement("instance_physics_scene");
		if (pscene != nullptr)
		{
			printf("TO DO PARSE PHYSICS SCENE\n");
		}
	}
}

std::string make_string(const std::string_view& sv)
{
	return std::string(sv.data(), sv.size());
}

void ColladaParser::ParseVScene(XMLNodeBase* vscene)
{
	auto sceneurl = vscene->getAttribute("url");
	if (sceneurl != nullptr)
	{
		std::string url = make_string(sceneurl->getString());
		auto scene_lib = myRoot->getChildElement("library_visual_scenes");
		if ((url.at(0) == '#') && scene_lib != nullptr)
		{
			url = url.substr(1, url.length() - 1);
			
			auto current_scene = SearchNode(scene_lib, "visual_scene", "id", url);
			if (current_scene != nullptr)
			{
				myScene = KigsCore::GetInstanceOf(url, "Scene3D");
				myScene->Init();
				RootNode = KigsCore::GetInstanceOf(url + "_RootNode", "Node3D");
				RootNode->Init();
				
				myScene->addItem(RootNode);
				
				
				// add each node to the scene
				int ncount = current_scene->getChildCount();
				
				if (ncount)
				{
					for (auto i = 0; i < ncount; i++)
					{
						bool isModernMeshRoot=false;
						if (current_scene->getChildElement(i) != nullptr)
						{
							CMSP toAdd = ParseNode(current_scene->getChildElement(i), isModernMeshRoot); 
							
							if (toAdd != nullptr)
							{
								RootNode->addItem(toAdd);
								toAdd->Init();
							}
						}
						
					}
				}
			}
		}
	}
	
}


CMSP ColladaParser::ParseCamera(XMLNodeBase* cameranode)
{
	SP<Camera> toAdd = nullptr;
	XMLAttributeBase* cameraurl = cameranode->getAttribute("url");
	if (cameraurl != nullptr)
	{
		std::string url = make_string(cameraurl->getString());
		XMLNodeBase* cam_lib = myRoot->getChildElement("library_cameras");
		
		if ((url.at(0) == '#') && cam_lib != nullptr)
		{
			url = url.substr(1, url.length() - 1);
			XMLNodeBase* current_cam = SearchNode(cam_lib, "camera", "id", url);
			if (current_cam != nullptr)
			{
				toAdd = KigsCore::GetInstanceOf(url, "Camera");
				toAdd->SetUpVector(0, 1, 0);
				toAdd->SetViewVector(0, 0, -1);
				
				// add each node to the scene
				int ncount = current_cam->getChildCount();
				
				if (ncount)
				{
					int i;
					for (i = 0; i < ncount; i++)
					{
						if (current_cam->getChildElement(i) != nullptr)
						{
							// search optics options
							if (current_cam->getChildElement(i)->getName() == "optics")
							{
								if (current_cam->getChildElement("technique_common") != nullptr)
								{
									XMLNodeBase* currentTechniqueCommon = current_cam->getChildElement("technique_common");
									if (currentTechniqueCommon->getChildElement(0) != nullptr)
									{
										XMLNodeBase* sonNode = currentTechniqueCommon->getChildElement(0);
										if (sonNode->getName() == "perspective")
										{
											float znear = sonNode->getChildElement("znear")->getChildElement(0)->getFloat();
											
											float zfar = sonNode->getChildElement("zfar")->getChildElement(0)->getFloat();
											
											
											toAdd->SetNearAndFar(znear, zfar);
											
											if (sonNode->getChildElement("aspect_ratio") != nullptr)
											{
												float ar = sonNode->getChildElement("aspect_ratio")->getChildElement(0)->getFloat();

												toAdd->setValue("AspectRatio", ar);
												if (sonNode->getChildElement("yfov") != nullptr)
												{
													toAdd->setValue("VerticalFOV", sonNode->getChildElement("yfov")->getChildElement(0)->getFloat());
												}
												else if (sonNode->getChildElement("xfov") != nullptr)
												{
													toAdd->setValue("VerticalFOV", sonNode->getChildElement("xfov")->getChildElement(0)->getFloat() / ar);
												}
											}
											else if (sonNode->getChildElement("yfov") != nullptr && sonNode->getChildElement("xfov") != nullptr)
											{
												toAdd->setValue("AspectRatio", sonNode->getChildElement("xfov")->getChildElement(0)->getFloat() / sonNode->getChildElement("yfov")->getChildElement(0)->getFloat());
												toAdd->setValue("VerticalFOV", sonNode->getChildElement("yfov")->getChildElement(0)->getFloat());
											}
											else
											{
												printf("TO DO CALCULATE ASPECT RATIO WITH VIEWPORT ASPECT_RATIO and xfov OR yfov\n");
												/*See https://www.khronos.org/files/collada_spec_1_5.pdf#page=138&zoom=auto,0,709 P149*/
											}
										}
										else
										{
											printf("TO DO Camera orthographique\n");
										}
									}
								}
							}
							else if (current_cam->getChildElement(i)->getName() == "asset")
							{
								/*TODO Information de position ect*/
								if (current_cam->getChildElement(i) != nullptr)
								{
									XMLNodeBase* current_ass = current_cam->getChildElement(i);
									int nCountAsset = current_ass->getChildCount();
									for (auto j = 0; j < nCountAsset; j++)
									{
										if (current_ass->getChildElement(j)->getName() == "up_axis")
										{
											if (current_ass->getChildElement(j)->getChildElement(0)->getString() == "Y_UP")
											{
												toAdd->SetUpVector(0, 1, 0);
											}
											else if (current_ass->getChildElement(j)->getChildElement(0)->getString() == "X_UP")
											{
												
												toAdd->SetUpVector(1, 0, 0);
											}
											else if (current_ass->getChildElement(j)->getChildElement(0)->getString() == "Z_UP")
											{
												toAdd->SetUpVector(0, 0, 1);
											}
											else
											{
												printf("ERROR: wrong value for camera->asset->upAxis\n");
											}
										}
										else
										{
											printf("TO DO Parse in asset camera: %s\n", make_string(current_ass->getChildElement(j)->getName()).c_str());
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return toAdd;
}



/*To FINISH, not the same attributes from collada than from the framework, over this*/
CMSP ColladaParser::ParseLight(XMLNodeBase* lightnode)
{
	SP<Light> toAdd = nullptr;
	
	XMLAttributeBase* lighturl = lightnode->getAttribute("url");
	
	if (lighturl)
	{
		std::string url = make_string(lighturl->getString());
		std::string name = "";
		XMLAttributeBase* lightnameattrib = lightnode->getAttribute("name");
		if (lightnameattrib)
		{
			name = make_string(lightnameattrib->getString());
		}
		//toAdd = KigsCore::GetInstanceOf(make_string(lighturl->getString()), "Light");
		
		XMLNodeBase* light_lib = myRoot->getChildElement("library_lights");
		
		if ((url.at(0) == '#') && light_lib)
		{
			url = url.substr(1, url.length() - 1);
			XMLNodeBase* current_light = SearchNode(light_lib, "light", "id", url);
			if (current_light)
			{
				if (name == "")
				{
					name = url;
				}
				
				XMLNodeBase* currentTechniqueCommon = current_light->getChildElement("technique_common");
				XMLNodeBase* currentTechnique = current_light->getChildElement("technique");
				if(currentTechniqueCommon)
				{
					toAdd = KigsCore::GetInstanceOf(name, "Light");
					
					// read common value
 					Vector3D diffusecolor = ReadVector3D(currentTechniqueCommon->getChildElement("color"));

					// in blender dae, light color seems multiplied with "energy" 
					// so read normalized values in extra/technique
					if (currentTechnique)
					{
						diffusecolor.x = ReadFloat(currentTechnique->getChildElement("red"));
						diffusecolor.y = ReadFloat(currentTechnique->getChildElement("green"));
						diffusecolor.z = ReadFloat(currentTechnique->getChildElement("blue"));
					}

					// clamp color
					if (diffusecolor.x > 1.0f)
					{
						diffusecolor.x = 1.0f;
					}
					if (diffusecolor.y > 1.0f)
					{
						diffusecolor.y = 1.0f;
					}
					if (diffusecolor.z > 1.0f)
					{
						diffusecolor.z = 1.0f;
					}
					XMLNodeBase* sonNode = currentTechniqueCommon->getChildElement(0);

					if (sonNode->getName() == "ambient")
					{
						// create a false ambient ( directionnal, with only ambient component)
						toAdd->SetAmbientColor(diffusecolor.x, diffusecolor.y, diffusecolor.z);
						toAdd->SetDiffuseColor(0, 0, 0);
						toAdd->SetSpecularColor(0, 0, 0);
						toAdd->setValue("LightType", "DIRECTIONAL");

					}
					else if (sonNode->getName() == "directional")
					{
						toAdd->SetDiffuseColor(diffusecolor.x, diffusecolor.y, diffusecolor.z);
						toAdd->SetAmbientColor(0, 0, 0);
						// set specular the same as diffuse
						toAdd->SetSpecularColor(diffusecolor.x, diffusecolor.y, diffusecolor.z);
						toAdd->setValue("LightType", "DIRECTIONAL");
					}
					else if (sonNode->getName() == "point")
					{
						toAdd->setValue("LightType", "POINT");
						int nCountPoint = sonNode->getChildCount();
						for (int j = 0; j < nCountPoint; j++)
						{
							XMLNodeBase* JNode = sonNode->getChildElement(j);
							if (JNode->getName() == "color")
							{
								toAdd->SetDiffuseColor(diffusecolor.x, diffusecolor.y, diffusecolor.z);
								toAdd->SetAmbientColor(0, 0, 0);
								// set specular the same as diffuse
								toAdd->SetSpecularColor(diffusecolor.x, diffusecolor.y, diffusecolor.z);
							}
							else if (JNode->getName() == "constant_attenuation")
							{
								toAdd->setValue("ConstAttenuation", JNode->getChildElement(0)->getFloat());
							}
							else if (JNode->getName() == "linear_attenuation")
							{
								toAdd->setValue("LinAttenuation", JNode->getChildElement(0)->getFloat());
							}
							else if (JNode->getName() == "quadratic_attenuation")
							{
								toAdd->setValue("QuadAttenuation", JNode->getChildElement(0)->getFloat());
							}
						}	
					}
					else if (sonNode->getName() == "spot")
					{
						toAdd->setValue("LightType", "SPOT");
						Vector3D color;
						int nCountPoint = sonNode->getChildCount();
						for (int j = 0; j < nCountPoint; j++)
						{
							XMLNodeBase* JNode = sonNode->getChildElement(j);
							if (JNode->getName() == "color")
							{
								toAdd->SetDiffuseColor(diffusecolor.x, diffusecolor.y, diffusecolor.z);
								toAdd->SetAmbientColor(0, 0, 0);
								// set specular the same as diffuse
								toAdd->SetSpecularColor(diffusecolor.x, diffusecolor.y, diffusecolor.z);
							}
							else if (JNode->getName() == "constant_attenuation")
							{
								toAdd->setValue("ConstAttenuation", JNode->getChildElement(0)->getFloat());
							}
							else if (sonNode->getChildElement(j)->getName() == "linear_attenuation")
							{
								toAdd->setValue("LinAttenuation", JNode->getChildElement(0)->getFloat());
							}
							else if (sonNode->getChildElement(j)->getName() == "quadratic_attenuation")
							{
								toAdd->setValue("QuadAttenuation", JNode->getChildElement(0)->getFloat());
							}
							else  if (sonNode->getChildElement(j)->getName() == "falloff_angle")
							{
								float degrees = JNode->getChildElement(0)->getFloat();
								// cutoff is half the angle
								toAdd->setValue("SpotCutOff", degrees * fPI / 360.0f);
							}
							else  if (sonNode->getChildElement(j)->getName() == "falloff_exponent")
							{
								float exponent = JNode->getChildElement(0)->getFloat();
								// better idea welcome
								exponent *= 1000.0f;
								toAdd->setValue("SpotAttenuation", exponent);
							}
						}
					}
					else
					{
						printf("TO DO Parse light type: %s\n", make_string(sonNode->getName()).c_str());
					}
				}
			}
		}
	}
	return toAdd;
}

CMSP ColladaParser::ParseGeometry(XMLNodeBase* geomnode)
{
	
	//Parse material
	for (auto i = 0; i < geomnode->getChildCount(); i++)
	{
		if (geomnode->getChildElement(i) != nullptr)
		{
			if (geomnode->getChildElement(i)->getName() == "bind_material")
			{
				ParseBindMaterial(geomnode->getChildElement(i));
			}
			else
			{
				printf("TO DO Parse: %s, son of instance_geometry\n", make_string(geomnode->getChildElement(i)->getName()).c_str());
			}
		}
	}
	
	CMSP toAdd = nullptr;
	XMLAttributeBase* geomurl = geomnode->getAttribute("url");
	if (geomurl != nullptr)
	{
		std::string url = make_string(geomurl->getString());
		XMLNodeBase* geom_lib = myRoot->getChildElement("library_geometries");
		
		if ((url.at(0) == '#') && geom_lib != nullptr)
		{
			url = url.substr(1, url.length() - 1);
			//if mesh is already parsed
			if (m_MeshList[url] != nullptr)
			{
				return m_MeshList[url];
			}
			m_CurrentObjectName = url;
			XMLNodeBase* current_geom = SearchNode(geom_lib, "geometry", "id", url);
			
			if (current_geom != nullptr)
			{
				// add each node to the scene
				int ncount = current_geom->getChildCount();
				
				if (ncount)
				{
					
					int i;
					for (i = 0; i < ncount; i++)
					{
						// search mesh 
						if (current_geom->getChildElement(i)->getName() == "mesh")
						{
							// retreive mesh name
							XMLAttributeBase* attrname = current_geom->getAttribute("name");
							if (attrname != nullptr)
							{
								toAdd = ParseMesh(current_geom->getChildElement(i), make_string(attrname->getString()), nullptr);
							}
							else
							{
								toAdd = ParseMesh(current_geom->getChildElement(i), m_CurrentObjectName, nullptr);
							}
							
						}
						else if (current_geom->getChildElement(i)->getName() == "extra")
						{
							// nothing to parse
						}
						else
						{
							printf("TO DO Parse library geometry child: %s\n", make_string(current_geom->getChildElement(i)->getName()).c_str());
						}
					}
				}
			}
		}
	}
	
	
	
	return toAdd;
}

void ColladaParser::ParseBindMaterial(XMLNodeBase* bindMaterialNode)
{
	if (bindMaterialNode->getChildElement("technique_common") != nullptr)
	{
		XMLNodeBase* techniqueCommonNode = bindMaterialNode->getChildElement("technique_common");
		for (auto i = 0; i < techniqueCommonNode->getChildCount(); i++)
		{
			if (techniqueCommonNode->getChildElement(i) != nullptr)
			{
				XMLNodeBase* instanceMaterialNode = techniqueCommonNode->getChildElement(i);
				if(instanceMaterialNode->getAttribute("target") != nullptr)
				{
					std::string targetString = make_string(instanceMaterialNode->getAttribute("target")->getString());
					if (myRoot->getChildElement("library_materials") != nullptr)
					{
						XMLNodeBase* materialLib = myRoot->getChildElement("library_materials");
						
						if ((targetString.at(0) == '#') && materialLib != nullptr)
						{
							targetString = targetString.substr(1, targetString.length() - 1);
							//m_CurrentObjectName = "Mesh_" + targetString;
							XMLNodeBase* current_material = SearchNode(materialLib, "material", "id", targetString);
							if (current_material->getChildElement("instance_effect") != nullptr)
							{
								XMLNodeBase* instanceEffectNode = current_material->getChildElement("instance_effect");
								std::string url = make_string(instanceEffectNode->getAttribute("url")->getString());
								
								if (myRoot->getChildElement("library_effects") != nullptr)
								{
									XMLNodeBase* effectsLib = myRoot->getChildElement("library_effects");
									if ((url.at(0) == '#') && effectsLib != nullptr)
									{
										url = url.substr(1, url.length() - 1);
										XMLNodeBase* current_effect = SearchNode(effectsLib, "effect", "id", url);
										ParseEffect(current_effect, make_string(instanceMaterialNode->getAttribute("symbol")->getString()));
									}
									else
									{
										printf("Path to the effect isn't Local, TO DO: extern path\n");
									}
								}
							}
						}
						else
						{
							printf("Path to the material isn't Local, TO DO: extern path\n");
						}
					}
				}				
			}			
		}
	}
}

void ColladaParser::ParseEffect(XMLNodeBase* effectNode, std::string materialName)
{
	ReadMaterial *mat = &m_materialList[materialName];
	mat->name = materialName;
	for (auto i = 0; i < effectNode->getChildCount(); i++)
	{
		XMLNodeBase* childNode = effectNode->getChildElement(i);
		if (childNode->getName() == "profile_COMMON")
		{
			XMLNodeBase* techniqueNode = childNode->getChildElement("technique");
			
			for (auto j = 0; j < techniqueNode->getChildCount(); j++)
			{
				
				if (techniqueNode->getChildElement(j)->getName() == "phong" || techniqueNode->getChildElement(j)->getName() == "blinn" || techniqueNode->getChildElement(j)->getName()=="lambert")
				{
					
					for (auto h = 0; h < techniqueNode->getChildElement(j)->getChildCount(); h++)
					{
						XMLNodeBase* paramMat = techniqueNode->getChildElement(j)->getChildElement(h);
						if (paramMat->getName() == "emission")
						{
							if (paramMat->getChildElement("color") != nullptr)
							{
								Vector3D color = ReadVector3D(paramMat->getChildElement("color"));
								mat->emissionColor[0] = color.x;
								mat->emissionColor[1] = color.y;
								mat->emissionColor[2] = color.z;
							}
						}
						else if (paramMat->getName() == "ambient")
						{
							
							if (paramMat->getChildElement("color") != nullptr)
							{
								Vector3D color = ReadVector3D(paramMat->getChildElement("color"));
								mat->ambiantColor[0] = color.x;
								mat->ambiantColor[1] = color.y;
								mat->ambiantColor[2] = color.z;
							}
						}
						else if (paramMat->getName() == "diffuse")
						{
							if (paramMat->getChildElement("color") != nullptr)
							{
								Vector3D color = ReadVector3D(paramMat->getChildElement("color"));
								mat->diffuseColor[0] = color.x;
								mat->diffuseColor[1] = color.y;
								mat->diffuseColor[2] = color.z;
							}
							else if (paramMat->getChildElement("texture") != nullptr)
							{
								std::string textureUrl = make_string(paramMat->getChildElement("texture")->getAttribute("texture")->getString());
								mat->diffuseColor[0] = 1;
								mat->diffuseColor[1] = 1;
								mat->diffuseColor[2] = 1;
								XMLNodeBase* textureNode = SearchNode(childNode, "newparam", "sid", textureUrl);
								if (textureNode == nullptr)
								{
									XMLNodeBase* libImage = myRoot->getChildElement("library_images");
									XMLNodeBase* imageNode = SearchNode(libImage, "image", "id", textureUrl);
									XMLNodeBase* imageInitNode = imageNode->getChildElement("init_from");
									if (imageInitNode->getChildElement(0)->getName() == "ref")
									{
										mat->texturesName.push_back(make_string(imageInitNode->getChildElement(0)->getChildElement(0)->getString()));
										m_TextureList.push_back(make_string(imageInitNode->getChildElement(0)->getChildElement(0)->getString()));
									}
									else
									{
										mat->texturesName.push_back(make_string(imageInitNode->getChildElement(0)->getString()));
										m_TextureList.push_back(make_string(imageInitNode->getChildElement(0)->getString()));
									}
									break;
								}
								
								auto sampler2DNode = textureNode->getChildElement("sampler2D");
								if (sampler2DNode != nullptr)
								{
									auto sourceNode = sampler2DNode->getChildElement("source");
									if (sourceNode != nullptr)
									{
										std::string surfaceUrl = make_string(sourceNode->getChildElement(0)->getString());
										auto surfaceNode = SearchNode(childNode, "newparam", "sid", surfaceUrl);
										surfaceNode = surfaceNode->getChildElement("surface");
										if (surfaceNode != nullptr)
										{
											XMLNodeBase* imageInitNode = surfaceNode->getChildElement("init_from");
											if (imageInitNode != nullptr)
											{
												XMLNodeBase* libImage = myRoot->getChildElement("library_images");
												XMLNodeBase* imageNode;
												if (imageInitNode->getChildElement(0)->getName() == "ref")
												{
													imageNode = SearchNode(libImage, "image", "id", make_string(imageInitNode->getChildElement(0)->getChildElement(0)->getString()));
												}
												else
												{
													imageNode = SearchNode(libImage, "image", "id", make_string(imageInitNode->getChildElement(0)->getString()));
												}
												
												imageInitNode = imageNode->getChildElement("init_from");
												if (imageInitNode->getChildElement(0)->getName() == "ref")
												{
													mat->texturesName.push_back(make_string(imageInitNode->getChildElement(0)->getChildElement(0)->getString()));
													m_TextureList.push_back(make_string(imageInitNode->getChildElement(0)->getChildElement(0)->getString()));
												}
												else
												{
													mat->texturesName.push_back(make_string(imageInitNode->getChildElement(0)->getString()));
													m_TextureList.push_back(make_string(imageInitNode->getChildElement(0)->getString()));
													
												}
											}
										}
										
									}
								}
							}
						}
						else if (paramMat->getName() == "specular")
						{
							if (paramMat->getChildElement("color") != nullptr)
							{
								Vector3D color = ReadVector3D(paramMat->getChildElement("color"));
								mat->specularColor[0] = color.x;
								mat->specularColor[1] = color.y;
								mat->specularColor[2] = color.z;
							}
							else
							{
								//No Specular map for now
								mat->specularColor[0] = 1;
								mat->specularColor[1] = 1;
								mat->specularColor[2] = 1;
							}
						}
						else if (paramMat->getName() == "shininess")
						{
							mat->shininess = paramMat->getChildElement("float")->getChildElement(0)->getFloat();
						}
					}
				}
				else if (techniqueNode->getChildElement(j)->getName() == "extra")
				{
					//For bump map
					for (auto h = 0; h < techniqueNode->getChildElement(j)->getChildCount(); h++)
					{
						XMLNodeBase* extraChild = techniqueNode->getChildElement(j)->getChildElement(h);
						if (extraChild->getName() == "technique")
						{
							for (auto k = 0; k < extraChild->getChildCount(); k++)
							{
								if (extraChild->getChildElement(k)->getName() == "bump")
								{
									std::string textureUrl = make_string(extraChild->getChildElement(k)->getChildElement("texture")->getAttribute("texture")->getString());
									XMLNodeBase* textureNode = SearchNode(childNode, "newparam", "sid", textureUrl);
									if (textureNode == nullptr)
									{
										XMLNodeBase* libImage = myRoot->getChildElement("library_images");
										XMLNodeBase* imageNode = SearchNode(libImage, "image", "id", textureUrl);
										XMLNodeBase* imageInitNode = imageNode->getChildElement("init_from");
										if (imageInitNode->getChildElement(0)->getName() == "ref")
										{
											mat->NormalMapName.push_back(make_string(imageInitNode->getChildElement(0)->getChildElement(0)->getString()));
											m_TextureList.push_back(make_string(imageInitNode->getChildElement(0)->getChildElement(0)->getString()));
										}
										else
										{
											mat->NormalMapName.push_back(make_string(imageInitNode->getChildElement(0)->getString()));
											m_TextureList.push_back(make_string(imageInitNode->getChildElement(0)->getString()));
										}
										break;
									}
									
									XMLNodeBase* sampler2DNode = textureNode->getChildElement("sampler2D");
									if (sampler2DNode != nullptr)
									{
										XMLNodeBase* sourceNode = sampler2DNode->getChildElement("source");
										if (sourceNode != nullptr)
										{
											std::string surfaceUrl = make_string(sourceNode->getChildElement(0)->getString());
											XMLNodeBase* surfaceNode = SearchNode(childNode, "newparam", "sid", surfaceUrl);
											surfaceNode = surfaceNode->getChildElement("surface");
											if (surfaceNode != nullptr)
											{
												XMLNodeBase* imageInitNode = surfaceNode->getChildElement("init_from");
												if (imageInitNode != nullptr)
												{
													XMLNodeBase* libImage = myRoot->getChildElement("library_images");
													XMLNodeBase* imageNode;
													if (imageInitNode->getChildElement(0)->getName() == "ref")
													{
														imageNode = SearchNode(libImage, "image", "id", make_string(imageInitNode->getChildElement(0)->getChildElement(0)->getString()));
													}
													else
													{
														imageNode = SearchNode(libImage, "image", "id", make_string(imageInitNode->getChildElement(0)->getString()));
													}
													
													imageInitNode = imageNode->getChildElement("init_from");
													if (imageInitNode->getChildElement(0)->getName() == "ref")
													{
														mat->NormalMapName.push_back(make_string(imageInitNode->getChildElement(0)->getChildElement(0)->getString()));
														m_TextureList.push_back(make_string(imageInitNode->getChildElement(0)->getChildElement(0)->getString()));
													}
													else
													{
														mat->NormalMapName.push_back(make_string(imageInitNode->getChildElement(0)->getString()));
														m_TextureList.push_back(make_string(imageInitNode->getChildElement(0)->getString()));
														
													}
												}
											}
											
										}
									}
								}
							}
						}
					}
				}
				else
				{
					printf("TO DO: Parse effect child: %s\n", make_string(techniqueNode->getChildElement(j)->getName()).c_str());
				}
			}
		}
	}
}


SourceData* ColladaParser::ParseSource(XMLNodeBase* sourceNode)
{
	
	return &m_sources.emplace(std::piecewise_construct,
							  std::forward_as_tuple(sourceNode),
							  std::forward_as_tuple(sourceNode)).first->second;
	
}

CMSP ColladaParser::ParseMesh(XMLNodeBase* meshnode, const std::string& name, Controller* controller)
{
	CMSP toAdd;
	int meshChildCount = meshnode->getChildCount();
	
	m_CurrentMeshCollada = &(m_MeshColladaList->push_back());
	
	for (auto i = 0; i < meshChildCount; i++)
	{
		XMLNodeBase* childNode = meshnode->getChildElement(i);
		
		if (childNode->getName() == "source")
		{
			m_CurrentMeshCollada->m_SourceBufferList[make_string(childNode->getAttribute("id")->getString())] = ParseSource(childNode);
		}
		// Polylist = polygon primitives that cannot contain holes
		else if (childNode->getName() == "polylist" || childNode->getName() == "triangles")
		{
			//Creating or getting facetGroup in facetGroupList
			std::string matName;
			if (childNode->getAttribute("material"))
				matName = childNode->getAttribute("material")->getString();
			else
				matName = "NoMaterial";
			m_CurrentMeshCollada->m_CurrentFacetGroup = new FacetGroup();
			if (m_CurrentMeshCollada->m_CurrentFacetGroup->matName == "")
			{
				m_CurrentMeshCollada->m_CurrentFacetGroup->matName = matName;
				m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetBuffer = new	DynamicGrowingBuffer<Triangle>(256);
			}
			m_CurrentMeshCollada->m_ReadFacetGroupCount++;
			m_CurrentMeshCollada->m_ReadFacetGroupBuffer->push_back(m_CurrentMeshCollada->m_CurrentFacetGroup);

			std::map<int, std::vector<char>> offsets;
			
			
			for (auto j = 0; j < childNode->getChildCount(); j++)
			{
				XMLNodeBase* childPolylisteNode = childNode->getChildElement(j);
				if (childPolylisteNode->getName() == "input")
				{
					XMLAttributeBase* semanticAttribute = childPolylisteNode->getAttribute("semantic");
					bool otherOnVertice = true;
					if (semanticAttribute->getString() == "VERTEX")
					{
						XMLNodeBase* VerticesNode = meshnode->getChildElement("vertices");
						for (auto h = 0; h < VerticesNode->getChildCount(); h++)
						{
							XMLAttributeBase* semanticOfVerticesAttribute = VerticesNode->getChildElement(h)->getAttribute("semantic");
							if (semanticOfVerticesAttribute->getString() == "POSITION")
							{
								offsets[childPolylisteNode->getAttribute("offset")->getInt()].push_back('P');
								m_CurrentMeshCollada->m_CurrentFacetGroup->setPositionUrl(make_string(VerticesNode->getChildElement(h)->getAttribute("source")->getString()));
							}
							else if (semanticOfVerticesAttribute->getString() == "NORMAL")
							{
								offsets[childPolylisteNode->getAttribute("offset")->getInt()].push_back('N');
								m_CurrentMeshCollada->m_CurrentFacetGroup->setNormalUrl(make_string(VerticesNode->getChildElement(h)->getAttribute("source")->getString()));
								m_CurrentMeshCollada->m_CurrentFacetGroup->m_HasNormal = true;
							}
							else if (semanticOfVerticesAttribute->getString() == "TEXCOORD")
							{
								offsets[childPolylisteNode->getAttribute("offset")->getInt()].push_back('T');
								m_CurrentMeshCollada->m_CurrentFacetGroup->setTextureCoordUrl(make_string(VerticesNode->getChildElement(h)->getAttribute("source")->getString()));
								m_CurrentMeshCollada->m_CurrentFacetGroup->m_HasTextCoords = true;
							}
							else if (semanticOfVerticesAttribute->getString() == "COLOR")
							{
								offsets[childPolylisteNode->getAttribute("offset")->getInt()].push_back('C');
								m_CurrentMeshCollada->m_CurrentFacetGroup->setColorUrl(make_string(VerticesNode->getChildElement(h)->getAttribute("source")->getString()));
								m_CurrentMeshCollada->m_CurrentFacetGroup->m_HasColor = true;
							}
							else if (semanticOfVerticesAttribute->getString() == "TANGENT")
							{
								offsets[childPolylisteNode->getAttribute("offset")->getInt()].push_back('G');
								m_CurrentMeshCollada->m_CurrentFacetGroup->setTangentUrl(make_string(VerticesNode->getChildElement(h)->getAttribute("source")->getString()));
								m_CurrentMeshCollada->m_CurrentFacetGroup->m_HasTangent = true;
							}
							else if (semanticOfVerticesAttribute->getString() == "BINORMAL")
							{
								offsets[childPolylisteNode->getAttribute("offset")->getInt()].push_back('B');
								m_CurrentMeshCollada->m_CurrentFacetGroup->setBinormalUrl(make_string(VerticesNode->getChildElement(h)->getAttribute("source")->getString()));
								m_CurrentMeshCollada->m_CurrentFacetGroup->m_hasBinormal = true;
							}
							else
							{
								offsets[childPolylisteNode->getAttribute("offset")->getInt()].push_back('U');
								printf("TO DO Parse in vertice semanticAttribute: %s\n", make_string(semanticOfVerticesAttribute->getString()).c_str());
							}
						}
					}
					else if (semanticAttribute->getString() == "NORMAL")
					{
						offsets[childPolylisteNode->getAttribute("offset")->getInt()].push_back('N');
						m_CurrentMeshCollada->m_CurrentFacetGroup->setNormalUrl(make_string(childPolylisteNode->getAttribute("source")->getString()));
						m_CurrentMeshCollada->m_CurrentFacetGroup->m_HasNormal = true;
						
					}
					else if (semanticAttribute->getString() == "TEXCOORD")
					{
						offsets[childPolylisteNode->getAttribute("offset")->getInt()].push_back('T');
						m_CurrentMeshCollada->m_CurrentFacetGroup->setTextureCoordUrl(make_string(childPolylisteNode->getAttribute("source")->getString()));
						m_CurrentMeshCollada->m_CurrentFacetGroup->m_HasTextCoords = true;
					}
					else if (semanticAttribute->getString() == "COLOR")
					{
						offsets[childPolylisteNode->getAttribute("offset")->getInt()].push_back('C');
						m_CurrentMeshCollada->m_CurrentFacetGroup->setColorUrl(make_string(childPolylisteNode->getAttribute("source")->getString()));
						m_CurrentMeshCollada->m_CurrentFacetGroup->m_HasColor = true;
					}
					else if (semanticAttribute->getString() == "TANGENT")
					{
						offsets[childPolylisteNode->getAttribute("offset")->getInt()].push_back('G');
						m_CurrentMeshCollada->m_CurrentFacetGroup->setTangentUrl(make_string(childPolylisteNode->getAttribute("source")->getString()));
						m_CurrentMeshCollada->m_CurrentFacetGroup->m_HasTangent = true;
					}
					else if (semanticAttribute->getString() == "BINORMAL")
					{
						offsets[childPolylisteNode->getAttribute("offset")->getInt()].push_back('B');
						m_CurrentMeshCollada->m_CurrentFacetGroup->setBinormalUrl(make_string(childPolylisteNode->getAttribute("source")->getString()));
						m_CurrentMeshCollada->m_CurrentFacetGroup->m_hasBinormal = true;
					}
					else if (semanticAttribute->getString() == "TEXTANGENT")
					{
						offsets[childPolylisteNode->getAttribute("offset")->getInt()].push_back('U');
						printf("TO DO Parse semanticAttribute: %s\n", make_string(semanticAttribute->getString()).c_str());
					}
					else if (semanticAttribute->getString() == "TEXBINORMAL")
					{
						offsets[childPolylisteNode->getAttribute("offset")->getInt()].push_back('U');
						printf("TO DO Parse semanticAttribute: %s\n", make_string(semanticAttribute->getString()).c_str());
					}
					else
					{
						offsets[childPolylisteNode->getAttribute("offset")->getInt()].push_back('U');
						printf("TO DO Parse semanticAttribute: %s\n", make_string(semanticAttribute->getString()).c_str());
					}
				}
				else if (childPolylisteNode->getName() == "vcount")
				{
					//PARSE THE FACES & Mesh collada
					parseFacetPolylist(childPolylisteNode, childNode->getChildElement("p"), offsets);
				}
				else if (childNode->getName() == "triangles" && childPolylisteNode->getName() == "p")
				{
					parseFacetTriangle(childPolylisteNode, offsets);
				}
				else if (childNode->getName() == "polygons" && childPolylisteNode->getName() == "p")
				{
					parseFacetPolygons(childPolylisteNode, offsets);
				}
				else if (childNode->getName() == "trifans" && childPolylisteNode->getName() == "p")
				{
					parseFacetTrifans(childPolylisteNode, offsets);
				}
				else
				{
					printf("TO DO Parse polyliste child: %s\n", make_string(childPolylisteNode->getName()).c_str());
				}
			}
		}
		else if (childNode->getName() == "vertices")
		{
			//already Parsed
		}
		else if (childNode->getName() == "polygons")
		{
			//parse polygons
		}
		else
		{
			printf("TO DO Parse mesh child: %s\n", make_string(childNode->getName()).c_str());
		}
	}
	toAdd = CreateMeshFromMeshCollada(controller);
	return toAdd;
}

CMSP ColladaParser::CreateMeshFromMeshCollada(Controller* controller)
{
	SP<ModernMesh> newmesh = KigsCore::GetInstanceOf(m_CurrentObjectName, "ModernMesh");
	// create each group
	//newmesh->setValue("Optimize", false);
#ifdef _DEBUG
	newmesh->setValue("Optimize", true);
#else
	newmesh->setValue("Optimize", true);
#endif
	
	bool	isEmpty=true;

	newmesh->StartMeshBuilder();
	
	bool has_weights = controller && controller->weights.size();
	
	std::map<std::string, ReadMaterial>::iterator itmat = m_materialList.begin();
	std::map<std::string, ReadMaterial>::iterator itmatend = m_materialList.end();
	
	while (itmat != itmatend)
	{
		(*itmat).second.Init();
		++itmat;
	}
	for (auto i = 0; i < m_CurrentMeshCollada->m_ReadFacetGroupCount; i++)
	{
		bool hasNormal = false;
		FacetGroup* current = (*m_CurrentMeshCollada->m_ReadFacetGroupBuffer)[i];

		bool hasTexture = false;
		if (m_materialList[current->matName].texturesName.size())
		{
			hasTexture = true;
		}

		if (current->m_ReadFacetCount)
		{
			int structSize = 0;
			
			CoreItemSP	description = MakeCoreMap();
			CoreItemSP	vertices = MakeCoreVector();
			description->set("vertices",vertices);
		
			structSize += 3 * sizeof(float);
			
			// vertices have a color, so
			if (current->m_HasColor)
			{
				CoreItemSP	colors = MakeCoreVector();
				description->set("colors", colors);
				structSize += 4 * sizeof(float);
			}
			
			if (current->m_HasNormal)
			{
				CoreItemSP	normal = MakeCoreVector();
				description->set("normals", normal);
				structSize += 3 * sizeof(float);
				hasNormal = true;
			}
			
			if (current->m_HasTextCoords && hasTexture)
			{
				CoreItemSP	texCoords = MakeCoreVector();
				description->set("texCoords", texCoords);
				structSize += 2 * sizeof(float);
			}
			
			if (has_weights)
			{
				CoreItemSP	boneWeights = MakeCoreVector();
				description->set("bone_weights", boneWeights);
				structSize += 4 * sizeof(unsigned char);
				
				CoreItemSP	boneIndexes = MakeCoreVector();
				description->set("bone_indexes", boneIndexes);
				structSize += 4 * sizeof(unsigned char);
			}
			
			
			
			unsigned char* v[3];
			v[0] = new unsigned char[structSize];
			v[1] = new unsigned char[structSize];
			v[2] = new unsigned char[structSize];
			
			newmesh->StartMeshGroup((CoreMap<std::string>*)description.get());

			// build triangles
			
			unsigned int tri_index;
			
			
			SourceData& position_data = (*m_CurrentMeshCollada->m_SourceBufferList[current->positionUrl]);
			SourceData& normal_data = (*m_CurrentMeshCollada->m_SourceBufferList[current->normalUrl]);
			SourceData& texture_coordinates_data = (*m_CurrentMeshCollada->m_SourceBufferList[current->textureCoordUrl]);
			
			for (tri_index = 0; tri_index < current->m_ReadFacetCount; tri_index++)
			{
				isEmpty = false;
				int decal = 0;
				int copysize = 3 * sizeof(float);
				
				Triangle* currenttri = &((*current->m_ReadFacetBuffer)[tri_index]);
				
				auto p1(position_data.at<Vector3D>(currenttri->indexes[0].iv));
				auto p2(position_data.at<Vector3D>(currenttri->indexes[1].iv));
				auto p3(position_data.at<Vector3D>(currenttri->indexes[2].iv));
				
				memcpy(v[0], &p1, copysize);
				memcpy(v[1], &p2, copysize);
				memcpy(v[2], &p3, copysize);
				
				if (current->m_HasColor)
				{
					decal += copysize;
					copysize = 4 * sizeof(float);
					
					
					SourceData& color_data = (*m_CurrentMeshCollada->m_SourceBufferList[current->colorUrl]);
					
					Vector4D c1(color_data.at<Point3D>(currenttri->indexes[0].ic));
					c1.w = 1.0f;
					Vector4D c2(color_data.at<Point3D>(currenttri->indexes[1].ic));
					c2.w = 1.0f;
					Vector4D c3(color_data.at<Point3D>(currenttri->indexes[2].ic));
					c3.w = 1.0f;
					
					memcpy(v[0] + decal, &c1, copysize);
					memcpy(v[1] + decal, &c2, copysize);
					memcpy(v[2] + decal, &c3, copysize);
				}
				
				if (current->m_HasNormal)
				{
					decal += copysize;
					copysize = 3 * sizeof(float);
					
					auto n1(normal_data.at<Vector3D>(currenttri->indexes[0].in));
					auto n2(normal_data.at<Vector3D>(currenttri->indexes[1].in));
					auto n3(normal_data.at<Vector3D>(currenttri->indexes[2].in));
					
					memcpy(v[0] + decal, &n1, copysize);
					memcpy(v[1] + decal, &n2, copysize);
					memcpy(v[2] + decal, &n3, copysize);
				}
				
				if (current->m_HasTextCoords && hasTexture)
				{
					decal += copysize;
					copysize = 2 * sizeof(float);
					
					if(texture_coordinates_data.type == SourceData::Vector3DArray)
					{
						auto t1 = texture_coordinates_data.at<Vector3D>(currenttri->indexes[0].it);
						auto t2 = texture_coordinates_data.at<Vector3D>(currenttri->indexes[1].it);
						auto t3 = texture_coordinates_data.at<Vector3D>(currenttri->indexes[2].it);
						memcpy(v[0] + decal, &t1, copysize);
						memcpy(v[1] + decal, &t2, copysize);
						memcpy(v[2] + decal, &t3, copysize);
					}
					else if(texture_coordinates_data.type == SourceData::Vector2DArray)
					{
						auto t1 = texture_coordinates_data.at<Point2D>(currenttri->indexes[0].it);
						t1.y = 1.0f - t1.y;
						auto t2 = texture_coordinates_data.at<Point2D>(currenttri->indexes[1].it);
						t2.y = 1.0f - t2.y;
						auto t3 = texture_coordinates_data.at<Point2D>(currenttri->indexes[2].it);
						t3.y = 1.0f - t3.y;
						memcpy(v[0] + decal, &t1, copysize);
						memcpy(v[1] + decal, &t2, copysize);
						memcpy(v[2] + decal, &t3, copysize);
					}
					
				}
				
				if(has_weights)
				{
					// Weights
					decal += copysize;
					copysize = 4*sizeof(unsigned char);
					memcpy(v[0] + decal, controller->weights[currenttri->indexes[0].iv].weight, copysize);
					memcpy(v[1] + decal, controller->weights[currenttri->indexes[1].iv].weight, copysize);
					memcpy(v[2] + decal, controller->weights[currenttri->indexes[2].iv].weight, copysize);
					
					// Indexes
					decal += copysize;
					copysize = 4*sizeof(unsigned char);
					memcpy(v[0] + decal, controller->weights[currenttri->indexes[0].iv].index, copysize);
					memcpy(v[1] + decal, controller->weights[currenttri->indexes[1].iv].index, copysize);
					memcpy(v[2] + decal, controller->weights[currenttri->indexes[2].iv].index, copysize);
				}
				newmesh->AddTriangle(v[0], v[1], v[2]);
			}
			
			delete[] v[0];
			delete[] v[1];
			delete[] v[2];
			
			SP<ModernMeshItemGroup> created = newmesh->EndMeshGroup();
			if (m_materialList[current->matName].m_Material)
			{

				if (!hasNormal && myConvertParams.myAddVertexNormal)
				{
					created->ComputeNormals();
				}

				// a normal map was found or explicit tangent computation asked ?
				if (m_materialList[current->matName].NormalMapName.size() || myConvertParams.myAddVertexTangent)
				{
					created->ComputeTangents((myConvertParams.myAddVertexTangent == 1) ? true : false);
				}
				created->addItem((CMSP&)m_materialList[current->matName].m_Material);
			}
			
		}
	}
	
	newmesh->EndMeshBuilder();
	
	newmesh->Init();			

	if (!isEmpty)
	{
		m_MeshList[m_CurrentObjectName] = newmesh;


		if (myConvertParams.myImportScaleFactor != 1.0f)
		{
			newmesh->ApplyScaleFactor(myConvertParams.myImportScaleFactor);
		}

		return newmesh;
	}
	else
	{
		m_MeshList[m_CurrentObjectName] = 0;
		newmesh=nullptr;
	}
	return 0;
}


void ColladaParser::parseFacetPolylist(XMLNodeBase* vertexCountNode, XMLNodeBase* indexesNode, std::map<int, std::vector<char>> offsets)
{
	XMLNodeBase* nbrVertexNode = vertexCountNode->getChildElement(0);
	std::string nbrVertexString = make_string(nbrVertexNode->getString());
	AsciiParserUtils numberVertexParser(const_cast<char*>(nbrVertexString.c_str()), nbrVertexString.size());
	AsciiParserUtils numberVertex(numberVertexParser);
	
	indexesNode = indexesNode->getChildElement(0);
	std::string indexesString = make_string(indexesNode->getString());
	
	AsciiParserUtils indexesParser(const_cast<char*>(indexesString.c_str()), indexesString.size());
	AsciiParserUtils indexesVertex(indexesParser);
	
	int nbrVertex, indexVertex;
	while (numberVertexParser.GetWord(numberVertex))
	{
		numberVertex.ReadInt(nbrVertex);
		Triangle theFacet;
		std::vector<IndexTrio>	indexlist;
		
		//Une face (à convertir en triangle si elle ne l'est pas)
		for (auto i = 0; i < nbrVertex; i++)
		{
			//Parcours de chaque sommets
			
			IndexTrio	toAdd;
			
			for (auto j = 0; offsets.count(j) > 0; j++)
			{
				// same offset can access several maps
				indexesParser.GetWord(indexesVertex);
				indexesVertex.ReadInt(indexVertex);
				
				for (auto h = 0; h < offsets[j].size(); h++)
				{
					
					if (offsets[j].at(h) == 'P')
					{
						toAdd.iv = indexVertex;
					}
					else if (offsets[j].at(h) == 'T')
					{
						
						toAdd.it = indexVertex;
					}
					else if (offsets[j].at(h) == 'N')
					{
						
						toAdd.in = indexVertex;
					}
					else if (offsets[j].at(h) == 'C')
					{
						toAdd.ic = indexVertex;
					}
					else if (offsets[j].at(h) == 'G')
					{
						toAdd.ig = indexVertex;
					}
					else if (offsets[j].at(h) == 'B')
					{
						toAdd.ib = indexVertex;
					}
					else if (offsets[j].at(h) == 'U')
					{
						//Unknown attribute
					}
				}
			}
			indexlist.push_back(toAdd);
		}
		
		Triangle* current = &(*m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetBuffer)[m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetCount++];
		
		if (indexlist.size() == 3)
		{
			//Triangle* current = &(*m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetBuffer)[m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetCount++];
			current->indexes[0] = indexlist.at(0);
			current->indexes[1] = indexlist.at(1);
			current->indexes[2] = indexlist.at(2);
		}
		else if (indexlist.size() == 4) // quad
		{
			//Triangle* current = &(*m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetBuffer)[m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetCount++];
			current->indexes[0] = indexlist.at(0);
			current->indexes[1] = indexlist.at(1);
			current->indexes[2] = indexlist.at(2);
			current = &(*m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetBuffer)[m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetCount++];
			current->indexes[0] = indexlist.at(0);
			current->indexes[1] = indexlist.at(2);
			current->indexes[2] = indexlist.at(3);
		}
	}
}

void ColladaParser::parseFacetTriangle(XMLNodeBase* indexesNode, std::map<int, std::vector<char>> offsets)
{
	std::string indexesString;
	if (indexesNode->getChildElement(0) != nullptr)
	{
		indexesNode = indexesNode->getChildElement(0);
	}
	
	indexesString = indexesNode->getString();
	
	AsciiParserUtils indexesParser((char*)indexesString.c_str(), indexesString.size());
	AsciiParserUtils indexesVertex(indexesParser);
	
	
	int indexVertex;
	
	bool hasWord= true;// = indexesParser.GetWord(indexesVertex);
	//hasWord = indexesParser.GetWord(indexesVertex);
	//indexesVertex.ReadInt(indexVertex);
	while (hasWord)
	{
		Triangle theFacet;
		std::vector<IndexTrio>	indexlist;
		
		//Une face (à convertir en triangle si elle ne l'est pas)
		for (auto i = 0; i < 3; i++)
		{
			//Parcours de chaque sommets
			IndexTrio	toAdd;
			for (auto j = 0; offsets.count(j) > 0; j++)
			{
				//printf("i: %d, j: %d, count: %d, indexVertex: %d\n",i, j, offsets.count(j),indexVertex);
				
				hasWord = indexesParser.GetWord(indexesVertex);
				if (!hasWord)
				{
					return;
				}
				indexesVertex.ReadInt(indexVertex);
				for (auto h = 0; h < offsets[j].size(); h++)
				{
					if (offsets[j].at(h) == 'P')
					{
						toAdd.iv = indexVertex;
					}
					else if (offsets[j].at(h) == 'T')
					{
						toAdd.it = indexVertex;
					}
					else if (offsets[j].at(h) == 'N')
					{
						toAdd.in = indexVertex;
					}
					else if (offsets[j].at(h) == 'C')
					{
						toAdd.ic = indexVertex;
					}
					else if (offsets[j].at(h) == 'G')
					{
						toAdd.ig = indexVertex;
					}
					else if (offsets[j].at(h) == 'B')
					{
						toAdd.ib = indexVertex;
					}
					else if (offsets[j].at(h) == 'U')
					{
						
					}
				}
			}
			indexlist.push_back(toAdd);
			
		}
		Triangle* current = &(*m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetBuffer)[m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetCount++];
		
		current->indexes[0] = indexlist.at(0); 
		current->indexes[1] = indexlist.at(1);
		current->indexes[2] = indexlist.at(2);
	}
}



void ColladaParser::parseFacetTrifans(XMLNodeBase* indexesNode, std::map<int, std::vector<char>> offsets)
{
	std::string indexesString = make_string(indexesNode->getChildElement(0)->getString());
	if (indexesNode->getChildElement(0) != nullptr)
	{		
		indexesNode = indexesNode->getChildElement(0);
	}
	AsciiParserUtils indexesParser(const_cast<char*>(indexesString.c_str()), indexesString.size());
	AsciiParserUtils indexesVertex(indexesParser);
	
	int indexVertex;
	
	bool hasWord = true;// = indexesParser.GetWord(indexesVertex);
	hasWord = indexesParser.GetWord(indexesVertex);
	indexesVertex.ReadInt(indexVertex);
	
	std::queue<IndexTrio>	indexlist;
	IndexTrio	toAdd;
	
	for (auto i = 0; i < 3; i++)
	{
		
		for (auto j = 0; offsets.count(j) > 0; j++)
		{
			
			for (auto h = 0; h < offsets[j].size(); h++)
			{
				if (offsets[j].at(h) == 'P')
				{
					toAdd.iv = indexVertex;
				}
				else if (offsets[j].at(h) == 'T')
				{
					toAdd.it = indexVertex;
				}
				else if (offsets[j].at(h) == 'N')
				{
					toAdd.in = indexVertex;
				}
				else if (offsets[j].at(h) == 'C')
				{
					toAdd.ic = indexVertex;
				}
				else if (offsets[j].at(h) == 'G')
				{
					toAdd.ig = indexVertex;
				}
				else if (offsets[j].at(h) == 'B')
				{
					toAdd.ib = indexVertex;
				}
				else if (offsets[j].at(h) == 'U')
				{
					
				}
			}
			hasWord = indexesParser.GetWord(indexesVertex);
			indexesVertex.ReadInt(indexVertex);
		}
		indexlist.push(toAdd);
	}
	
	Triangle* current = &(*m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetBuffer)[m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetCount++];
	
	
	current->indexes[0] = indexlist.front();
	indexlist.pop();
	current->indexes[1] = indexlist.front();
	current->indexes[2] = indexlist.back();
	
	while (hasWord)
	{
		for (auto j = 0; offsets.count(j) > 0; j++)
		{
			
			for (auto h = 0; h < offsets[j].size(); h++)
			{
				if (offsets[j].at(h) == 'P')
				{
					toAdd.iv = indexVertex;
				}
				else if (offsets[j].at(h) == 'T')
				{
					toAdd.it = indexVertex;
				}
				else if (offsets[j].at(h) == 'N')
				{
					toAdd.in = indexVertex;
				}
				else if (offsets[j].at(h) == 'C')
				{
					toAdd.ic = indexVertex;
				}
				else if (offsets[j].at(h) == 'G')
				{
					toAdd.ig = indexVertex;
				}
				else if (offsets[j].at(h) == 'B')
				{
					toAdd.ib = indexVertex;
				}
				else if (offsets[j].at(h) == 'U')
				{
					
				}
			}
			hasWord = indexesParser.GetWord(indexesVertex);
			indexesVertex.ReadInt(indexVertex);
		}
		indexlist.push(toAdd);
		current = &(*m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetBuffer)[m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetCount++];
		current->indexes[0] = indexlist.front();
		indexlist.pop();
		current->indexes[1] = indexlist.front();
		current->indexes[2] = indexlist.back();
		
	}
	
}

void ColladaParser::parseFacetPolygons(XMLNodeBase* indexesNode, std::map<int, std::vector<char>> offsets)
{
	std::string indexesString = make_string(indexesNode->getChildElement(0)->getString());
	indexesNode = indexesNode->getChildElement(0);
	AsciiParserUtils indexesParser(const_cast<char*>(indexesString.c_str()), indexesString.size());
	AsciiParserUtils indexesVertex(indexesParser);
	
	int indexVertex;
	
	bool hasWord = indexesParser.GetWord(indexesVertex);
	
	while (hasWord)
	{
		Triangle theFacet;
		std::vector<IndexTrio>	indexlist;
		
		
		//Parcours de chaque sommets
		indexesVertex.ReadInt(indexVertex);
		
		IndexTrio	toAdd;
		
		for (auto j = 0; offsets.count(j) > 0; j++)
		{
			for (auto h = 0; h < offsets[j].size(); h++)
			{
				if (offsets[j].at(h) == 'P')
				{
					toAdd.iv = indexVertex;
					hasWord = indexesParser.GetWord(indexesVertex);
					indexesVertex.ReadInt(indexVertex);
				}
				else if (offsets[j].at(h) == 'T')
				{
					toAdd.it = indexVertex;
					hasWord = indexesParser.GetWord(indexesVertex);
					indexesVertex.ReadInt(indexVertex);
				}
				else if (offsets[j].at(h) == 'N')
				{
					toAdd.in = indexVertex;
					hasWord = indexesParser.GetWord(indexesVertex);
					indexesVertex.ReadInt(indexVertex);
				}
				else if (offsets[j].at(h) == 'C')
				{
					toAdd.ic = indexVertex;
					hasWord = indexesParser.GetWord(indexesVertex);
					indexesVertex.ReadInt(indexVertex);
				}
				else if (offsets[j].at(h) == 'G')
				{
					toAdd.ig = indexVertex;
					hasWord = indexesParser.GetWord(indexesVertex);
					indexesVertex.ReadInt(indexVertex);
				}
				else if (offsets[j].at(h) == 'B')
				{
					toAdd.ib = indexVertex;
					hasWord = indexesParser.GetWord(indexesVertex);
					indexesVertex.ReadInt(indexVertex);
				}
				else if (offsets[j].at(h) == 'U')
				{
					//UNKNOWN ATTRIBUTE
					hasWord = indexesParser.GetWord(indexesVertex);
					indexesVertex.ReadInt(indexVertex);
				}
			}
		}
		indexlist.push_back(toAdd);
		
		Triangle* current;
		
		if (indexlist.size() == 3)
		{
			current = &(*m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetBuffer)[m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetCount++];
			current->indexes[0] = indexlist.at(0);
			current->indexes[1] = indexlist.at(1);
			current->indexes[2] = indexlist.at(2);
		}
		else if (indexlist.size() == 4) // quad
		{
			current = &(*m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetBuffer)[m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetCount++];
			current->indexes[0] = indexlist.at(0);
			current->indexes[1] = indexlist.at(1);
			current->indexes[2] = indexlist.at(2);
			current = &(*m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetBuffer)[m_CurrentMeshCollada->m_CurrentFacetGroup->m_ReadFacetCount++];
			current->indexes[0] = indexlist.at(0);
			current->indexes[1] = indexlist.at(2);
			current->indexes[2] = indexlist.at(3);
		}
		else
		{
			printf("ERROR, POLYGONS UNPARSE: do the triangulation of polygones");
			exit(0);
		}
	}
}


Matrix3x4 ColladaParser::ParseTransform(XMLNodeBase* scenenode)
{
	// Init
	Matrix3x4 transform;
	transform.SetIdentity();
	
	Point3D translate(0, 0, 0), scale(1, 1, 1);
	Quaternion rotate[3];
	
	int currentReadRotate = 0;
	rotate[0].Set(0, 0, 0, 0);
	rotate[1].Set(0, 0, 0, 0);
	rotate[2].Set(0, 0, 0, 0);
	
	int ncount = scenenode->getChildCount();
	for (int i = 0; i < ncount; i++)
	{
		XMLNodeBase* childNode = scenenode->getChildElement(i);
		// Parse translation
		if (childNode->getName() == "translate")
		{
			translate = ReadVector3D(childNode);
		}
		// Parse rotation
		else if (childNode->getName() == "rotate")
		{
			if (currentReadRotate < 3)
			{
				rotate[currentReadRotate++] = ReadQuaternion(childNode);
			}
			else
			{
				KIGS_ERROR("Wrong rotation management in node", 1);
			}
		}
		// Parse scale
		else if (childNode->getName() == "scale")
		{
			scale = ReadVector3D(childNode);
		}
		// Parse complete matrix
		else if (childNode->getName() == "matrix")
		{
			transform = ReadMatrix3x4(childNode);
		}
	}
	
	if (transform.IsIdentity())
	{
		transform.SetTranslation(translate);
		transform.PreScale(scale.x, scale.y, scale.z);
		
		// TODO(antoine) : verify this ?
		// Degree to radian conversion
		transform.PreRotateX(PI*rotate[0].w / 180.0f);
		transform.PreRotateY(PI*rotate[1].w / 180.0f);
		transform.PreRotateZ(PI*rotate[2].w / 180.0f);
	}
	return transform;
}




/* FUNCTION Refer to <node> element
read https://www.khronos.org/files/collada_spec_1_5.pdf p197
Transformation to do: <skew> ???, lookat
instance_node
instance_controller for the animations
*/
CMSP ColladaParser::ParseNode(XMLNodeBase* scenenode, bool &isModernMeshRoot)
{
	std::string nodeName = GetNodeName(scenenode);
	SP<Node3D> result = KigsCore::GetInstanceOf(nodeName, "Node3D");
	
	Matrix3x4 transform = ParseTransform(scenenode);

	if (myConvertParams.myImportScaleFactor != 1.0f)
	{
		// apply scale on position only
		transform.Pos *= myConvertParams.myImportScaleFactor;
	}
	
	int ncount = scenenode->getChildCount();
	if (ncount)
	{
		int i;
		for (i = 0; i < ncount; i++)
		{
			XMLNodeBase* childNode = scenenode->getChildElement(i);
			std::string childNodeName = make_string(childNode->getName());
			if (childNodeName == "node")
			{
				CMSP toAdd = ParseNode(childNode, isModernMeshRoot);
				if (toAdd)
				{
					result->addItem(toAdd);
					toAdd->Init();
				
				}
			}
			else if (childNodeName == "instance_geometry")
			{
				CMSP toAdd = ParseGeometry(childNode);
				if (toAdd != nullptr)
				{
					result->addItem(toAdd);
					toAdd->Init();
					
				}
			}
			else if (childNodeName == "instance_camera")
			{
				CMSP toAdd = ParseCamera(childNode);
				if (toAdd != nullptr)
				{
					result->addItem(toAdd);
					toAdd->Init();
				}
			}
			else if (childNodeName == "instance_light")
			{
				CMSP toAdd = ParseLight(childNode);
				if (toAdd != nullptr)
				{
					result->addItem(toAdd);
					toAdd->Init();
				}
			}
			else if (childNodeName == "instance_controller")
			{
				CMSP toAdd = ParseInstanceController(childNode);
				if (toAdd != nullptr)
				{
					result->addItem(toAdd);
					toAdd->Init();
				}
			}
			else if (childNodeName == "extra")
			{
				// nothing to parse
			}
			
#ifdef _DEBUG
			else if(childNodeName != "matrix" && childNodeName != "rotate" && childNodeName != "translate" && childNodeName != "scale")
			{
				printf("TO DO: parse %s in NODE child node \n", childNodeName.c_str());
			}
#endif
			
		}
	}
	result->ChangeMatrix(transform);
	return result;
}

void ColladaParser::ParseWeights(Controller* controller)
{
	XMLNodeBase* skin_node = controller->skin_node;
	// Parse Weights
	XMLNodeBase* vertex_weights = skin_node->getChildElement("vertex_weights");
	if(!vertex_weights) return;
	
	// Get count
	int vertex_weights_count = vertex_weights->getAttribute("count")->getInt();
	
	// Find inputs
	XMLNodeBase* input_joint_node=nullptr;
	XMLNodeBase* input_weights_node=nullptr;
	XMLNodeBase* vcount_node=nullptr;
	XMLNodeBase* v_node=nullptr;
	
	for(int n=0; n<vertex_weights->getChildCount(); ++n)
	{
		XMLNodeBase* vertex_weights_child = vertex_weights->getChildElement(n);
		std::string name = make_string(vertex_weights_child->getName());
		
		// input :
		// The <input> elements describe the joints and the attributes to be associated with them
		if(name == "input")
		{
			XMLAttributeBase* semantic_attr = vertex_weights_child->getAttribute("semantic");
			// Skin influence identifier 
			if(semantic_attr->getString() == "JOINT")
				input_joint_node = vertex_weights_child;
			// Skin influence weighting value
			else if(semantic_attr->getString() == "WEIGHT")
				input_weights_node = vertex_weights_child;
		}
		
		// vcount :
		// Contains a list of integers, each specifying the number of bones associated with one of the influences defined by <vertex_weights>
		else if(name == "vcount")
			vcount_node = vertex_weights_child;
		
		// v :
		// Contains a list of indices that describe which bones and attributes are associated with each vertex. 
		// An index of -1 into the array of joints refers to the bind shape. Weights should be normalized before use. This element has no attributes.
		else if(name == "v")
			v_node = vertex_weights_child;
	}
	
	if(!(input_weights_node && input_joint_node && vcount_node && v_node))
		KIGS_ERROR("Missing nodes when parsing weights", 1);
	
	std::string joints_url = make_string(input_joint_node->getAttribute("source")->getString());
	std::string weight_url = make_string(input_weights_node->getAttribute("source")->getString());
	
	joints_url = joints_url.substr(1, joints_url.length()-1);
	weight_url= weight_url.substr(1, weight_url.length()-1);
	
	XMLNodeBase* weight_source_node=nullptr;
	
	// Parse <skin> child nodes 
	for(int n=0; n<skin_node->getChildCount(); ++n)
	{
		XMLNodeBase* child_skin = skin_node->getChildElement(n);
		if(child_skin->getName() != "source") continue;
		if(child_skin->getAttribute("id")->getString() == weight_url)
		{
			weight_source_node = child_skin;
			break;
		}
	}
	if(!weight_source_node) 
		KIGS_ERROR("Cannot find source node for the weights", 1);
	
	
	auto& weight_data = *ParseSource(weight_source_node);
	
	
	std::string vcount_string = make_string(vcount_node->getChildElement(0)->getString());
	std::string v_string = make_string(v_node->getChildElement(0)->getString());
	
	AsciiParserUtils vcount_parser(const_cast<char*>(vcount_string.c_str()), vcount_string.length());
	AsciiParserUtils v_parser(const_cast<char*>(v_string.c_str()), v_string.length());
	
	int k=0;
	int nb_weights = 0;
	
	std::vector<std::pair<int, float>> current_vertex_weights;
	current_vertex_weights.reserve(16);
	controller->weights.resize(vertex_weights_count);
	
	while(k<vertex_weights_count && vcount_parser.ReadInt(nb_weights))
	{
		current_vertex_weights.clear();
		current_vertex_weights.resize(std::max(nb_weights, 4));
		
		float total_weight = 0.0f;
		
		for(int j=0; j<nb_weights; ++j)
		{
			int joint_index = -1;
			int weight_index = -1;
			
			if(!v_parser.ReadInt(joint_index) || !v_parser.ReadInt(weight_index))
				KIGS_ERROR("Error parsing v array", 1);
			
			if(joint_index == -1)
			{
				KIGS_ERROR("weight towards bind shape unsupported", 1);
			}
			
			current_vertex_weights[j].first = joint_index;
			current_vertex_weights[j].second = weight_data.at<float>(weight_index);
			
			total_weight += current_vertex_weights[j].second;
		}
		
		// Handle vertex associated to more than 4 weights
		if(nb_weights > 4)
		{
			std::sort(current_vertex_weights.begin(), current_vertex_weights.end(), 
					  [](const std::pair<int, float>& a, const std::pair<int, float>& b){ return a.second > b.second; });
			current_vertex_weights.resize(4);
			total_weight = 0.0f;
			for(auto& p : current_vertex_weights)
				total_weight += p.second;
		}
		
		for(int j=0; j<4; ++j)
		{
			controller->weights[k].index[j] = current_vertex_weights[j].first;
			controller->weights[k].weight[j] = (unsigned char)(0.5f + 255.0f*(current_vertex_weights[j].second / total_weight));
		}
		++k;
	}
	
	if (vertex_weights_count != k)
		KIGS_ERROR("Missing elements in weight/index varray", 1);
}


Controller* ColladaParser::ParseController(std::string url)
{
	for(auto& controller : m_ParsedControllers)
	{
		if(controller.id == url) 
			return &controller;
	}
	
	// A skinning <controller> associates a geometry with a skeleton. The skeleton is considered to be in its resting position, or bind pose. 
	// The bind pose is the world-space position and orientation of each joint when the skeleton was bound to the geometry. 
	// This world space is also called the bind-pose space to distinguish it from other world-space coordinate systems.
	// A skinning <instance_controller> instantiates a skinning <controller> and associates it with a run-time skeleton. 
	// COLLADA defines skinning in object space, so the <instance_controller>’s placement in the <node> hierarchy contributes to the final vertex location. 
	
	XMLNodeBase* lib_controllers = myRoot->getChildElement("library_controllers");
	if(!lib_controllers) return nullptr;
	
	url = url.substr(1, url.length() - 1);
	
	for (int i = 0; i < lib_controllers->getChildCount(); i++)
	{
		XMLNodeBase* controller_node = lib_controllers->getChildElement(i);
		if(!controller_node) continue;
		if(controller_node->getAttribute("id")->getString() != url) continue;
		
		Controller controller;
		controller.id = std::move(url);
		
		// Get skin node
		XMLNodeBase* skin_node = controller_node->getChildElement("skin");
		if(!skin_node) continue;
		controller.skin_node = skin_node;
		
		// Bind shape matrix :  
		// A single matrix that represents the transform of the bind-shape at the time when	the mesh was bound to a skeleton. This matrix transforms the bind-shape from object space to bind-space.
		
		Matrix3x4 bind_shape_matrix;
		bind_shape_matrix.SetIdentity();
		
		// Get bind shape matrix node
		XMLNodeBase* bind_shape_matrix_node = skin_node->getChildElement("bind_shape_matrix");
		if(bind_shape_matrix_node)
		{
			std::string matrix_string = make_string(bind_shape_matrix_node->getChildElement(0)->getString());
			AsciiParserUtils float_parser(const_cast<char*>(matrix_string.data()), matrix_string.length());
			
			for(int l=0; l<3; ++l)
			{
				for(int c=0; c<4; ++c)
				{
					if(!float_parser.ReadFloat(bind_shape_matrix.e[c][l]))
						KIGS_ERROR("Error parsing matrix", 1);
				}
			}
		}
		
		ParseWeights(&controller);
		
		// Get joints node
		XMLNodeBase* joints_node = skin_node->getChildElement("joints");
		if(!joints_node) continue;
		
		XMLNodeBase* joint_input_field = SearchNode(joints_node, "input", "semantic", "JOINT");
		
		// Inverse bind matrix :
		// Inverse of local-to-world matrix. Typically stored in a <float_array> taken 16 floating-point numbers at a time.
		// The <joints> element associates the joints to their inverse bind matrices.
		
		XMLNodeBase* inv_bind_matrix_input_field = SearchNode(joints_node, "input", "semantic", "INV_BIND_MATRIX");
		
		XMLNodeBase* joint_source, *inv_bind_matrix_source;
		if(joint_input_field)
		{
			std::string joint_source_id = make_string(joint_input_field->getAttribute("source")->getString());
			joint_source_id = joint_source_id.substr(1, joint_source_id.length()-1);
			joint_source = SearchNode(skin_node, "source", "id", joint_source_id);
		}
		if(inv_bind_matrix_input_field)
		{
			std::string inv_bind_matrix_source_id = make_string(inv_bind_matrix_input_field->getAttribute("source")->getString());
			inv_bind_matrix_source_id = inv_bind_matrix_source_id.substr(1, inv_bind_matrix_source_id.length()-1);
			inv_bind_matrix_source = SearchNode(skin_node, "source", "id", inv_bind_matrix_source_id);
		}
		
		if(!joint_source || !inv_bind_matrix_source) continue;
		
		// Get name array (joint name array)
		XMLNodeBase* name_array_node = joint_source->getChildElement("Name_array");
		if(!name_array_node) continue;
		std::string names = make_string(name_array_node->getChildElement(0)->getString());
		//auto vec = SplitStringByCharacter(names, ' ');
		
		uintptr_t last_pos = -1;
		for (auto& c : names)
		{
			if (c == ' ' || c == '\n')
			{
				uintptr_t current_pos = &c - names.data();
				controller.sids.push_back(names.substr(last_pos+1, current_pos - last_pos - 1));
				last_pos = current_pos;
			}
		}
		controller.sids.push_back(names.substr(last_pos + 1));
		
		//for(auto& str : vec)
		{
			//	if(vec.empty()) continue;
			//	controller.sids.push_back(std::move(str));
		}
		
		if(name_array_node->getAttribute("count")->getInt() != controller.sids.size())
		{
			KIGS_ERROR("Number of joints in the Name_array is different from its size", 1);
		}
		
		// NOTE(antoine) assume always float4x4
		XMLNodeBase* accessor = inv_bind_matrix_source->getChildElement("accessor");
		int matrix_count = accessor->getAttribute("count")->getInt();
		
		if(matrix_count != controller.sids.size())
		{
			KIGS_ERROR("Number of INV_BIND_MATRIX different from the number of joints", 1);
		}
		
		if(accessor->getChildElement(0)->getAttribute("type")->getString() != "float4x4")
		{
			KIGS_ERROR("float4x4 is the only supported type in INV_BIND_MATRIX accessor", 1);
		}
		
		XMLNodeBase* float_array_node = inv_bind_matrix_source->getChildElement("float_array");
		if(!float_array_node) continue;
		
		std::string floats_string = make_string(float_array_node->getChildElement(0)->getString());
		AsciiParserUtils float_parser(const_cast<char*>(floats_string.data()), floats_string.length());
		
		// Parse inverse bind matrices
		for(int n=0; n<matrix_count; ++n)
		{
			controller.joints_inv_bind_matrix.emplace_back();
			Matrix3x4& mat = controller.joints_inv_bind_matrix.back();
			for(int l=0; l<3; ++l)
			{
				for(int c=0; c<4; ++c)
				{
					if(!float_parser.ReadFloat(mat.e[c][l]))
						KIGS_ERROR("Error parsing matrix", 1);
				}
			}
			float f;
			for(int c=0; c<4; ++c)
			{
				if(!float_parser.ReadFloat(f))
					KIGS_ERROR("Error parsing matrix", 1);
			}
			
			
			mat = mat*bind_shape_matrix;
		}
		
		
		if(controller.sids.size())
		{
			m_ParsedControllers.push_back(std::move(controller));
			return &m_ParsedControllers.back();
		}
		
		return nullptr;
	}
	
	return nullptr;
}

static void FindSkeleton(XMLNodeBase* current_node, const std::vector<std::string>& sids, std::vector<int>& remaining_sids, std::vector<std::pair<int, XMLNodeBase*>>& joints)
{
	XMLAttributeBase* sid_attr = current_node->getAttribute("sid");
	
	if(sid_attr)
	{
		std::string current_sid = make_string(sid_attr->getString());
		
		for(auto it = remaining_sids.begin(); it != remaining_sids.end(); ++it)
		{
			if(sids[*it] == current_sid)
			{
				joints.emplace_back(*it, current_node);
				remaining_sids.erase(it);
				break;
			}
		}
	}
	if(remaining_sids.empty()) return;
	
	for(int i=0; i<current_node->getChildCount(); ++i)
	{
		XMLNodeBase* child = current_node->getChildElement(i);
		if(!child) continue;
		
		// Iterate through skeleton hierarchy
		FindSkeleton(child, sids, remaining_sids, joints);
		if(remaining_sids.empty()) return;
	}
}

CMSP ColladaParser::ParseInstanceController(XMLNodeBase* controller_node)
{
	CMSP toAdd = nullptr;
	XMLAttributeBase* instance_controller_url = controller_node->getAttribute("url");
	
	// Parse controller
	Controller* controller = ParseController(make_string(instance_controller_url->getString()));
	if(!controller)
	{
		printf("Cannot find controller to instanciate: %s\n", make_string(instance_controller_url->getString()).c_str());
		return nullptr;
	}
	
	std::vector<int> remainings_sids; remainings_sids.resize(controller->sids.size());
	std::iota(remainings_sids.begin(), remainings_sids.end(), 0);
	
	auto& joints = controller->joints_table;
	XMLNodeBase* skeleton_root = nullptr;
	for (int i = 0; i < controller_node->getChildCount(); i++)
	{	
		XMLNodeBase* child = controller_node->getChildElement(i);
		
		if (child == nullptr)
			continue;

		// Parse material
		if (child->getName() == "bind_material")
		{
			ParseBindMaterial(child);
		}
		else if (child->getName() == "skeleton")
		{
			std::string url = make_string(child->getChildElement(0)->getString());
			url = url.substr(1, url.length()-1);
			
			//TODO(antoine): skeleton root might not be a root in the scene and might need to be transformed
			
			XMLNodeBase* library_visual_scenes = myRoot->getChildElement("library_visual_scenes");
			if(!library_visual_scenes) 
			{
				KIGS_ERROR("Could not find a visual scene", 1);
			}
			// Get skeleton root
			skeleton_root = SearchNode(library_visual_scenes, "node", "id", url, true);
			if(!skeleton_root)
			{
				KIGS_ERROR("Could not find skeleton root", 1);
			}
			FindSkeleton(skeleton_root, controller->sids, remainings_sids, joints);
		}
		else
		{
			printf("TO DO: parse %s in instance_controller child\n", make_string(child->getName()).c_str());
		}
	}
	
	SP<AObjectSkeletonResource> skeleton_resource(nullptr);

	// Remaining joints not found
	if(remainings_sids.size())
	{
		printf("The following joints were not found : \n");
		for(auto& sid : remainings_sids)
		{
			printf("\t%s\n", controller->sids[sid].c_str());
		}
		KIGS_ERROR("Could not find all the joints in the skeleton", 1);
	}
	else
	{
		// TODO(antoine) what happens if the joints are separated by nodes that are not in the joint list?
		skeleton_resource = KigsCore::GetInstanceOf("skeleton_resource", "AObjectSkeletonResource");
		
		// Init skeleton resource
		skeleton_resource->initSkeleton(joints.size(), sizeof(PRSKey));
		
		int i = 0;
		for(const auto& pair : joints)
		{
			XMLNodeBase* node = pair.second;
			std::string nodeName = GetNodeName(node);
			
			unsigned int id = pair.first + 1;
			
			unsigned int uid = 0;
			if(node->getAttribute("id"))
				uid = CharToID::GetID(node->getAttribute("id")->getString());
			
			
			XMLNodeBase* parent=node;
			unsigned int parent_id = 0;
			while(parent = (XMLNodeBase*)parent->getParent())
			{
				if(parent->getName() != "node") break;
				unsigned int joint_index = -1;
				bool found = false;
				for(auto& test : joints)
				{
					if(test.second == parent)
					{
						found = true;
						joint_index = test.first;
						break;
					}
				}
				
				if(found)
				{
					/*XMLAttributeBase* id_attr = parent->getAttribute("id");
					if(id_attr)
					 parent_id = CharToID::GetID(id_attr->getString());
					else
					 parent_id = CharToID::GetID(controller->sids[joint_index]);*/
					
					parent_id = joint_index + 1;
					
					break;
				}
			}
			auto transform = ParseTransform(node);
			printf("%s (%d) id=%u -> parent=%u\n", nodeName.c_str(), i, id, parent_id);
			
			// Add bone to skeleton resource
			skeleton_resource->addBone(i, uid, id, parent_id, controller->joints_inv_bind_matrix[pair.first]);
			
			reinterpret_cast<PRSKey*>(skeleton_resource->getStandData(i))->set(transform);
			reinterpret_cast<PRSKey*>(skeleton_resource->getStandData(i))->m_RotationKey.Normalize();
			++i;
		}		
		
		//TODO(antoine) Save skeleton binary data in xml in base64
		// Export skeleton resource (.SKL)
		skeleton_resource->setValue("SkeletonFileName", shortFileName + "_" + make_string(((XMLNodeBase*)controller_node->getParent())->getAttribute("id")->getString()) + ".skl");
		skeleton_resource->Export();
		
	}
	
	
	std::string source;
	std::map<int, std::vector<char>> offsets;
	
	XMLNodeBase* skin_node = controller->skin_node;
	
	source = skin_node->getAttribute("source")->getString();
	source = source.substr(1, source.length() - 1);
	XMLNodeBase* geometry_lib  = myRoot->getChildElement("library_geometries");
	if (SearchNode(geometry_lib, "geometry", "id", source) != nullptr)
	{
		XMLNodeBase* mesh_node = SearchNode(geometry_lib, "geometry", "id", source);
		toAdd = ParseMesh(mesh_node->getChildElement("mesh"), source, controller);
	}

	// add animated object with skeleton directly on the mesh
	if (toAdd && skeleton_resource)
	{
		CMSP aobject=KigsCore::GetInstanceOf(source + "AObject", "AObject");
		KigsCore::GetCoreApplication()->AddAutoUpdate(aobject.get());
		aobject->addItem(skeleton_resource);
		toAdd->addItem(aobject);
	}

	return toAdd;
}

static std::pair<unsigned int, unsigned int> GetAnimationSizeAndCount(AnimationData* anim)
{
 	std::pair<unsigned int, unsigned int> size_and_count{0,0};
	
	for(auto& channel : anim->channels)
	{
		size_and_count.first += sizeof(IntU32) + sizeof(PRSInfo) + channel.keyCount*(sizeof(PositionKey) + sizeof(RotationKey)+ sizeof(ScaleKey)) + 2*sizeof(IntU32);
		size_and_count.second++;
	}
	
	for(auto sub_anim : anim->sub_anims)
	{
		auto to_add = GetAnimationSizeAndCount(sub_anim);
		size_and_count.first += to_add.first;
		size_and_count.second += to_add.second;
	}
	return size_and_count;
}

static void FillAnimationResource(AnimationResourceInfo* anim_resource, AnimationData* anim, int* current_index)
{
	for(auto& channel : anim->channels)
	{
		unsigned int stream_size = sizeof(IntU32) + sizeof(PRSInfo) + channel.keyCount*(sizeof(PositionKey) + sizeof(RotationKey) + sizeof(ScaleKey));
		AStreamResourceInfo* stream = anim_resource->SetStreamResourceInfo(*current_index, stream_size);
		
		*current_index += 1;
		
		stream->m_StreamGroupID = CharToID::GetID(channel.channelTarget);
		PRSInfo* header = (PRSInfo*)stream->getData();
		
		// Set AnimationResource Header
		// m_Length in milliseconds
		header->m_Length = channel.length*1000;
		header->m_NbPositionKeys = channel.keyCount;
		header->m_NbRotationKeys = channel.keyCount;
		header->m_NbScaleKeys = channel.keyCount;
		
		// Memory copying
		memcpy(header->GetPositionKeys(), channel.positionKeys.data(), channel.keyCount * sizeof(PositionKey));
		memcpy(header->GetRotationKeys(), channel.rotationKeys.data(), channel.keyCount * sizeof(RotationKey));
		memcpy(header->GetScaleKeys(), channel.scaleKeys.data(), channel.keyCount * sizeof(ScaleKey));
	}
	
	// Sub animations iteration
	for(auto sub_anim : anim->sub_anims)
	{
		FillAnimationResource(anim_resource, sub_anim, current_index);
	}
}


static void CreateAnimationFile(std::string animation_name, const std::vector<AnimationData*>& anims)
{
	std::string stream_type = "APRSKeyStream";
	
	// Everything must be aligned on 4 bytes boundaries
	int stream_type_aligned_size = (stream_type.length()+1 + 4 - 1) & ~(4 - 1);
	
	unsigned int total_file_size = sizeof(AResourceFileHeader) + stream_type_aligned_size;
	unsigned int stream_count = 0;
	
	// Malloc
	for(auto anim : anims)
	{
		auto to_add = GetAnimationSizeAndCount(anim);
		total_file_size += to_add.first;
		stream_count += to_add.second;
		
	}
	
	if(!stream_count) return;
	
	AnimationResourceInfo* animationResourceInfo = (AnimationResourceInfo*)malloc(total_file_size);
	animationResourceInfo->m_head.mID = CharToID::GetID(animation_name);
	animationResourceInfo->m_head.m_StreamCount = stream_count;
	animationResourceInfo->m_head.m_StreamTypeNameSize = stream_type_aligned_size;
	
	animationResourceInfo->setStreamType(stream_type);
	
	int stream_index = 0;
	for (auto k = 0; k < anims.size(); k++)
	{
		FillAnimationResource(animationResourceInfo, anims[k], &stream_index);
	}
	animationResourceInfo->m_head.m_StreamCount = stream_index;
	
	// Export animation resource (.ANIM)
	animation_name += ".anim";
	SmartPointer<FileHandle> file = Platform_fopen(animation_name.c_str(), "wb");
	if (file->mFile)
	{
		Platform_fwrite(animationResourceInfo, 1, total_file_size, file.get());
		Platform_fclose(file.get());
	}
	
	free(animationResourceInfo);
}

bool ColladaParser::ParseAnimationClips()
{
	int animationClipCount = 0;
	std::string animationClipName, instanceAnimationUrl;
	
	// Get animation clips node
	XMLNodeBase* animationClipLib = myRoot->getChildElement("library_animation_clips");
	if(!animationClipLib) return false;
	
	animationClipCount = animationClipLib->getChildCount();
	bool found = false;
	
	// Get animations
	XMLNodeBase* anim_library = myRoot->getChildElement("library_animations");
	for (auto i = 0; i < animationClipCount; i++)
	{
		if (animationClipLib->getChildElement("animation_clip") != nullptr)
		{
			XMLNodeBase* currentAnimationClipNode = animationClipLib->getChildElement(i);				
			auto instanceAnimationCount = currentAnimationClipNode->getChildCount();
			
			std::vector<AnimationData*> clip_anims;
			for (auto j = 0; j < instanceAnimationCount; j++)
			{
				if (currentAnimationClipNode->getChildElement(j) != nullptr)
				{
					XMLNodeBase* instanceAnimationNode = currentAnimationClipNode->getChildElement(j);
					if (instanceAnimationNode->getName() == "instance_animation")
					{
						instanceAnimationUrl = instanceAnimationNode->getAttribute("url")->getString();
						instanceAnimationUrl = instanceAnimationUrl.substr(1, instanceAnimationUrl.length() - 1);
						m_CurrentObjectName = instanceAnimationUrl;
						XMLNodeBase* animationNode = SearchNode(anim_library, "animation", "id", instanceAnimationUrl);
						
						if(!animationNode) 
							KIGS_ERROR("Cannot find animation from clip", 1);
						
						// Parse animation and add it to the clip
						clip_anims.push_back(ParseAnimation(animationNode));
					}
				}
			}
			// Create animation file
			CreateAnimationFile(make_string(currentAnimationClipNode->getAttribute("name")->getString()), clip_anims);
			found = true;
		}
	}
	return found;
}

AnimationData* ColladaParser::ParseAnimation(XMLNodeBase* currentAnimationNode)
{
	auto pair = m_animations.emplace(currentAnimationNode, AnimationData{});
	if(!pair.second)
	{
		return &pair.first->second;
	}
	
	AnimationData* animationData = &pair.first->second;
	int animChildCount = 0;
	
	animationData->source_node = currentAnimationNode;
	animChildCount = currentAnimationNode->getChildCount();
	bool has_data = false;
	
	// Sampler data structure
	struct SamplerData
	{
		SourceData* time_source;
		SourceData* matrix_source;
		SourceData* interpolation_source;
	};
	
	std::map<std::string, SamplerData> samplers;
	
	for (auto i = 0; i < animChildCount; i++)
	{
		XMLNodeBase* currentAnimationChildNode = currentAnimationNode->getChildElement(i);
		std::string nodeName = make_string(currentAnimationChildNode->getName());
		if (nodeName  == "sampler")
		{
			int childCount = currentAnimationChildNode->getChildCount();
			
			SamplerData& sampler = samplers[make_string(currentAnimationChildNode->getAttribute("id")->getString())];
			
			for (auto j = 0; j < childCount; j++)
			{
				XMLNodeBase* childSamplerNode = currentAnimationChildNode->getChildElement(j);
				if (childSamplerNode->getName() == "input")
				{
					if (childSamplerNode->getAttribute("semantic") != nullptr)
					{
						XMLAttributeBase* semanticAttribute = childSamplerNode->getAttribute("semantic");
						// Sampler input (time)
						if (semanticAttribute->getString() == "INPUT")
						{
							std::string source = make_string(childSamplerNode->getAttribute("source")->getString());
							source = source.substr(1, source.length() - 1);
							XMLNodeBase* sourceNode = SearchNode(currentAnimationNode, "source", "id", source);
							if (sourceNode)
							{
								sampler.time_source = ParseSource(sourceNode);
							}
						}
						// Sampler output (matrix)
						else if (semanticAttribute->getString() == "OUTPUT")
						{
							std::string source = make_string(childSamplerNode->getAttribute("source")->getString());
							source = source.substr(1, source.length() - 1);
							XMLNodeBase* sourceNode = SearchNode(currentAnimationNode, "source", "id", source);
							if (sourceNode)
							{
								sampler.matrix_source = ParseSource(sourceNode);
							}
						}
						// Sampler interpolation type
						else if (semanticAttribute->getString() == "INTERPOLATION")
						{
							std::string source = make_string(childSamplerNode->getAttribute("source")->getString());
							source = source.substr(1, source.length() - 1);
							XMLNodeBase* sourceNode = SearchNode(currentAnimationNode, "source", "id", source);
							if (sourceNode)
							{
								sampler.interpolation_source = ParseSource(sourceNode);
							}
						}
					}
				}
			}
		}
		
		// Fill animation data
		else if(nodeName == "animation")
		{
			animationData->sub_anims.push_back(ParseAnimation(currentAnimationChildNode));
		}
	}
	
	// Now we can create the channels
	for (auto i = 0; i < animChildCount; i++)
	{
		XMLNodeBase* currentAnimationChildNode = currentAnimationNode->getChildElement(i);
		std::string nodeName = make_string(currentAnimationChildNode->getName());
		if (nodeName == "channel")
		{
			if (currentAnimationChildNode->getAttribute("target") != nullptr)
			{
				std::string target = make_string(currentAnimationChildNode->getAttribute("target")->getString());
				int indexSlash = target.find("/");
				target = target.substr(0, indexSlash);
				
				std::string source_url = make_string(currentAnimationChildNode->getAttribute("source")->getString());
				source_url = source_url.substr(1, source_url.length()-1);
				
				auto it = samplers.find(source_url);
				if(it != samplers.end())
				{
					CreateChannel(animationData, target, it->second.time_source, it->second.matrix_source, it->second.interpolation_source);
				}
				else
					KIGS_ERROR("Cannot find corresponding sampler", 1);
			}
		}
	}
	return animationData;
}

void ColladaParser::CreateChannel(AnimationData* anim, const std::string& target, SourceData* time_source, SourceData* matrix_source,SourceData* interpolation_source)
{
	anim->channels.emplace_back();
	ChannelData* channel = &anim->channels.back();
	channel->keyCount = time_source->item_count;
	channel->channelTarget = target;
	
	channel->positionKeys.resize(matrix_source->item_count);
	channel->rotationKeys.resize(matrix_source->item_count);
	channel->scaleKeys.resize(matrix_source->item_count);
	
	
	// Time values
	for(int i=0; i<time_source->item_count; ++i)
	{
		channel->positionKeys[i].m_Time = time_source->at<float>(i);
		channel->rotationKeys[i].m_Time = time_source->at<float>(i);
		channel->scaleKeys[i].m_Time = time_source->at<float>(i);
	}
	
	channel->length = channel->positionKeys[time_source->item_count-1].m_Time;
	
	// Transformation matrix values
	for(int i=0; i<matrix_source->item_count; ++i)
	{
		const Matrix3x4& m = matrix_source->at<Matrix3x4>(i);
		PRSKey prs;
		prs.set(m);
		channel->positionKeys[i].m_Position = prs.m_PositionKey;
		channel->rotationKeys[i].m_Orientation = prs.m_RotationKey;
		
		channel->rotationKeys[i].m_Orientation.Normalize();
		if (absF(Norm(channel->rotationKeys[i].m_Orientation)-1.0f)>FLT_EPSILON)
		{
			assert(0);
		}
		
		channel->scaleKeys[i].m_ScaleValue = prs.m_ScaleKey;
	}
	
	// Interpolation
	channel->interpolations.resize(interpolation_source->item_count);
	for(int i=0; i<interpolation_source->item_count; ++i)
		channel->interpolations[i] = interpolation_source->at<std::string>(i);
	
}


void ColladaParser::ParseStandaloneAnimations()
{
	int animCount, animChildCount = 0;
	
	XMLNodeBase* animationLibNode = myRoot->getChildElement("library_animations");
	
	if (!animationLibNode)
		return;
	
	animCount = animationLibNode->getChildCount();
	std::vector<AnimationData*> anims;
	if(!animCount) return;
	
	// Parse animations through array 
	for (auto i = 0; i < animCount; i++)
	{
		XMLNodeBase* currentAnimationNode = animationLibNode->getChildElement(i);
		if(currentAnimationNode->getName()=="animation")
			anims.push_back(ParseAnimation(currentAnimationNode));
	}
	
	// Create animation file from array
	if(anims.size() == 1 && anims.front()->channels.size() == 0)
	{
		CreateAnimationFile(GetNodeUniqueID(anims.front()->source_node), anims.front()->sub_anims);
	}
	else
	{
		CreateAnimationFile(anims.size() == 1 ? GetNodeUniqueID(anims.front()->source_node) : shortFileName, anims);
	}
}

Matrix3x4 ColladaParser::ReadMatrix3x4(XMLNodeBase* sceneNode)
{
	Matrix3x4 result;
	XMLNodeBase* value = sceneNode->getChildElement(0);
	
	std::string toParse = make_string(value->getString());
	const char* buffer = toParse.c_str();
	
	AsciiParserUtils parseFloats(const_cast<char*>(buffer), strlen(buffer));
	
	// Parse 3x4 matrix with line/column inversion
	for (auto i = 0; i < 3; i++)
	{
		for (auto j = 0; j < 4; j++)
		{
			parseFloats.ReadFloat(result.e[j][i]);
		}
	}
	return result;
}

float ColladaParser::ReadFloat(XMLNodeBase* sceneNode)
{
	float result = 0.0f;
	XMLNodeBase* value = sceneNode;

	if (sceneNode->getString() == "") // if value is empty for sceneNode, then the value is in first child
	{
		value = sceneNode->getChildElement(0);
	}
	std::string toParse = make_string(value->getString());
	const char* buffer = toParse.c_str();
	AsciiParserUtils	parseFloats(const_cast<char*>(buffer), strlen(buffer));
	parseFloats.ReadFloat(result);
	return result;

}

Point3D ColladaParser::ReadVector3D(XMLNodeBase* sceneNode)
{
	XMLNodeBase* value = sceneNode;

	if (sceneNode->getString() == "") // if value is empty for sceneNode, then the value is in first child
	{
		value = sceneNode->getChildElement(0);
	}

	Point3D result;
	std::string toParse = make_string(value->getString());
	const char* buffer = toParse.c_str();
	
	AsciiParserUtils	parseFloats(const_cast<char*>(buffer), strlen(buffer));
	
	parseFloats.ReadFloat(result.x);
	parseFloats.ReadFloat(result.y);
	parseFloats.ReadFloat(result.z);
	return result;
}

Vector4D ColladaParser::ReadVector4D(XMLNodeBase* sceneNode)
{
	Vector4D result;
	XMLNodeBase* value = sceneNode->getChildElement(0);
	
	std::string toParse = make_string(value->getString());
	const char* buffer = toParse.c_str();
	
	AsciiParserUtils	parseFloats(const_cast<char*>(buffer), strlen(buffer));
	
	parseFloats.ReadFloat(result.x);
	parseFloats.ReadFloat(result.y);
	parseFloats.ReadFloat(result.z);
	parseFloats.ReadFloat(result.w);
	
	return result;
}

Quaternion ColladaParser::ReadQuaternion(XMLNodeBase* sceneNode)
{
	Vector4D result = ReadVector4D(sceneNode);
	return Quaternion(result.x, result.y, result.z, result.x);
}


ColladaParser::ReadMaterial::ReadMaterial() : shininess(1.0f), indexOfRefraction(1.0f), transparency(1.0f), name(""), m_Material(0)
{
	ambiantColor[0] = ambiantColor[1] = ambiantColor[2] = 0.2f;
	emissionColor[0] = emissionColor[1] = emissionColor[2] = 0.8f;
	diffuseColor[0] = diffuseColor[1] = diffuseColor[2] = 0.0f;
	specularColor[0] = specularColor[1] = specularColor[2] = 0.0f;
	
}

ColladaParser::ReadMaterial::~ReadMaterial()
{

}

void	ColladaParser::ReadMaterial::Init()
{
	if (!m_Material)
	{
		m_Material = KigsCore::GetInstanceOf(name, "Material");
#ifndef NO_MULTISTAGE_RENDERING
#ifndef DO_MULTISTAGE_RENDERING
#error	"missing ScenegraphDefines.h"
#endif
		if (!texturesName.empty())
		{
			SP<MaterialStage> MatStage = KigsCore::GetInstanceOf(name + "MatStage1", "MaterialStage");
			
			for (auto i = 0; i < texturesName.size(); i++)
			{
				SP<TextureFileManager>	fileManager = KigsCore::GetSingleton("TextureFileManager");
				SP<Texture> Tex = fileManager->GetTexture(texturesName.at(i), false);
				Tex->setValue("ForcePow2", true);
				Tex->setValue("HasMipmap", true);
				Tex->Init();
				MatStage->addItem((CMSP&)Tex);
				
				//Tex = nullptr;
			}
			MatStage->Init();
			
			m_Material->addItem((CMSP&)MatStage);
		}
		if (!NormalMapName.empty())
		{
			SP<MaterialStage> MatStage = KigsCore::GetInstanceOf(name + "MatStage2", "MaterialStage");
			
			for (auto i = 0; i < NormalMapName.size(); i++)
			{
				SP<TextureFileManager>	fileManager = KigsCore::GetSingleton("TextureFileManager");
				SP<Texture> Tex = fileManager->GetTexture(NormalMapName.at(i), false);
				Tex->setValue("ForcePow2", true);
				Tex->setValue("HasMipmap", true);
				Tex->Init();
				MatStage->addItem((CMSP&)Tex);
				MatStage->setValue("TextureChannel", 1);
			}
			MatStage->Init();
			
			m_Material->addItem((CMSP&)MatStage);
		}
		
#else
		if (textureName != "")
		{
			TextureFileManager*	fileManager = (TextureFileManager*)KigsCore::GetSingleton("TextureFileManager");
			Texture* Tex = fileManager->GetTexture(textureName, false);
			Tex->setValue("ForcePow2", true);
			Tex->Init();
			
			m_Material->addItem(Tex);
			Tex->Destroy();
			Tex = nullptr;
		}
		
#endif
		m_Material->SetAmbientColor(ambiantColor[0], ambiantColor[1], ambiantColor[2]);
		m_Material->SetDiffuseColor(diffuseColor[0], diffuseColor[1], diffuseColor[2]);
		m_Material->SetSpecularColor(specularColor[0], specularColor[1], specularColor[2]);
		m_Material->SetEmissionColor(emissionColor[0], emissionColor[1], emissionColor[2]);
		m_Material->setValue("Shininess", shininess);
		m_Material->setValue("Transparency", transparency);
		
		//m_Material->Init();
	}
}


SourceData::SourceData(XMLNodeBase* node)
{
	source_node = node;
	XMLNodeBase* array = source_node->getChildElement("float_array");
	if(!array) array = source_node->getChildElement("Name_array");
	if(!array) array = source_node->getChildElement("int_array");
	
	
	if (!array)
	{
		KIGS_ERROR("No array found under the source node", 1);
	}
	
	XMLNodeBase* technique_common_node = source_node->getChildElement("technique_common");
	if (!technique_common_node)
	{
		KIGS_ERROR("No technique_common node under the source node", 1);
	}
	
	XMLNodeBase* accessor = technique_common_node->getChildElement("accessor");
	if (!accessor)
	{
		KIGS_ERROR("No accessor node under the technique_common node", 1);
	}
	
	
	if (accessor->getChildCount() == 0)
	{
		KIGS_ERROR("No param node found under the accessor node", 1);
	}
	
	XMLNodeBase* param = accessor->getChildElement(0);
	
	std::string type_str = make_string(param->getAttribute("type")->getString());
	std::transform(type_str.begin(), type_str.end(), type_str.begin(), ::tolower);
	
	int stride = 1;
	if (accessor->getAttribute("stride") != nullptr)
	{
		stride = accessor->getAttribute("stride")->getInt();
	}
	
	
	int element_count = array->getAttribute("count")->getInt();
	item_count = stride ? element_count / stride : element_count;

	if (element_count == 0)
	{
		return;
	}

	// name array
	if (type_str == "name")
	{
		type = NameArray;
		dyn_buffer = new DynamicGrowingBuffer<char[128]>(item_count);
	}
	// float array
	else if (type_str == "float")
	{
		if (stride == 1)
		{
			type = FloatArray;
			dyn_buffer = new DynamicGrowingBuffer<float>(item_count);
		}
		else if (stride == 2)
		{
			type = Vector2DArray;
			dyn_buffer = new DynamicGrowingBuffer<Point2D>(item_count);
		}
		else if (stride == 3)
		{
			type = Vector3DArray;
			dyn_buffer = new DynamicGrowingBuffer<Vector3D>(item_count);
		}
		else if (stride == 4)
		{
			type = Vector4DArray;
			dyn_buffer = new DynamicGrowingBuffer<Vector4D>(item_count);
		}
		else
		{
			KIGS_ERROR("Unsupported float array stride", 1);
		}
	}
	// 4x4 matrix array
	else if (type_str == "float4x4")
	{
		type = MatrixArray;
		dyn_buffer = new DynamicGrowingBuffer<Matrix3x4>(item_count);
	}
	else
	{
		KIGS_ERROR("Unsupported array element type", 1);
	}
	
	std::string data_string = make_string(array->getChildElement(0)->getString());
	
	
	AsciiParserUtils parser(const_cast<char*>(data_string.c_str()), data_string.length());
	
	int k=0;
	while(k<item_count)
	{
		switch(type)
		{
			case NameArray:
			{
				auto& buffer = *(DynamicGrowingBuffer<char[128]>*)(dyn_buffer);
				parser.ReadString(buffer[k], 128);
			}
			break;
			case FloatArray:
			{
				auto& buffer = *(DynamicGrowingBuffer<float>*)(dyn_buffer);
				parser.ReadFloat(buffer[k]);
			}
			break;
			case Vector2DArray:
			{
				auto& buffer = *(DynamicGrowingBuffer<Point2D>*)(dyn_buffer);
				parser.ReadFloat(buffer[k].x);
				parser.ReadFloat(buffer[k].y);
			}
			break;
			case Vector3DArray:
			{
				auto& buffer = *(DynamicGrowingBuffer<Vector3D>*)(dyn_buffer);
				parser.ReadFloat(buffer[k].x);
				parser.ReadFloat(buffer[k].y);
				parser.ReadFloat(buffer[k].z);
			}
			break;
			case Vector4DArray:
			{
				auto& buffer = *(DynamicGrowingBuffer<Vector4D>*)(dyn_buffer);
				parser.ReadFloat(buffer[k].x);
				parser.ReadFloat(buffer[k].y);
				parser.ReadFloat(buffer[k].z);
				parser.ReadFloat(buffer[k].w);
			}
			break;
			case MatrixArray:
			{
				auto& buffer = *(DynamicGrowingBuffer<Matrix3x4>*)(dyn_buffer);
				for (auto i = 0; i < 3; i++)
				{
					for (auto j = 0; j < 4; j++)
					{
						parser.ReadFloat(buffer[k].e[j][i]);
					}
				}
				float f;
				for (auto j = 0; j < 4; j++)
					parser.ReadFloat(f);
			}
			break;
			default:
			KIGS_ERROR("Unknown stride", 1);
		}
		++k;
	}
}


SourceData::~SourceData()
{
	if(dyn_buffer)
	{
		switch(type)
		{
			case NameArray:
			delete (DynamicGrowingBuffer<char[128]>*)dyn_buffer;
			break;
			case FloatArray:
			delete (DynamicGrowingBuffer<float>*)dyn_buffer;
			break;
			case Vector2DArray:
			delete (DynamicGrowingBuffer<Point2D>*)dyn_buffer;
			break;
			case Vector3DArray:
			delete (DynamicGrowingBuffer<Vector3D>*)dyn_buffer;
			break;
			case Vector4DArray:
			delete (DynamicGrowingBuffer<Vector4D>*)dyn_buffer;
			break;
			case MatrixArray:
			delete (DynamicGrowingBuffer<Matrix3x4>*)dyn_buffer;
			break;
			default:
			KIGS_ERROR("Unknown type", 1);
		}
	}
}

