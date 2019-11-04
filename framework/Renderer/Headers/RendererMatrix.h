#ifndef _RENDERERMATRIX_H_
#define _RENDERERMATRIX_H_

#include "Drawable.h"
#include "TecLibs/Tec3D.h"

// ****************************************
// * RendererMatrix class
// * --------------------------------------
/**
 * \file	RendererMatrix.h
 * \class	RendererMatrix
 * \ingroup Drawable
 * \ingroup RendererDrawable
 * \brief	base for a matrix renderer
 * \author	ukn
 * \version ukn
 * \date	ukn
 *
 * Exported parameters :<br>
 * <ul>
 * <li>
 *		string <strong>Matrix</strong> : used matrix
 * </li>
 * </ul>
 */
// ****************************************
class RendererMatrix : public Drawable 
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(RendererMatrix,Drawable,Renderer)

	/**
	 * \brief	constructor
	 * \fn 		RendererMatrix(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
    RendererMatrix(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
    
	/**
	 * \brief	initialise pre draw method
	 * \fn 		virtual bool PreDraw(TravState*);
	 * \param	travstate : camera state
	 * \return	TRUE if a could PreDraw
	 */
	bool	PreDraw(TravState* state) override
	{
		PushMatrix(state);
		return true;
	}

	/**
	 * \brief	initialise PostDraw method
	 * \fn 		virtual bool PostDraw(TravState*);
	 * \param	travstate : camera state
	 * \return	TRUE if a could PostDraw
	 */
	bool	PostDraw(TravState*  state ) override
	{
		PopMatrix(state);
		return true;
	}
	  
	/**
	 * \brief		initialize with a 3x4 matrix
	 * \fn			void	Init(const Matrix3x4& matrix); 
	 * \param		matrix : used matrix
	 */
	void	Init(const Matrix3x4& matrix);
	using	Drawable::Init;
	
	/**
	 * \brief		initialize with a 4x4 matrix
	 * \fn			void	Init(const Matrix4x4& matrix); 
	 * \param		matrix : used matrix
	 */
	void	Init(const Matrix4x4& matrix);


	/**
	 * \brief		push the matrix
	 * \fn			virtual void	PushMatrix() const =0;
	 */
	virtual void	PushMatrix(TravState* state) const =0;

	/**
	 * \brief		set the matrix
	 * \fn			virtual void	SetMatrix() const = 0;
	 */
	virtual void	SetMatrix(TravState* state) const = 0;

	/**
	 * \brief		pop the matrix
	 * \fn			virtual void	PopMatrix() const =0;
	 */
	virtual void	PopMatrix(TravState* state) const =0;

	/**
	 * \brief		Retrieve the matrix from the current rendering state 
	 * \fn			virtual void	RetrieveFromCurrentRenderingState()=0; 
	 */
	virtual void	RetrieveFromCurrentRenderingState(TravState* state)=0;

	/**
	 * \brief		initialize with the identity matrix
	 * \fn			void	InitToIdentity();
	 * \param		matrix : used matrix
	 */
	void	InitToIdentity();

	unsigned int	GetSelfDrawingNeeds() override
	{
		return ((unsigned int)Need_Predraw)|((unsigned int)Need_Postdraw);
	}

	const kfloat*	GetMatrixValues()
	{
		return myMatrix.getConstArrayBuffer();
	}
	
protected:
	/**
	 * \brief	destructor
	 * \fn 		~RendererMatrix();
	 */
    virtual ~RendererMatrix();  

	//! used matrix
	maVect16DF	myMatrix;
}; 

#endif //_RENDERERMATRIX_H_
