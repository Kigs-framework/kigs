#pragma once

#include "Material.h"

struct ID3D11Buffer;
namespace Kigs
{
	namespace Draw
	{
		// ****************************************
		// * DX11Material class
		// * --------------------------------------
		/**
		 * \file	DX11Material.h
		 * \class	DX11Material
		 * \ingroup Renderer
		 * \brief	DX11 implementation of Material.
		 *
		 */
		 // ****************************************
		class DX11Material : public Material
		{
		public:
			DECLARE_CLASS_INFO(DX11Material, Material, Renderer)
				DECLARE_INLINE_CONSTRUCTOR(DX11Material) {}
			~DX11Material() override;

			bool	PreDraw(TravState*) override;
			bool	PostDraw(TravState* travstate) override;

		protected:
			void NotifyUpdate(unsigned int labelid) override;

		};

	}
}
