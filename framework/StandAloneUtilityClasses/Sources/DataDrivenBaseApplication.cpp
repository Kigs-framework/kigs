#include "PrecompiledHeaders.h"
#include "DataDrivenBaseApplication.h"
#include "FilePathManager.h"
#include "../GUI/Headers/ModuleGUI.h"
#include "../Renderer/Headers/ModuleRenderer.h"
#include "../SceneGraph/Headers/ModuleSceneGraph.h"
#include "../Input/Headers/ModuleInput.h"
#include "../2DLayers/Headers/Module2DLayers.h"
#include "../GUI/Headers/DisplayDeviceCaps.h"
#include "../CoreAnimation/Headers/ModuleCoreAnimation.h"
#include "../LuaKigsBind/Headers/LuaKigsBindModule.h"
#include "../StandAloneUtilityClasses/Headers/LocalizationManager.h"
#include "NotificationCenter.h"
#include "maReference.h"
#include "BaseUI2DLayer.h"
#include "ModuleInput.h"
#include "TouchInputEventManager.h"
#include "AttributePacking.h"

#include "Remotery.h"

#include "Sources/KigsTools.cpp"

IMPLEMENT_CLASS_INFO(DataDrivenSequence)

DataDrivenSequence::DataDrivenSequence(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, myKeepParamsOnStateChange(*this, false, LABEL_AND_ID(KeepParamsOnStateChange), false)
, myManager(*this, true, LABEL_AND_ID(SequenceManager),"DataDrivenSequenceManager:AppSequenceManager" ) // default is app
{
}

DataDrivenSequence::~DataDrivenSequence()
{
}

DataDrivenSequenceManager*	DataDrivenSequence::getManager()
{
	return (DataDrivenSequenceManager*)(CoreModifiable*)myManager;
}

void DataDrivenSequence::saveParams(kstl::map<unsigned int, kstl::string>& params)
{
	if (myKeepParamsOnStateChange)
	{
		params.clear();
		const auto& attribmap = getAttributes();
		auto itattribcurrent = attribmap.begin();
		auto itattribend = attribmap.end();

		while (itattribcurrent != itattribend)
		{
			kstl::string value;
			if (getValue((*itattribcurrent).first, value))
			{
				params[(*itattribcurrent).first.toUInt()] = value;
			}
			++itattribcurrent;
		}

	}
}
void DataDrivenSequence::restoreParams(const kstl::map<unsigned int, kstl::string>& params)
{
	if (myKeepParamsOnStateChange)
	{
		kstl::map<unsigned int, kstl::string>::const_iterator	itattribcurrent = params.begin();
		kstl::map<unsigned int, kstl::string>::const_iterator	itattribend = params.end();

		while (itattribcurrent != itattribend)
		{
			setValue((*itattribcurrent).first, (*itattribcurrent).second);
			++itattribcurrent;
		}

	}
}

void DataDrivenSequence::InitModifiable()
{
	DataDrivenSequenceManager* currentManager = (DataDrivenSequenceManager*)getManager();
	if (!currentManager)
	{
		return;
	}

	CoreModifiable::InitModifiable();

	CoreModifiable* currentSequence = currentManager->GetCurrentSequence();

	// first search for a transition
	kstl::set<CoreModifiable*>	instances;
	GetSonInstancesByType("DataDrivenTransition", instances);

	if (instances.size() == 0) // if no transition, destroy previous scene else everything is managed by transition
	{
		if (currentSequence)
		{
			currentManager->ProtectedCloseSequence(currentSequence->getName());
			if (currentSequence->isSubType(DataDrivenSequence::myClassID))
			{
				kstl::map<unsigned int, kstl::string> savedParamsList;
				((DataDrivenSequence*)currentSequence)->saveParams(savedParamsList);
				if (savedParamsList.size())
				{
					currentManager->mySequenceParametersMap[currentSequence->getName()] = savedParamsList;
				}
			}
			if (currentSequence) currentSequence->SimpleCall("SequenceEnd");
			currentManager->myCurrentSequence = nullptr;
			currentSequence->UnInit();
			currentSequence->Destroy();
		}
	}

	// search son layout and add them to scenegraph
	// Abstract2DLayer inherit Scene3D
	instances.clear();
	GetSonInstancesByType("Scene3D", instances);

	ModuleSceneGraph* scenegraph = (ModuleSceneGraph*)KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex);

	kstl::set<CoreModifiable*>::iterator	it = instances.begin();
	kstl::set<CoreModifiable*>::iterator	itend = instances.end();

	for (; it != itend; ++it)
	{
		scenegraph->addItem(*it);
	}
}

void DataDrivenSequence::UninitModifiable()
{
	// close all sons
	const kstl::vector<ModifiableItemStruct>& items = getItems();
	kstl::vector<ModifiableItemStruct>::const_iterator	it = items.begin();
	kstl::vector<ModifiableItemStruct>::const_iterator	itend = items.end();

	for (; it != itend; ++it)
	{
		(*it).myItem->UnInit();
	}
	CoreModifiable::UninitModifiable();
}

IMPLEMENT_CLASS_INFO(DataDrivenTransition)

DataDrivenTransition::DataDrivenTransition(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, myPreviousSequence(0)
, myAnimPrev(*this, false, LABEL_AND_ID(PreviousAnim))
, myAnimNext(*this, false, LABEL_AND_ID(NextAnim))
, myIsFirstUpdate(false)
, myManager(*this, true, LABEL_AND_ID(SequenceManager), "DataDrivenSequenceManager:AppSequenceManager") // default is app
{}

DataDrivenSequenceManager*	DataDrivenTransition::getManager()
{
	return (DataDrivenSequenceManager*)(CoreModifiable*)myManager;
}

DataDrivenTransition::~DataDrivenTransition()
{
	setInTransition(false);

	kstl::vector<CoreModifiable*>::iterator itlauncher;
	for (itlauncher = myPrevLauncherList.begin(); itlauncher != myPrevLauncherList.end(); ++itlauncher)
		(*itlauncher)->Destroy();
	myPrevLauncherList.clear();
	for (itlauncher = myNextLauncherList.begin(); itlauncher != myNextLauncherList.end(); ++itlauncher)
		(*itlauncher)->Destroy();
	myNextLauncherList.clear();

	if (myPreviousSequence)
	{
		myPreviousSequence->UnInit();
		myPreviousSequence->Destroy();
		myPreviousSequence = 0;
	}
}

void DataDrivenTransition::setInTransition(bool intransition)
{
	DataDrivenSequenceManager* currentManager = (DataDrivenSequenceManager*)getManager();
	if (currentManager)
	{
		currentManager->setInTransition(intransition);
	}

	DataDrivenBaseApplication*	currentApp = (DataDrivenBaseApplication*)KigsCore::GetCoreApplication();
	if (currentApp)
	{
		currentApp->setInTransition(this, intransition);
	}
}

void DataDrivenTransition::InitModifiable()
{
	DataDrivenSequenceManager* currentManager = (DataDrivenSequenceManager*)getManager();
	if (!currentManager)
	{
		return;
	}
	myIsFirstUpdate = true;
	CoreModifiable* currentSequence = currentManager->GetCurrentSequence();
	myPreviousSequence = (currentSequence) ? currentSequence : NULL;
	
	myPrevLauncherList.clear();
	myNextLauncherList.clear();

	KigsCore::GetCoreApplication()->AddAutoUpdate(this);
	setInTransition(true);
	CoreModifiable::InitModifiable();

}

void DataDrivenTransition::Update(const Timer&  timer, void* addParam)
{
	DataDrivenSequenceManager* currentManager = (DataDrivenSequenceManager*)getManager();

	int skip_mode = currentManager->SkipTransitionMode();
	if (myIsFirstUpdate)
	{
		myIsFirstUpdate = false;


		kstl::set<CoreModifiable*>	instances;
		//CoreModifiable* currentSequence = currentApp->GetCurrentSequence();
		if (myPreviousSequence)
		{
			if (myPreviousSequence->isSubType("Scene3D"))
			{
				instances.insert(myPreviousSequence);
			}
			else
			{
				myPreviousSequence->GetSonInstancesByType("Scene3D", instances, true);
			}
			// block all layers 
			kstl::set<CoreModifiable*>::iterator	it = instances.begin();
			kstl::set<CoreModifiable*>::iterator	itend = instances.end();

			for (; it != itend; ++it)
			{
				(*it)->setValue("IsInteractive", false);

				if (((CoreItem*)myAnimPrev) && (skip_mode&SkipTransition_Prev) == 0)
				{
					// add prev sequence animation
					CoreModifiable*	prevsequencelauncher = (KigsCore::GetInstanceOf("prevsequencelauncher", "CoreSequenceLauncher"));
					prevsequencelauncher->setValue("Sequence", ((CoreItem*)myAnimPrev));
					prevsequencelauncher->setValue("StartOnFirstUpdate", true);
					(*it)->addItem(prevsequencelauncher);
					prevsequencelauncher->Init();
					myPrevLauncherList.push_back(prevsequencelauncher);
				}
			}
		}

		instances.clear();
		// search father sequence for layers
		GetParents()[0]->GetSonInstancesByType("Scene3D", instances);

		kstl::set<CoreModifiable*>::iterator	it = instances.begin();
		kstl::set<CoreModifiable*>::iterator	itend = instances.end();

		for (; it != itend; ++it)
		{
			// block layer during transition
			(*it)->setValue("IsInteractive", false);
			if (((CoreItem*)myAnimNext) && (skip_mode&SkipTransition_Next) == 0)
			{
				// add prev sequence animation
				CoreModifiable*	nextsequencelauncher = (KigsCore::GetInstanceOf("nextsequencelauncher", "CoreSequenceLauncher"));
				nextsequencelauncher->setValue("Sequence", ((CoreItem*)myAnimNext));
				nextsequencelauncher->setValue("StartOnFirstUpdate", true);
				(*it)->addItem(nextsequencelauncher);
				nextsequencelauncher->Init();
				myNextLauncherList.push_back(nextsequencelauncher);
			}
		}
	}
	else
	{
		// check if transition is finished
		kstl::vector<CoreModifiable*>::iterator itlauncher;
		bool	isFinished = true;
		for (itlauncher = myPrevLauncherList.begin(); itlauncher != myPrevLauncherList.end(); ++itlauncher)
		{
			isFinished = isFinished && (*itlauncher)->SimpleCall<bool>("IsFinished", nullptr);
		}
		if (isFinished)
		{
			for (itlauncher = myNextLauncherList.begin(); itlauncher != myNextLauncherList.end(); ++itlauncher)
			{
				isFinished = isFinished && (*itlauncher)->SimpleCall<bool>("IsFinished", nullptr);
			}
		}

		if (isFinished)
		{
			for (itlauncher = myPrevLauncherList.begin(); itlauncher != myPrevLauncherList.end(); ++itlauncher)
				(*itlauncher)->Destroy();
			myPrevLauncherList.clear();
			for (itlauncher = myNextLauncherList.begin(); itlauncher != myNextLauncherList.end(); ++itlauncher)
				(*itlauncher)->Destroy();
			myNextLauncherList.clear();

			if (myPreviousSequence)
			{
				currentManager->ProtectedCloseSequence(myPreviousSequence->getName());
				if (myPreviousSequence->isSubType(DataDrivenSequence::myClassID))
				{
					kstl::map<unsigned int, kstl::string> savedParamsList;
					((DataDrivenSequence*)myPreviousSequence)->saveParams(savedParamsList);
					if (savedParamsList.size())
					{
						currentManager->mySequenceParametersMap[myPreviousSequence->getName()] = savedParamsList;
					}
				}
				myPreviousSequence->UnInit();
				myPreviousSequence->Destroy();
				myPreviousSequence = 0;
			}
			kstl::set<CoreModifiable*>	instances;
			GetParents()[0]->GetSonInstancesByType("Abstract2DLayer", instances);

			kstl::set<CoreModifiable*>::iterator	it = instances.begin();
			kstl::set<CoreModifiable*>::iterator	itend = instances.end();

			for (; it != itend; ++it)
			{
				(*it)->setValue("IsInteractive", true);
			}
			KigsCore::GetCoreApplication()->RemoveAutoUpdate(this);
			setInTransition(false);
		}

	}
}

IMPLEMENT_CLASS_INFO(DataDrivenSequenceManager)
IMPLEMENT_CONSTRUCTOR(DataDrivenSequenceManager)
, myCurrentSequence(0)
, myInTransition(false)
, mySceneGraph(0)
{
	m_StateStack.clear();
	m_RequestedState = "";
	mySequenceParametersMap.clear();
}

DataDrivenSequenceManager::~DataDrivenSequenceManager()
{
	if (myCurrentSequence)
	{
		myCurrentSequence->Destroy();
		myCurrentSequence = 0;
	}
}

void DataDrivenSequenceManager::ProtectedInitSequence(const kstl::string& sequence)
{
	// check if app manager
	//kigsprintf("%s init sequence %s(%p)\n", getName().c_str(), sequence.c_str(), &sequence);
	DataDrivenBaseApplication* isApp = aggregate_cast<DataDrivenBaseApplication>(this);
	if (isApp)
	{
		isApp->ProtectedInitSequence(sequence);
	}

}

void DataDrivenSequenceManager::ProtectedCloseSequence(const kstl::string& sequence)
{
	//kigsprintf("%s close sequence %s(%p)\n", getName().c_str(), sequence.c_str(), &sequence);
	DataDrivenBaseApplication* isApp = aggregate_cast<DataDrivenBaseApplication>(this);
	if (isApp)
	{
		isApp->ProtectedCloseSequence(sequence);
	}
}

void DataDrivenSequenceManager::InitModifiable()
{
	CoreModifiable::InitModifiable();

	KigsCore::GetNotificationCenter()->addObserver(this, "ChangeSequence", "ChangeSequence");
	KigsCore::GetNotificationCenter()->addObserver(this, "StackSequence", "StackSequence");
	KigsCore::GetNotificationCenter()->addObserver(this, "BackSequence", "BackSequence");
	KigsCore::GetNotificationCenter()->addObserver(this, "BackSequence", "CancelExitConfirmationPopup");

	mySceneGraph = (ModuleSceneGraph*)KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex);


	// register this as auto update class in application
	CoreBaseApplication*	currentApp = KigsCore::GetCoreApplication();
	if (currentApp )
	{
		currentApp->AddAutoUpdate(this);
	}

	if (myStartingSequence.const_ref() != "")
	{
		RequestStateChange(myStartingSequence);
	}
}

void DataDrivenSequenceManager::UninitModifiable()
{
	CoreModifiable::UninitModifiable();

	if (myCurrentSequence)
	{
		ProtectedCloseSequence(myCurrentSequence->getName());
		myCurrentSequence->UnInit();
		myCurrentSequence->Destroy();
		myCurrentSequence = 0;
	}
}

double GlobalAppDT = 0;

IMPLEMENT_CLASS_INFO(DataDrivenBaseApplication)
IMPLEMENT_CONSTRUCTOR(DataDrivenBaseApplication)
, myGUI(0)
, myRenderer(0)
, mySceneGraph(0)
, myInputModule(0)
, my2DLayers(0)
, myLuaModule(0)
, myRenderingScreen(0)
, myConfirmExitPopup(0)
, myPreviousShortcutEnabled(true)
{
	
	CONSTRUCT_METHOD(DataDrivenBaseApplication, Exit);
	CONSTRUCT_METHOD(DataDrivenBaseApplication, RegisterLuaMethod);

	m_SequenceManager = nullptr;
	m_GlobalConfig = 0;
}

DataDrivenBaseApplication::~DataDrivenBaseApplication()
{
	if (m_GlobalConfig)
		m_GlobalConfig->Destroy();
}

void DataDrivenBaseApplication::ProtectedPreInit()
{
	myGUI = CoreCreateModule(ModuleGUI, 0);
	myInputModule = CoreCreateModule(ModuleInput, 0);
	myRenderer = CoreCreateModule(ModuleRenderer, 0);
	mySceneGraph = CoreCreateModule(ModuleSceneGraph, 0);
	my2DLayers = CoreCreateModule(Module2DLayers, 0);
	CoreCreateModule(ModuleCoreAnimation, 0);

	myLuaModule = CoreCreateModule(LuaKigsBindModule, 0);

	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), DataDrivenTransition, DataDrivenTransition, Core)
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), DataDrivenSequence, DataDrivenSequence, Core)
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), DataDrivenSequenceManager, DataDrivenSequenceManager, Core)
	
	FilePathManager* pathManager = (FilePathManager*)KigsCore::GetSingleton("FilePathManager");
	pathManager->LoadPackage("Assets.kpkg");
}

void DataDrivenBaseApplication::ProtectedInit()
{
	FilePathManager* pathManager = (FilePathManager*)KigsCore::GetSingleton("FilePathManager");
	bool has_kpkg = pathManager->GetLoadedPackage("Assets.kpkg");

	// load an anonymous CoreModifiableInstance containing global params
	// try to load platform specific config file
	kstl::string configFileName = "GlobalConfig";

	configFileName += getPlatformName();
	configFileName += ".xml";
	m_GlobalConfig = Import(configFileName);

	if (m_GlobalConfig == 0) // if no platform specific config, try generic config file
		m_GlobalConfig = Import("GlobalConfig.xml");

	// AppInit is the window, and have a rendering screen child
	CoreModifiable*	AppInit = Import("AppInit.xml", true);
	if (AppInit == nullptr)
		return;

	
	kstl::string tmpFilename;
	// get dynamic attributes (DataDrivenBaseApplication V1.1)
	// package file ?
	if (AppInit->getValue("PackageFileName", tmpFilename))
	{
		pathManager->LoadPackage(tmpFilename);
		AppInit->RemoveDynamicAttribute("PackageFileName");
	}
	// bundle file ?
	if (!has_kpkg && AppInit->getValue("BundleFileName", tmpFilename))
	{
		pathManager->InitBundleList(tmpFilename);
		AppInit->RemoveDynamicAttribute("BundleFileName");
	}
	// pathmanager init ?
	if (AppInit->getValue("FilePathManagerInitFile", tmpFilename))
	{
		pathManager->InitWithConfigFile(tmpFilename);
		AppInit->RemoveDynamicAttribute("FilePathManagerInitFile");
	}
	// localization init ?
	if (AppInit->getValue("LocalizationInitFile", tmpFilename))
	{
		LocalizationManager* L_LocalizationManager = (LocalizationManager*)KigsCore::GetSingleton("LocalizationManager");
		L_LocalizationManager->InitWithConfigFile(tmpFilename);
		AppInit->RemoveDynamicAttribute("LocalizationInitFile");
	}
	
	// device display caps to get screen resolution
	DisplayDeviceCaps*	L_displaycaps = (DisplayDeviceCaps*)(KigsCore::GetInstanceOf("getdisplaycaps", "DisplayDeviceCaps"));
	const kstl::vector<DisplayDeviceCaps::DisplayDeviceCapacity>*	L_capacitylist = L_displaycaps->GetMainDisplayDeviceCapacityList();

	// default
	int L_ScreenSizeX = 1280;
	int L_ScreenSizeY = 720;

	// retrieve wanted values
	AppInit->getValue("SizeX", L_ScreenSizeX);
	AppInit->getValue("SizeY", L_ScreenSizeY);

	int L_ScreenPosX = 0;
	int L_ScreenPosY = 0;

	AppInit->getValue("PositionX", L_ScreenPosX);
	AppInit->getValue("PositionY", L_ScreenPosY);

	bool	L_isWindowed = false;

	if (L_displaycaps->SupportWindowedMode())
	{
		
		// windowed on platform where it's possible
		// except if Windows is fullscreen
		L_isWindowed = true;
		bool fullScreen = false;
		AppInit->getValue("FullScreen", fullScreen);
		if (fullScreen)
		{
			L_isWindowed = false;
		}
	}
	if (L_capacitylist)
	{
		kstl::vector<DisplayDeviceCaps::DisplayDeviceCapacity>::const_iterator	it = (*L_capacitylist).begin();
		while (it != (*L_capacitylist).end())
		{
			const DisplayDeviceCaps::DisplayDeviceCapacity& current = (*it);
			if (current.myIsCurrent)
			{
				if (!L_isWindowed)
				{
					// if not windowed, just set pos at 0,0
					L_ScreenPosX = 0;
					L_ScreenPosY = 0;

					L_ScreenSizeX = current.myWidth;
					L_ScreenSizeY = current.myHeight;
				}
				else
				{
					if (L_ScreenPosX == -1)
						L_ScreenPosX = (current.myWidth - L_ScreenSizeX) >> 1;

					if (L_ScreenPosY == -1)
						L_ScreenPosY = (current.myHeight - L_ScreenSizeY) >> 1;
				}
				break;
			}
			it++;
		}
	}
	L_displaycaps->Destroy();

	// reset value according to display caps

	AppInit->setValue("PositionX", L_ScreenPosX);
	AppInit->setValue("PositionY", L_ScreenPosY);
	AppInit->setValue("SizeX", L_ScreenSizeX);
	AppInit->setValue("SizeY", L_ScreenSizeY);

	// retrieve rendering screen
	kstl::set<CoreModifiable*>	instances;
	AppInit->GetSonInstancesByType("RenderingScreen", instances);
	if (instances.size() == 1)
	{
		myRenderingScreen = (*instances.begin());

		myRenderingScreen->setValue("SizeX", L_ScreenSizeX);
		myRenderingScreen->setValue("SizeY", L_ScreenSizeY);

		ModuleInput* theInputModule = (ModuleInput*)KigsCore::GetModule("ModuleInput");
		theInputModule->getTouchManager()->addTouchSupport(myRenderingScreen,0);	// root touchsupport

	}


	// check if exit confirmation popup is there
	instances.clear();
	AppInit->GetSonInstancesByName("UIItem", "ConfirmExitPopup", instances);

	if (instances.size() == 1)
	{
		myConfirmExitPopup = (*instances.begin());
		myConfirmExitPopup->setValue("IsHidden", true);
		// remove this instance from AppInit, so get e ref first
		myConfirmExitPopup->GetRef();
		AppInit->removeItem(myConfirmExitPopup);
	}

	AppInit->Init();
	myGUI->addItem(AppInit);

	// first sequence name ?
	tmpFilename = "LaunchScreen.xml";
	if (AppInit->getValue("FirstSequenceFile", tmpFilename))
	{
		AppInit->RemoveDynamicAttribute("FirstSequenceFile");
	}

	CreateSequenceManager();

	AppInit->Destroy();


	KigsCore::GetNotificationCenter()->addObserver(this, "Exit", "OKExitConfirmationPopup");

	// set the first state
	m_SequenceManager->RequestStateChange(tmpFilename);
}

void DataDrivenBaseApplication::CreateSequenceManager()
{
	// create manager

	m_SequenceManager= (DataDrivenSequenceManager*)(KigsCore::GetInstanceOf("AppSequenceManager", "DataDrivenSequenceManager"));
	m_SequenceManager->Init();
	RemoveAutoUpdate(m_SequenceManager);
	aggregateWith(m_SequenceManager);
	m_SequenceManager->Destroy();
}

void DataDrivenBaseApplication::setInTransition(DataDrivenTransition* transition, bool active)
{
	auto found = myInTransition.find(transition);
	if (found != myInTransition.end())
	{
		if (active == false)
		{
			myInTransition.erase(found);
		}
	}
	else
	{
		if (active == true)
		{
			myInTransition[transition] = transition;
		}
	}
}

void DataDrivenBaseApplication::ProtectedUpdate()
{
	bool can_update = true;
#ifdef KIGS_TOOLS
	can_update = mCanUpdateNextFrame;
#endif
	
	GlobalAppDT = myApplicationTimer->GetDt(this);

	bool inputUpdate = true;
	if (myInTransition.size())
	{
		inputUpdate = false;
	}
	((ModuleInput*)myInputModule)->getTouchManager()->activate(inputUpdate);

	rmt_BeginCPUSample(myInputModule, 0);
	if (HasFocus())
		myInputModule->CallUpdate(*myApplicationTimer, 0);
	rmt_EndCPUSample();

	if (can_update)
	{
		DoAutoUpdate();

		rmt_BeginCPUSample(AnimationModule, 0);
		KigsCore::Instance()->GetMainModuleInList(CoreAnimationModuleCoreIndex)->CallUpdate(*myApplicationTimer, 0);
		rmt_EndCPUSample();

		rmt_BeginCPUSample(myLuaModule, 0);
		myLuaModule->CallUpdate(*myApplicationTimer, 0);
		rmt_EndCPUSample();
	}
	else
	{
		myAutoUpdateDone = true;
	}

	bool render = false;
	if (m_SequenceManager)
	{
		rmt_BeginCPUSample(myRenderer, 0);
		render = m_SequenceManager->AllowRender();
		if (render)
			myRenderer->CallUpdate(*myApplicationTimer, 0);
		rmt_EndCPUSample();
	}

	rmt_BeginCPUSample(mySceneGraph, 0);
	mySceneGraph->CallUpdate(*myApplicationTimer, 0);
	rmt_EndCPUSample();

	rmt_BeginCPUSample(my2DLayers, 0);
	my2DLayers->CallUpdate(*myApplicationTimer, 0);
	rmt_EndCPUSample();

	rmt_BeginCPUSample(myGUI, 0);
	myGUI->CallUpdate(*myApplicationTimer, &render);
	rmt_EndCPUSample();

	if (can_update)
	{
		if (m_SequenceManager && myPreviousShortcutEnabled && BackKeyWasPressed() && !m_SequenceManager->IsInTransition())
		{
			m_SequenceManager->RequestBackToPreviousState();
		}
	}

#ifdef KIGS_TOOLS
	mCanUpdateNextFrame = UpdateKigsTools();
#endif

}

void DataDrivenBaseApplication::ProtectedClose()
{
#ifdef KIGS_TOOLS
	DestroyKigsTools();
#endif
	
	m_SequenceManager->UnInit();

	if (myConfirmExitPopup)
	{
		myConfirmExitPopup->Destroy();
		myConfirmExitPopup = 0;
	}

	removeAggregateWith(m_SequenceManager);
	m_SequenceManager = 0;

	KigsCore::ReleaseSingleton("FilePathManager");
	KigsCore::ReleaseSingleton("LocalizationManager");

	CoreDestroyModule(ModuleGUI);
	CoreDestroyModule(ModuleSceneGraph);
	CoreDestroyModule(Module2DLayers);
	CoreDestroyModule(ModuleInput);
	CoreDestroyModule(ModuleRenderer);
	CoreDestroyModule(ModuleCoreAnimation);
	CoreDestroyModule(LuaKigsBindModule);
}

void DataDrivenSequenceManager::SetState(State_t NewState)
{
	DataDrivenBaseApplication* isApp = aggregate_cast<DataDrivenBaseApplication>(this);
	if (myInTransition) return;

	if (NewState == State_Empty)
	{
		if (myCurrentSequence)
		{
			
			if (isApp)
			{
				isApp->removeConfirmationPopup(myCurrentSequence);
			}
			
			ProtectedCloseSequence(myCurrentSequence->getName());
			if (myCurrentSequence->isSubType("DataDrivenSequence"))
			{
				kstl::map<unsigned int, kstl::string> savedParamsList;
				((DataDrivenSequence*)myCurrentSequence)->saveParams(savedParamsList);
				if (savedParamsList.size())
				{
					mySequenceParametersMap[myCurrentSequence->getName()] = savedParamsList;
				}
			}
			myCurrentSequence->UnInit();
			
			mySceneGraph->removeItem(myCurrentSequence);
			myCurrentSequence->Destroy();

			// create empty UI2dlayer
			myCurrentSequence = KigsCore::GetInstanceOf("Empty", "UI2DLayer");
			myCurrentSequence->setValue("RenderingScreen", "RenderingScreen:theRenderingScreen");
			mySceneGraph->addItem(myCurrentSequence);
			myCurrentSequence->Init();
		}

		m_StateStack.clear();
	}
	if (isApp)
	{
		// don't change screen during exit popup
		if (isApp->myConfirmExitPopup)
		{
			bool isHidden = true;
			isApp->myConfirmExitPopup->getValue("IsHidden", isHidden);
			if (!isHidden)
			{
				// return and wait for popup to be cancelled or confirmed
				return;
			}
		}
	}

	if (m_StateStack.size())
	{
		if (m_StateStack.back() != NewState)
		{
			m_StateStack.push_back(NewState);
		}
	}
	else
	{
		m_StateStack.push_back(NewState);
	}
	m_RequestedState = State_None;

	if (NewState != State_None)
	{
		if (myCurrentSequence)
		{
			if (isApp)
			{
				isApp->removeConfirmationPopup(myCurrentSequence);
			}
		}
		CoreModifiable* L_tmp = NULL;

		L_tmp = CoreModifiable::Import(NewState);
		
		if (!L_tmp)
			return;

		ProtectedInitSequence(L_tmp->getName());

		if (myCurrentSequence) myCurrentSequence->SimpleCall("SequenceEnd");
		L_tmp->SimpleCall("SequenceStart");

		if (L_tmp->isSubType("DataDrivenSequence"))
		{
		
			auto itfound = mySequenceParametersMap.find(L_tmp->getName());
			// check if params were saved for this sequence
			if (itfound != mySequenceParametersMap.end())
			{
				((DataDrivenSequence*)L_tmp)->restoreParams((*itfound).second);
			}
			// everything is managed by datadriven sequence init
			myCurrentSequence = L_tmp;

			/*kstl::set<CoreModifiable*> instances;
			myCurrentSequence->GetSonInstancesByType("Abstract2DLayer", instances, false);
			for (auto itr : instances)
				((Abstract2DLayer*)itr)->SetMouseInfo(theMouseInfo);*/

		}
		// add to scenegraph for layers
		else if (L_tmp->isSubType("Abstract2DLayer"))
		{
			if (myCurrentSequence)
			{
				ProtectedCloseSequence(myCurrentSequence->getName());
				if (myCurrentSequence->isSubType("DataDrivenSequence"))
				{
					kstl::map<unsigned int, kstl::string> savedParamsList;
					((DataDrivenSequence*)myCurrentSequence)->saveParams(savedParamsList);
					if (savedParamsList.size())
					{
						mySequenceParametersMap[myCurrentSequence->getName()] = savedParamsList;
					}
				}
				myCurrentSequence->UnInit();
				myCurrentSequence->Destroy();
			}
			myCurrentSequence = L_tmp;
			mySceneGraph->addItem(L_tmp);

			//((Abstract2DLayer*)myCurrentSequence)->SetMouseInfo(theMouseInfo);
		}
		if (isApp)
		{
			isApp->addConfirmationPopup(myCurrentSequence);
		}
	}
}

void DataDrivenBaseApplication::removeConfirmationPopup(CoreModifiable* sequence)
{

	if (!myConfirmExitPopup)
	{
		return;
	}
	if (sequence->isSubType("BaseUI2DLayer"))
	{
		// get root UIItem

		const kstl::vector<ModifiableItemStruct>& uilayersons = sequence->getItems();
		kstl::vector<ModifiableItemStruct>::const_iterator uilayersonsbegin = uilayersons.begin();
		kstl::vector<ModifiableItemStruct>::const_iterator uilayersonsend = uilayersons.end();

		while (uilayersonsbegin != uilayersonsend)
		{
			if ((*uilayersonsbegin).myItem->isSubType("UIItem"))
			{
				(*uilayersonsbegin).myItem->removeItem(myConfirmExitPopup);
				break;
			}
			uilayersonsbegin++;
		}
	}
	else
	{
		// search for BaseUI
		kstl::set<CoreModifiable*>	instances;
		sequence->GetSonInstancesByType("BaseUI2DLayer", instances);

		if (instances.size() > 0)
		{
			kstl::set<CoreModifiable*>::iterator	it = instances.begin();
			kstl::set<CoreModifiable*>::iterator	itend = instances.end();

			for (; it != itend; ++it)
			{
				// get root UIItem
				// TODO : change root item management in BaseUI2DLayer (use classic additem / removeitem...)
				const kstl::vector<ModifiableItemStruct>& uilayersons = (*it)->getItems();
				kstl::vector<ModifiableItemStruct>::const_iterator uilayersonsbegin = uilayersons.begin();
				kstl::vector<ModifiableItemStruct>::const_iterator uilayersonsend = uilayersons.end();

				while (uilayersonsbegin != uilayersonsend)
				{
					if ((*uilayersonsbegin).myItem->isSubType("UIItem"))
					{
						(*uilayersonsbegin).myItem->removeItem(myConfirmExitPopup);
						break;
					}
					uilayersonsbegin++;
				}
			}
		}
	}
}

void DataDrivenBaseApplication::addConfirmationPopup(CoreModifiable* sequence)
{

	if (!myConfirmExitPopup)
	{
		return;
	}
	if (sequence->isSubType("BaseUI2DLayer"))
	{
		// get root UIItem

		const kstl::vector<ModifiableItemStruct>& uilayersons = sequence->getItems();
		kstl::vector<ModifiableItemStruct>::const_iterator uilayersonsbegin = uilayersons.begin();
		kstl::vector<ModifiableItemStruct>::const_iterator uilayersonsend = uilayersons.end();

		while (uilayersonsbegin != uilayersonsend)
		{
			if ((*uilayersonsbegin).myItem->isSubType("UIItem"))
			{
				(*uilayersonsbegin).myItem->addItem(myConfirmExitPopup);
				myConfirmExitPopup->Init();

				break;
			}
			uilayersonsbegin++;
		}
	}
	else
	{
		// search for BaseUI
		kstl::set<CoreModifiable*>	instances;
		sequence->GetSonInstancesByType("BaseUI2DLayer", instances);

		if (instances.size() > 0)
		{
			kstl::set<CoreModifiable*>::iterator	it = instances.begin();
			kstl::set<CoreModifiable*>::iterator	itend = instances.end();

			for (; it != itend; ++it)
			{
				// get root UIItem
				// TODO : change root item management in BaseUI2DLayer (use classic additem / removeitem...)
				const kstl::vector<ModifiableItemStruct>& uilayersons = (*it)->getItems();
				kstl::vector<ModifiableItemStruct>::const_iterator uilayersonsbegin = uilayersons.begin();
				kstl::vector<ModifiableItemStruct>::const_iterator uilayersonsend = uilayersons.end();

				while (uilayersonsbegin != uilayersonsend)
				{
					if ((*uilayersonsbegin).myItem->isSubType("UIItem"))
					{
						(*uilayersonsbegin).myItem->addItem(myConfirmExitPopup);
						myConfirmExitPopup->Init();
						break;
					}
					uilayersonsbegin++;
				}
			}
		}
	}
}

void DataDrivenSequenceManager::RequestStateChange(State_t NewState)
{
	m_RequestedState = NewState;
}

void DataDrivenSequenceManager::Update(const Timer& t, void* v)
{
	CoreModifiable::Update(t, v);

	// change state if needed
	if (m_RequestedState != State_None)
		SetState(m_RequestedState);

	if (myCurrentSequence)
		myCurrentSequence->CallUpdate(t, 0);
}

void DataDrivenSequenceManager::ReloadState()
{
	SetState(m_StateStack.back());
}

void DataDrivenSequenceManager::RequestBackToPreviousState()
{
	DataDrivenBaseApplication* isApp = aggregate_cast<DataDrivenBaseApplication>(this);

	if (m_StateStack.size() < 2)
	{
		if (isApp)
		{
			if (isApp->myConfirmExitPopup) // show or hide exit confirmation
			{
				bool isHidden = true;
				isApp->myConfirmExitPopup->getValue("IsHidden", isHidden);
				isHidden = !isHidden;
				isApp->myConfirmExitPopup->setValue("IsHidden", isHidden);
			}
			else
			{
				isApp->myNeedExit = true;
			}
		}
		return;
	}
	m_StateStack.pop_back();

	RequestStateChange(m_StateStack.back());
}

bool DataDrivenSequenceManager::IsParent(CoreModifiable* toCheck,CoreModifiable* p)
{
	if (p == toCheck)
		return true;
	
	for(auto parent : toCheck->GetParents())
	{
		bool ok = IsParent(parent,p);
		if (ok)
			return ok;
	}

	return false;
}

// recursive check if a parent of s is a sequence and return it
CoreModifiable*	DataDrivenSequenceManager::getParentSequence(CoreModifiable* s)
{
	if (s->isSubType(BaseUI2DLayer::myClassID))
	{
		return s;
	}
	if (s->isSubType(DataDrivenSequence::myClassID))
	{
		return s;
	}
	const kstl::vector<CoreModifiable*>& p=s->GetParents();

	auto itp = p.begin();
	auto ite = p.end();

	while (itp != ite)
	{
		CoreModifiable* f = getParentSequence((*itp));
		if (f)
		{
			return (f);
		}
		++itp;
	}

	// not found
	
	return 0;
}

DEFINE_METHOD(DataDrivenSequenceManager, ChangeSequence)
{
	if (!IsParent(sender, myCurrentSequence))
		return false;
	
	if (privateParams != NULL)
	{
		m_StateStack.clear();
		kstl::string L_tmp = static_cast<usString*>(privateParams)->ToString();
		RequestStateChange(L_tmp);
	}
	else if (!params.empty())
	{
		m_StateStack.clear();
		kstl::string tmp;
		// should search for the good param
		params[0]->getValue(tmp);
		RequestStateChange(tmp);
	}

	return false;
}

DEFINE_METHOD(DataDrivenSequenceManager, StackSequence)
{
	if (!IsParent(sender, myCurrentSequence))
		return false;

	if (privateParams != NULL)
	{
		kstl::string L_tmp = static_cast<usString*>(privateParams)->ToString();
		RequestStateChange(L_tmp);
	}
	else if (!params.empty())
	{
		kstl::string tmp;
		// should search for the good param
		params[0]->getValue(tmp);
		RequestStateChange(tmp);
	}

	return false;
}

void DataDrivenSequenceManager::SetSequence(CoreModifiable* sender, const std::string& param)
{
	if (!IsParent(sender, myCurrentSequence))
		return;
	m_StateStack.clear();
	RequestStateChange(param);
}

void DataDrivenSequenceManager::PushSequence(CoreModifiable* sender, const std::string& param)
{
	if (!IsParent(sender, myCurrentSequence))
		return;
	RequestStateChange(param);
}

void DataDrivenSequenceManager::WrapChangeSequence(CoreModifiable* sender, usString param)
{
	if (!IsParent(sender, myCurrentSequence))
		return;

	m_StateStack.clear();
	RequestStateChange(param.ToString());
	return;
}

void DataDrivenSequenceManager::WrapStackSequence(CoreModifiable* sender, usString param)
{
	if (!IsParent(sender, myCurrentSequence))
		return;

	RequestStateChange(param.ToString());
	return;
}

DEFINE_METHOD(DataDrivenSequenceManager, BackSequence)
{
	if (!IsParent(sender, myCurrentSequence))
		return false;

	RequestBackToPreviousState();
	return false;
}

DEFINE_METHOD(DataDrivenBaseApplication, Exit)
{
	myNeedExit = true;
	return false;
}

