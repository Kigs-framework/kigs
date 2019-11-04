#include "PrecompiledHeaders.h"
#include "maCoreItem.h"
#include "JSonFileParser.h"
#include "CoreItemOperator.h"




bool maCoreItemValue::ExportToString(kstl::string& value) const
{
	JSonFileParser L_JsonParser;
	return L_JsonParser.ExportToString((CoreMap<kstl::string> *)(CoreItem*)item.get(), value);
}

void maCoreItemValue::InitWithJSON(const kstl::string& currentval, CoreModifiable* owner)
{
	if(item)
	{
		item.Reset();
	}

	JSonFileParser L_JsonParser;
	if(currentval[0]=='#')
	{
		const char* filename=currentval.c_str()+1;
		ref_file = filename;
		item = OwningRawPtrToSmartPtr(L_JsonParser.Get_JsonDictionary(filename));
	}
	else
	{
		if (currentval.substr(0, 4) == "eval")
		{
			// check if eval float or string
			if (currentval.substr(4, 3) == "Str")
			{
				kstl::string toeval = currentval.substr(7, currentval.length() - 7);

				CoreItem* toAdd = &CoreItemOperator<kstl::string>::Construct(toeval, owner, KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
				item = OwningRawPtrToSmartPtr(toAdd);
			}
			else
			{
				kstl::string toeval = currentval.substr(4, currentval.length() - 4);

				CoreItem* toAdd = &CoreItemOperator<kfloat>::Construct(toeval, owner, KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
				item = OwningRawPtrToSmartPtr(toAdd);
			}
		}
		else
		{
			ref_file = "";
			item = OwningRawPtrToSmartPtr(L_JsonParser.Get_JsonDictionaryFromString(currentval));
		}
	}
}






