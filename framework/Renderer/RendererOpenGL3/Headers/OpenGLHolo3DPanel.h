#ifndef _OPENGLHOLO3DPANEL_H_
#define _OPENGLHOLO3DPANEL_H_

#include "Holo3DPanel.h"

// ****************************************
// * OpenGLHolo3DPanel class
// * --------------------------------------
/**
 * \file	OpenGLHolo3DPanel.h
 * \class	OpenGLHolo3DPanel
 * \ingroup Renderer
 * \brief	OpenGL implementation of Holo3DPanel.
 */
 // ****************************************
class OpenGLHolo3DPanel : public Holo3DPanel
{
	DECLARE_CLASS_INFO(OpenGLHolo3DPanel, Holo3DPanel, Renderer);
	DECLARE_INLINE_CONSTRUCTOR(OpenGLHolo3DPanel) {}

public:
	bool Draw(TravState* travstate) override;
};

#endif //_HOLO3DPANEL_H_
