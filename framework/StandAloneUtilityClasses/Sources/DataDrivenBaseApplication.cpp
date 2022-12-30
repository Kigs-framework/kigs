#include "PrecompiledHeaders.h"
#include "KigsApplication.h"
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
#include "LocalizationManager.h"
#include "NotificationCenter.h"
#include "maReference.h"
#include "BaseUI2DLayer.h"
#include "ModuleInput.h"
#include "TouchInputEventManager.h"
#include "AttributePacking.h"

#include "Remotery.h"

#include "Sources/KigsTools.cpp"

using namespace Kigs::DDriven;
using namespace Kigs::Action;
using namespace Kigs::Lua;
using namespace Kigs::Gui;
using namespace Kigs::Input;
using namespace Kigs::Draw;
using namespace Kigs::Draw2D;

namespace Kigs
{
	namespace DDriven
	{
		double GlobalAppDT = 0;
	}
}

#ifdef KIGS_TOOLS
using namespace Kigs::Tools;
#endif

IMPLEMENT_CLASS_INFO(DataDrivenSequence)

DataDrivenSequence::DataDrivenSequence(const std::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, mKeepParamsOnStateChange(*this, false, "KeepParamsOnStateChange", false)
, mSequenceManager(*this, true, "SequenceManager","DataDrivenSequenceManager:AppSequenceManager" ) // default is app
{
}

DataDrivenSequence::~DataDrivenSequence()
{
}

DataDrivenSequenceManager*	DataDrivenSequence::getManager()
{
	return (DataDrivenSequenceManager*)(CoreModifiable*)mSequenceManager;
}

void DataDrivenSequence::saveParams(std::map<unsigned int, std::string>& params)
{
	if (mKeepParamsOnStateChange)
	{
		params.clear();
		const auto& attribmap = getAttributes();
		auto itattribcurrent = attribmap.begin();
		auto itattribend = attribmap.end();

		while (itattribcurrent != itattribend)
		{
			std::string value;
			if (getValue((*itattribcurrent).first, value))
			{
				params[(*itattribcurrent).first.toUInt()] = value;
			}
			++itattribcurrent;
		}

	}
}
void DataDrivenSequence::restoreParams(const std::map<unsigned int, std::string>& params)
{
	if (mKeepParamsOnStateChange)
	{
		std::map<unsigned int, std::string>::const_iterator	itattribcurrent = params.begin();
		std::map<unsigned int, std::string>::const_iterator	itattribend = params.end();

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

	CMSP currentSequence = currentManager->GetCurrentSequence();

	// first search for a transition
	std::vector<CMSP>	instances;
	GetSonInstancesByType("DataDrivenTransition", instances);

	if (instances.size() == 0) // if no transition, destroy previous scene else everything is managed by transition
	{
		if (currentSequence)
		{
			currentManager->ProtectedCloseSequence(currentSequence->getName());
			if (currentSequence->isSubType(DataDrivenSequence::mClassID))
			{
				std::map<unsigned int, std::string> savedParamsList;
				currentSequence->as<DataDrivenSequence>()->saveParams(savedParamsList);
				if (savedParamsList.size())
				{
					currentManager->mSequenceParametersMap[currentSequence->getName()] = savedParamsList;
				}
			}
			if (currentSequence)
			{
				currentSequence->SimpleCall("SequenceEnd");
				currentSequence->UnInit();
				currentSequence = nullptr;
				currentManager->mCurrentSequence = nullptr;
			}
		}
	}

	// search son layout and add them to scenegraph
	// Abstract2DLayer inherit Scene3D
	instances.clear();
	GetSonInstancesByType("Scene3D", instances);

	Scene::ModuleSceneGraph* scenegraph = (Scene::ModuleSceneGraph*)KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex);

	std::vector<CMSP>::iterator	it = instances.begin();
	std::vector<CMSP>::iterator	itend = instances.end();

	for (; it != itend; ++it)
	{
		scenegraph->addItem(*it);
	}
}

void DataDrivenSequence::UninitModifiable()
{
	// close all sons
	const std::vector<ModifiableItemStruct>& items = getItems();
	std::vector<ModifiableItemStruct>::const_iterator	it = items.begin();
	std::vector<ModifiableItemStruct>::const_iterator	itend = items.end();

	for (; it != itend; ++it)
	{
		(*it).mItem->UnInit();
	}
	CoreModifiable::UninitModifiable();
}

IMPLEMENT_CLASS_INFO(DataDrivenTransition)

DataDrivenTransition::DataDrivenTransition(const std::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, mPreviousSequence(nullptr)
, mPreviousAnim(*this, false, "PreviousAnim")
, mNextAnim(*this, false, "NextAnim")
, mIsFirstUpdate(false)
, mSequenceManager(*this, true, "SequenceManager", "DataDrivenSequenceManager:AppSequenceManager") // default is app
{

}

DataDrivenSequenceManager*	DataDrivenTransition::getManager()
{
	return (DataDrivenSequenceManager*)(CoreModifiable*)mSequenceManager;
}

DataDrivenTransition::~DataDrivenTransition()
{
	setInTransition(false);
	mPrevLauncherList.clear();
	mNextLauncherList.clear();

	if (mPreviousSequence)
	{
		mPreviousSequence->UnInit();
		mPreviousSequence = 0;
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
	mIsFirstUpdate = true;
	CMSP currentSequence = currentManager->GetCurrentSequence();
	mPreviousSequence = currentSequence;
	
	mPrevLauncherList.clear();
	mNextLauncherList.clear();

	KigsCore::GetCoreApplication()->AddAutoUpdate(this);
	setInTransition(true);
	CoreModifiable::InitModifiable();

}

void DataDrivenTransition::Update(const Timer&  timer, void* addParam)
{
	DataDrivenSequenceManager* currentManager = (DataDrivenSequenceManager*)getManager();

	int skip_mode = currentManager->SkipTransitionMode();
	if (mIsFirstUpdate)
	{
		mIsFirstUpdate = false;


		std::vector<CMSP>	instances;
		//CoreModifiable* currentSequence = currentApp->GetCurrentSequence();
		if (mPreviousSequence)
		{
			if (mPreviousSequence->isSubType("Scene3D"))
			{
				instances.push_back(mPreviousSequence);
			}
			else
			{
				mPreviousSequence->GetSonInstancesByType("Scene3D", instances, true);
			}
			// block all layers 
			std::vector<CMSP>::iterator	it = instances.begin();
			std::vector<CMSP>::iterator	itend = instances.end();

			for (; it != itend; ++it)
			{
				(*it)->setValue("IsInteractive", false);

				if (((CoreItem*)mPreviousAnim) && (skip_mode&SkipTransition_Prev) == 0)
				{
					// add prev sequence animation
					CMSP	prevsequencelauncher = KigsCore::GetInstanceOf("prevsequencelauncher", "CoreSequenceLauncher");
					prevsequencelauncher->setValue("Sequence", ((CoreItem*)mPreviousAnim));
					prevsequencelauncher->setValue("StartOnFirstUpdate", true);
					(*it)->addItem(prevsequencelauncher);
					prevsequencelauncher->Init();
					mPrevLauncherList.push_back(prevsequencelauncher);
				}
			}
		}

		instances.clear();
		// search father sequence for layers
		GetParents()[0]->GetSonInstancesByType("Scene3D", instances);

		std::vector<CMSP>::iterator	it = instances.begin();
		std::vector<CMSP>::iterator	itend = instances.end();

		for (; it != itend; ++it)
		{
			// block layer during transition
			(*it)->setValue("IsInteractive", false);
			if (((CoreItem*)mNextAnim) && (skip_mode&SkipTransition_Next) == 0)
			{
				// add prev sequence animation
				CMSP nextsequencelauncher = KigsCore::GetInstanceOf("nextsequencelauncher", "CoreSequenceLauncher");
				nextsequencelauncher->setValue("Sequence", ((CoreItem*)mNextAnim));
				nextsequencelauncher->setValue("StartOnFirstUpdate", true);
				(*it)->addItem(nextsequencelauncher);
				nextsequencelauncher->Init();
				mNextLauncherList.push_back(nextsequencelauncher);
			}
		}
	}
	else
	{
		// check if transition is finished
		std::vector<CMSP>::iterator itlauncher;
		bool	isFinished = true;
		for (itlauncher = mPrevLauncherList.begin(); itlauncher != mPrevLauncherList.end(); ++itlauncher)
		{
			isFinished = isFinished && (*itlauncher)->SimpleCall<bool>("IsFinished", nullptr);
		}
		if (isFinished)
		{
			for (itlauncher = mNextLauncherList.begin(); itlauncher != mNextLauncherList.end(); ++itlauncher)
			{
				isFinished = isFinished && (*itlauncher)->SimpleCall<bool>("IsFinished", nullptr);
			}
		}

		if (isFinished)
		{
			mPrevLauncherList.clear();
			mNextLauncherList.clear();

			if (mPreviousSequence)
			{
				currentManager->ProtectedCloseSequence(mPreviousSequence->getName());
				if (mPreviousSequence->isSubType(DataDrivenSequence::mClassID))
				{
					std::map<unsigned int, std::string> savedParamsList;
					mPreviousSequence->as<DataDrivenSequence>()->saveParams(savedParamsList);
					if (savedParamsList.size())
					{
						currentManager->mSequenceParametersMap[mPreviousSequence->getName()] = savedParamsList;
					}
				}
				mPreviousSequence->UnInit();
				mPreviousSequence = 0;
			}
			std::vector<CMSP>	instances;
			GetParents()[0]->GetSonInstancesByType("Abstract2DLayer", instances);

			std::vector<CMSP>::iterator	it = instances.begin();
			std::vector<CMSP>::iterator	itend = instances.end();

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
, mCurrentSequence(0)
, mInTransition(false)
, mSceneGraph(0)
{
	mStateStack.clear();
	mRequestedState = "";
	mSequenceParametersMap.clear();
}

void DataDrivenSequenceManager::ProtectedInitSequence(const std::string& sequence)
{
	// check if app manager
	//kigsprintf("%s init sequence %s(%p)\n", getName().c_str(), sequence.c_str(), &sequence);
	DataDrivenBaseApplication* isApp = aggregate_cast<DataDrivenBaseApplication>(this);
	if (isApp)
	{
		isApp->ProtectedInitSequence(sequence);
	}

}

void DataDrivenSequenceManager::ProtectedCloseSequence(const std::string& sequence)
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

	mSceneGraph = (ModuleSceneGraph*)KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex);


	// register this as auto update class in application
	CoreBaseApplication*	currentApp = KigsCore::GetCoreApplication();
	if (currentApp )
	{
		currentApp->AddAutoUpdate(this);
	}

	if (mStartingSequence.const_ref() != "")
	{
		RequestStateChange(mStartingSequence);
	}
}

void DataDrivenSequenceManager::UninitModifiable()
{
	CoreModifiable::UninitModifiable();

	if (mCurrentSequence)
	{
		ProtectedCloseSequence(mCurrentSequence->getName());
		mCurrentSequence->UnInit();
		mCurrentSequence = 0;
	}
}


IMPLEMENT_CLASS_INFO(DataDrivenBaseApplication)
IMPLEMENT_CONSTRUCTOR(DataDrivenBaseApplication)
, mGUI(0)
, mRenderer(0)
, mSceneGraph(0)
, mInputModule(0)
, m2DLayers(0)
, mLuaModule(0)
, mRenderingScreen(0)
, mPreviousShortcutEnabled(true)
, mSequenceManager(nullptr)
, mGlobalConfig(nullptr)
{

}

#ifndef APP_KIGS_PACKAGE_NAME
#define APP_KIGS_PACKAGE_NAME "Assets.kpkg"
#endif

void DataDrivenBaseApplication::ProtectedPreInit()
{
	mGUI = CoreCreateModule(ModuleGUI, 0).get();
	mInputModule = CoreCreateModule(ModuleInput, 0).get();
	mRenderer = CoreCreateModule(ModuleRenderer, 0).get();
	mSceneGraph = CoreCreateModule(ModuleSceneGraph, 0).get();
	m2DLayers = CoreCreateModule(Module2DLayers, 0).get();
	CoreCreateModule(ModuleCoreAnimation, 0);

	mLuaModule = CoreCreateModule(LuaKigsBindModule, 0).get();

	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), DataDrivenTransition, DataDrivenTransition, Core)
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), DataDrivenSequence, DataDrivenSequence, Core)
	DECLARE_FULL_CLASS_INFO(KigsCore::Instance(), DataDrivenSequenceManager, DataDrivenSequenceManager, Core)
	
	auto pathManager = KigsCore::Singleton<File::FilePathManager>();
	pathManager->LoadPackage(APP_KIGS_PACKAGE_NAME);
}


void DataDrivenBaseApplication::ProtectedInit()
{
	auto pathManager = KigsCore::Singleton<File::FilePathManager>();
	bool has_kpkg = pathManager->GetLoadedPackage(APP_KIGS_PACKAGE_NAME);

	// load an anonymous CoreModifiableInstance containing global params
	// try to load platform specific config file
	std::string configFileName = "GlobalConfig";

	configFileName += getPlatformName();
	configFileName += ".xml";
	mGlobalConfig = Import(configFileName);

	if (mGlobalConfig == nullptr) // if no platform specific config, try generic config file
		mGlobalConfig = Import("GlobalConfig.xml");

	// AppInit is the window, and have a rendering screen child
	CMSP	AppInit = Import("AppInit.xml", true);
	if (AppInit == nullptr)
		return;

	
	std::string tmpFilename;
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
		auto theLocalizationManager = KigsCore::Singleton<LocalizationManager>();
		theLocalizationManager->InitWithConfigFile(tmpFilename);
		AppInit->RemoveDynamicAttribute("LocalizationInitFile");
	}
	
	// device display caps to get screen resolution
	SP<DisplayDeviceCaps>	L_displaycaps = KigsCore::GetInstanceOf("getdisplaycaps", "DisplayDeviceCaps");

	const std::vector<DisplayDeviceCaps::DisplayDeviceCapacity>* L_capacitylist = nullptr;


	std::string L_DeviceName;
	if (AppInit->getValue("DeviceName", L_DeviceName))
	{
		if (L_DeviceName != "")
		{
			L_capacitylist = L_displaycaps->GetDisplayDeviceCapacityList(L_DeviceName);
		}
	}

	if(L_capacitylist == nullptr)
		L_capacitylist = L_displaycaps->GetMainDisplayDeviceCapacityList();

	// default
	v2f L_ScreenSize( 1280, 720 );

	// retrieve wanted values
	AppInit->getValue("Size", L_ScreenSize);

	v2f L_ScreenPos(0,0);

	AppInit->getValue("Position", L_ScreenPos);

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
		
		std::vector<DisplayDeviceCaps::DisplayDeviceCapacity>::const_iterator	it = (*L_capacitylist).begin();
		while (it != (*L_capacitylist).end())
		{
			const DisplayDeviceCaps::DisplayDeviceCapacity& current = (*it);
			if (current.mIsCurrent)
			{
				if (!L_isWindowed)
				{
					// if not windowed, just set pos at 0,0
					L_ScreenPos.Set(0,0);

					L_ScreenSize.x = current.mWidth;
					L_ScreenSize.y = current.mHeight;
				}
				else
				{
					mApplicationScaling = L_displaycaps->GetMainDisplayDeviceScaling();
					int w = current.mWidth;
					int h = current.mHeight;

					if (L_ScreenSize.x < 0)
						L_ScreenSize.x = (w * -L_ScreenSize.x) / 100;

					if (L_ScreenSize.y < 0)
						L_ScreenSize.y = (h * -L_ScreenSize.y) / 100;

					// if -1 center window
					if (L_ScreenPos.x == -1)
						L_ScreenPos.x = (w - (int)L_ScreenSize.x) >> 1;

					// if -1 center window
					if (L_ScreenPos.y == -1)
						L_ScreenPos.y = (h - (int)L_ScreenSize.y) >> 1;

					// if negative (and not -1) set position relative to right border 
					if(L_ScreenPos.x < 0)
						L_ScreenPos.x = w - L_ScreenSize.x + L_ScreenPos.x;

					// if negative (and not -1) set position relative to bottom border 
					if (L_ScreenPos.y < 0)
						L_ScreenPos.y = h - L_ScreenSize.y + L_ScreenPos.y;
				}
				break;
			}
			it++;
		}
	}

	if (L_ScreenSize.x < 0) L_ScreenSize.x = 1280;
	if (L_ScreenSize.y < 0) L_ScreenSize.y = 720;

	// reset value according to display caps

	AppInit->setValue("Position", L_ScreenPos);
	AppInit->setValue("Size", L_ScreenSize);

	// retrieve rendering screen
	std::vector<CMSP>	instances;
	AppInit->GetSonInstancesByType("RenderingScreen", instances);
	if (instances.size() == 1)
	{
		mRenderingScreen = instances[0].get();

		mRenderingScreen->setValue("Size", L_ScreenSize);

		auto theInputModule = KigsCore::GetModule<ModuleInput>();
		theInputModule->getTouchManager()->addTouchSupport(mRenderingScreen,0);	// root touchsupport

	}


	// check if exit confirmation popup is there
	instances.clear();

	AppInit->Init();
	mGUI->addItem(AppInit);

	// first sequence name ?
	tmpFilename = "LaunchScreen.xml";
	if (AppInit->getValue("FirstSequenceFile", tmpFilename))
	{
		AppInit->RemoveDynamicAttribute("FirstSequenceFile");
	}

	CreateSequenceManager();


	// set the first state
	mSequenceManager->RequestStateChange(tmpFilename);
}

void DataDrivenBaseApplication::CreateSequenceManager()
{
	// create manager

	mSequenceManager= KigsCore::GetInstanceOf("AppSequenceManager", "DataDrivenSequenceManager");
	mSequenceManager->Init();
	RemoveAutoUpdate(mSequenceManager.get());
	aggregateWith(mSequenceManager);
}

void DataDrivenBaseApplication::setInTransition(DataDrivenTransition* transition, bool active)
{
	auto found = mInTransition.find(transition);
	if (found != mInTransition.end())
	{
		if (active == false)
		{
			mInTransition.erase(found);
		}
	}
	else
	{
		if (active == true)
		{
			mInTransition[transition] = transition;
		}
	}
}

void DataDrivenBaseApplication::ProtectedUpdate()
{
	bool can_update = true;
#ifdef KIGS_TOOLS
	can_update = mCanUpdateNextFrame;
#endif
	
	GlobalAppDT = mApplicationTimer->GetDt(this);

	bool inputUpdate = true;
	if (mInTransition.size())
	{
		inputUpdate = false;
	}
	((ModuleInput*)mInputModule)->getTouchManager()->activate(inputUpdate);

	rmt_BeginCPUSample(mInputModule, 0);
	if (HasFocus())
		mInputModule->CallUpdate(*mApplicationTimer.get(), 0);
	rmt_EndCPUSample();

	if (can_update)
	{
		DoAutoUpdate();

		rmt_BeginCPUSample(AnimationModule, 0);
		KigsCore::Instance()->GetMainModuleInList(CoreAnimationModuleCoreIndex)->CallUpdate(*mApplicationTimer.get(), 0);
		rmt_EndCPUSample();

		rmt_BeginCPUSample(mLuaModule, 0);
		mLuaModule->CallUpdate(*mApplicationTimer.get(), 0);
		rmt_EndCPUSample();
	}
	else
	{
		mAutoUpdateDone = true;
	}

	bool render = false;
	if (mSequenceManager)
	{
		rmt_BeginCPUSample(mRenderer, 0);
		render = mSequenceManager->AllowRender();
		if (render)
			mRenderer->CallUpdate(*mApplicationTimer.get(), 0);
		rmt_EndCPUSample();
	}

	rmt_BeginCPUSample(mSceneGraph, 0);
	mSceneGraph->CallUpdate(*mApplicationTimer.get(), 0);
	rmt_EndCPUSample();

	rmt_BeginCPUSample(m2DLayers, 0);
	m2DLayers->CallUpdate(*mApplicationTimer.get(), 0);
	rmt_EndCPUSample();

	rmt_BeginCPUSample(mGUI, 0);
	mGUI->CallUpdate(*mApplicationTimer.get(), &render);
	rmt_EndCPUSample();

	if (can_update)
	{
		if (mSequenceManager && mPreviousShortcutEnabled && BackKeyWasPressed() && !mSequenceManager->IsInTransition())
		{
			mSequenceManager->RequestBackToPreviousState();
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
	
	mSequenceManager->UnInit();

	removeAggregateWith(mSequenceManager);
	mSequenceManager = nullptr;

	KigsCore::Instance()->ManagePostDestruction(); // Before module destruction

	KigsCore::ReleaseSingleton("FilePathManager");
	KigsCore::ReleaseSingleton("LocalizationManager");

	// close lua first as it can have dependencies to other modules
	CoreDestroyModule(LuaKigsBindModule);
	CoreDestroyModule(ModuleGUI);
	CoreDestroyModule(ModuleSceneGraph);
	CoreDestroyModule(Module2DLayers);
	CoreDestroyModule(ModuleInput);
	CoreDestroyModule(ModuleRenderer);
	CoreDestroyModule(ModuleCoreAnimation);

	KigsCore::Instance()->ManagePostDestruction(); // After module destruction
}

void DataDrivenSequenceManager::SetState(State_t NewState)
{

	if (mInTransition) return;

	if (NewState == State_Empty)
	{
		if (mCurrentSequence)
		{
			
			ProtectedCloseSequence(mCurrentSequence->getName());
			if (mCurrentSequence->isSubType("DataDrivenSequence"))
			{
				std::map<unsigned int, std::string> savedParamsList;
				((DataDrivenSequence*)mCurrentSequence.get())->saveParams(savedParamsList);
				if (savedParamsList.size())
				{
					mSequenceParametersMap[mCurrentSequence->getName()] = savedParamsList;
				}
			}
			mCurrentSequence->UnInit();
			
			mSceneGraph->removeItem(mCurrentSequence);
			

			// create empty UI2dlayer
			mCurrentSequence = KigsCore::GetInstanceOf("Empty", "UI2DLayer");
			mCurrentSequence->setValue("RenderingScreen", "RenderingScreen:theRenderingScreen");
			mSceneGraph->addItem(mCurrentSequence);
			mCurrentSequence->Init();
		}

		mStateStack.clear();
	}

	if (mStateStack.size())
	{
		if (mStateStack.back() != NewState)
		{
			mStateStack.push_back(NewState);
		}
	}
	else
	{
		mStateStack.push_back(NewState);
	}
	mRequestedState = State_None;

	if (NewState != State_None)
	{
		CMSP L_tmp(nullptr);

		L_tmp = CoreModifiable::Import(NewState);
		
		if (!L_tmp)
			return;

		ProtectedInitSequence(L_tmp->getName());

		if (mCurrentSequence) mCurrentSequence->SimpleCall("SequenceEnd");
		L_tmp->SimpleCall("SequenceStart");

		if (L_tmp->isSubType("DataDrivenSequence"))
		{
		
			auto itfound = mSequenceParametersMap.find(L_tmp->getName());
			// check if params were saved for this sequence
			if (itfound != mSequenceParametersMap.end())
			{
				((DataDrivenSequence*)L_tmp.get())->restoreParams((*itfound).second);
			}
			// everything is managed by datadriven sequence init
			mCurrentSequence = L_tmp;

		}
		// add to scenegraph for layers
		else if (L_tmp->isSubType("Abstract2DLayer"))
		{
			if (mCurrentSequence)
			{
				ProtectedCloseSequence(mCurrentSequence->getName());
				if (mCurrentSequence->isSubType("DataDrivenSequence"))
				{
					std::map<unsigned int, std::string> savedParamsList;
					((DataDrivenSequence*)mCurrentSequence.get())->saveParams(savedParamsList);
					if (savedParamsList.size())
					{
						mSequenceParametersMap[mCurrentSequence->getName()] = savedParamsList;
					}
				}
				mCurrentSequence->UnInit();
			}
			mCurrentSequence = L_tmp;
			mSceneGraph->addItem(L_tmp);

			//((Abstract2DLayer*)myCurrentSequence)->SetMouseInfo(theMouseInfo);
		}
	}
}

void DataDrivenSequenceManager::RequestStateChange(State_t NewState)
{
	mRequestedState = NewState;
}

void DataDrivenSequenceManager::Update(const Timer& t, void* v)
{
	CoreModifiable::Update(t, v);

	// change state if needed
	if (mRequestedState != State_None)
		SetState(mRequestedState);

	if (mCurrentSequence)
		mCurrentSequence->CallUpdate(t, 0);
}

void DataDrivenSequenceManager::ReloadState()
{
	SetState(mStateStack.back());
}

void DataDrivenSequenceManager::RequestBackToPreviousState()
{
	DataDrivenBaseApplication* isApp = aggregate_cast<DataDrivenBaseApplication>(this);

	if (mStateStack.size() < 2)
	{
		if (isApp)
		{
			isApp->mNeedExit = true;
		}
		return;
	}
	mStateStack.pop_back();

	RequestStateChange(mStateStack.back());
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
	if (s->isSubType(BaseUI2DLayer::mClassID))
	{
		return s;
	}
	if (s->isSubType(DataDrivenSequence::mClassID))
	{
		return s;
	}
	const std::vector<CoreModifiable*>& p=s->GetParents();

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
	if (!IsParent(sender, mCurrentSequence.get()))
		return false;
	
	if (privateParams != NULL)
	{
		mStateStack.clear();
		std::string L_tmp = static_cast<usString*>(privateParams)->ToString();
		RequestStateChange(L_tmp);
	}
	else if (!params.empty())
	{
		mStateStack.clear();
		std::string tmp;
		// should search for the good param
		params[0]->getValue(tmp);
		RequestStateChange(tmp);
	}

	return false;
}

DEFINE_METHOD(DataDrivenSequenceManager, StackSequence)
{
	if (!IsParent(sender, mCurrentSequence.get()))
		return false;

	if (privateParams != NULL)
	{
		std::string L_tmp = static_cast<usString*>(privateParams)->ToString();
		RequestStateChange(L_tmp);
	}
	else if (!params.empty())
	{
		std::string tmp;
		// should search for the good param
		params[0]->getValue(tmp);
		RequestStateChange(tmp);
	}

	return false;
}

void DataDrivenSequenceManager::SetSequence(CoreModifiable* sender, const std::string& param)
{
	if (!IsParent(sender, mCurrentSequence.get()))
		return;
	mStateStack.clear();
	RequestStateChange(param);
}

void DataDrivenSequenceManager::PushSequence(CoreModifiable* sender, const std::string& param)
{
	if (!IsParent(sender, mCurrentSequence.get()))
		return;
	RequestStateChange(param);
}

void DataDrivenSequenceManager::WrapChangeSequence(CoreModifiable* sender, usString param)
{
	if (!IsParent(sender, mCurrentSequence.get()))
		return;

	mStateStack.clear();
	RequestStateChange(param.ToString());
	return;
}

void DataDrivenSequenceManager::WrapStackSequence(CoreModifiable* sender, usString param)
{
	if (!IsParent(sender, mCurrentSequence.get()))
		return;

	RequestStateChange(param.ToString());
	return;
}

DEFINE_METHOD(DataDrivenSequenceManager, BackSequence)
{
	if (!IsParent(sender, mCurrentSequence.get()))
		return false;

	RequestBackToPreviousState();
	return false;
}

DEFINE_METHOD(DataDrivenBaseApplication, Exit)
{
	mNeedExit = true;
	return false;
}

