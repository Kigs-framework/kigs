#pragma once

#include "Platform/Main/BaseApp.h"
#include "Platform/Core/PlatformCore.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "winrt/Windows.UI.Text.Core.h"
#include "winrt/Windows.UI.ViewManagement.h"


using namespace winrt::Windows::UI::Text::Core;
using namespace winrt::Windows::UI::ViewManagement;

struct ImGuiUWPCoreTextEditContext
{
	std::wstring text;
	std::string utf8_text;

	std::string ghost_text;

	CoreTextEditContext editContext = nullptr;
	InputPane inputPane = nullptr;

	bool selection_need_set = false;
	bool text_need_set = false;
	CoreTextRange selection;
	bool internalFocus = false;
	bool extendingLeft = false;

	bool reset_focus = false;

	bool multiline = false;

	winrt::Windows::UI::Core::CoreWindow::KeyDown_revoker revoker;

	void Draw(const char* label, v2f size = v2f(0,0), ImGuiInputTextFlags flags = 0)
	{
		utf8_text = to_utf8(text);
		auto& state = ImGui::GetCurrentContext()->InputTextState;

		if (internalFocus && text_need_set && state.TextA.Data)
		{
			text_need_set = false;
			state.TextW.resize(text.size()+1);
			memcpy(state.TextW.Data, text.data(), (text.size()+1)*sizeof(wchar_t));

			state.InitialTextA.resize(utf8_text.size()+1);
			memcpy(state.InitialTextA.Data, utf8_text.data(), utf8_text.size()+1);

			state.TextA.resize(utf8_text.size());
			memcpy(state.TextA.Data, utf8_text.data(), utf8_text.size()+1);	
			state.BufCapacityA = utf8_text.size();

			state.CurLenW = text.size();
			state.CurLenA = utf8_text.size();
		}

		if (internalFocus && selection_need_set)
		{
			selection_need_set = false;
			auto& state = ImGui::GetCurrentContext()->InputTextState;
			state.Stb.select_start = selection.StartCaretPosition;
			state.Stb.select_end = selection.EndCaretPosition;
			state.Stb.cursor = selection.EndCaretPosition;
		}

		auto pos_before = ImGui::GetCursorPos();
		if (multiline)
		{
			ImGui::InputTextMultiline(label, utf8_text.data(), utf8_text.size() + 1, size, flags | ImGuiInputTextFlags_ReadOnly);
		}
		else
		{
			if (size.x != 0) ImGui::PushItemWidth(size.x);
			ImGui::InputText(label, utf8_text.data(), utf8_text.size() + 1, flags | ImGuiInputTextFlags_ReadOnly);
			if (size.x != 0) ImGui::PopItemWidth();
		}

		if (internalFocus)
		{
			auto& state = ImGui::GetCurrentContext()->InputTextState;
			auto new_range = std::minmax(state.Stb.select_start, state.Stb.select_end);
			if (selection.StartCaretPosition != new_range.first || selection.EndCaretPosition != new_range.second)
			{
				CoreTextRange range;
				range.StartCaretPosition = new_range.first;
				range.EndCaretPosition = new_range.second;
				SetSelectionAndNotify(range);
			}
		}

		if (ImGui::IsItemActivated())
		{
			SetInternalFocus();
		}
		if (ImGui::IsItemDeactivated())
		{
			RemoveInternalFocus();
		}

		auto pos_after = ImGui::GetCursorPos();
		if (!internalFocus && utf8_text.empty() && !ghost_text.empty())
		{
			//ImGui::SetCursorPos(pos_before);
			//ImGui::TextDisabled(ghost_text.c_str());
			pos_before.x += ImGui::GetStyle().ItemInnerSpacing.x;
			ImGui::GetWindowDrawList()->AddText(pos_before, ImColor(v4f(0.5f, 0.5f, 0.5f, 1.0f)), ghost_text.c_str());
			//ImGui::SetCursorPos(pos_after);
		}

	}

	ImGuiUWPCoreTextEditContext()
	{
		auto ctsm = CoreTextServicesManager::GetForCurrentView();
		editContext = ctsm.CreateEditContext();
		inputPane = InputPane::GetForCurrentView();
		editContext.InputPaneDisplayPolicy(CoreTextInputPaneDisplayPolicy::Manual);
		editContext.InputScope(CoreTextInputScope::Text);

		selection.StartCaretPosition = selection.EndCaretPosition = 0;

		editContext.TextRequested([this](CoreTextEditContext ctx, CoreTextTextRequestedEventArgs args)
		{
			CoreTextTextRequest request = args.Request();
			auto range = request.Range();
			request.Text(text.substr(range.StartCaretPosition, std::min(range.EndCaretPosition, (s32)text.size()) - range.StartCaretPosition));
		});

		editContext.SelectionRequested([this](CoreTextEditContext ctx, CoreTextSelectionRequestedEventArgs args)
		{
			CoreTextSelectionRequest request = args.Request();
			request.Selection(selection);
		});

		editContext.TextUpdating([this](CoreTextEditContext ctx, CoreTextTextUpdatingEventArgs args)
		{
			CoreTextRange range = args.Range();

			auto newText = args.Text();
			CoreTextRange newSelection = args.NewSelection();

			// Modify the internal text store.
			text = text.substr(0, range.StartCaretPosition) +
				newText +
				text.substr(std::min((s32)text.size(), range.EndCaretPosition));

			// You can set the proper font or direction for the updated text based on the language by checking
			// args.InputLanguage.  We will not do that in this sample.

			// Modify the current selection.
			newSelection.EndCaretPosition = newSelection.StartCaretPosition;

			// Update the selection of the edit context. There is no need to notify the system
			// because the system itself changed the selection.
			SetSelection(newSelection);
			text_need_set = true;
		});

		editContext.SelectionUpdating([this](CoreTextEditContext ctx, CoreTextSelectionUpdatingEventArgs args)
		{
			SetSelection(args.Selection());
		});

		editContext.FormatUpdating([this](CoreTextEditContext ctx, CoreTextFormatUpdatingEventArgs args)
		{
		});

		editContext.LayoutRequested([this](CoreTextEditContext ctx, CoreTextLayoutRequestedEventArgs args)
		{
		});

		editContext.CompositionStarted([this](CoreTextEditContext ctx, CoreTextCompositionStartedEventArgs args)
		{
		});

		editContext.CompositionCompleted([this](CoreTextEditContext ctx, CoreTextCompositionCompletedEventArgs args)
		{
		});


		auto window = App::GetApp()->GetWindow();
		revoker = window.KeyDown(winrt::auto_revoke, [this](winrt::Windows::UI::Core::CoreWindow const& window, winrt::Windows::UI::Core::KeyEventArgs args)
		{
			// Do not process keyboard input if the custom edit control does not
			// have focus.
			if (!internalFocus)
			{
				return;
			}

			// This holds the range we intend to operate on, or which we intend
			// to become the new selection. Start with the current selection.
			CoreTextRange range = selection;

			// For the purpose of this sample, we will support only the left and right
			// arrow keys and the backspace key. A more complete text edit control
			// would also handle keys like Home, End, and Delete, as well as
			// hotkeys like Ctrl+V to paste.
			//
			// Note that this sample does not properly handle surrogate pairs
			// nor does it handle grapheme clusters.

			switch (args.VirtualKey())
			{
				// Backspace
			case winrt::Windows::System::VirtualKey::Back:
				// If there is a selection, then delete the selection.
				if (HasSelection())
				{
					// Set the text in the selection to nothing.
					ReplaceText(range, L"");
				}
				else
				{
					// Delete the character to the left of the caret, if one exists,
					// by creating a range that encloses the character to the left
					// of the caret, and setting the contents of that range to nothing.
					range.StartCaretPosition = std::max(0, range.StartCaretPosition - 1);
					ReplaceText(range, L"");
				}
				break;
				// Enter
			case winrt::Windows::System::VirtualKey::Enter:
				ReplaceText(range, L"\n");
				break;
				// Left arrow
			case winrt::Windows::System::VirtualKey::Left:
				// If the shift key is down, then adjust the size of the selection.
				if (window.GetKeyState(winrt::Windows::System::VirtualKey::Shift) == winrt::Windows::UI::Core::CoreVirtualKeyStates::Down)
				{
					// If this is the start of a selection, then remember which edge we are adjusting.
					if (!HasSelection())
					{
						extendingLeft = true;
					}

					// Adjust the selection and notify CoreTextEditContext.
					AdjustSelectionEndpoint(-1);
				}
				else
				{
					// The shift key is not down. If there was a selection, then snap the
					// caret at the left edge of the selection.
					if (HasSelection())
					{
						range.EndCaretPosition = range.StartCaretPosition;
						selection_need_set = true;
						SetSelectionAndNotify(range);
					}
					else
					{
						// There was no selection. Move the caret left one code unit if possible.
						range.StartCaretPosition = std::max(0, range.StartCaretPosition - 1);
						range.EndCaretPosition = range.StartCaretPosition;
						selection_need_set = true;
						SetSelectionAndNotify(range);
					}
				}
				break;

				// Right arrow
			case winrt::Windows::System::VirtualKey::Right:
				// If the shift key is down, then adjust the size of the selection.
				if (window.GetKeyState(winrt::Windows::System::VirtualKey::Shift) == winrt::Windows::UI::Core::CoreVirtualKeyStates::Down)
				{
					// If this is the start of a selection, then remember which edge we are adjusting.
					if (!HasSelection())
					{
						extendingLeft = false;
					}

					// Adjust the selection and notify CoreTextEditContext.
					AdjustSelectionEndpoint(+1);
				}
				else
				{
					// The shift key is not down. If there was a selection, then snap the
					// caret at the right edge of the selection.
					if (HasSelection())
					{
						range.StartCaretPosition = range.EndCaretPosition;
						selection_need_set = true;
						SetSelectionAndNotify(range);
					}
					else
					{
						// There was no selection. Move the caret right one code unit if possible.
						range.StartCaretPosition = std::min((s32)text.size(), range.StartCaretPosition + 1);
						range.EndCaretPosition = range.StartCaretPosition;
						selection_need_set = true;
						SetSelectionAndNotify(range);
					}
				}
				break;
			}
		});
	}

	ImGuiUWPCoreTextEditContext(const ImGuiUWPCoreTextEditContext&) = delete;
	ImGuiUWPCoreTextEditContext(ImGuiUWPCoreTextEditContext&&) = default;
	ImGuiUWPCoreTextEditContext& operator=(const ImGuiUWPCoreTextEditContext&) = delete;
	ImGuiUWPCoreTextEditContext& operator=(ImGuiUWPCoreTextEditContext&&) = default;

	explicit ImGuiUWPCoreTextEditContext(const std::wstring& txt) : ImGuiUWPCoreTextEditContext()
	{
		ghost_text = to_utf8(txt);
		/*text = txt;
		utf8_text = to_utf8(text);*/
	}

	void AdjustSelectionEndpoint(int direction)
	{
		CoreTextRange range = selection;
		if (extendingLeft)
		{
			range.StartCaretPosition = std::max(0, range.StartCaretPosition + direction);
		}
		else
		{
			range.EndCaretPosition = std::min((s32)text.size(), range.EndCaretPosition + direction);
		}
		
		selection_need_set = true;
		SetSelectionAndNotify(range);
	}

	bool HasSelection()
	{
		return selection.StartCaretPosition != selection.EndCaretPosition;
	}

	void SetInternalFocus()
	{
		if (!internalFocus)
		{
			// Update internal notion of focus.
			internalFocus = true;

			// Notify the CoreTextEditContext that the edit context has focus,
			// so it should start processing text input.
			editContext.NotifyFocusEnter();
		}

		// Ask the software keyboard to show.  The system will ultimately decide if it will show.
		// For example, it will not show if there is a keyboard attached.
		inputPane.TryShow();

	}

	void RemoveInternalFocusWorker()
	{
		//Update the internal notion of focus
		internalFocus = false;
		// Ask the software keyboard to dismiss.
		inputPane.TryHide();
	}

	void RemoveInternalFocus()
	{
		if (internalFocus)
		{
			//Notify the system that this edit context is no longer in focus
			editContext.NotifyFocusLeave();
			RemoveInternalFocusWorker();
		}
	}

	void SetSelection(CoreTextRange newSelection)
	{
		selection = newSelection;
		//state.Stb.select_start = newSelection.StartCaretPosition;
		//state.Stb.select_end = newSelection.EndCaretPosition;
		selection_need_set = true;
	}

	void ReplaceEntireText(std::wstring newtext)
	{
		CoreTextRange range;
		range.StartCaretPosition = 0;
		range.EndCaretPosition = text.size();
		ReplaceText(range, newtext);
	}

	void ReplaceText(CoreTextRange modifiedRange, std::wstring newtext)
	{
		// Modify the internal text store.
		auto old_text = text;
		text = text.substr(0, modifiedRange.StartCaretPosition) +
			newtext +
			text.substr(modifiedRange.EndCaretPosition);

		if (text == old_text) return;

		// Move the caret to the end of the replacement text.
		selection.StartCaretPosition = modifiedRange.StartCaretPosition + newtext.size();
		selection.EndCaretPosition = selection.StartCaretPosition;

		// Update the selection of the edit context.  There is no need to notify the system
		// of the selection change because we are going to call NotifyTextChanged soon.
		SetSelection(selection);
		text_need_set = true;
		// Let the CoreTextEditContext know what changed.
		editContext.NotifyTextChanged(modifiedRange, text.size(), selection);
	}

	void SetSelectionAndNotify(CoreTextRange newSelection)
	{
		selection = newSelection;
		editContext.NotifySelectionChanged(selection);
	}

};