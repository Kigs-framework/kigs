#pragma once

#include "ModuleBase.h"
#include "AMDefines.h"

/*! \defgroup Animation Animation module
 *  animation module
*/

// ****************************************
// * GenericAnimationModule class
// * --------------------------------------
/*!  \class GenericAnimationModule
     this class is the module manager class.
  \ingroup Animation
*/
// ****************************************

#include <map>
#include "kstlstring.h"
#include "AnimationResourceInfo.h"

#include "AttributePacking.h"

namespace Kigs
{
	namespace Anim
	{
		using namespace Kigs::Core;
		class AObject;
		class AAnimatedValue;
		class GenericAnimationModule;

		extern GenericAnimationModule* gGenericAnimationModule;

		class GenericAnimationModule : public ModuleBase
		{
		public:
			DECLARE_CLASS_INFO(GenericAnimationModule, ModuleBase, Animation);
			DECLARE_CONSTRUCTOR(GenericAnimationModule);

			//! module init
			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;

			//! module close
			void Close() override;

			//! module update
			void Update(const Time::Timer& timer, void* addParam) override;



			WRAP_METHODS(LoadAnimation);


			AnimationResourceInfo* LoadAnimation(const std::string& fileName);


			void	UnLoad(const std::string& fileName);
			void	UnLoad(AnimationResourceInfo* info);

			void	addShader(SP<CoreModifiable> parent, SP<CoreModifiable> shader)
			{
				mPostAddShaderList.push_back(std::pair<SP<CoreModifiable>, SP<CoreModifiable>>{parent, shader});
			}
			virtual ~GenericAnimationModule();

		protected:


			std::map<std::string, SP<CoreRawBuffer>>	mResourceInfoMap;

			void ManagePostAdd()
			{
				std::vector<std::pair<SP<CoreModifiable>, SP<CoreModifiable>> >::iterator	itc = mPostAddShaderList.begin();
				std::vector<std::pair<SP<CoreModifiable>, SP<CoreModifiable>> >::iterator	ite = mPostAddShaderList.end();

				while (itc != ite)
				{
					(*itc).first->addItem((*itc).second, CoreModifiable::First);
					itc++;
				}
				mPostAddShaderList.clear();
			}
			std::vector<std::pair<SP<CoreModifiable>, SP<CoreModifiable>> >	mPostAddShaderList;


		};

	}
}
