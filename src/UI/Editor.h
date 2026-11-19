#pragma once
#include <filesystem>
#include <string>

#include "TextDiff.h"
#include "TextEditor.h"
#include "extras/LspBridge.h"
#include "extras/TrieAutoComplete.h"


class CodeEditor {
    // properties
    std::string originalText;
    TextEditor editor;
    TextDiff diff;
    std::filesystem::path filename;
    size_t version;
    bool done = false;
    bool visible = false;
    bool popup = true;
    std::string errorMessage;
    std::function<void()> onConfirmClose;

    TextEditor::LineBreakConfig lineBreakConfig;

    TrieAutoComplete trieAutoComplete;
    LspBridge lsp;
    static constexpr int lspOptions = LspBridge::autocomplete | LspBridge::showHoverHelp;

    std::function<std::string()> getBackendDebugInformation;

    size_t squiggleType = 1;
    ImColor squiggleColor{1.0f, 0.2f, 0.0f, 0.8f};
    char squiggleToolTip[128] = {};
    TextEditor::SquiggleStyle squiggleStyle = TextEditor::SquiggleStyle::wave;

    bool demoTrieAutoComplete = false;
    bool demoLspBridge = false;
    bool showWordAtMouse = false;
    bool showQualifiedIdentifierAtMouse = false;
    bool showDocPosAtMouse = false;
    bool showLineMarkers = false;
    bool showLineDecorator = false;
    bool showCustomCaret = false;
    bool showCustomLineNumbers = false;
    bool showContextMenus = false;
    bool enableUnicodeLineBreakAlgorithm = false;
    bool showDebugInformation = false;
    bool showDebugWindow = false;

    // editor state
    enum class State {
        edit,
        diff,
        newFile,
        openFile,
        saveFileAs,
        confirmClose,
        confirmQuit,
        confirmError,
        addSquiggle,
        clearSquiggles
    } state = State::edit;

public:
    CodeEditor();

    void Render();

    void Show() { visible = true; }
    bool IsVisible() const { return visible; }

    void NewFile();
    void OpenFile();
    void OpenFile(const std::filesystem::path& path);
    void SaveFile();

    void TryToQuit();
    bool IsDone() const { return done; }

    void setDebugInformation(std::function<std::string()> callback) { getBackendDebugInformation = callback; }

private:
    // private functions
    void RenderMenuBar();
    void RenderStatusBar();

    void ShowDiff();
    void ShowFileOpen();
    void ShowSaveFileAs();
    void ShowConfirmClose(std::function<void()> callback);
    void ShowConfirmQuit();
    void ShowError(const std::string& message);
    void ShowAddSquiggle();
    void ShowClearSquiggles();

    void RenderDiff();
    void RenderFileOpen();
    void RenderSaveAs();
    void RenderConfirmClose();
    void RenderConfirmQuit();
    void RenderConfirmError();
    void RenderAddSquiggle();
    void RenderClearSquiggle();

    void RenderDebugInformation();

    bool IsDirty() const { return editor.GetUndoIndex() != version; }
    bool IsSavable() const { return IsDirty() && filename != "untitled"; }

    void SetLanguage(const TextEditor::Language* language);
    void SetLanguageByName(const std::string& name);

    float fontSize = 17.0f;
    void resetFontSize() { fontSize = 17.0f; }
    void increaseFontSize() { fontSize = std::clamp(fontSize + 1.0f, 8.0f, 24.0f); }
    void decreaseFontSize() { fontSize = std::clamp(fontSize - 1.0f, 8.0f, 24.0f); }
};
