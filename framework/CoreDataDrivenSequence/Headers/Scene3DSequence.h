#ifndef _SCENE3DSEQUENCE_H_
#define _SCENE3DSEQUENCE_H_

#include "BaseDDSequence.h"
#include "maUSString.h"


class Scene3DSequence : public BaseDDSequence
{
public:
	DECLARE_CLASS_INFO(Scene3DSequence, BaseDDSequence, CoreDataDrivenSequence)

	//! constructor
	Scene3DSequence(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

protected:
	//! destructor
	virtual ~Scene3DSequence();
	
	void	InitModifiable() override;
	void    ProtectedDestroy() override;
};

#endif //_SCENE3DSEQUENCE_H_
