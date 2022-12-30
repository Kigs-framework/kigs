#pragma once

#include "CoreModifiable.h"
#include "maReference.h"
#include "CoreModifiableAttribute.h"

namespace Kigs
{
	namespace Scene
	{
		class TravState;
	}
	namespace Draw
	{
		using namespace Kigs::Core;
		class API3DDeferred;



		// ****************************************
		// * API3DDeferredFilter class
		// * --------------------------------------
		/**
		 * \file	GLSLDeferredFilter.h
		 * \class	API3DDeferredFilter
		 * \ingroup Renderer
		 * \brief	GLSL deferred shader implementation.
		 *
		 * ?? Obsolete ??
		 */
		 // ****************************************

		class API3DDeferredFilter : public CoreModifiable
		{
		public:
			friend class API3DDeferred;

			const static unsigned int PreDrawPass = 1;
			const static unsigned int PreGeoPass = 2;
			const static unsigned int PostGeoPass = 4;
			const static unsigned int PreLightPass = 8;
			const static unsigned int PostLightPass = 16;
			const static unsigned int PostDrawPass = 32;

			struct PriorityCompare
			{
				//! overload operator () for comparison
				bool operator()(const API3DDeferredFilter* a1, const API3DDeferredFilter* a2) const
				{
					if (a1->mPriority == a2->mPriority)
						return (a1) < (a2);
					return a1->mPriority < a2->mPriority;
				}
			};

			DECLARE_CLASS_INFO(API3DDeferredFilter, CoreModifiable, Renderer)
				DECLARE_CONSTRUCTOR(API3DDeferredFilter)


				bool NeedPass(const unsigned int);

		protected:
			virtual unsigned int PreDrawTreatment(Scene::TravState*, API3DDeferred*) { return -1; }
			virtual unsigned int PreGeometryTreatment(Scene::TravState*, API3DDeferred*) { return -1; }
			virtual unsigned int PostGeometryTreatment(Scene::TravState*, API3DDeferred*) { return -1; }
			virtual unsigned int PreLightTreatment(Scene::TravState*, API3DDeferred*) { return -1; }
			virtual unsigned int PostLightTreatment(Scene::TravState*, API3DDeferred*) { return -1; }
			virtual unsigned int PostDrawTreatment(Scene::TravState*, API3DDeferred*) { return -1; }

			maUInt mPriority;
			maUInt mDrawPass;

			maReference mTarget;
		};

	}
}
