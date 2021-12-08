#define KIGS_TOOLS_IMPL
#include "KigsTools.h"


bool gKigsToolsAvailable = false;

#ifdef KIGS_TOOLS
#include "ModuleImGui.h"
#include "ImGuiLayer.h"
#include "ModuleFileManager.h"

#include "CoreBaseApplication.h"
#include "JSonFileParser.h"

#include "KeyboardDevice.h"
#include "MultiTouchDevice.h"
#include "XMLIncludes.h"
#include "GLSLDebugDraw.h"

#include "LuaKigsBindModule.h"
#include "BaseUI2DLayer.h"
#include "RenderingScreen.h"
#include "Texture.h"
#include "Camera.h"
#include "Timer.h"
#include "Node3DDelayed.h"
#include "XMLWriterFile.h"

#include "IconsForkAwesome.h"
#include "imgui_internal.h"
#include "ImGuiCustom.h"

#include "XML.h"
#include "XMLNode.h"
#include "XMLAttribute.h"

#include "BinarySerializer.h"

#include <algorithm>
#include <regex>
#include <unordered_set>

#ifdef WUP
#include <utf8.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.Storage.AccessCache.h>

#include <winrt_helpers.h>
using namespace winrt::Windows::Storage;
#endif

extern bool gCullingDrawBBox;
extern std::function<bool(Node3D*)> gDrawBBoxForNode;

#if KIGS_COLLISION_AVAILABLE
extern int gCollisionDrawLevel;
#endif

#if !KIGS_COREDATADRIVENSEQUENCE_AVAILABLE
class DataDrivenSequenceManager;
#endif

#if KIGS_COREANIMATION_AVAILABLE
#include "ModuleCoreAnimation.h"
#endif

#if KIGS_RENDERER_AVAILABLE
#include "ModuleRenderer.h"
#endif

extern bool gFreezeNode3DDelayed;


enum EditorFlags
{
	EditorFlag_AddedToXML = 1,
};
	
struct AttributeNameSorter
{
	bool operator()(CoreModifiableAttribute* a, CoreModifiableAttribute* b) const
	{
		return a->getID()._id_name < b->getID()._id_name;
	}
};

struct EnterStringPopup
{
	std::string popup_id;
	std::string label;
	std::string value;
	bool ok = false;
	bool first_open = false;
	bool always_ok = false;
	std::function<void(EnterStringPopup&)> on_ok;
	std::function<void(EnterStringPopup&)> on_cancel;
};

struct DragItemPayload
{
	CoreModifiable* item = nullptr;
	CoreModifiable* parent = nullptr;
	bool copy = false;
};

struct KigsToolsState
{
	ModuleImGui* ImGuiModule = nullptr;
	bool ImGuiModuleOwned = false;
	ModuleInput* InputModule = nullptr;
	CoreBaseApplication* Application = nullptr;
	ImGuiLayer* ImGuiLayer = nullptr;

	std::vector<EnterStringPopup> EnterStringPopups;

	maReference mClipboardItem;

	maReference CurrentSequenceManager{ 0u, maWeakReferenceObject{"DataDrivenSequenceManager:AppSequenceManager"} };

	kigs::unordered_map<std::string, std::pair<std::function<void()>, bool>> CustomWidgets;

	kigs::unordered_map<Timer*, std::pair<Timer::State, SP<Timer>>> TimerStates;
	struct
	{
		bool MenuBar = false;
		bool Hierarchy = true;
		bool Attributes = true;
	} ShowWindow;

	int FramesToRun = 0;
	bool Paused = false;
	bool CanPauseWithMultiTouch = true;

	enum class ShowNextDraw
	{
		DoNothing,
		Show,
		Hide
	};
	ShowNextDraw ShowNextDrawAction = ShowNextDraw::DoNothing;

	struct Settings
	{
		bool IsSetAsParent = false;
		bool Node2DDebugDraw = false;
		bool HideNotInXML = false;
		bool HideUnconnectedSignals = false;

		bool AttributesOpen = true;
		bool MethodsOpen = false;
		bool ConnectionsOpen = false;
		v2i MainWindowPos = { 0, 0 };
		v2i MainWindowSize = { 1280, 800 };

		bool Node3DDebugDraw = false;
		bool ShowUID = false;
		bool SequenceEditor = false;
		bool AutoCommit = true;

		bool LibraryOpen = false;
		bool ResourcesOpen = false;

		std::string ProjectLocation = ""; // "C:\\work\\nextbim\\private\\projects\\NextBIMExplorer\\assets\\";
		std::string ProjectFolderAccessToken;

		std::unordered_map<std::string, std::string> TrueFilePaths;

		bool operator==(const Settings& o) const 
		{
			return IsSetAsParent == o.IsSetAsParent &&
				Node2DDebugDraw == o.Node2DDebugDraw &&
				HideNotInXML == o.HideNotInXML &&
				HideUnconnectedSignals == o.HideUnconnectedSignals &&
				AttributesOpen == o.AttributesOpen &&
				MethodsOpen == o.MethodsOpen &&
				ConnectionsOpen == o.ConnectionsOpen &&
				MainWindowPos == o.MainWindowPos &&
				MainWindowSize == o.MainWindowSize &&
				Node3DDebugDraw == o.Node3DDebugDraw &&
				ShowUID == o.ShowUID &&
				SequenceEditor == o.SequenceEditor &&
				AutoCommit == o.AutoCommit &&
				LibraryOpen == o.LibraryOpen &&
				ResourcesOpen == o.ResourcesOpen &&
				ProjectLocation == o.ProjectLocation &&
				ProjectFolderAccessToken == o.ProjectFolderAccessToken &&
				TrueFilePaths == o.TrueFilePaths;
		}

		bool operator!=(const Settings& o) const
		{
			return !(*this == o);
		}

		template<typename PacketStream>
		bool Serialize(PacketStream& stream)
		{
			SERIALIZE_VERSION(stream, 1);
			CHECK_SERIALIZE(serialize_object(stream, IsSetAsParent));
			CHECK_SERIALIZE(serialize_object(stream, Node2DDebugDraw));
			CHECK_SERIALIZE(serialize_object(stream, HideNotInXML));
			CHECK_SERIALIZE(serialize_object(stream, HideUnconnectedSignals));
			CHECK_SERIALIZE(serialize_object(stream, AttributesOpen));
			CHECK_SERIALIZE(serialize_object(stream, MethodsOpen));
			CHECK_SERIALIZE(serialize_object(stream, ConnectionsOpen));
			CHECK_SERIALIZE(serialize_object(stream, MainWindowPos));
			CHECK_SERIALIZE(serialize_object(stream, MainWindowSize));
			CHECK_SERIALIZE(serialize_object(stream, Node3DDebugDraw));
			CHECK_SERIALIZE(serialize_object(stream, ShowUID));
			CHECK_SERIALIZE(serialize_object(stream, SequenceEditor));
			CHECK_SERIALIZE(serialize_object(stream, AutoCommit));
			CHECK_SERIALIZE(serialize_object(stream, LibraryOpen));
			CHECK_SERIALIZE(serialize_object(stream, ResourcesOpen));
			CHECK_SERIALIZE(serialize_object(stream, ProjectLocation));
			CHECK_SERIALIZE(serialize_object(stream, ProjectFolderAccessToken));
			CHECK_SERIALIZE(serialize_object(stream, TrueFilePaths));
			return true;
		}
	};

	Settings CurrentSettings;
	Settings LastSettings;

	
	struct XMLChange
	{
		std::unordered_set<std::shared_ptr<XMLBase>> FileChanged;
	};
	kigs::unordered_map<CoreModifiable*, XMLChange> XMLChanged;

	maReference ActiveXMLItem;
	std::shared_ptr<XMLBase> ActiveXMLFile;

	struct HierarchyWindowState
	{
		ImGuiTextFilter Filter;
		bool ClearFilter = false;
		maReference SelectedItem{ "SelectedItem", {} };

		std::vector<CoreModifiable*> ToExpand;
		bool ForceExpandAll = false;

		std::vector<CoreModifiable*> Scope{ nullptr };
	} HierarchyWindow;

	struct LibraryWindowState
	{
		std::string SearchString;
	} LibraryWindow;

	struct ResourcesWindowState
	{
		std::string SearchString;
	} ResourcesWindow;

	struct AdvancedSearchWindowState
	{
		bool SearchAttributeName = false;
		std::string SearchAttributeNameString;
	} AdvancedSearchWindow;

};

std::unique_ptr<KigsToolsState> gKigsTools;

DEFINE_DYNAMIC_METHOD(CoreModifiable, KigsToolsOnDestroy)
{
	if (!gKigsTools) return false;
	// TODO(antoine) ask to save 
	gKigsTools->XMLChanged.erase(sender);
	
	u32 i = 0;
	for (; i < gKigsTools->HierarchyWindow.Scope.size(); ++i)
	{
		if (sender && gKigsTools->HierarchyWindow.Scope[i] == sender)
		{
			break;
		}
	}
	gKigsTools->HierarchyWindow.Scope.erase(gKigsTools->HierarchyWindow.Scope.begin() + i, gKigsTools->HierarchyWindow.Scope.end());
	return false;
}

void LoadSettings()
{
	auto location = gKigsTools->CurrentSettings.ProjectLocation;
	SmartPointer<CoreRawBuffer> buffer;
	u64 len;

	buffer = ModuleFileManager::LoadFile("debug.kigstools", len);

	if (location.empty())
	{
#ifdef WUP
		auto local_folder = ApplicationData::Current().LocalFolder().Path();
		utf8::utf16to8(local_folder.begin(), local_folder.end(), std::back_inserter(location));
#else
		std::string utf8_path = "../";
#endif
	}

	if(!buffer)
		buffer = ModuleFileManager::LoadFile((location + "\\debug.kigstools").c_str(), len);

	if (buffer)
	{
		PacketReadStream stream{ buffer->data(), buffer->size() };
		gKigsTools->LastSettings = gKigsTools->CurrentSettings;
		if (!serialize_object(stream, gKigsTools->CurrentSettings))
		{
			gKigsTools->CurrentSettings = gKigsTools->LastSettings;
			return;
		}
		gKigsTools->LastSettings = gKigsTools->CurrentSettings;
	}
}

void SaveSettings()
{
	auto location = gKigsTools->CurrentSettings.ProjectLocation;
	if (location.empty())
	{
#ifdef WUP
		auto local_folder = ApplicationData::Current().LocalFolder().Path();
		utf8::utf16to8(local_folder.begin(), local_folder.end(), std::back_inserter(location));
#else
		std::string utf8_path = "../";
#endif
	}

#ifdef WUP
	no_await_lambda([location]() -> winrt::Windows::Foundation::IAsyncAction
	{
		std::vector<u32> data;
		VectorWriteStream stream{ data };
		if (!serialize_object(stream, gKigsTools->CurrentSettings)) return;
		stream.Flush();

		auto folder = co_await StorageFolder::GetFolderFromPathAsync(to_wchar(location));
		if (!folder) co_return;
		auto file = co_await folder.CreateFileAsync(L"debug.kigstools", CreationCollisionOption::ReplaceExisting);
		if (!file) co_return;
		auto file_stream = co_await file.OpenAsync(FileAccessMode::ReadWrite);
		Streams::DataWriter writer(file_stream);
		writer.WriteBytes({ (u8*)data.data(), (u8*)(data.data() + data.size()) });
		co_await writer.StoreAsync();
	});
	
#else
	std::vector<u32> data;
	VectorWriteStream stream{ data };
	if (!serialize_object(stream, gKigsTools->CurrentSettings)) return;
	stream.Flush();
	ModuleFileManager::Get()->SaveFile((location + "\\debug.kigstools").c_str(), (u8*)data.data(), data.size()*sizeof(u32));
#endif
}

void RequestAccessToProjectFolder()
{
	return;

#ifdef WUP
	no_await_lambda([]() -> winrt::Windows::Foundation::IAsyncAction
	{
		auto token = gKigsTools->CurrentSettings.ProjectFolderAccessToken;
		auto access_list = winrt::Windows::Storage::AccessCache::StorageApplicationPermissions::FutureAccessList();
		bool request = true;

		StorageFolder folder = nullptr;

		if (!token.empty())
		{
			if (access_list.ContainsItem(to_wchar(token)))
			{
				folder = co_await access_list.GetFolderAsync(to_wchar(token));
				if (folder)
				{
					request = false;
				}
			}
		}

		if (request)
		{
			using namespace winrt::Windows::Storage::Pickers;
			FolderPicker picker;
			picker.SuggestedStartLocation(PickerLocationId::ComputerFolder);
			picker.FileTypeFilter().Append(L"*");
			folder = co_await picker.PickSingleFolderAsync();
			if (!folder) co_return;
			token = to_utf8(access_list.Add(folder).c_str());
		}

		if (folder)
		{
			gKigsTools->CurrentSettings.ProjectFolderAccessToken = token;
		}
	});

#endif
}
	
bool SetupTools() 
{
#ifdef _M_ARM
	return false;
#endif
	if (!gKigsTools) gKigsTools = std::make_unique<KigsToolsState>();
		
	if (!gKigsTools->ImGuiModule)
	{
		gKigsTools->ImGuiModule = (ModuleImGui*)CoreGetModule(ModuleImGui).get();
		if (!gKigsTools->ImGuiModule)
		{
			gKigsTools->ImGuiModule = CoreCreateModule(ModuleImGui, 0).get();
			gKigsTools->ImGuiModuleOwned = true;
		}
	}

	gKigsToolsAvailable = gKigsTools->ImGuiModule;
	if(gKigsToolsAvailable)
		gKigsTools->ImGuiLayer = gKigsTools->ImGuiModule->CreateDebugLayer();

	if (!gKigsTools->InputModule)
	{
		gKigsTools->InputModule = (ModuleInput*)CoreGetModule(ModuleInput).get();
	}
	if (!gKigsTools->Application)
	{
		gKigsTools->Application = KigsCore::GetCoreApplication();
		gKigsTools->Application->INSERT_DYNAMIC_METHOD(KigsToolsOnDestroy, KigsToolsOnDestroy);
		LoadSettings();

		RequestAccessToProjectFolder();

		gDrawBBoxForNode = [](Node3D* n) -> bool
		{
			if (!gKigsTools->HierarchyWindow.SelectedItem || !gKigsTools->HierarchyWindow.SelectedItem->isSubType("Node3D"))
			{
				return !n->isSubType("Camera");
			}
			if (n == (Node3D*)gKigsTools->HierarchyWindow.SelectedItem) return true;

			auto current = n->getFather();
			while (current)
			{
				if (current == (Node3D*)gKigsTools->HierarchyWindow.SelectedItem)
				{
					return true;
				}
				current = current->getFather();
			}
			return false;
		};

		//@TEMPORARY Draw Specific BBox
		/*
		gDrawBBoxForNode = [](Node3D* n)
		{
			return n->getName() == "DRAW";
		};
		*/

	}
	return gKigsTools->ImGuiModule && gKigsTools->InputModule;
}

void PushToScope(CoreModifiable* item)
{
	gKigsTools->HierarchyWindow.Scope.push_back(item);
	KigsCore::Connect(item, "Destroy", gKigsTools->Application, "KigsToolsOnDestroy");
}

/*
CoreModifiable* FindRootXMLFile(CoreModifiable* item)
{
	auto& parents = item->GetParents();
	for (auto it = parents.begin(); it != parents.end(); ++it)
	{
		CoreModifiable* found = FindRootXMLFile(*it);
		if (found) return found;
	}
	if (item->GetXMLFile()) return item;
	return nullptr;
}
*/

void DragSourceItem(CoreModifiable* item, CoreModifiable* parent = nullptr)
{
	if (ImGui::BeginDragDropSource())
	{
		DragItemPayload payload;
		payload.item = item;
		payload.parent = parent;
		payload.copy = ImGui::GetIO().KeyCtrl;
		ImGui::SetDragDropPayload("DND_KIGS_ITEM", &payload, sizeof(DragItemPayload));
		ImGui::Text("%s%s:%s", payload.copy ? "[COPY]" : "", item->getExactType().c_str(), item->getName().c_str());
		ImGui::EndDragDropSource();
	}
}

void DragSourceKigsID(KigsID id)
{
	if (ImGui::BeginDragDropSource())
	{
		ImGui::Text(id._id_name.c_str());
		ImGui::SetDragDropPayload("DND_KIGS_KIGSID", id._id_name.c_str(), id._id_name.size()+1);
		ImGui::EndDragDropSource();
	}
}

bool IsInXMLFile(XML* file, XMLNode* node)
{
	auto root_node = file->getRoot();
	auto parent = (XMLNodeBase*)node;
	for (;;)
	{
		if (!parent) return false;
		if (parent == root_node) return true;
		parent = parent->getParent();
	}
	return false;
}

CoreModifiable* FindFirstNodeInFile(CoreModifiable* item, XML* file)
{
	auto current_item = item;
	for (;;)
	{
		if (!current_item) return nullptr;
		if (current_item && current_item->mXMLNodes.find(file) != current_item->mXMLNodes.end()) return current_item;
		current_item = current_item->getFirstParent("CoreModifiable");
	}
	return nullptr;
}

void AddToXMLChanged(CoreModifiable* item, const std::shared_ptr<XMLBase>& xml)
{
	if (!item) return;
	auto& data = gKigsTools->XMLChanged[item];
	data.FileChanged.insert(xml);
	KigsCore::Connect(item, "Destroy", gKigsTools->Application, "KigsToolsOnDestroy");
}

auto PrepareWildcardSearch(std::string search_string)
{
	str_tolower(search_string);
	return SplitStringByCharacter(search_string, '*');
}

bool PassWildcardSearch(const std::vector<std::string>& splitted_search, const std::string& to_search)
{
	auto to_test_lower = ToLowerCase(to_search);
	size_t current_pos = 0;
	for (auto& part : splitted_search)
	{
		auto found = to_test_lower.find(part, current_pos);
		if (found == std::string::npos) return false;
		current_pos = found;
	}
	return true;
}

void DrawMenuBar()
{
	constexpr size_t dt_nb_samples = 60;
	static long long dts[dt_nb_samples] = {};
	static size_t dt_index = 0;
	static auto last_t = std::chrono::high_resolution_clock::now();
	auto t = std::chrono::high_resolution_clock::now();
	auto dt = t - last_t;
	last_t = t;
	dts[dt_index%dt_nb_samples] = dt.count();
	dt_index++;
	size_t total_time = 0;
	for (size_t i = 0; i < std::min(dt_nb_samples, dt_index); ++i)
	{
		total_time += dts[i];
	}
	auto frame_time = (double)total_time / (1'000'000 * std::min(dt_nb_samples, dt_index));
		
		
	if (!gKigsTools->ShowWindow.MenuBar && !gKigsTools->Paused) return;
	
	static std::string edited_location;
	bool open_location_popup = false;
	auto show_menu_bar = ImGui::BeginMainMenuBar();
	ImVec2 menu_bar_size = ImGui::GetWindowSize();
	if (show_menu_bar)
	{
		if (ImGui::BeginMenu("Draw"))
		{
			if (ImGui::MenuItem(gKigsTools->CurrentSettings.Node3DDebugDraw ? "Hide Node3D Bounding Box" : "Show Node3D Bounding Box")) gKigsTools->CurrentSettings.Node3DDebugDraw = !gKigsTools->CurrentSettings.Node3DDebugDraw;
			if (ImGui::MenuItem(gKigsTools->CurrentSettings.Node2DDebugDraw ? "Hide Node2D Size" : "Show Node2D Size")) gKigsTools->CurrentSettings.Node2DDebugDraw = !gKigsTools->CurrentSettings.Node2DDebugDraw;

#if KIGS_COLLISION_AVAILABLE
			if (ImGui::InputInt("Collision Draw Level", &gCollisionDrawLevel))
			{
				gCollisionDrawLevel = std::min(std::max(0, gCollisionDrawLevel), 2);
			}
#endif

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Show/Hide MenuBar", "F1")) gKigsTools->ShowWindow.MenuBar = !gKigsTools->ShowWindow.MenuBar;
			if (ImGui::MenuItem("Show/Hide Hierarchy", "F8")) gKigsTools->ShowWindow.Hierarchy = !gKigsTools->ShowWindow.Hierarchy;
			if (ImGui::MenuItem("Show/Hide Attributes", "F9")) gKigsTools->ShowWindow.Attributes = !gKigsTools->ShowWindow.Attributes;
			if (ImGui::MenuItem("Show/Hide Library")) gKigsTools->CurrentSettings.LibraryOpen = !gKigsTools->CurrentSettings.LibraryOpen;
			if (ImGui::MenuItem("Show/Hide Resources")) gKigsTools->CurrentSettings.ResourcesOpen = !gKigsTools->CurrentSettings.ResourcesOpen;
			ImGui::EndMenu();
		}

		
		if (ImGui::BeginMenu("Settings"))
		{
			if (ImGui::MenuItem("Show/Hide UID")) gKigsTools->CurrentSettings.ShowUID = !gKigsTools->CurrentSettings.ShowUID;
			if (ImGui::MenuItem("Reset Settings")) gKigsTools->CurrentSettings = {};
#if KIGS_COREDATADRIVENSEQUENCE_AVAILABLE
			if (ImGui::MenuItem("Sequence Editor")) gKigsTools->CurrentSettings.SequenceEditor = !gKigsTools->CurrentSettings.SequenceEditor;
#endif
			if (ImGui::MenuItem("Project Location"))
			{
				open_location_popup = true;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Widgets"))
		{
			if (ImGui::MenuItem("Hide All Widgets"))
			{
				for (auto& w : gKigsTools->CustomWidgets) w.second.second = false;
			}
			ImGui::Separator();
			for (auto& w : gKigsTools->CustomWidgets)
			{
				if (ImGui::MenuItem(w.first.c_str())) w.second.second = !w.second.second;
			}
			ImGui::EndMenu();
		}

		//ImGui::Checkbox("Freeze Node3DDelayed", &gFreezeNode3DDelayed);

		CoreModifiable* item = gKigsTools->ActiveXMLItem;
		if (item)
		{
			ImGui::Text("editing %s", gKigsTools->ActiveXMLFile->getPath().c_str());
			auto it = gKigsTools->XMLChanged.find(gKigsTools->ActiveXMLItem);
			if (it != gKigsTools->XMLChanged.end() 
				&&	it->second.FileChanged.find(gKigsTools->ActiveXMLFile) != it->second.FileChanged.end())
			{
				ImGui::TextColored(ImColor(255, 0, 0), "*");
			}
		}

		if (gKigsTools->Paused)
			ImGui::TextColored(ImColor{ 255,0,0 }, "PAUSED (F4)");

#if KIGS_RENDERER_AVAILABLE
		if (gRendererStats.DrawCalls)
		{
			ImGui::Text("%4.1fms (%3.0ffps) %4d draw calls", frame_time, (1000.0 / frame_time), gRendererStats.DrawCalls);
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Render :\n\tTriangles : %d\n\tShader switch : %d", gRendererStats.DrawCallsTriangleCount, gRendererStats.ShaderSwitch);
				ImGui::Text("Occlusion queries :\n\tRequested : %4d  / Started : %4d", gRendererStats.OcclusionQueriesRequested, gRendererStats.OcclusionQueriesStarted);
				ImGui::Text("Buffers :\n\tAllocated : %5d", gRendererStats.AllocatedBuffers);
				ImGui::EndTooltip();
			}
		}
#endif
		ImGui::EndMainMenuBar();
	}
	if (open_location_popup)
	{
		edited_location = gKigsTools->CurrentSettings.ProjectLocation;
		ImGui::OpenPopup("ProjectLocation");
	}
	if (ImGui::BeginPopup("ProjectLocation"))
	{
		ImGui::InputText("Project Location", edited_location);
		if (ImGui::Button("Ok"))
		{
			gKigsTools->CurrentSettings.ProjectLocation = edited_location;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	auto viewport = ImGui::GetMainViewport();

	auto dock_space_pos = viewport->Pos;
	dock_space_pos.y += menu_bar_size.y;
	auto dock_space_size = viewport->Size;
	dock_space_size.y -= menu_bar_size.y;
	ImGui::SetNextWindowPos(dock_space_pos);
	ImGui::SetNextWindowSize(dock_space_size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags host_window_flags = 0;
	host_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
	host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	host_window_flags |= ImGuiWindowFlags_NoBackground;

	char label[32];
	ImFormatString(label, IM_ARRAYSIZE(label), "DockspaceViewport_%08X", viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin(label, NULL, host_window_flags);
	ImGui::PopStyleVar(3);

	ImGuiID dockspace_id = ImGui::GetID("Dockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode, 0);

	ImGui::End();
}

#pragma optimize("", off)
void Break(CoreModifiable* item)
{
	int UID = item->getUID();
	auto type = item->getExactType();
#ifdef WIN32
	__debugbreak();
#else
	*(int*)0 = 0;
#endif
}
#pragma optimize("", on)

void CoreModifiableContextMenu(CoreModifiable* item, CoreModifiable* parent=nullptr)
{
	auto& state = gKigsTools->HierarchyWindow;
	if (ImGui::BeginPopupContextItem(item->GetRuntimeID().c_str()))
	{
		ImGui::Text(item->getName().c_str());
		ImGui::Separator();
		if (ImGui::MenuItem("Init")) item->Init();
		if (item->getItems().size() && ImGui::MenuItem("Remove all sons")) item->EmptyItemList();
		if (state.Scope.back() != item && ImGui::MenuItem("Scope to this"))
		{
			PushToScope(item);
			state.ClearFilter = true;
		}

		bool toggle_trace_ref = false;
		if (item->mTraceRef) toggle_trace_ref = ImGui::MenuItem("Disable TraceRef");
		else toggle_trace_ref = ImGui::MenuItem("Enable TraceRef");
		if (toggle_trace_ref) item->mTraceRef = !item->mTraceRef;

		if (ImGui::MenuItem("Inspect in debugger"))
			Break(item);

		for (auto& xmlfile : item->mXMLFiles)
		{
			if (gKigsTools->ActiveXMLItem && xmlfile == gKigsTools->ActiveXMLFile && item == gKigsTools->ActiveXMLItem) continue;
			auto path = xmlfile->getPath();
			STACK_STRING(str, 2048, "Set %s as active XML file", path.c_str());
			if (ImGui::MenuItem(str))
			{
				gKigsTools->ActiveXMLItem = item;
				gKigsTools->ActiveXMLFile = xmlfile;
			}
		}

		if (gKigsTools->ActiveXMLItem)
		{
			auto xml_file = (XML*)gKigsTools->ActiveXMLFile.get();
			auto xml_node = item->GetXMLNodeForFile(xml_file);
			if (!xml_node)
			{
				for (auto user : item->GetParents())
				{
					if (parent && user != parent) continue;
					auto parent_xml = user->GetXMLNodeForFile(xml_file);
					CoreModifiable* first_parent_in_xml = nullptr;
					if (!parent_xml)
					{
						first_parent_in_xml = FindFirstNodeInFile(user, xml_file);
						if (!first_parent_in_xml) continue;
						parent_xml = first_parent_in_xml->GetXMLNodeForFile(xml_file);
					}
					if (!parent_xml) continue;

					auto create_rel_node_chain = [=]() -> XMLNode*
					{
						// Create relation chain
						auto current_item = user;
						XMLNode* child_rel_node = nullptr;
						XMLNode* xml_node = nullptr;
						for (;;)
						{
							auto new_rel = new XMLNode(XMLNodeType::XML_NODE_ELEMENT, "Rel");
							new_rel->addAttribute(new XMLAttribute("P", current_item->getName()));

							current_item->mXMLNodes[xml_file] = new_rel;

							if (child_rel_node)
							{
								new_rel->addChild(child_rel_node);
								child_rel_node->setParent(new_rel);
							}
							child_rel_node = new_rel;
							if (!xml_node) xml_node = new_rel;

							current_item = current_item->getFirstParent("CoreModifiable");
							if (current_item == first_parent_in_xml || !current_item) break;
						}

						parent_xml->addChild(child_rel_node);
						child_rel_node->setParent(parent_xml);

						return xml_node;
					};
					
					
					ImGui::PushID(user);
					ImGui::Text("With %s as parent :", user->getName().c_str());
					if (ImGui::MenuItem("	Add to XML"))
					{
						XMLNode* node = CoreModifiable::ExportToXMLNode(item, xml_file, false);
						if (first_parent_in_xml) parent_xml = create_rel_node_chain();
						parent_xml->addChild(node);
						node->setParent(parent_xml);
						AddToXMLChanged(gKigsTools->ActiveXMLItem, gKigsTools->ActiveXMLFile);
					}
					if (ImGui::MenuItem("	Add to XML recursively"))
					{
						XMLNode* node = CoreModifiable::ExportToXMLNode(item, xml_file, true);
						if (first_parent_in_xml) parent_xml = create_rel_node_chain();
						parent_xml->addChild(node);
						node->setParent(parent_xml);
						AddToXMLChanged(gKigsTools->ActiveXMLItem, gKigsTools->ActiveXMLFile);
					}
					if (ImGui::MenuItem("	Add as new XML include"))
					{
						EnterStringPopup popup;
						popup.on_ok = [=] (EnterStringPopup& popup) mutable
						{
							CoreModifiable::Export(popup.value + ".xml", item, true);
							XMLNode* inc = new XMLNode(XMLNodeType::XML_NODE_ELEMENT, "Inst");
							inc->addAttribute(new XMLAttribute("P", popup.value + ".xml"));
							item->mXMLNodes[xml_file] = inc;
							if (first_parent_in_xml) parent_xml = create_rel_node_chain();
							parent_xml->addChild(inc);
							inc->setParent(parent_xml);
							AddToXMLChanged(gKigsTools->ActiveXMLItem, gKigsTools->ActiveXMLFile);
						};
						popup.label = ".xml";
						popup.popup_id = "###Export XML";
						gKigsTools->EnterStringPopups.push_back(popup);

					}
					ImGui::PopID();
					
				}
			}
		}

		ImGui::EndPopup();
	}
}

void RecursiveHierarchyTree(CoreModifiable* parent, const std::vector<CMSP>& instances, bool use_filter = false)
{
	auto& state = gKigsTools->HierarchyWindow;
	s32 max_size = 0;
	for (auto item : instances)
	{
		if (!use_filter || state.Filter.PassFilter(item->getName().c_str()) || state.Filter.PassFilter(item->getExactType().c_str()))
			max_size = std::max(max_size, (s32)item->getExactType().size());
	}

	for (auto it_item : instances)
	{
		auto item = it_item;
		if (!use_filter || state.Filter.PassFilter(item->getName().c_str()) || state.Filter.PassFilter(item->getExactType().c_str()))
		{
			char tmpStr[2048];

			std::string label_str = item->getName();
			if (gKigsTools->CurrentSettings.ShowUID) label_str += " (" + std::to_string(item->getUID()) + ")";
			strcpy(tmpStr, label_str.c_str());
			
			/*if(gKigsTools->CurrentSettings.ShowUID)
				snprintf(tmpStr, 2048, "[%*s] %s", -(max_size), item->getExactType().c_str(), item->getName().c_str());
			else
				snprintf(tmpStr, 2048, "[%*s] %s (%u)", -(max_size), item->getExactType().c_str(), item->getName().c_str(), item->getUID());
				*/
			

			CoreModifiable* active_xml_item = gKigsTools->ActiveXMLItem; // FindRootXMLFile(item);
			v4f color = ImGui::GetStyleColorVec4(ImGuiCol_Text);

			if (!active_xml_item || !item->GetXMLNodeForFile((XML*)gKigsTools->ActiveXMLFile.get()))
			{
				color = v4f(0.5f, 0.5f, 0.5f, 1.0);
			}
			else
			{
				auto xml_node = (XMLNode*)item->GetXMLNodeForFile((XML*)gKigsTools->ActiveXMLFile.get());
				if (xml_node)
				{
					if (xml_node->nameOneOf("Rel", "RelativePath"))
					{
						color = v4f(0.0f, 0.5f, 1.0f, 1.0);
					}
					else if (xml_node->getAttribute("P", "Path"))
					{
						color = v4f(0.0f, 1.0f, 0.5f, 1.0);
					}
				}
			}
			ImGui::PushStyleColor(ImGuiCol_Text, color);

			int flags = item->getItems().size() ? 0 : ImGuiTreeNodeFlags_Leaf;
			flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;

			if (item == (CoreModifiable*)state.SelectedItem)
				flags |= ImGuiTreeNodeFlags_Selected;

			if (gKigsTools->HierarchyWindow.ForceExpandAll || std::any_of(gKigsTools->HierarchyWindow.ToExpand.begin(), gKigsTools->HierarchyWindow.ToExpand.end(), [item](auto i) { return i == item.get(); }))
				ImGui::SetNextTreeNodeOpen(true);
			
			bool was_open = ImGui::TreeNodeBehaviorIsOpen(ImGui::GetID((void*)item.get()), flags);
			bool opened = ImGui::TreeNodeAdv((void*)item.get(), tmpStr, flags);

			ImGui::PopStyleColor();

			if (ImGui::IsItemFocused())
			{
				if (ImGui::IsKeyPressed(VK_C, false) && ImGui::GetIO().KeyCtrl)
				{
					gKigsTools->mClipboardItem = item.get();
				}
				if (ImGui::IsKeyPressed(VK_V, false) && ImGui::GetIO().KeyCtrl)
				{

				}
			}
			DragSourceItem(item.get(), parent);

			
			if (ImGui::BeginDragDropTarget())
			{
				if (auto pl = ImGui::AcceptDragDropPayload("DND_KIGS_KIGSID"))
				{
					KigsID type_id = (std::string)(const char*)pl->Data;
					CMSP new_item = KigsCore::Instance()->GetInstanceFactory()->GetInstance("unnamed", type_id);
					if (new_item)
					{
						item->addItem(new_item);
						state.SelectedItem = new_item.get();
						state.ToExpand.push_back(item.get());
					}
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginDragDropTarget())
			{
				auto peek_payload = ImGui::GetDragDropPayload();
				if (peek_payload->IsDataType("DND_KIGS_ITEM"))
				{
					// Check that we can move this here
					DragItemPayload data = *(DragItemPayload*)peek_payload->Data;

					bool ok = true;
					auto parent = item.get();
					while (parent && ok)
					{
						if (parent == data.item) ok = false;
						parent = parent->getFirstParent("CoreModifiable");
					}
					if (ok)
					{
						if (auto pl = ImGui::AcceptDragDropPayload("DND_KIGS_ITEM"))
						{
							auto ref = data.item->SharedFromThis();
							//@TODO(antoine): XML
							if (data.copy)
							{
								
							}
							else
							{
								data.parent->removeItem(ref);
								item->addItem(ref);
							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip(item->getExactType().c_str());
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0) && was_open == opened)
			{
				state.SelectedItem = item.get();
			}
			CoreModifiableContextMenu(item.get(), parent);
			if (!item->IsInit())
			{
				ImGui::SameLine();
				ImGui::TextColored(ImColor{ 255,0,0 }, "[U]");
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Uninitialized");
			}

			if (item->mXMLFiles.size())
			{
				ImGui::SameLine();
				ImColor color{ 0,127,0 };
					
				auto it = gKigsTools->XMLChanged.find(item.get());
				if (it != gKigsTools->XMLChanged.end())
				{
					for (auto& f : item->mXMLFiles)
					{
						if (it->second.FileChanged.find(f) != it->second.FileChanged.end())
						{
							color = { 255,127,37 };
							break;
						}
					}
				}
					
				ImGui::TextColored(color, "[XML]");
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(item->mXMLFiles.size()>1 ? "Has multiple XML files" : "Has a XML file");
			}

			if (opened)
			{
				
				std::vector<CMSP> toPass;
				for (auto c : item->getItems())
				{
					toPass.push_back(c.mItem);
				}

				RecursiveHierarchyTree(item.get(), toPass);
				ImGui::TreePop();
			}
		}
	}
}
	
void OpenInHierarchy(CoreModifiable* item)
{
	gKigsTools->HierarchyWindow.ToExpand.clear();
	auto parent = item;
	while (parent)
	{
		gKigsTools->HierarchyWindow.ToExpand.push_back(parent);
		parent = parent->getFirstParent("CoreModifiable");
	}
}

void DrawHierarchy()
{
	if (!gKigsTools->ShowWindow.Hierarchy || !gKigsTools->ShowWindow.MenuBar) return;
		
	ImGui::SetNextWindowSize(ImVec2{ 400,700 }, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2{ 50, 50 }, ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Hierarchy", &gKigsTools->ShowWindow.Hierarchy))
	{
		//ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 5.0f);
		auto& state = gKigsTools->HierarchyWindow;
			
		CoreModifiable* selected_item = state.SelectedItem;

		if (state.ClearFilter)
		{
			state.Filter.Clear();
			state.ClearFilter = false;
		}
		u32 base_number_of_scopes = gKigsTools->CurrentSettings.SequenceEditor ? 2 : 1;
		if (state.Scope.size() > base_number_of_scopes)
		{
			if (ImGui::Button("<-")) state.Scope.erase(--state.Scope.end());

			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				if (state.Scope.size() > base_number_of_scopes+1)
				{
					ImGui::Text("Scope to %s", (*----state.Scope.end())->getName().c_str());
				}
				else
				{
					ImGui::Text(gKigsTools->CurrentSettings.SequenceEditor ? "Scope to current sequence" : "Scope to root objects");
				}

				ImGui::EndTooltip();
			}
			ImGui::SameLine();
		}

		state.Filter.Draw("Filter");
		ImGui::Separator();

		ImGui::BeginGroup();
		if (ImGui::BeginChild("HierarchyScrollArea", ImVec2(0, 0), true))
		{


			std::vector<CMSP> instances;

			if (state.Scope.size() > 1)
			{
				char tmpstr[1024];
				auto current_scope = state.Scope.back();
				snprintf(tmpstr, 1024, "Current scope : %s", current_scope->getName().c_str());
				ImGui::CollapsingHeader(tmpstr, ImGuiTreeNodeFlags_Leaf);
				CoreModifiableContextMenu(current_scope);

				std::vector<CMSP> toPass;
				for (auto c : current_scope->getItems())
				{
					toPass.push_back(c.mItem);
				}

				RecursiveHierarchyTree(current_scope, toPass, true);
			}
			else if (!state.Filter.IsActive())
			{
				instances = CoreModifiable::GetRootInstances("CoreModifiable");

				std::vector<CMSP> regular;
				std::vector<CMSP> modules;
				std::vector<CMSP> singletons;
				std::vector<CMSP> textures;
				std::vector<CMSP> apps;
				std::vector<CMSP> inputs;
				std::vector<CMSP> timers;
				
				apps.push_back(KigsCore::GetCoreApplication()->SharedFromThis());

				for (auto& cm : instances)
				{

					if (cm->isSubType("ModuleBase"))
						modules.push_back(cm);
					else if (cm->isSubType("Texture"))
						textures.push_back(cm);
					else if (cm->isSubType("Timer"))
						timers.push_back(cm);
					else if (cm->isSubType("InputDevice"))
						inputs.push_back(cm);
					else if (cm->getName().substr(0, 10) == "Singleton_")
						singletons.push_back(cm);
					else
						regular.push_back(cm);
				}

				ImGui::TreePush((void*)0);
				RecursiveHierarchyTree(nullptr, regular, true);
				ImGui::TreePop();

				if (textures.size())
				{
					bool open = ImGui::TreeNode("Textures");
					ImGui::SameLine();
					ImGui::Separator();
					if (open)
					{
						RecursiveHierarchyTree(nullptr, textures, true);
						ImGui::TreePop();
					}
				}
				if (timers.size())
				{
					bool open = ImGui::TreeNode("Timers");
					ImGui::SameLine();
					ImGui::Separator();
					if (open)
					{
						RecursiveHierarchyTree(nullptr, timers, true);
						ImGui::TreePop();
					}
				}
				if (inputs.size())
				{
					bool open = ImGui::TreeNode("Input Devices");
					ImGui::SameLine();
					ImGui::Separator();
					if (open)
					{
						RecursiveHierarchyTree(nullptr, inputs, true);
						ImGui::TreePop();
					}
				}
				if (modules.size())
				{
					bool open = ImGui::TreeNode("Modules");
					ImGui::SameLine();
					ImGui::Separator();
					if (open)
					{
						RecursiveHierarchyTree(nullptr, modules, true);
						ImGui::TreePop();
					}
				}
				if (singletons.size())
				{
					bool open = ImGui::TreeNode("Singletons");
					ImGui::SameLine();
					ImGui::Separator();
					if (open)
					{
						RecursiveHierarchyTree(nullptr, singletons, true);
						ImGui::TreePop();
					}
				}
				if (apps.size())
				{
					bool open = ImGui::TreeNode("Applications");
					ImGui::SameLine();
					ImGui::Separator();
					if (open)
					{
						RecursiveHierarchyTree(nullptr, apps, true);
						ImGui::TreePop();
					}
				}


			}
			else
			{
				instances = CoreModifiable::GetInstances("CoreModifiable");
				RecursiveHierarchyTree(nullptr, instances, true);
			}


			gKigsTools->HierarchyWindow.ToExpand.clear();
			gKigsTools->HierarchyWindow.ForceExpandAll = false;
		}
		ImGui::EndChild();
		ImGui::EndGroup();
		//ImGui::PopStyleVar();
	}
	ImGui::End();
}

void DrawCreateInstance()
{
	if (!gKigsTools->CurrentSettings.LibraryOpen || !gKigsTools->ShowWindow.MenuBar) return;
	if (ImGui::Begin("Library", &gKigsTools->CurrentSettings.LibraryOpen))
	{
		auto factory = KigsCore::Instance()->GetInstanceFactory();
			
		ImGui::InputText("Search", gKigsTools->LibraryWindow.SearchString);
		auto search = PrepareWildcardSearch(gKigsTools->LibraryWindow.SearchString);
		if (search.size())
		{
			int index = 0;
			for (auto& it : factory->GetModuleList())
			{
				for (auto& id : it.second.mClassMap)
				{
					if (PassWildcardSearch(search, id.first._id_name))
					{
						ImGui::Selectable(id.first._id_name.c_str());
						DragSourceKigsID(id.first);
						++index;
					}
				}
			}
		}
		else
		{

		}

		/*
		static KigsID current_type;
		static std::string search_str;
		if (ImGui::BeginCombo("Type", current_type._id_name.c_str()))
		{
			for (auto& it : factory->GetModuleList())
			{
				for (auto& id : it.second.mClassMap)
				{
					if (ImGui::Selectable(id.first._id_name.c_str()))
					{
						current_type = id.first;
					}
					if (id.first == current_type)
						ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		*/
	}
	ImGui::End();
}

void ResetContext()
{
	kstl::vector<CMSP>	instances = 	CoreModifiable::GetInstances("RenderingScreen");
	if (instances.size())
	{
		instances[0]->CallMethod("ResetContext", NULL);
	}

	kstl::vector<CMSP>::iterator itInstances;

	// then reinit layers
	instances.clear();
	instances = CoreModifiable::GetInstances("Base2DLayer");

	for (itInstances = instances.begin(); itInstances != instances.end(); itInstances++)
	{
		CMSP current = (*itInstances);
		current->setValue("Reinit", true);
	}
	instances.clear();

	// then reinit sprites
	instances.clear();
	instances = CoreModifiable::GetInstances("BaseSprite");

	for (itInstances = instances.begin(); itInstances != instances.end(); itInstances++)
	{
		CMSP current = (*itInstances);
		current->setValue("Reinit", true);
	}
}
	

struct AttributeChange
{
	bool changed = false;
	std::string value;
	std::string old_value;
	XMLAttributeBase* attr_value = nullptr;
};

AttributeChange CheckAttributeChange(XMLNode* attr_xml, CoreModifiableAttribute* attr)
{
	AttributeChange result;
	result.old_value = "";
	auto type = attr->getType();
	result.attr_value = attr_xml->getAttribute("Value", "V");
	if (result.attr_value)
	{
		result.old_value = result.attr_value->XMLAttributeBase::getString();
		if (type == CoreModifiable::ATTRIBUTE_TYPE::FLOAT)
		{
			float val;
			if (CoreConvertString2Value(result.old_value, val))
				CoreConvertValue2String(result.old_value, val);
		}
		else if (type == CoreModifiable::ATTRIBUTE_TYPE::DOUBLE)
		{
			double val;
			if (CoreConvertString2Value(result.old_value, val))
				CoreConvertValue2String(result.old_value, val);
		}
		else if (type == CoreModifiable::ATTRIBUTE_TYPE::ARRAY)
		{
#define NORMALIZE_STRING(size, type) if (array_size == size)\
				if (CoreConvertString2Array<type>(result.old_value, values, size))\
					CoreConvertArray2String<type>(result.old_value, values, size);

			int array_size = attr->getNbArrayElements();
			if (attr->getArrayElementType() == CoreModifiable::ATTRIBUTE_TYPE::FLOAT)
			{
				float values[16];
				NORMALIZE_STRING(2, float);
				NORMALIZE_STRING(3, float);
				NORMALIZE_STRING(4, float);
				NORMALIZE_STRING(16, float);
			}
			else if (attr->getArrayElementType() == CoreModifiable::ATTRIBUTE_TYPE::DOUBLE)
			{
				double values[16];
				NORMALIZE_STRING(2, double);
				NORMALIZE_STRING(3, double);
				NORMALIZE_STRING(4, double);
				NORMALIZE_STRING(16, double);
			}
			else if (attr->getArrayElementType() == CoreModifiable::ATTRIBUTE_TYPE::INT)
			{
				int values[16];
				NORMALIZE_STRING(2, int);
				NORMALIZE_STRING(3, int);
				NORMALIZE_STRING(4, int);
				NORMALIZE_STRING(16, int);
			}
			else if (attr->getArrayElementType() == CoreModifiable::ATTRIBUTE_TYPE::UINT)
			{
				u32 values[16];
				NORMALIZE_STRING(2, u32);
				NORMALIZE_STRING(3, u32);
				NORMALIZE_STRING(4, u32);
				NORMALIZE_STRING(16, u32);
			}
		}

	}

	result.value;
	attr->getValue(result.value);
	result.changed = result.value != result.old_value;
	return result;
}

	
void DecorateAttribute(CoreModifiableAttribute* attr)
{
	return;
	float offset = 0.0f;

	if (attr->isDynamic())
	{
		offset += ImGui::CalcTextSize("D").x;
		ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - offset);
		ImGui::Text("D");
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Dynamic");
	}
	if (attr->isReadOnly())
	{
		offset += ImGui::CalcTextSize("R").x;
		ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - offset);
		ImGui::Text("R");
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Read only");
	}
	if (attr->isInitParam())
	{
		offset += ImGui::CalcTextSize("I").x;
		ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - offset);
		ImGui::Text("I");
		if (ImGui::IsItemHovered())ImGui::SetTooltip("Init param");
	}
}

void CustomAttributeEditor(CoreModifiable* item)
{
	if (item->isSubType("Scene3D"))
	{
		auto scene = item->as<Scene3D>();
		std::vector<CoreModifiable*> cams;
		scene->GetCameraVector(cams);

		for (auto c : cams)
		{
			auto camera = c->as<Camera>();
			auto& passes = camera->GetRenderPasses();
			static bool OnlyDraws = true;
			if (ImGui::CollapsingHeader("RenderPasses"))
			{
				ImGui::Checkbox("Only show draws", &OnlyDraws);
				for (auto& p : passes)
				{
					if (ImGui::CollapsingHeader((p.name._id_name + " [" + std::to_string(p.pass_mask) + "]").c_str()))
					{
						p.record_pass = true;
						int step = 0;
						for (auto& el : p.debug_draw_path)
						{
							if (el.DrawStep == RenderPass::DrawPathElement::Step::PostDraw)
								--step;

							if (el.DrawStep == RenderPass::DrawPathElement::Step::Draw || !OnlyDraws)
							{
								auto txt = el.Object->getName() + " [" + std::to_string(el.Object->getUID()) + "]";
								
								

								if (!OnlyDraws)
								{
									if (el.DrawStep == RenderPass::DrawPathElement::Step::PreDraw)
										txt = "| PreDraw  | " + txt;
									else if (el.DrawStep == RenderPass::DrawPathElement::Step::Draw)
										txt = "| Draw     | " + txt;
									else if(el.DrawStep == RenderPass::DrawPathElement::Step::PostDraw)
										txt = "| PostDraw | " + txt;
								}

								for (int i = 1; i < step ; ++i)
									txt = " " + txt;

								ImGui::Text(txt.c_str());
							}

							if (el.DrawStep == RenderPass::DrawPathElement::Step::PreDraw)
								++step;
						}
					}
				}
			}

		}

	}

	if (item->isSubType("BaseUI2DLayer"))
	{
		auto layer = static_cast<BaseUI2DLayer*>(item);
		v2i size;
		layer->getSize(size.x, size.y);
		ImGui::Text("Size : %dx%d", size.x, size.y);
	}

	if (item->isSubType("RenderingScreen"))
	{
		auto screen = item->as<RenderingScreen>();

		auto f = [&](float scale)
		{
			ImGui::BeginTooltip();
			ImVec2 size = { screen->getValue<float>("FBOSizeX") * scale, screen->getValue<float>("FBOSizeY") * scale };
			ImGui::Image((ImTextureID)screen->GetFBOTexture().get(), size, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::EndTooltip();
		};
		if (screen->GetFBOTexture())
		{
			ImGui::Text("1/5");
			if (ImGui::IsItemHovered())
			{
				f(1 / 5.0f);
			}
			ImGui::SameLine();
			ImGui::Text("1/1");
			if (ImGui::IsItemHovered())
			{
				f(1.0f);
			}
		}
	}

	if (item->isSubType("Texture"))
	{
		auto tex = item->as<Texture>();
		f32 w, h;
		tex->GetSize(w, h);
		v2f uv;
		tex->GetRatio(uv.x, uv.y);

		auto f = [&](float scale)
		{
			ImGui::BeginTooltip();
			ImVec2 size = { w*scale, h*scale };
			ImGui::Image((ImTextureID)tex, size, ImVec2(0, 0), uv);
			ImGui::EndTooltip();
		};

		ImGui::Text("1/5");
		if (ImGui::IsItemHovered())
		{
			f(1 / 5.0f);
		}
		ImGui::SameLine();
		ImGui::Text("1/1");
		if (ImGui::IsItemHovered())
		{
			f(1.0f);
		}
			
	}

	if (item->isSubType("Node3D") && !item->isSubType("CoordinateSystem"))
	{

		auto f = [](float* v, float amount)
		{
			STACK_STRING(str, 128, "%.0f°", amount);
			ImGui::SameLine();
			if (ImGui::SmallButton(str))
				*v = amount * fPI_180;
		};
		v3f rot(0, 0, 0);
		if (ImGui::CollapsingHeader("Controls"))
		{
			ImGui::PushID("X");
			ImGui::Text("Rotate X");
			f(&rot.x, -90);
			f(&rot.x, -45);
			f(&rot.x, -15);
			f(&rot.x, 15);
			f(&rot.x, 45);
			f(&rot.x, 90);
			ImGui::PopID();

			ImGui::PushID("Y");
			ImGui::Text("Rotate Y");
			f(&rot.y, -90);
			f(&rot.y, -45);
			f(&rot.y, -15);
			f(&rot.y, 15);
			f(&rot.y, 45);
			f(&rot.y, 90);
			ImGui::PopID();

			ImGui::PushID("Z");
			ImGui::Text("Rotate Z");
			f(&rot.z, -90);
			f(&rot.z, -45);
			f(&rot.z, -15);
			f(&rot.z, 15);
			f(&rot.z, 45);
			f(&rot.z, 90);
			ImGui::PopID();
		}
		if (rot.x != 0 || rot.y != 0 || rot.z != 0)
		{
			auto n = item->as<Node3D>();
			//auto m = Matrix3x4::IdentityMatrix();
			//m.SetRotationXYZ(rot.x, rot.y, rot.z);
			auto m = n->GetLocal();
			m.PreRotateXYZ(rot.x, rot.y, rot.z);
			n->ChangeMatrix(m);
		}

		if (ImGui::CollapsingHeader("Matrix"))
		{
			auto& m = item->as<Node3D>()->GetLocal();
			ImGui::Text(V3F_FMT(3), V3F_EXP(m.XAxis));
			ImGui::Text(V3F_FMT(3), V3F_EXP(m.YAxis));
			ImGui::Text(V3F_FMT(3), V3F_EXP(m.ZAxis));
			ImGui::Text(V3F_FMT(3), V3F_EXP(m.Pos));
		}

		if (ImGui::Button("Fit To View"))
		{
			std::vector<CMSP> insts = CoreModifiable::GetInstances("Camera");
			auto itfind = std::find_if(insts.begin(), insts.end(), [](auto cm) { return cm->template getValue<bool>("CameraIsEnabled"); });
			if (itfind != insts.end())
			{
				auto active_cam = (Camera*)(*itfind).get();
				auto n = item->as<Node3D>();
				auto bbox = n->GetGlobalBoundingBox();
				auto view = active_cam->GetViewVector();
				auto d = (Norm(bbox.Size()) / 2) / tan(deg2rad(active_cam->template getValue<float>("VerticalFOV")) / 2);
				auto target = bbox.Center() - view * d;
				active_cam->setValue("Position", target);
			}
		}

		auto bbox = item->as<Node3D>()->GetLocalBoundingBox();
		ImGui::Text("BBOX: " V3F_FMT(2) " -> " V3F_FMT(2), V3F_EXP(bbox.m_Min), V3F_EXP(bbox.m_Max));
	}

	if (item->isSubType("Node3DDelayed"))
	{
		auto node = (Node3DDelayed*)item;

		if (ImGui::Button("Force Load"))
		{
			node->setValue("UpdateMode", "Manual");
			node->SetAllowLoadedStateChange(true);
			node->SetShowContent(true);
			//node->ChangeLoadedState(Node3DDelayed::LoadedState::Displayed);
		}
		if (ImGui::Button("Force UnLoad"))
		{
			node->setValue("UpdateMode", "Manual");
			node->SetAllowLoadedStateChange(true);
			//node->ChangeLoadedState(Node3DDelayed::LoadedState::Unloaded);
			node->SetShowContent(false);
		}
	}

	if (item->isSubType("Camera"))
	{
		auto cam = item->as<Camera>();
			
		if (ImGui::CollapsingHeader("Hits"))
		{
			std::vector<CMSP> nodes = CoreModifiable::GetInstances("Node3D");

			auto origin_global = cam->GetPosition();
			Vector3D direction_global = cam->GetViewVector();

			std::vector<std::pair<float, Node3D*>> hit;

			for (auto n : nodes)
			{
				auto bbox = n->as<Node3D>()->GetLocalBoundingBox();
				auto g2l = n->as<Node3D>()->GetGlobalToLocal();

				auto origin_local = g2l * origin_global;
				auto direction_local = g2l * direction_global;
				double dist = FLT_MAX;
				v3f intersection;
				v3f normal;
				if (Intersection::IntersectionRayBBox(origin_local, direction_local, bbox.m_Min, bbox.m_Max, intersection, normal,dist))
				{
					hit.push_back({ NormSquare(n->as<Node3D>()->GetLocalToGlobal() * intersection - origin_global), n->as<Node3D>() });
				}
			}
			std::sort(hit.begin(), hit.end(), [](auto&& a, auto&& b) { return a.first < b.first; });

			ImGui::Text("Hit:");
			for (auto&& pair : hit)
			{
				ImGui::PushID(pair.second);
				STACK_STRING(str, 1024, "%s", pair.second->getName().c_str());
				if (ImGui::Button(str))
				{
					OpenInHierarchy(pair.second);
					gKigsTools->HierarchyWindow.SelectedItem.setValue(pair.second);
				}
				if (ImGui::IsItemHovered())
				{
					dd::local_bbox(pair.second->GetLocalToGlobal(), pair.second->GetLocalBoundingBox(), { 0,1,0 });
				}
				ImGui::PopID();
			}
		}
	}

	if (item->isSubType("API3DShader"))
	{
		if(ImGui::Button("Reload Shader"))
			item->SimpleCall("Reload");
	}


	if (item->isSubType("CoreBaseApplication"))
	{
		auto app = item->as<CoreBaseApplication>();
		auto list = app->GetAutoUpdateList();

		if (ImGui::CollapsingHeader("AutoUpdateList"))
		{
			for (auto el : list)
			{
				ImGui::Text("%s - %s", el->getExactType().c_str(), el->getName().c_str());
			}
		}
	}

}

void AttributesEditor(CoreModifiable* item, void* id=nullptr, bool nobegin=false)
{
	if (id)
		ImGui::PushID(id);


	if (!nobegin)
		ImGui::Begin("Attributes Editor");


	if (!item)
	{
		if (!nobegin)
			ImGui::End();

		if (id)
			ImGui::PopID();

		return;
	}

	char tmpStr[2048];
	ImGui::Text(item->getExactType().c_str());
	ImGui::SameLine();

	CoreModifiable* xml_file_item = gKigsTools->ActiveXMLItem; // FindRootXMLFile(item);
	XML* xml_file = xml_file_item ? (XML*)gKigsTools->ActiveXMLFile.get() : nullptr;
	XMLNode* xml_node = xml_file_item ? (XMLNode*)item->GetXMLNodeForFile(xml_file) : nullptr;

	std::string name = item->getName();
	if (xml_node && (!xml_node->XMLNodeBase::compareName("Rel")))
	{
		auto name_attr = xml_node->getAttribute("N", "Name");
		if (name_attr)
		{
			name = name_attr->getString();
		}
	}
	std::string old_name = name;

	u32 name_input_flags = ImGuiInputTextFlags_EnterReturnsTrue;
	if ((xml_node && xml_node->XMLNodeBase::compareName("Rel")) || (!xml_node && item->mXMLNodes.size()))
		name_input_flags |= ImGuiInputTextFlags_ReadOnly;
	
	if (ImGui::InputText("Name", name, name_input_flags))
	{
		std::regex r{ "[a-zA-Z0-9_]+" };
		if (std::regex_match(name, r))
		{
			item->setName(name);
			if (xml_node)
			{
				KIGS_ASSERT(xml_node->nameOneOf("Inst", "Instance"));
				auto name_attr = static_cast<XMLAttribute*>(xml_node->getAttribute("N", "Name"));
				if (name_attr)
				{
					name_attr->setString(name);
				}
				else
				{
					xml_node->addAttribute(new XMLAttribute("N", name));
				}
				for (auto& n : item->mXMLNodes)
				{
					if (((XMLNode*)n.second)->nameOneOf("Rel", "RelativePath"))
					{
						auto path_attr = static_cast<XMLAttribute*>(((XMLNode*)n.second)->getAttribute("P", "Path"));
						if (path_attr)
						{
							if (path_attr->XMLAttributeBase::getString() == old_name)
							{
								path_attr->setString(name);
								AddToXMLChanged(item, *std::find_if(item->mXMLFiles.begin(), item->mXMLFiles.end(), [&](auto& sp) { return sp.get() == n.first; }));
							}
						}
					}
				}
				AddToXMLChanged(item, gKigsTools->ActiveXMLFile);
			}
		}
	}
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Only letters, numbers and underscore");

	auto typenode = item->GetTypeNode();
	std::string typestring;
	while (typenode && typenode->mID != 0u)
	{
		typestring = typenode->mID._id_name + "/" + typestring;
		typenode = typenode->mFather;
	}

	ImGui::Text(typestring.c_str());

	sprintf(tmpStr, "AttributesEditor##%s", item->getName().c_str());
		
	ImGui::Separator();

	CustomAttributeEditor(item);

	auto attrs_map = item->getAttributes();
	std::set<CoreModifiableAttribute*, AttributeNameSorter> attrs;
	for (auto&& pair : attrs_map)
	{
		attrs.insert(pair.second);
	}

	

	struct lua_method_ref
	{
		CoreModifiableAttribute* attr;
		XMLNode* node;
	};

	kigs::unordered_map<unsigned int, lua_method_ref> lua_methods;

	/// Attributes
	ImGui::SetNextTreeNodeOpen(gKigsTools->CurrentSettings.AttributesOpen, ImGuiCond_Once);
	if ((gKigsTools->CurrentSettings.AttributesOpen = ImGui::CollapsingHeader("Attributes")))
	{
		ImGui::Checkbox("Hide attributes not in the XML file", &gKigsTools->CurrentSettings.HideNotInXML);
		ImGui::Checkbox("AutoCommit", &gKigsTools->CurrentSettings.AutoCommit);

		bool commitall = false;
		
		ImGui::SameLine();
		commitall = ImGui::Button("Commit all changes");
		
		ImGui::Separator();

		for (auto attr : attrs)
		{
			KigsID id = attr->getID();


			CoreModifiable::ATTRIBUTE_TYPE type = attr->getType();
			snprintf(tmpStr, 2048, "%s##%p", attr->getLabel()._id_name.c_str(), attr);

			std::string func_name;
			bool is_lua_method = false;
			if (attr->isDynamic() && attr->getType() == CoreModifiable::ATTRIBUTE_TYPE::STRING)
			{
				std::vector<std::string> splitted = SplitStringByCharacter(attr->getLabel()._id_name, '§');
				if (splitted.size() == 3 && splitted[1] == "LUA_CODE")
				{
					func_name = splitted[2];
					is_lua_method = true;
				}
			}

			// Match with xml
			XMLNode* attr_xml = 0;
			if (xml_node)
			{
				for (int i = 0; i < xml_node->getChildCount(); i++)
				{
					auto child = static_cast<XMLNode*>(xml_node->getChildElement(i));
					auto child_name = child->XMLNodeBase::getName();
					if (child->nameOneOf("CoreModifiableAttribute", "Attr"))
					{
						auto name = child->getAttribute("Name", "N");
						if (name && KigsID(name->getString()) == attr->getLabel())
						{
							attr_xml = child;
							break;
						}
					}
					else if (is_lua_method && child->XMLNodeBase::getName() == "LUA")
					{
						auto name = child->getAttribute("Name", "N");
						if (name && name->getString() == func_name)
						{
							attr_xml = child;
							lua_methods[CharToID::GetID(func_name)] = { attr, child };
							break;
						}
					}
				}
			}

			if (!attr_xml && xml_node && gKigsTools->CurrentSettings.HideNotInXML)
				continue;

			
			ImGui::PushID(id._id);

			bool add_to_xml = false;

			if (xml_node && attr_xml)
			{
				if (ImGui::Button(ICON_FK_TRASH))
				{
					xml_node->removeChild(attr_xml);
					delete attr_xml;
					attr_xml = nullptr;
					if (xml_node->getChildCount() == 0 && xml_node->XMLNodeBase::getName() == "Rel")
					{
						auto parent_node = xml_node->getParent();
						if (parent_node) ((XMLNode*)parent_node)->removeChild(xml_node);
						item->mXMLNodes.erase(xml_file);
						delete xml_node;
						xml_node = nullptr;
					}
					AddToXMLChanged(xml_file_item, gKigsTools->ActiveXMLFile);
				}
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Remove %s from XML", id._id_name.c_str());
					
				ImGui::SameLine();
			}

			bool can_add = xml_file_item
				&& item->mXMLNodes.size() // The item need to be defined in at least one xml file 
				&& (!xml_node || !attr_xml);
			
			CoreModifiable* first_item_in_file = item;

			if (can_add) first_item_in_file = FindFirstNodeInFile(item, xml_file);
			can_add = can_add && first_item_in_file;

			if(can_add)
			{
				std::string value = "";
				attr->getValue(value);

				if (ImGui::Button(ICON_FK_PLUS))
				{
					add_to_xml = true;
				}
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Add %s to %s", id._id_name.c_str(), xml_file->getPath().c_str());

				ImGui::SameLine();
			}


			if (!attr_xml)
				ImGui::PushStyleColor(ImGuiCol_Text, v4f(0.5f, 0.5f, 0.5f, 1.0f));

			bool changed = false;

			ImGui::PushItemWidth(-200);
			switch (type)
			{

			case CoreModifiable::ATTRIBUTE_TYPE::BOOL:
			{
				bool value;
				attr->getValue(value);

				ImGui::Dummy(ImGui::CalcItemSize(ImVec2{ -200 - ImGui::GetTextLineHeight() * 2, 0 }, 0, 0)); ImGui::SameLine();
				if (ImGui::Checkbox(tmpStr, &value))
				{
					attr->setValue(value);
					changed = true;
				}
				DecorateAttribute(attr);
				break;

			}


			case CoreModifiable::ATTRIBUTE_TYPE::CHAR:
			case CoreModifiable::ATTRIBUTE_TYPE::SHORT:
			case CoreModifiable::ATTRIBUTE_TYPE::INT:
			case CoreModifiable::ATTRIBUTE_TYPE::LONG:
			{
				int value;
				attr->getValue(value);
				if (ImGui::InputInt(tmpStr, &value))
				{
					attr->setValue(value);
					changed = true;
				}
				DecorateAttribute(attr);
				break;
			}
			case CoreModifiable::ATTRIBUTE_TYPE::UCHAR:
			case CoreModifiable::ATTRIBUTE_TYPE::USHORT:
			case CoreModifiable::ATTRIBUTE_TYPE::UINT:
			case CoreModifiable::ATTRIBUTE_TYPE::ULONG:
			{
				int value;
				attr->getValue(value);
				if (ImGui::InputInt(tmpStr, &value))
				{
					if (value < 0) value = 0;
					attr->setValue(value);
					changed = true;
				}
				DecorateAttribute(attr);
				break;
			}


			case CoreModifiable::ATTRIBUTE_TYPE::FLOAT:
			case CoreModifiable::ATTRIBUTE_TYPE::DOUBLE:
			{
				float value;
				attr->getValue(value);
				if (ImGui::InputFloat(tmpStr, &value, 0.01f, 0.1f))
				{
					attr->setValue(value);
					changed = true;
				}
				DecorateAttribute(attr);
				break;
			}


			case CoreModifiable::ATTRIBUTE_TYPE::ARRAY:
			{
				int size = attr->getNbArrayColumns();
				CoreModifiable::ATTRIBUTE_TYPE array_type = attr->getArrayElementType();

#define ImGuiInputArray(size, type, label, var) ImGui::Input##type##size(label, var)

				if (array_type == CoreModifiable::ATTRIBUTE_TYPE::FLOAT || array_type == CoreModifiable::ATTRIBUTE_TYPE::DOUBLE)
				{

					bool is_color = id._id_name.find("Color") != std::string::npos || id._id_name.find("color") != std::string::npos;

					if (size == 2)
					{
						float arr[2] = {};
						attr->getArrayValue(arr, 2);
						changed = ImGuiInputArray(2, Float, tmpStr, arr);

						if (id == "Dock" || id == "Anchor")
						{
							if (ImGui::BeginPopupContextItem(tmpStr))
							{
								if (ImGui::Button("Center"))
								{
									arr[0] = arr[1] = 0.5f;
									changed = true;
									ImGui::CloseCurrentPopup();
								}
								ImGui::EndPopup();
							}
						}
						if(changed) attr->setArrayValue(arr, 2);
					}
					else if (size == 3)
					{
						float arr[3] = {};
						attr->getArrayValue(arr, 3);
						if (is_color)
							changed = ImGui::ColorEdit3(tmpStr, arr);
						else
							changed = ImGuiInputArray(3, Float, tmpStr, arr);
						if(changed) attr->setArrayValue(arr, 3);

					}
					else if (size == 4)
					{
						float arr[4] = {};
						attr->getArrayValue(arr, 4);

						if (is_color)
							changed = ImGui::ColorEdit4(tmpStr, arr);
						else
							changed = ImGuiInputArray(4, Float, tmpStr, arr);
						
						if(changed) attr->setArrayValue(arr, 4);
					}
				}
				else
				{
					if (size == 2)
					{
						int arr[2] = {};
						attr->getArrayValue(arr, 2);
						if (ImGuiInputArray(2, Int, tmpStr, arr))
						{
							attr->setArrayValue(arr, 2);
							changed = true;
						}
					}
					else if (size == 3)
					{
						int arr[3] = {};
						attr->getArrayValue(arr, 3);
						if (ImGuiInputArray(3, Int, tmpStr, arr))
						{
							attr->setArrayValue(arr, 3);
							changed = true;
						}
					}
					else if (size == 4)
					{
						int arr[4] = {};
						attr->getArrayValue(arr, 4);
						if (ImGuiInputArray(4, Int, tmpStr, arr))
						{
							attr->setArrayValue(arr, 4);
							changed = true;
						}
					}
				}


				DecorateAttribute(attr);
				break;
			}


			case CoreModifiable::ATTRIBUTE_TYPE::ENUM:
			{
				auto enumElements = attr->getEnumElements();

				int current_item = 0;
				attr->getValue(current_item);
				if (ImGui::Combo(tmpStr, &current_item, enumElements))
				{
					attr->setValue(current_item);
					changed = true;
				}
				DecorateAttribute(attr);
				break;
			}

			case CoreModifiable::ATTRIBUTE_TYPE::STRING:
			case CoreModifiable::ATTRIBUTE_TYPE::WEAK_REFERENCE:
			case CoreModifiable::ATTRIBUTE_TYPE::STRONG_REFERENCE:
			case CoreModifiable::ATTRIBUTE_TYPE::USSTRING:
			{
				if (!is_lua_method)
				{

					std::string str; attr->getValue(str);
					if (str.size() >= 1024)
					{
						ImGui::Text("Text too long to edit");
					}
					else
					{
						char txt[1024];
						snprintf(txt, 1024, "%s", str.c_str());

						if (ImGui::InputText(tmpStr, txt, 1024))
						{
							attr->setValue(txt);
							changed = true;
						}

						if (ImGui::BeginDragDropTarget())
						{
							if (auto pl = ImGui::AcceptDragDropPayload("DND_KIGS_ITEM"))
							{
								//@TODO(antoine) relative path instead of global search if possible
								DragItemPayload data = *(DragItemPayload*)pl->Data;
								attr->setValue(data.item->getExactType() + ":" + data.item->getName());
								changed = true;
							}

							if (auto pl = ImGui::AcceptDragDropPayload("DND_KIGS_STRING"))
							{
								attr->setValue((const char*)pl->Data);
								changed = true;
							}

							ImGui::EndDragDropTarget();
						}
					}
					DecorateAttribute(attr);
				}
				break;
			}

			case CoreModifiable::ATTRIBUTE_TYPE::COREITEM:
			{
				ImGui::Text("TODO CoreItem : %s", tmpStr);
				break;
			}
			case CoreModifiable::ATTRIBUTE_TYPE::UNKNOWN: break;
			default: break;
			}

			ImGui::PopItemWidth();

			if (!attr_xml)
				ImGui::PopStyleColor();

			if (changed && can_add)
			{
				add_to_xml = true;
			}
			if (add_to_xml)
			{
				std::string value = "";
				attr->getValue(value);

				if (!xml_node)
				{
					auto parent_xml_node = first_item_in_file->mXMLNodes[xml_file];
				
					// Create relation chain
					auto current_item = item;
					XMLNode* child_rel_node = nullptr;
					//XMLNode* rel_node = nullptr;
					for (;;)
					{
						auto new_rel = new XMLNode(XMLNodeType::XML_NODE_ELEMENT, "Rel");
						new_rel->addAttribute(new XMLAttribute("P", current_item->getName()));
						
						current_item->mXMLNodes[xml_file] = new_rel;
						
						if (child_rel_node)
						{
							new_rel->addChild(child_rel_node);
							child_rel_node->setParent(new_rel);
						}
						child_rel_node = new_rel;
						if (!xml_node) xml_node = new_rel;

						current_item = current_item->getFirstParent("CoreModifiable");
						if (current_item == first_item_in_file || !current_item) break;
					}
					parent_xml_node->addChild(child_rel_node);
					child_rel_node->setParent(parent_xml_node);
					//xml_node = rel_node;
					//item->mXMLNodes[xml_file] = xml_node;
				}

				XMLNode* new_node = new XMLNode(XML_NODE_ELEMENT, "Attr");
				new_node->addAttribute(new XMLAttribute("N", attr->getLabel()._id_name));
				if (attr->isDynamic())
					new_node->addAttribute(new XMLAttribute("T", CoreModifiableAttribute::typeToString(attr->getType())));

				new_node->addAttribute(new XMLAttribute("V", value));
				if (attr->isDynamic())
					new_node->addAttribute(new XMLAttribute("Dyn", "true"));
				xml_node->addChild(new_node);
				new_node->setParent(xml_node); //@NOTE(antoine) it is really to have to do this
				AddToXMLChanged(xml_file_item, gKigsTools->ActiveXMLFile);

				attr_xml = new_node;
				
			}

			AttributeChange attr_change;
			XMLAttributeBase* lua_initializer = nullptr;
			if (!is_lua_method && attr_xml)
			{
				if (attr_xml->getAttribute("Value", "V"))
				{
					attr_change = CheckAttributeChange(attr_xml, attr);
				}
				else
					lua_initializer = attr_xml->getAttribute("LUA", "L");
			}

			if (attr_change.changed)
			{
				std::string str = "Commit change to XML (old was ";

				str += attr_change.old_value;
				str += ")";

				bool save = (gKigsTools->CurrentSettings.AutoCommit && changed) || commitall;
				if (!save)
				{
					ImGui::SameLine();
					save = ImGui::Button(ICON_FK_FLOPPY_O);
				}
				if (save)
				{
					if (is_lua_method)
						attr_xml->setString(attr_change.value);
					else
						static_cast<XMLAttribute*>(attr_change.attr_value)->setString(attr_change.value);

					AddToXMLChanged(xml_file_item, gKigsTools->ActiveXMLFile);
				}
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(str.c_str());
			}
			else if (lua_initializer)
			{

				ImGui::SameLine();
				bool clicked = ImGui::Button(ICON_FK_CODE);
				if (ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("Lua initializer: %s", lua_initializer->getString().c_str());
				}
				if (clicked)
				{
					ModuleFileManager* fm = (ModuleFileManager*)KigsCore::Instance()->GetMainModuleInList(FileManagerModuleCoreIndex);
#if 0
					mStringToEditExternally = &const_cast<std::string&>(lua_initializer->getString());
					mExternalTempFile = "temp.lua";

					fm->SaveFile(mExternalTempFile.c_str(), (unsigned char*)mStringToEditExternally->c_str(), mStringToEditExternally->size());
					OpenFileWithPreferredEditor(mExternalTempFile.c_str());
					mOpenExternEditPopup = true;
#ifdef WIN32
					wd::unwatch(mExternalTempFile);
					wd::watch(mExternalTempFile, [item](const ci::fs::path& path)
					{
						unsigned long len;
						CoreRawBuffer* buf = ModuleFileManager::LoadFileAsCharString(mExternalTempFile.c_str(), len);
						if (buf)
						{
							if (*mStringToEditExternally != buf->buffer())
							{
								*mStringToEditExternally = buf->buffer();
								AddToXMLChanged(FindXMLFile(item));
							}
							buf->Destroy();
						}
					});
#endif		
#endif
				}

			}

			ImGui::PopID();
		}
	}

	/// mMethods
	ImGui::SetNextTreeNodeOpen(gKigsTools->CurrentSettings.MethodsOpen, ImGuiCond_Once);
	if ((gKigsTools->CurrentSettings.MethodsOpen = ImGui::CollapsingHeader("Methods")))
	{
		for (auto&& pair : item->GetTypeNode()->mMethods)
		{
			if (pair.second.IsMethod())
				ImGui::Text("%s - %s", pair.first._id_name.c_str(), pair.second.GetMethod().mName.c_str());
		}

		if (item->GetTypeNode()->mMethods.size())
			ImGui::Text("-----");

		auto lz = item->GetLazyContentNoCreate();
		if (lz)
		{
			for (auto&& pair : lz->mMethods)
			{
				ImGui::PushID(pair.first.toUInt());
				if(pair.second.IsMethod())
					ImGui::Text("%s - %s", pair.first._id_name.c_str(), pair.second.GetMethod().mName.c_str());

				auto it = lua_methods.find(pair.first.toUInt());
				if (it != lua_methods.end())
				{
					ImGui::SameLine();
					bool clicked = ImGui::Button("Edit lua...");
					std::string code;
					it->second.attr->getValue(code);
					//@TODO
				}
				ImGui::PopID();
			}
		}

	}

	/// Connections
	ImGui::SetNextTreeNodeOpen(gKigsTools->CurrentSettings.ConnectionsOpen, ImGuiCond_Once);
	if ((gKigsTools->CurrentSettings.ConnectionsOpen = ImGui::CollapsingHeader("Connections")))
	{
		auto lz = item->GetLazyContentNoCreate();
		if(lz)
		{
			for (auto& obj : lz->mConnectedTo)
			{
				for (auto& c : obj.second)
				{
					if(auto ptr = c.second.lock())
						ImGui::Text("%s => %s->%s()", obj.first._id_name.c_str(), ptr->getName().c_str(), c.first._id_name.c_str());
				}
			}
		}

		auto signals = item->GetSignalList();
		static KigsID selected_signal = "";
		static KigsID selected_slot = "";
		static bool custom_signal = false;
		static bool custom_slot = false;
		
		ImGui::Columns(3);

		if (custom_signal)
		{
			std::string id = selected_signal._id_name;
			if (ImGui::InputText("Signal", id))
			{
				selected_signal = id;
			}
		}
		else
		{
			if (ImGui::BeginCombo("Signal", selected_signal._id_name.c_str()))
			{
				for (auto s : signals)
				{
					if (ImGui::Selectable(s._id_name.c_str())) selected_signal = s;
				}
				ImGui::EndCombo();
			}
		}
		if (ImGui::IsMouseClicked(1) && ImGui::IsItemHovered())
		{
			custom_signal = !custom_signal;
		}

		ImGui::NextColumn();

		static maReference connect_to;
		CoreModifiable* connect_to_item = connect_to;
		ImGui::Text(connect_to_item ? connect_to_item->getName().c_str() : "DRAG RECEIVER HERE");

		if (ImGui::BeginDragDropTarget())
		{
			if (auto pl = ImGui::AcceptDragDropPayload("DND_KIGS_ITEM"))
			{
				DragItemPayload data = *(DragItemPayload*)pl->Data;
				connect_to = data.item;
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::NextColumn();

		if (custom_slot || !connect_to_item)
		{
			std::string id = selected_slot._id_name;
			if (ImGui::InputText("Slot", id))
			{
				selected_slot = id;
			}
		}
		else
		{
			if (ImGui::BeginCombo("Slot", selected_slot._id_name.c_str()))
			{
				for (auto&& pair : connect_to_item->GetTypeNode()->mMethods)
				{
					if (ImGui::Selectable(pair.first._id_name.c_str())) selected_slot = pair.first;
				}

				ImGui::TextColored(v4f(0.5f,0.5f,0.5f,1.0f), "Dynamic Methods");

				auto lz = connect_to_item->GetLazyContentNoCreate();
				if (lz)
				{
					for (auto&& pair : lz->mMethods)
					{
						if (ImGui::Selectable(pair.first._id_name.c_str())) selected_slot = pair.first;
					}
				}
				ImGui::EndCombo();
			}
		}
		if (ImGui::IsMouseClicked(1) && ImGui::IsItemHovered())
		{
			custom_slot = !custom_slot;
		}


		ImGui::Columns();

		

		// TODO REDO
		/*
		CoreConnectionManager* connection_manager = KigsCore::GetConnectionManager();
		auto connections = connection_manager->GetConnectionMap();

		auto display_slots = [item, node](CoreConnectionManager::ConnectionStruct* current, KigsID signal)
		{
			while (current)
			{
				ImGui::PushID(current->m_slotReceiver);
				bool pop = false;

				auto methods = current->m_slotReceiver->GetMethods();
				bool is_lua = false;
				if (methods)
				{
					auto it = methods->find(current->m_slotID);
					if (it != methods->end())
					{
						is_lua = it->second.m_Method == static_cast<RefCountedClass::ModifiableMethod>(&DynamicMethodLuaGlobalCallback::LuaGlobalCallback);
					}
				}

				if (node)
				{

					if (current->xmlattr)
					{

						if (ImGui::Button(ICON_FK_TRASH))
						{

						}
						if (ImGui::IsItemHovered())
						{
							ImGui::SetTooltip("Remove from XML");
						}
					}
					else
					{
						if (ImGui::Button(ICON_FK_PLUS))
						{
							auto connect = node->addChildElement("Connect");

							XMLAttribute* attr_sender = new XMLAttribute("E", "this");
							XMLAttribute* attr_receiver = new XMLAttribute("R", current->m_slotReceiver->getExactTypeID()._id_name + ":" + current->m_slotReceiver->getName());

							XMLAttribute* attr_signal = new XMLAttribute("Si", signal._id_name);
							XMLAttribute* attr_slot = new XMLAttribute("Sl", current->m_slotID._id_name);

							connect->addAttribute(attr_sender);
							connect->addAttribute(attr_receiver);
							connect->addAttribute(attr_signal);
							connect->addAttribute(attr_slot);

							AddToXMLChanged(item);
							current->xmlattr = connect;
						}
						if (ImGui::IsItemHovered())
						{
							ImGui::SetTooltip("Add to XML");
						}

						ImGui::PushStyleColor(ImGuiCol_Text, ImColor{ 128,128,128 });
						pop = true;
					}
					ImGui::SameLine();
				}

				ImGui::Text("---> \"%s\" of %s (%s)", current->m_slotID._id_name.c_str(), current->m_slotReceiver->getName().c_str(), is_lua ? "LUA" : "C++");
				current = current->m_next;

				if (pop)
					ImGui::PopStyleColor();

				ImGui::PopID();
			}
		};
			
		auto itc = connections.find(item);
		bool has_connections = itc != connections.end();

		ImGui::Checkbox("Hide unconnected signals", &gKigsTools->CurrentSettings.HideUnconnectedSignals);

		auto signals = item->GetSignalList();

		for (auto s : signals)
		{

			if (gKigsTools->CurrentSettings.HideUnconnectedSignals)
			{
				if (!has_connections) continue;

				auto it = itc->second.find(s);
				if (it == itc->second.end())
				{
					continue;
				}
			}

			ImGui::TreePush(s._id_name.c_str());
			bool open = ImGui::TreeNodeAdv(s._id_name.c_str(), ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowOverlapMode);

			ImGui::SameLine();

			ImGui::PushID(s.mID);
			if (ImGui::Button(ICON_FK_PLUG))
			{

			}
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Connect to slot...");
			}
			ImGui::PopID();

			if (has_connections)
			{
				int slot_count = 0;

				auto it = itc->second.find(s);

				if (it != itc->second.end())
				{
					auto current = it->second;
					while (current)
					{
						slot_count += 1;
						current = current->m_next;
					}
				}

				ImGui::SameLine();
				ImGui::Text("%d connections", slot_count);

				if (it != itc->second.end())
				{
					if (open)
						display_slots(it->second, s);
					itc->second.erase(s);
				}
			}

			if (open)
				ImGui::TreePop();

			ImGui::TreePop();
		}
		if (has_connections)
		{
			for (auto pair : itc->second)
			{
				std::string signal = pair.first._id_name;
				ImGui::TreePush(signal.c_str());
				bool open = ImGui::TreeNodeAdv(signal.c_str(), ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowOverlapMode);
				if (open)
				{
					display_slots(pair.second, pair.first);
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
		}
		*/
	}


	if (!nobegin)
		ImGui::End();

	if (id)
		ImGui::PopID();
}

void DrawResources()
{
	if (!gKigsTools->CurrentSettings.ResourcesOpen || !gKigsTools->ShowWindow.MenuBar) return;

	if (ImGui::Begin("Resources", &gKigsTools->CurrentSettings.ResourcesOpen))
	{
		auto fpm = FilePathManager::Get();
		auto& bundle = fpm->GetBundleList();
		auto root = fpm->GetBundleRoot();

		ImGui::InputText("Search", gKigsTools->ResourcesWindow.SearchString);
		auto search = PrepareWildcardSearch(gKigsTools->ResourcesWindow.SearchString);

		for (auto& path : bundle)
		{
			auto fullpath = (root + (path.second.size() ? path.second[0] : "") + path.first);
			if (search.size() && !PassWildcardSearch(search, fullpath)) continue;
			
			auto label = path.first;
			if (path.second.size() >= 2) label += " ! MULTIPLE PATHS FOR THE SAME NAME !";
			ImGui::Selectable(label.c_str());
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip(fullpath.c_str());
			}

			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("DND_KIGS_STRING", path.first.c_str(), path.first.size() + 1);
				ImGui::Text(path.first.c_str());
				ImGui::EndDragDropSource();
			}
			
		}
	}
	ImGui::End();
}

void DrawXMLNode(XMLNode* node);

void DrawRelativeNode(XMLNode* node)
{

}
void DrawReferenceNode(XMLNode* node)
{

}
void DrawIncludeNode(XMLNode* node)
{

}

void DrawInstanceNode(XMLNode* node)
{
	XMLAttributeBase* name_attr		= node->getAttribute("N", "Name");
	XMLAttributeBase* type_attr		= node->getAttribute("T", "Type");
	XMLAttributeBase* path_attr		= node->getAttribute("P", "Path");
	XMLAttributeBase* unique_attr	= node->getAttribute("U", "Unique");
	std::string name = "";
	if (name_attr) name = name_attr->getString();

	if (type_attr)
	{
		if (ImGui::TreeNode(node, "%s - %s", type_attr->getString().c_str(), name.c_str()))
		{
			for (u32 i = 0; i < node->getChildCount(); ++i)
			{
				DrawXMLNode(static_cast<XMLNode*>(node->getChildElement(i)));
			}
			ImGui::TreePop();
		}
	}
}

void DrawXMLNode(XMLNode* node)
{
	if (node->nameOneOf("Instance", "Inst"))
		DrawInstanceNode(node);
	else if (node->XMLNodeBase::getName() == "Include")
		DrawIncludeNode(node);
	else if (node->nameOneOf("Ref", "Reference"))
		DrawReferenceNode(node);
	else if (node->nameOneOf("Rel", "RelativePath"))
		DrawRelativeNode(node);
	else if (node->nameOneOf("Attr", "CoreModifiableAttribute"))
	{
		auto val = node->getAttribute("V", "Value");
		ImGui::Text("%s = %s", node->getAttribute("N", "Name")->getString().c_str(), val ? val->getString().c_str() : "???");
	}
}

void DrawXMLEditor(const std::string& filename, XML* file)
{
	if (ImGui::Begin("XML Editor"))
	{
		ImGui::Text("%s - %s", filename.c_str(), file->getEncoding().c_str());
		auto node = (XMLNode*)file->getRoot();
		DrawXMLNode(node);
	}
	ImGui::End();
}

void DrawAttributes()
{
	if (!gKigsTools->ShowWindow.Attributes || !gKigsTools->ShowWindow.MenuBar) return;

	ImGui::SetNextWindowSize(ImVec2{400,700}, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2{800, 50}, ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Attributes", &gKigsTools->ShowWindow.Attributes))
	{
		CoreModifiable* item = gKigsTools->HierarchyWindow.SelectedItem;
		if (item)
		{
			AttributesEditor(item, nullptr, true);
		}
	}
	ImGui::End();
}

void DrawCustomWidgets()
{
	for (auto& w : gKigsTools->CustomWidgets)
	{
		if (w.second.second)
		{
			ImGui::SetNextWindowSize(v2f(0, 0), ImGuiCond_FirstUseEver);
			if (ImGui::Begin(w.first.c_str(), &w.second.second))
			{
				w.second.first();
			}
			ImGui::End();
		}
	}
}

void ReloadCurrentSequence()
{
#if KIGS_COREDATADRIVENSEQUENCE_AVAILABLE
	auto manager = gKigsTools->CurrentSequenceManager->as<DataDrivenSequenceManager>();
	if (manager)
	{
		auto state = manager->GetState();
		manager->RequestStateChange(state);
		gKigsTools->FramesToRun = std::max(gKigsTools->FramesToRun, 2);
	}
#endif
}

void DrawAnimationCoreItemRec(const std::string& key,const CoreItemSP& current_item)
{
	std::string name = "";
	switch (current_item->GetType())
	{
	case CoreItem::COREVALUE: name = (std::string)*current_item; break;
	case CoreItem::CORENAMEDVALUE: name = "" + (std::string)*current_item; break;
	case CoreItem::COREMAP: name = "map"; break;
	case CoreItem::COREVECTOR: name = "vector"; break;
	case CoreItem::CORENAMEDMAP: name = "named map"; break;
	case CoreItem::CORENAMEDVECTOR: name = "named vector"; break;
	} 
	if (key.size()) name = key + " : " + name;

	if (ImGui::TreeNode((void*)(current_item.get()), name.c_str()))
	{
		if (!current_item->empty())
		{
			for (auto it = current_item->begin(); it != current_item->end(); ++it)
			{
				std::string nkey;
				it.getKey(nkey);
				DrawAnimationCoreItemRec(nkey, (*it));
			}
		}
		ImGui::TreePop();
	}
}

void DrawAnimationEditor()
{
	return;
	static std::string json_animation = R"==({
"CircleAnimation":[{
"Combo":[{
"Serie":[{
"Notification":[3,"ShowInsideElements"]
}
]
}
,{
"DoWhile":["forever",{
"Serie":[{
"Linear1D":[12,0,6.283185,"RotationAngle",0]
}
,{
"Wait":[0]
}
]
}
]
}
,{
"Serie":[{
"Linear1D":[3,0,1,"Opacity",0]
}
]
}
]
}
]
})==";

	auto update_item = [&]()
	{
		JSonFileParser json_parser;
		return json_parser.Get_JsonDictionaryFromString(json_animation);
	};

	static CoreItemSP core_item = update_item();

	if (ImGui::Begin("AnimationEditor"))
	{
		DrawAnimationCoreItemRec("", core_item);
	}
	ImGui::End();
}

void DrawEditor()
{
	auto last_mode = gKigsTools->ShowWindow.MenuBar;
	auto kb = gKigsTools->InputModule->GetKeyboard(); 

	auto mt = gKigsTools->InputModule->GetMultiTouch();

	if (mt)
	{
		auto count = mt->getMaxTouch(); 
		int down = 0;
		for (int i = 0; i < count; ++i)
		{
			if (mt->getTouchState(i)) down++;
		}
		if (down == count)
		{
			if (gKigsTools->CanPauseWithMultiTouch)
			{
				gKigsTools->Paused = !gKigsTools->Paused;
				if (gKigsTools->Paused) gKigsTools->ShowWindow.MenuBar = true;
				gKigsTools->CanPauseWithMultiTouch = false;
			}
		}
		else if (!gKigsTools->CanPauseWithMultiTouch)
			gKigsTools->CanPauseWithMultiTouch = true;
	}
	if (kb)
	{
		auto& keys = kb->Get_KeyDownList();
		for (auto& ev : keys)
		{
			switch (ev.KeyCode)
			{
			case VK_F1:
				if (ImGui::GetIO().KeyCtrl)
				{
					if (!gKigsTools->Paused)
						gKigsTools->ShowWindow.MenuBar = !gKigsTools->ShowWindow.MenuBar;
				}
				break;
			case VK_F4:
				if (ImGui::GetIO().KeyCtrl)
				{
					gKigsTools->Paused = !gKigsTools->Paused;
					if (gKigsTools->Paused) gKigsTools->ShowWindow.MenuBar = true;
				}
				break;
			case VK_F8:
				if (ImGui::GetIO().KeyCtrl)
				{
					if (!gKigsTools->ShowWindow.MenuBar)
						gKigsTools->ShowWindow.Hierarchy = gKigsTools->ShowWindow.MenuBar = true;
					else
						gKigsTools->ShowWindow.Hierarchy = !gKigsTools->ShowWindow.Hierarchy;
				}
				break;
			case VK_F9:
				if (ImGui::GetIO().KeyCtrl)
				{
					if (!gKigsTools->ShowWindow.MenuBar)
						gKigsTools->ShowWindow.Attributes = gKigsTools->ShowWindow.MenuBar = true;
					else
						gKigsTools->ShowWindow.Attributes = !gKigsTools->ShowWindow.Attributes;
					break;
				}
			case VK_F10:
			{
				if (ImGui::GetIO().KeyCtrl)
				{
					std::vector<CMSP> shaders = CoreModifiable::GetInstances("API3DShader");
					for (auto shader : shaders)
					{
						shader->SimpleCall("Reload");
					}
				}
				break;
			}
			case VK_S:
			{
				if (ImGui::GetIO().KeyCtrl)
				{
					static bool saving = false;
					if (!saving)
					{
						saving = true;
#ifdef WUP
						no_await_lambda([]() -> winrt::Windows::Foundation::IAsyncAction
						{
#endif
							kigs_defer
							{
								saving = false;
							};
							kigs::unordered_map<CoreModifiable*, KigsToolsState::XMLChange> ToReAdd;
							for (auto& el : gKigsTools->XMLChanged)
							{
								for (auto& xml : el.second.FileChanged)
								{
									auto path = xml->getPath();
#ifdef WUP
									if (auto itfind = gKigsTools->CurrentSettings.TrueFilePaths.find(path); itfind != gKigsTools->CurrentSettings.TrueFilePaths.end())
									{
										path = itfind->second;
									}
									else
									{
										using namespace winrt::Windows::Storage::Pickers;
										FileSavePicker picker;
										picker.SuggestedStartLocation(PickerLocationId::ComputerFolder);
										picker.SuggestedFileName(to_wchar(path));
										auto types = winrt::single_threaded_vector<winrt::hstring>({ L".xml" });
										picker.FileTypeChoices().Insert(L"XML", types);
										auto file = co_await picker.PickSaveFileAsync();

										if (file)
										{
											auto true_path = to_utf8(file.Path().c_str());
											gKigsTools->CurrentSettings.TrueFilePaths[path] = true_path;
											path = true_path;
										}
										else
										{
											path = "";
										}
									}
#endif
									if (path.empty())
									{
										ToReAdd[el.first].FileChanged.insert(xml);
									}
									else
									{
#ifdef WUP
										std::string output;
										XMLWriterFile::WriteString(*(XML*)xml.get(), output);
										auto file = co_await StorageFile::GetFileFromPathAsync(to_wchar(path));
										if (file)
										{
											auto file_stream = co_await file.OpenAsync(FileAccessMode::ReadWrite);
											Streams::DataWriter writer(file_stream);
											writer.WriteBytes({ (u8*)output.data(), (u8*)(output.data() + output.size()) });
											co_await writer.StoreAsync();
										}
#else
										((XML*)xml.get())->WriteFile(path);
#endif
									}
								}
							}
							gKigsTools->XMLChanged = ToReAdd;
#ifdef WUP
						});
#endif
					}
				}
				break;
			}

			case VK_R:
			{
				if (ImGui::GetIO().KeyCtrl)
					ReloadCurrentSequence();
				break;
			}
			case VK_F6:
			{
				//ResetContext();
				break;
			}
			}
		}
	}

	if (gKigsTools->ShowNextDrawAction != KigsToolsState::ShowNextDraw::DoNothing)
	{
		if (gKigsTools->ShowNextDrawAction == KigsToolsState::ShowNextDraw::Show)
			gKigsTools->ShowWindow.MenuBar = true;
		else if (gKigsTools->ShowNextDrawAction == KigsToolsState::ShowNextDraw::Hide)
			gKigsTools->ShowWindow.MenuBar = false;

		gKigsTools->ShowNextDrawAction = KigsToolsState::ShowNextDraw::DoNothing;
	}

	if (last_mode != gKigsTools->ShowWindow.MenuBar)
	{
		if (gKigsTools->ShowWindow.MenuBar)
		{
			//gKigsTools->InputModule->getTouchManager()->pushNewState();
			//gKigsTools->ImGuiLayer->RegisterTouch();
		}
		else
		{
			//gKigsTools->InputModule->getTouchManager()->popState();
		}
	}

#if KIGS_COREDATADRIVENSEQUENCE_AVAILABLE
	if (gKigsTools->CurrentSettings.SequenceEditor)
	{
		auto manager = gKigsTools->CurrentSequenceManager->as<DataDrivenSequenceManager>();
		if (manager)
		{
			if (gKigsTools->Paused)
				manager->SkipTransitions = SkipTransition_Both;
			else
				manager->SkipTransitions = SkipTransition_NoSkip;

			auto seq = manager->GetCurrentSequence();
			if (gKigsTools->HierarchyWindow.Scope.size()==1 && seq)
			{
				PushToScope(seq.get());
				//gKigsTools->HierarchyWindow.ForceExpandAll = true;
				if (seq->mXMLFiles.size())
				{
					gKigsTools->ActiveXMLItem = seq.get();
					gKigsTools->ActiveXMLFile = seq->mXMLFiles.back();
				}
			}
		}
	}
#else
	gKigsTools->CurrentSettings.SequenceEditor=false
#endif

	//ImGuiViewport* viewport = ImGui::GetMainViewport();
	//ImGuiID id = ImGui::GetID("editor_dockspace");
	//ImGui::DockSpaceOverViewport(viewport);

	DrawMenuBar();
	DrawHierarchy();
	DrawAttributes();
	DrawCreateInstance();
	DrawCustomWidgets();
	DrawResources();
	DrawAnimationEditor();

	if (gKigsTools->ShowWindow.MenuBar)
	{
		for (auto it = gKigsTools->EnterStringPopups.begin(); it != gKigsTools->EnterStringPopups.end();)
		{
			if (!ImGui::IsPopupOpen(it->popup_id.c_str()) && !it->first_open)
			{
				it->first_open = true;
				ImGui::OpenPopup(it->popup_id.c_str());
			}
			if (ImGui::BeginPopup(it->popup_id.c_str()))
			{
				auto enter = ImGui::InputText(it->label.c_str(), it->value, ImGuiInputTextFlags_EnterReturnsTrue);
				
				if (ImGui::Button("Cancel") || (ImGui::IsKeyReleased(VK_ESCAPE) && ImGui::IsWindowFocused()))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Ok") || enter)
				{
					it->ok = true;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			if(!ImGui::IsPopupOpen(it->popup_id.c_str()))
			{
				if (it->ok || it->always_ok) it->on_ok(*it);
				else if (it->on_cancel) it->on_cancel(*it);
				it = gKigsTools->EnterStringPopups.erase(it);
				continue;
			}
			++it;
		}
	}


	if (gKigsTools->HierarchyWindow.SelectedItem 
		&& gKigsTools->HierarchyWindow.SelectedItem->isSubType("Node3D"))
	{
		if (gKigsTools->HierarchyWindow.SelectedItem->isSubType("Camera"))
		{
			Camera* node = gKigsTools->HierarchyWindow.SelectedItem;
			if(!node->getValue<bool>("CameraIsEnabled"))
				dd::camera(node, { 0,1,0 });
		}
		else
		{
			Node3D* node = gKigsTools->HierarchyWindow.SelectedItem;
			auto l2g = node->GetLocalToGlobal();
			Matrix4x4 m{ l2g };
			dd::axisTriad(&m.e[0][0], 0.1f, 0.2f);
		}
			
	}

	//@TEMPORARY: Force DebugDraw Node3D
	//gKigsTools->CurrentSettings.Node3DDebugDraw = true;
	gCullingDrawBBox = gKigsTools->CurrentSettings.Node3DDebugDraw;

	if (gKigsTools->CurrentSettings != gKigsTools->LastSettings)
	{
		SaveSettings();
	}
	gKigsTools->LastSettings = gKigsTools->CurrentSettings;
}

bool UpdateKigsTools()
{
	if (!SetupTools()) return true;

	bool was_paused = gKigsTools->Paused;

	if (gKigsTools->ImGuiLayer)
	{
		auto old = gKigsTools->ImGuiLayer->SetActiveImGuiLayer();
		if(gKigsTools->ImGuiLayer->mHasFrame)
			DrawEditor();
		ImGui::SetCurrentContext(old);
	}

#if KIGS_RENDERER_AVAILABLE
	gRendererStats = {};
#endif

	if (gKigsTools->Paused && was_paused != gKigsTools->Paused)
	{
		std::vector<CMSP> timers = CoreModifiable::GetInstances("Timer");
		for (auto cm : timers)
		{
			SP<Timer> t = cm;
			gKigsTools->TimerStates[t.get()] = { t->GetState(), t };
			t->SetState(Timer::PAUSED);
		}
	}
	
#if 0
	while(gKigsTools->Paused)
	{
		auto timer = gKigsTools->Application->GetApplicationTimer();
		
		timer->Sleep(1);
		
		GlobalAppDT = 0.0f;
		if (gKigsTools->Application->HasFocus())
			gKigsTools->InputModule->CallUpdate(*timer, 0);

		bool render = false;
		if (gKigsTools->Application->GetSequenceManager())
		{
			render = gKigsTools->Application->GetSequenceManager()->AllowRender();
			if (render)
			{
				CoreGetModule(ModuleRenderer)->CallUpdate(*timer, 0);
			}
				
		}
		CoreGetModule(ModuleSceneGraph)->CallUpdate(*timer, 0);
		CoreGetModule(Module2DLayers)->CallUpdate(*timer, 0);
		CoreGetModule(ModuleGUI)->CallUpdate(*timer, &render);

		DrawEditor();
	}
#endif
	if (was_paused != gKigsTools->Paused && !gKigsTools->Paused)
	{
		for (auto pair : gKigsTools->TimerStates)
		{
			if (pair.second.first == Timer::State::NORMAL)
			{
				pair.first->SetState(Timer::State::NORMAL);
			}
		}
		gKigsTools->TimerStates.clear();
	}

	if (gKigsTools->FramesToRun) { gKigsTools->FramesToRun--; return true; }
	return !gKigsTools->Paused;
}

void DestroyKigsTools()
{
	if (gKigsTools->ImGuiModule && gKigsTools->ImGuiModuleOwned)
	{
		CoreDestroyModule(ModuleImGui);
	}
	gKigsTools = nullptr;
}

void ShowKigsTools(bool show)
{
	gKigsTools->ShowNextDrawAction = show ? KigsToolsState::ShowNextDraw::Show : KigsToolsState::ShowNextDraw::Hide;
}

void RegisterWidget(const std::string& id, std::function<void()> draw_function)
{
	if (!SetupTools()) return;
	if (!draw_function) gKigsTools->CustomWidgets.erase(id);
	auto& w = gKigsTools->CustomWidgets[id];
	w.first = draw_function;
	//w.second = true;
}

void SelectObjectKigsTools(CoreModifiable* obj)
{
	OpenInHierarchy(obj);
	gKigsTools->HierarchyWindow.SelectedItem = obj;
}

#else
void SelectObjectKigsTools(CoreModifiable* obj){}
bool UpdateKigsTools() { return false; }
void DestroyKigsTools(){}
void ShowKigsTools(bool show) { (void)show; }
void RegisterWidget(const std::string& id, std::function<void()> draw_function) { (void)id; (void)draw_function; }
#endif
