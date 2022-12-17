#ifndef _TEXTUREFILEMANAGER_H_
#define _TEXTUREFILEMANAGER_H_

#include "CoreModifiable.h"
#include "Texture.h"



// ****************************************
// * TextureFileManager class
// * --------------------------------------
/**
* \file	TextureFileManager.h
* \class	TextureFileManager
* \ingroup Renderer
* \brief Manage texture to avoid redundant instances.
*
*/
// ****************************************
class TextureFileManager : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(TextureFileManager,CoreModifiable,Renderer)

	/**
	* \brief	constructor
	* \fn		TextureFileManager(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	TextureFileManager(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	/**
	* \brief	destructor
	* \fn		~TextureFileManager();
	*/
	virtual ~TextureFileManager();

	/**
	* \brief	return TRUE if a texture with the name 'fileName' exist
	* \fn		bool	HasTexture(const std::string& fileName);
	* \param	fileName : filemane of the asked texture
	* \return	TRUE if a texture with the name 'fileName' exist
	*/
	bool HasTexture(const std::string& fileName);

	/**
	* \brief	get a texture
	* \fn		Texture* GetTexture(const std::string& fileName);
	* \param	fileName : filemane of the asked texture
	* \return	the asked texture if found
	*/
	SP<Texture> GetTexture(const std::string& fileName,bool doInit=true);

	/**
	* \brief	get a texture
	* \fn		Texture* GetTexture(const std::string& fileName);
	* \param	fileName : filemane of the asked texture
	* \return	the asked texture if found
	*/
	SP<Texture> GetTexture(const std::string& fileName, const std::string& a_textureName, bool doInit=true);

	/**
	* \brief	add a texture
	* \fn		void AddTexture(const std::string& fileName, Texture* tex);
	* \param	fileName : filemane of the texture to add
	* \param	tex : texture to add
	*/
	void AddTexture(const std::string& fileName, CoreModifiable* Tex);
	void AddTexture(const std::string& fileName, const CMSP& Tex);

	/**
	* \brief	unload a texture
	* \fn 		void  UnloadTexture(Texture* tex);
	* \param	tex : filemane of the texture to unload
	*/
	void UnloadTexture(Texture* Texture);
	void UnloadAllTexture();
	void ResetAllTexture();

	void ClearCache();

	CMSP CreateTexture(const std::string& textureName);


	KIGS_TOOLS_ONLY(auto& GetTextureMap() const { return mTextureMap; })

	SP<SpriteSheetDataResource>	GetTextureFromSpriteSheetJSON(const std::string& json) const
	{
		const auto& found = mSpritesheetMap.find(json);
		if (found != mSpritesheetMap.end())
		{
			return (*found).second.lock();
		}
		return nullptr;
	}

	void	InsertSpriteSheetJSONReference(const std::string& json, SP<SpriteSheetDataResource> dr)
	{
		if (dr)
		{
			mSpritesheetMap[json] = dr;
		}
	}

protected:
	//! associate a texture to its name
	std::map<std::string, CoreModifiable*> mTextureMap;
	//! associate a spritesheet json file to its texture name
	std::map<std::string, std::weak_ptr<SpriteSheetDataResource>>		mSpritesheetMap;
};

#endif //_TEXTUREFILEMANAGER_H_