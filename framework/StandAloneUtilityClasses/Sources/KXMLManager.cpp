#include "KXMLManager.h"

IMPLEMENT_CLASS_INFO(KXMLManager);

//-------------------------------------------------------------------------
//Constructor
KXMLManager::KXMLManager(const kstl::string& name, CLASS_NAME_TREE_ARG)
:CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, myUseZSTD(*this, false, LABEL_AND_ID(UseZSTD),true)
{

}

//-------------------------------------------------------------------------
//Destructor

KXMLManager::~KXMLManager()
{
	for (auto& dctx : mZSTDDecompressContexts)
	{
		ZSTD_freeDCtx(dctx.second);
	}
	for (auto& cctx : mZSTDCompressContexts)
	{
		ZSTD_freeCCtx(cctx.second);
	}
}
