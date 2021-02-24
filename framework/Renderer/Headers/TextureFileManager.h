#ifndef _TEXTUREFILEMANAGER_H_
#define _TEXTUREFILEMANAGER_H_

#include "CoreModifiable.h"
#include "SpriteSheetTexture.h"

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
	* \fn		TextureFileManager(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	TextureFileManager(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	/**
	* \brief	return TRUE if a texture with the name 'fileName' exist
	* \fn		bool	HasTexture(const kstl::string& fileName);
	* \param	fileName : filemane of the asked texture
	* \return	TRUE if a texture with the name 'fileName' exist
	*/
	bool HasTexture(const kstl::string& fileName);

	/**
	* \brief	get a texture
	* \fn		Texture* GetTexture(const kstl::string& fileName);
	* \param	fileName : filemane of the asked texture
	* \return	the asked texture if found
	*/
	SP<Texture> GetTexture(const kstl::string& fileName,bool doInit=true);

	/**
	* \brief	get a texture
	* \fn		Texture* GetTexture(const kstl::string& fileName);
	* \param	fileName : filemane of the asked texture
	* \return	the asked texture if found
	*/
	SP<Texture> GetTexture(const kstl::string& fileName, const kstl::string& a_textureName, bool doInit=true);

	/**
	* \brief	get a spritesheey
	* \fn		SpriteSheetTexture* GetSpriteSheetTexture(const kstl::string& fileName);
	* \param	fileName : filemane of the asked SpriteSheet Texture
	* \return	the asked SpriteSheetTexture if found
	*/
	SP<SpriteSheetTexture> GetSpriteSheetTexture(const kstl::string& fileName);

	/**
	* \brief	add a texture
	* \fn		void AddTexture(const kstl::string& fileName, Texture* tex);
	* \param	fileName : filemane of the texture to add
	* \param	tex : texture to add
	*/
	void AddTexture(const kstl::string& fileName, CoreModifiable* Tex);
	void AddTexture(const kstl::string& fileName, CMSP& Tex);

	/**
	* \brief	unload a texture
	* \fn 		void  UnloadTexture(Texture* tex);
	* \param	tex : filemane of the texture to unload
	*/
	void UnloadTexture(Texture* Texture);
	void UnloadTexture(SpriteSheetTexture* Tex);
	void UnloadAllTexture();
	void ResetAllTexture();

	void ClearCache();

	CMSP CreateTexture(const kstl::string& textureName);
	CMSP CreateSpriteSheetTexture(const kstl::string& textureName);


	KIGS_TOOLS_ONLY(auto& GetTextureMap() const { return mTextureMap; })


	std::string	GetTextureFromSpriteSheetJSON(const std::string& json)
	{
		const auto& found = mSpritesheetMap.find(json);
		if (found != mSpritesheetMap.end())
		{
			return (*found).second;
		}
		return "";
	}

protected:
	/**
	* \brief	destructor
	* \fn		~TextureFileManager();
	*/
	virtual ~TextureFileManager();

	//! associate a texture to its name
	kstl::map<std::string, CoreModifiable*> mTextureMap;
	//! associate a spritesheet json file to its texture name
	kstl::map<std::string, std::string>		mSpritesheetMap;
};

#endif //_TEXTUREFILEMANAGER_H_