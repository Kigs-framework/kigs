#ifndef _CORESEQUENCELAUNCHER_H_
#define _CORESEQUENCELAUNCHER_H_

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "maCoreItem.h"

#include "CoreSequence.h"

#include "AttributePacking.h"

class CoreSequenceLauncher : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(CoreSequenceLauncher, CoreModifiable, CoreAnimation);
	DECLARE_INLINE_CONSTRUCTOR(CoreSequenceLauncher){}
	WRAP_METHODS(Start, Stop, IsFinished);

protected:
	void	InitModifiable() override;

	void	checkDeadSequences();
	void	addSequencesToParents();

	//! parameter for fullscreen window
	maBool			myStartOnFirstUpdate = BASE_ATTRIBUTE(StartOnFirstUpdate, false);
	maCoreItem		mySequence = BASE_ATTRIBUTE(Sequence);
	maString		myStartMessage = BASE_ATTRIBUTE(StartMessage, "");
	maBool			myOnce = BASE_ATTRIBUTE(Once, false);

	kigs::unordered_map<CoreModifiable*, SmartPointer<CoreSequence>>	mySequenceMap;

	void Start();
	void Stop();
	bool IsFinished();
};

#endif //_CORESEQUENCELAUNCHER_H_
