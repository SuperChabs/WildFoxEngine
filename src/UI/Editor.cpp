#include "Editor.h"

#include <fstream>
#include <utility>

#include "core/logging/Logger.h"
#include "FileSelector.h"
#include "imgui_internal.h"
#include "Toastr.h"

#define SHORTCUT "Ctrl-"

[[maybe_unused]] static std::filesystem::path GetDocumentDirectory() {
    const char* home = std::getenv("HOME");

    if (home) {
        auto path = std::filesystem::path(home) / "Documents";

        if (std::filesystem::exists(path)) {
            return std::filesystem::path(home) / "Documents";
        }
    }

    return std::filesystem::current_path();
}

CodeEditor::CodeEditor() {
    // setup text editor
    originalText = "";
    editor.SetText("");
    version = editor.GetUndoIndex();
    filename = "untiled.as";

    SetLanguageByName("AngelScript");

    //editor.RegisterLanguage();
    editor.SetShowMiniMapEnabled(true);
    editor.SetFocus();

    // configure line breaker
    lineBreakConfig.lb2 = false;
    lineBreakConfig.lb3 = false;
    lineBreakConfig.lb4 = false;
    lineBreakConfig.lb5 = false;
    lineBreakConfig.lb6 = false;
    lineBreakConfig.lb13 = false;
    lineBreakConfig.lb14 = false;
    lineBreakConfig.lb15d = false;
    lineBreakConfig.lb19a = false;
    lineBreakConfig.lb21a = false;
    lineBreakConfig.lb21b = false;
    lineBreakConfig.lb26 = false;
    lineBreakConfig.lb27 = false;
    lineBreakConfig.lb28a = false;
    lineBreakConfig.lb29 = false;
    lineBreakConfig.lb30 = false;
    lineBreakConfig.lb30a = false;
    lineBreakConfig.lb30b = false;
}

void CodeEditor::Render() {
    if (!visible) return;

    // support language server bridge
    if (lsp.IsRunning())
        lsp.Update(filename);

    // create the outer window (a regular dockable engine window)
    ImGui::SetNextWindowSize(ImVec2(800.0f, 600.0f), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Script Editor", &visible, ImGuiWindowFlags_MenuBar)) {
        ImGui::End();
        return;
    }

    // add a menu bar
    RenderMenuBar();

    // determine text editor size
    const auto area = ImGui::GetContentRegionAvail();
    const auto &style = ImGui::GetStyle();
    const auto statusBarHeight = ImGui::GetFrameHeight() + 2.0f * style.WindowPadding.y;
    const auto editorSize = ImVec2(0.0f, area.y - style.ItemSpacing.y - statusBarHeight);

    // render the text editor widget
    ImGui::PushFont(nullptr, fontSize);
    editor.Render("TextEditor", editorSize);
    ImGui::PopFont();

    // render a statusBar
    ImGui::Spacing();
    RenderStatusBar();

    if (state == State::diff)
        RenderDiff();
    else if (state == State::openFile)
        RenderFileOpen();
    else if (state == State::saveFileAs)
        RenderSaveAs();
    else if (state == State::confirmClose)
        RenderConfirmClose();
    else if (state == State::confirmQuit)
        RenderConfirmQuit();
    else if (state == State::confirmError)
        RenderConfirmError();
    else if (state == State::addSquiggle)
        RenderAddSquiggle();
    else if (state == State::clearSquiggles)
        RenderClearSquiggle();

    ImGui::End();

    // render debug information (if required)
    if (showDebugInformation)
        RenderDebugInformation();

    // render notifications
    const auto mainWindowSize = ImGui::GetMainViewport()->Size;
    const auto mainWindowPos = ImGui::GetMainViewport()->Pos;
    const float offset = statusBarHeight + style.ItemSpacing.y * 2.0f;

    Toastr::Instance().SetTextWidth(30);

    Toastr::Instance().Render(
        ImVec2(
            mainWindowPos.x + mainWindowSize.x - ImGui::GetStyle().ItemSpacing.x,
            mainWindowPos.y + mainWindowSize.y - ImGui::GetStyle().ItemSpacing.y - offset),
        Toastr::AnchorType::bottomRight);

    // show Dear ImGui metrics (if required)
    if (showDebugWindow)
        ImGui::ShowMetricsWindow();
}

void CodeEditor::NewFile() {
    if (IsDirty()) {
        ShowConfirmClose([this] {
            originalText.clear();
            editor.SetText("");
            version = editor.GetUndoIndex();
            filename = "untiled";
            SetLanguage(nullptr);
        });
    }
    else {
        if (lsp.IsRunning())
            lsp.CloseDocument(filename);

        originalText.clear();
        editor.SetText("");
        version = editor.GetUndoIndex();
        filename = "untiled";
        SetLanguage(nullptr);
    }
}

void CodeEditor::OpenFile() {
    if (IsDirty())
        ShowConfirmClose([this] { ShowFileOpen(); });
    else ShowFileOpen();
}

void CodeEditor::OpenFile(const std::filesystem::path &path) {
    try {
        if (lsp.IsRunning())
            lsp.CloseDocument(filename);

        std::ifstream stream(path.c_str());
        std::string text;

        stream.seekg(0, std::ios::end);
        text.reserve(stream.tellg());
        stream.seekg(0, std::ios::beg);

        text.assign((std::istreambuf_iterator(stream)), std::istreambuf_iterator<char>());
        stream.close();

        originalText = text;
        editor.SetText(text);
        version = editor.GetUndoIndex();
        filename = path;

        SetLanguage(TextEditor::LookupLanguageByExtension(filename.extension().string()));
    }
    catch (std::exception &e) {
        ShowError(e.what());
        Logger::Log(LogLevel::ERROR, e.what());
    }
}

/*
 * @brief Save current text file.
 * Writes the editor contents to the file specified by "filename".
 * @throws std::exception File operation error.
 */
void CodeEditor::SaveFile() {
    try {
    	// @brief Remove trailing whitespaces before saving.
        editor.StripTrailingWhitespaces();

        std::ofstream stream(filename.c_str());
        stream << editor.GetText();
        stream.close();

    	// @brief Store the current undo index as the saved version.
        version = editor.GetUndoIndex();
    }
    catch (std::exception &e) {
        ShowError(e.what());
        Logger::Log(LogLevel::ERROR, e.what());
    }
}

void CodeEditor::TryToQuit() {
}

void CodeEditor::RenderMenuBar() {
    // create menubar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New", SHORTCUT "N")) NewFile();
            if (ImGui::MenuItem("Open", SHORTCUT "O")) OpenFile();

            ImGui::Separator();
            if (ImGui::MenuItem("Save", SHORTCUT "S", nullptr, IsSavable())) SaveFile();
            if (ImGui::MenuItem("Save As...")) ShowSaveFileAs();

            ImGui::Separator();
            if (ImGui::MenuItem("Quit", SHORTCUT "Q")) TryToQuit();

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", SHORTCUT "Z", nullptr, editor.CanUndo())) editor.Undo();
            if (ImGui::MenuItem("Redo", SHORTCUT "Y", nullptr, editor.CanRedo())) editor.Redo();

            ImGui::Separator();
            if (ImGui::MenuItem("Cut", " " SHORTCUT "X", nullptr, editor.AnyCursorHasSelection())) editor.Cut();
            if (ImGui::MenuItem("Copy", " " SHORTCUT "C", nullptr, editor.AnyCursorHasSelection())) editor.Copy();
            if (ImGui::MenuItem("Paste", " " SHORTCUT "V", nullptr, ImGui::GetClipboardText() != nullptr)) editor.Paste();

            ImGui::Separator();
            bool flag;
            flag = editor.IsInsertSpacesOnTabs();
            if (ImGui::MenuItem("Insert Spaces on Tabs", nullptr, &flag)) editor.SetInsertSpacesOnTabs(flag);

            if (ImGui::MenuItem("Tabs To Spaces")) editor.TabsToSpaces();
            if (ImGui::MenuItem("Spaces To Tabs")) editor.SpacesToTabs();
            if (ImGui::MenuItem("Strip Trailing WhiteSPaces")) editor.StripTrailingWhitespaces();

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Selection")) {
            if (ImGui::MenuItem("Select All", " " SHORTCUT "A", nullptr, !editor.IsEmpty())) { editor.SelectAll(); }

            ImGui::Separator();
            if (ImGui::MenuItem("Indent Line(s)", " " SHORTCUT "]", nullptr, !editor.IsEmpty())) { editor.IndentLines(); }
            if (ImGui::MenuItem("Deindent Line(s)", " " SHORTCUT "[", nullptr, !editor.IsEmpty())) { editor.DeindentLines(); }
            if (ImGui::MenuItem("Move Line(s) Up", nullptr, nullptr, !editor.IsEmpty())) { editor.MoveUpLines(); }
            if (ImGui::MenuItem("Move Line(s) Down", nullptr, nullptr, !editor.IsEmpty())) { editor.MoveDownLines(); }
            if (ImGui::MenuItem("Toggle Comments", " " SHORTCUT "/", nullptr, editor.HasLanguage())) { editor.ToggleComments(); }

            ImGui::Separator();
            if (ImGui::MenuItem("To Uppercase", nullptr, nullptr, editor.AnyCursorHasSelection())) { editor.SelectionToUpperCase(); }
            if (ImGui::MenuItem("To Lowercase", nullptr, nullptr, editor.AnyCursorHasSelection())) { editor.SelectionToLowerCase(); }

            ImGui::Separator();
            if (ImGui::MenuItem("Add Next Occurrence", " " SHORTCUT "D", nullptr, editor.CurrentCursorHasSelection())) { editor.AddNextOccurrence(); }
            if (ImGui::MenuItem("Select All Occurrences", "^" SHORTCUT "D", nullptr, editor.CurrentCursorHasSelection())) { editor.SelectAllOccurrences(); }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
			if (ImGui::MenuItem("Reset Zoom", " " SHORTCUT "0")) { resetFontSize(); }
			if (ImGui::MenuItem("Zoom In", " " SHORTCUT "+")) { increaseFontSize(); }
			if (ImGui::MenuItem("Zoom Out", " " SHORTCUT "-")) { decreaseFontSize(); }

			ImGui::Separator();
			if (ImGui::MenuItem("Show Diff", " " SHORTCUT "I")) { ShowDiff(); }

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Find", editor.IsFindReplaceEnabled())) {
			if (ImGui::MenuItem("Find", " " SHORTCUT "F")) { editor.OpenFindReplaceWindow(); }
			if (ImGui::MenuItem("Find Next", " " SHORTCUT "G", nullptr, editor.HasFindString())) { editor.FindNext(); }
			if (ImGui::MenuItem("Find All", "^" SHORTCUT "G", nullptr, editor.HasFindString())) { editor.FindAll(); }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Options")) {
			if (ImGui::BeginMenu("Tab Size")) {
				if (ImGui::MenuItem("1")) { editor.SetTabSize(1); }
				if (ImGui::MenuItem("2")) { editor.SetTabSize(2); }
				if (ImGui::MenuItem("4")) { editor.SetTabSize(4); }
				if (ImGui::MenuItem("8")) { editor.SetTabSize(8); }
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Line Spacing")) {
				if (ImGui::MenuItem("1.0")) { editor.SetLineSpacing(1.0f); }
				if (ImGui::MenuItem("1.25")) { editor.SetLineSpacing(1.25f); }
				if (ImGui::MenuItem("1.5")) { editor.SetLineSpacing(1.5f); }
				if (ImGui::MenuItem("1.75")) { editor.SetLineSpacing(1.75f); }
				if (ImGui::MenuItem("2.0")) { editor.SetLineSpacing(2.0f); }
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("MiniMap Columns")) {
				int width = static_cast<int>(editor.GetMiniMapColumns());
				if (ImGui::SliderInt("##miniMapColumns", &width, 0, 200)) { editor.SetMiniMapColumns(static_cast<size_t>(width)); }
				ImGui::EndMenu();
			}

			ImGui::Separator();

			bool flag;
			flag = editor.IsReadOnlyEnabled(); if (ImGui::MenuItem("Read Only", nullptr, &flag)) { editor.SetReadOnlyEnabled(flag); };
			flag = editor.IsCaretsVisible(); if (ImGui::MenuItem("Carets Visible", nullptr, &flag)) { editor.SetCaretsVisible(flag); };
			flag = editor.IsOverwriteEnabled(); if (ImGui::MenuItem("Overwrite", nullptr, &flag)) { editor.SetOverwriteEnabled(flag); };
			flag = editor.IsWordWrapEnabled(); if (ImGui::MenuItem("Word Wrap", nullptr, &flag)) { editor.SetWordWrapEnabled(flag); };
			flag = editor.IsLineFoldingEnabled(); if (ImGui::MenuItem("Line Folding", nullptr, &flag)) { editor.SetLineFoldingEnabled(flag); };
			flag = editor.IsAutoIndentEnabled(); if (ImGui::MenuItem("Auto Indent", nullptr, &flag)) { editor.SetAutoIndentEnabled(flag); };
			flag = editor.IsInsertSpacesOnTabs(); if (ImGui::MenuItem("Insert Spaces on Tabs", nullptr, &flag)) { editor.SetInsertSpacesOnTabs(flag); };
			flag = editor.IsShowWhitespacesEnabled(); if (ImGui::MenuItem("Show Whitespaces", nullptr, &flag)) { editor.SetShowWhitespacesEnabled(flag); };
			flag = editor.IsShowSpacesEnabled(); if (ImGui::MenuItem("Show Spaces", nullptr, &flag)) { editor.SetShowSpacesEnabled(flag); };
			flag = editor.IsShowTabsEnabled(); if (ImGui::MenuItem("Show Tabs", nullptr, &flag)) { editor.SetShowTabsEnabled(flag); };
			flag = editor.IsShortTabsEnabled(); if (ImGui::MenuItem("Show Short Tabs", nullptr, &flag)) { editor.SetShortTabsEnabled(flag); };
			flag = editor.IsShowLineNumbersEnabled(); if (ImGui::MenuItem("Show Line Numbers", nullptr, &flag)) { editor.SetShowLineNumbersEnabled(flag); };
			flag = editor.IsShowCurrentLineHighlightEnabled(); if (ImGui::MenuItem("Show Current Line Highlight", nullptr, &flag)) { editor.SetShowCurrentLineHighlightEnabled(flag); };
			flag = editor.IsShowingMatchingBrackets(); if (ImGui::MenuItem("Show Matching Brackets", nullptr, &flag)) { editor.SetShowMatchingBrackets(flag); };
			flag = editor.IsCompletingPairedGlyphs(); if (ImGui::MenuItem("Complete Matching Glyphs", nullptr, &flag)) { editor.SetCompletePairedGlyphs(flag); };
			flag = editor.IsShowMiniMapEnabled(); if (ImGui::MenuItem("Show Mini Map", nullptr, &flag)) { editor.SetShowMiniMapEnabled(flag); };
			flag = editor.IsShowScrollbarMiniMapEnabled(); if (ImGui::MenuItem("Show Scrollbar Mini Map", nullptr, &flag)) { editor.SetShowScrollbarMiniMapEnabled(flag); };
			flag = editor.IsShowPanScrollIndicatorEnabled(); if (ImGui::MenuItem("Show Pan/Scroll Indicator", nullptr, &flag)) { editor.SetShowPanScrollIndicatorEnabled(flag); };
			flag = editor.IsSmoothScrollingEnabled(); if (ImGui::MenuItem("Smooth Scrolling", nullptr, &flag)) { editor.SetSmoothScrollingEnabled(flag); };
			flag = editor.IsMiddleMousePanMode(); if (ImGui::MenuItem("Middle Mouse Pan Mode", nullptr, &flag)) { if (flag) editor.SetMiddleMousePanMode(); else editor.SetMiddleMouseScrollMode(); };
			flag = editor.IsFindReplaceEnabled(); if (ImGui::MenuItem("Find/Replace Enabled", nullptr, &flag)) { editor.SetFindReplaceEnabled(flag); };

			ImGui::Separator();

			if (ImGui::BeginMenu("Left Margin")) {
				int margin = static_cast<int>(editor.GetLeftMargin());
				if (ImGui::SliderInt("##margin", &margin, 0, 4)) { editor.SetLeftMargin(static_cast<size_t>(margin)); }
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Decoration Left Margin")) {
				int margin = static_cast<int>(editor.GetDecorationLeftMargin());
				if (ImGui::SliderInt("##margin", &margin, 0, 4)) { editor.SetDecorationLeftMargin(static_cast<size_t>(margin)); }
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Text Left Margin")) {
				int margin = static_cast<int>(editor.GetTextLeftMargin());
				if (ImGui::SliderInt("##margin", &margin, 0, 4)) { editor.SetTextLeftMargin(static_cast<size_t>(margin)); }
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

        ImGui::EndMenuBar();
    }

	// handle keyboard shortcuts
	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_N)) { NewFile(); }
	else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O)) { OpenFile(); }
	else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S)) { if (filename == "untitled") ShowSaveFileAs(); else SaveFile(); }
	else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_I)) { ShowDiff(); }
	else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_0)) { resetFontSize(); }
	else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Equal)) { increaseFontSize(); }
	else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Minus)) { decreaseFontSize(); }
	else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Alt | ImGuiKey_I, ImGuiInputFlags_RouteAlways)) { showDebugWindow = !showDebugWindow; }
}

void CodeEditor::RenderStatusBar() {
	// create statusbar window
	ImGui::PushStyleColor(ImGuiCol_ChildBg, editor.GetPalette().get(TextEditor::Color::background));
	ImGui::BeginChild("Status Bar", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders);
	ImGui::SetNextItemWidth(120.0f);

	auto currentLanguage = editor.GetLanguageName();

	auto languageSelector = [this, currentLanguage](const std::string &language) {
		bool selected = (language == currentLanguage);

		if (ImGui::Selectable(language.c_str(), selected))
			SetLanguageByName(language);

		if (selected)
			ImGui::SetItemDefaultFocus();
	};

	// allow user to select language for colorizing
	if (ImGui::BeginCombo("##LanguageSelector", currentLanguage.c_str())) {
		languageSelector("None");

		for (const auto& language : TextEditor::GetRegisteredLanguages())
			languageSelector(language);

		ImGui::EndCombo();
	}

	// support show docpos at mouse
	std::string docPosStatus;

	if (showDocPosAtMouse) {
		const auto mousePos = ImGui::GetMousePos();

		if (editor.IsMousePosOverTextArea(mousePos))
			if (editor.IsMousePosOverGlyph(mousePos) || ImGui::IsKeyDown(ImGuiMod_Shift)) {
				auto docPos = editor.GetDocPosAtMousePos(mousePos);

				docPosStatus = std::format(
					"MousePos: {}, {}. DocPos:: {}, {}  ",
					mousePos.x, mousePos.y,
					docPos.line, docPos.index
				);
			}
	}

	// support show word at mouse
	std::string atMouseStatus;

	if (showWordAtMouse) {
		auto word = editor.GetWordAtMousePos(ImGui::GetMousePos());

		if (word.size()) atMouseStatus = std::format("Word: {}", word);
	}
	else if (showQualifiedIdentifierAtMouse) {
		const auto name = editor.GetQualifiedIdentifier(editor.GetDocPosAtMousePos(ImGui::GetMousePos()));

		if (name.size()) atMouseStatus = std::format("Qualified name: {}", name);
	}

	// determine status message
	auto tabSize = editor.GetTabSize();
	auto cursorPos = editor.DocPos2VisPos(editor.GetCurrentCursorPosition());
	auto fn = std::filesystem::path(filename).filename().string();

	auto status = std::format(
		"{}{}Ln {}, Col {} Tab Size: {} File: {}",
		docPosStatus,
		atMouseStatus,
		cursorPos.row + 1,
		cursorPos.column + 1,
		tabSize,
		fn
	);

	// determine horizontal gap so the rest is right aligned
	ImGui::SameLine(0.0f, 0.0f);
	auto availableSpace = ImGui::GetContentRegionAvail().x;
	auto messageWidth = ImGui::CalcTextSize(status.c_str()).x;
	auto dirtyWidth = ImGui::CalcTextSize("#"). x * 3.0f;
	ImGui::SameLine(0.0f, availableSpace - messageWidth - dirtyWidth);

	// render status text
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(status.c_str());

	// render "text dirty" indicator
	ImGui::SameLine(0.0f, ImGui::CalcTextSize("#").x * 1.0f);
	auto drawList = ImGui::GetWindowDrawList();
	const auto pos = ImGui::GetCursorScreenPos();
	const auto offset = ImGui::GetFrameHeight() * 0.5f;
	const auto radius = offset * 0.6f;
	const auto color = IsDirty() ? IM_COL32(164, 0, 0, 255) : IM_COL32(164, 164, 164, 255);
	drawList->AddCircleFilled(ImVec2(pos.x + offset, pos.y + offset), radius, color);

	ImGui::EndChild();
	ImGui::PopStyleColor();
}

void CodeEditor::ShowDiff() {
	diff.SetText(originalText, editor.GetText());
	state = State::diff;
	popup = true;
}

void CodeEditor::ShowFileOpen() {
	if (FileSelector::Instance().OpenFile())
		state = State::openFile;
}

void CodeEditor::ShowSaveFileAs() {
	if (FileSelector::Instance().SaveAs())
		state = State::saveFileAs;
}

void CodeEditor::ShowConfirmClose(std::function<void()> callback) {
	onConfirmClose = std::move(callback);
	state = State::confirmClose;
	popup = true;
}

void CodeEditor::ShowConfirmQuit() {
	state = State::confirmQuit;
	popup = true;
}

void CodeEditor::ShowError(const std::string &message) {
	errorMessage = message;
	state = State::confirmError;
	popup = true;
}

void CodeEditor::ShowAddSquiggle() {
	state = State::addSquiggle;
	popup = true;
}

void CodeEditor::ShowClearSquiggles() {
	state = State::clearSquiggles;
	popup = true;
}

void CodeEditor::RenderDiff() {
	const auto viewport = ImGui::GetMainViewport();

	if (popup) {
		ImGui::OpenPopup("Changes since Opening File##diff");
		ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		diff.SetFocus();
		popup = false;
	}

	if (ImGui::BeginPopupModal("Changes since Opening File##diff")) {
		diff.Render("diff", {viewport->Size.x * 0.8f, viewport->Size.y * 0.8f}, ImGuiChildFlags_Borders);

		ImGui::Separator();
		static constexpr float buttonWidth = 80.0f;
		const auto buttonOffset = ImGui::GetContentRegionAvail().x - buttonWidth;
		bool sizeBySide = diff.GetSideBySideMode();
		bool wordWrap = diff.IsWordWrapEnabled();

		if (ImGui::Checkbox("Side-by-Side", &sizeBySide))
			diff.SetSideBySideMode(sizeBySide);

		ImGui::SameLine();

		if (ImGui::Checkbox("Word Wrap", &wordWrap))
			diff.SetWordWrapEnabled(wordWrap);

		ImGui::SameLine();
		ImGui::Indent(buttonOffset);

		if (ImGui::Button("OK", ImVec2(buttonWidth, 0.0f)) || ImGui::Shortcut(ImGuiKey_Escape)) {
			ImGui::CloseCurrentPopup();
			state = State::edit;
		}

		ImGui::EndPopup();
	}
}

void CodeEditor::RenderFileOpen() {
	// handle file open dialog
	auto &dialog = FileSelector::Instance();

	if (dialog.Render()) {
		// open selected file if required
		if (dialog.SelectedOpenFile())
			OpenFile(dialog.GetSelectedPath());

		state = State::edit;
	}
}

void CodeEditor::RenderSaveAs() {
	// handle saves dialog
	auto &dialog = FileSelector::Instance();

	if (!dialog.Render()) return;

	// save selected file if required
	if (dialog.SelectedSaveAs()) {
		filename = dialog.GetSelectedPath();
		SaveFile();
		state = State::edit;
	}

}

void CodeEditor::RenderConfirmClose() {
	if (popup) {
		ImGui::OpenPopup("Confirm Close");
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		popup = false;
	}

	if (!ImGui::BeginPopupModal("Confirm Close", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) return;

	ImGui::Text("This file has changed!\nDo you really want to delete it?\n\n");
	ImGui::Separator();

	static constexpr float buttonWidth = 80.0f;
	ImGui::Indent(ImGui::GetContentRegionAvail().x - buttonWidth * 2.0f - ImGui::GetStyle().ItemSpacing.x);

	if (ImGui::Button("OK", ImVec2(buttonWidth, 0.0f))) {
		state = State::edit;
		onConfirmClose();
		ImGui::CloseCurrentPopup();
	}

	ImGui::SameLine();

	if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f)) || ImGui::Shortcut(ImGuiKey_Escape)) {
		state = State::edit;
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndPopup();
}

void CodeEditor::RenderConfirmQuit() {
	if (popup) {
		ImGui::OpenPopup("Quit Editor?");
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		popup = false;
	}

	if (!ImGui::BeginPopupModal("Quit Editor?")) return;

	ImGui::Text("Your text has changed and is not saved!\nDo your really want to quit?\n\n");
	ImGui::Separator();

	static constexpr float buttonWidth = 80.0f;
	ImGui::Indent(ImGui::GetContentRegionAvail().x - buttonWidth * 2.0f - ImGui::GetStyle().ItemSpacing.x);

	if (ImGui::Button("OK", ImVec2(buttonWidth, 0.0f))) {
		done = true;
		state = State::edit;
		ImGui::CloseCurrentPopup();
	}

	ImGui::SameLine();

	if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f)) || ImGui::Shortcut(ImGuiKey_Escape)) {
		state = State::edit;
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndPopup();
}

void CodeEditor::RenderConfirmError() {
	if (popup) {
		ImGui::OpenPopup("Error");
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		popup = false;
	}

	if (!ImGui::BeginPopupModal("Error")) return;

	ImGui::Text("%s\n", errorMessage.c_str());
	ImGui::Separator();

	static constexpr float buttonWidth = 80.0f;
	ImGui::Indent(ImGui::GetContentRegionAvail().x - buttonWidth);

	if (ImGui::Button("OK", ImVec2(buttonWidth, 0.0f)) || ImGui::Shortcut(ImGuiKey_Escape)) {
		errorMessage.clear();
		state = State::edit;
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndPopup();
}

void CodeEditor::RenderAddSquiggle() {
	if (popup) {
		ImGui::OpenPopup("Add Squiggle");
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		popup = false;
	}

	if (!ImGui::BeginPopupModal("Add Squiggle", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) return;

	auto type = static_cast<int>(squiggleType);
	if (ImGui::SliderInt("Type", &type, 1, 5)) squiggleType = static_cast<size_t>(type);
	ImGui::ColorEdit4("Color", (float*)&squiggleColor);
	ImGui::InputText("Tool Tip", squiggleToolTip, sizeof(squiggleToolTip));

	static constexpr const char* stylesNames[] = {"Wave", "Background"};
	static constexpr size_t stylesCount = std::size(stylesNames);

	if (ImGui::BeginCombo("Style", stylesNames[squiggleType])) {
		for (size_t i = 0; i < stylesCount; ++i) {
			const auto currentStyle = static_cast<TextEditor::SquiggleStyle>(i);
			const auto isSelected = squiggleType == static_cast<size_t>(currentStyle);

			if (ImGui::Selectable(stylesNames[i], isSelected))
				squiggleType = static_cast<size_t>(currentStyle);

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	ImGui::Separator();

	static constexpr float buttonWidth = 80.0f;
	ImGui::Indent(ImGui::GetContentRegionAvail().x - buttonWidth * 2.0f - 5.0f);

	if (ImGui::Button("OK", ImVec2(buttonWidth, 0.0f))) {
		const ImU32 color = squiggleColor;

		for (size_t i = 0; i < editor.GetNumberOfCursors(); ++i) {
			auto selection = editor.GetCursorSelection(i);

			if (selection.start != selection.end)
				editor.AddSquiggle(selection.start, selection.end, squiggleType, color,
					squiggleToolTip, squiggleStyle);
		}

		state = State::edit;
		ImGui::CloseCurrentPopup();
	}

	ImGui::SameLine(0.0f, 5.0f);

	if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f)) || ImGui::Shortcut(ImGuiKey_Escape)) {
		state = State::edit;
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndPopup();
}

void CodeEditor::RenderClearSquiggle() {
	if (popup) {
		ImGui::OpenPopup("Clear Squiggle");
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		popup = false;
	}

	if (!ImGui::BeginPopupModal("Clear Squiggle", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) return;

	auto type = static_cast<int>(squiggleType);
	if (ImGui::SliderInt("Type", &type, 1, 5)) squiggleType = static_cast<size_t>(type);
	ImGui::Separator();

	static constexpr float buttonWidth = 80.0f;
	ImGui::Indent(ImGui::GetContentRegionAvail().x - buttonWidth * 2.0f - 5.0f);

	if (ImGui::Button("OK", ImVec2(buttonWidth, 0.0f))) {
		editor.ClearSquiggles(squiggleType);
		state = State::edit;
		ImGui::CloseCurrentPopup();
	}

	ImGui::SameLine(0.0f, 5.0f);

	if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0.0f)) || ImGui::Shortcut(ImGuiKey_Escape)) {
		state = State::edit;
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndPopup();
}

void CodeEditor::RenderDebugInformation() {
	if (!showDebugInformation) return;

	std::string information;

	if (getBackendDebugInformation)
		information = getBackendDebugInformation();

	const auto &io = ImGui::GetIO();
	const auto &style = ImGui::GetStyle();

	information += "Dear ImGui:\n";
	information += std::format("io.DisplaySize: {}, {}\n", io.DisplaySize.x, io.DisplaySize.y);
	information += std::format("io.DisplayFramebufferScale: {}, {}\n",
		io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
	information += std::format("style.FontSizeBase: {}\n", style.FontSizeBase);
	information += std::format("style.FontScaleMain : {}\n", style.FontScaleMain);
	information += std::format("style.FontScaleDpi: {}\n", style.FontScaleDpi);
	information += std::format("GetLineHeight: {}", editor.GetLineHeight());
	information += std::format("GetGlyphWidth: {}", editor.GetGlyphWidth());

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration		  |
	                                   ImGuiWindowFlags_NoNav				  |
	                                   ImGuiWindowFlags_NoBringToFrontOnFocus |
	                                   ImGuiWindowFlags_NoFocusOnAppearing;

	ImGui::Begin("Debug Information", nullptr, flags);
	ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
	ImGui::TextUnformatted(information.c_str());
	ImGui::End();
}

void CodeEditor::SetLanguage(const TextEditor::Language *language) {
	if (editor.GetLanguage() != language) {
		editor.SetLanguage(language);
		diff.SetLanguage(language);

		if (lsp.IsRunning()) {
			lsp.CloseDocument(filename);

			if (language->name == "C++")
				lsp.OpenDocument(filename, editor, lspOptions);
		}
	}
}

void CodeEditor::SetLanguageByName(const std::string &name) {
	SetLanguage(TextEditor::LookupLanguage(name));
}
