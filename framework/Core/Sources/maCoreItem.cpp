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
		item = nullptr;
	}

	JSonFileParser L_JsonParser;
	if(currentval[0]=='#')
	{
		const char* filename=currentval.c_str()+1;
		ref_file = filename;
		item = L_JsonParser.Get_JsonDictionary(filename);
	}
	else
	{
		if (currentval.substr(0, 4) == "eval")
		{
			// check if eval float or string
			if (currentval.substr(4, 3) == "Str")
			{
				kstl::string toeval = currentval.substr(7, currentval.length() - 7);
				item = CoreItemOperator<kstl::string>::Construct(toeval, owner, KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
			}
			else if(currentval.substr(4, 2) == "2D")
			{
				kstl::string toeval = currentval.substr(6, currentval.length() - 6);
				item = CoreItemOperator<Point2D>::Construct(toeval, owner, KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
			}
			else if (currentval.substr(4, 2) == "3D")
			{
				kstl::string toeval = currentval.substr(6, currentval.length() - 6);
				item = CoreItemOperator<Point3D>::Construct(toeval, owner, KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
			}
			else if (currentval.substr(4, 2) == "4D")
			{
				kstl::string toeval = currentval.substr(6, currentval.length() - 6);
				item = CoreItemOperator<Vector4D>::Construct(toeval, owner, KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
			}
			else
			{
				int cutpos = 4;
				if (currentval.substr(4, 2) == "1D")
				{
					cutpos = 6;
				}
				kstl::string toeval = currentval.substr(cutpos, currentval.length() - cutpos);
				item = CoreItemOperator<kfloat>::Construct(toeval, owner, KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap());
			}
		}
		else
		{
			ref_file = "";
			item = L_JsonParser.Get_JsonDictionaryFromString(currentval);
		}
	}
}






