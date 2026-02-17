#include "editor.hpp"
#include "mem.h"


spd::Editor::Editor() {
	// start with one empty line
    GapBuffer initialLine;
    m_lines.EmplaceBack(std::move(initialLine));
    
    LOG_I("Editor initialized with 1 line.\n");
}

spd::Editor::~Editor() {
	for (size_t i = 0; i < m_lines.Size(); i++) {
        m_lines[i].~GapBuffer();
    }
}

void spd::Editor::Run() {
	m_terminal.EnableRawMode();
    m_terminal.Clear();

    while (m_isRunning) {
        Render();
        ProcessInput();
    }

    m_terminal.DisableRawMode();
    m_terminal.Clear();
}

void spd::Editor::ProcessInput() {
	KeyEvent e = m_terminal.ReadKey();
    
    // Debug log every key
    LOG_D("Input: KeyID=%d, Char='%c'\n", (int)e.key, e.c ? e.c : ' ');

    switch (e.key) {
        case Key::Escape: 
            m_isRunning = false; 
            break;
            
        case Key::ArrowUp:    if (m_cursorRow > 0) m_cursorRow--; break;
        case Key::ArrowDown:  if (m_cursorRow < m_lines.Size() - 1) m_cursorRow++; break;
        case Key::ArrowLeft:  if (m_cursorCol > 0) m_cursorCol--; break;
        case Key::ArrowRight: m_cursorCol++; break; // Simplified for now

        case Key::Backspace:  HandleBackspace(); break;
        case Key::Enter:      HandleEnter(); break;

        default:
            if ((uint32_t)e.key < 1000 && e.c != 0) {
                InsertChar(e.c);
            }
            break;
    }
}

void spd::Editor::Render() {
	m_terminal.HideCursor();
	m_terminal.SetCursorPos(0, 0);

    // Simple Render: Draw every line
    //for (size_t i = 0; i < m_lines.Size(); i++) {
    for (spd::iterator<spd::GapBuffer> it = m_lines.begin(); it != m_lines.end(); it++) {
        spd::StringView<char> prefix = it->GetPrefixView();
        m_terminal.Write(prefix.GetData(), prefix.GetLength());

        spd::StringView<char> suffix = it->GetSuffixView();
        m_terminal.Write(suffix.GetData(), suffix.GetLength());
    }

    m_terminal.SetCursorPos((int)m_cursorRow, (int)m_cursorCol);
    m_terminal.ShowCursor();
    m_terminal.Flush();
}

void spd::Editor::InsertChar(char c) {
	GapBuffer& line = m_lines[m_cursorRow];
    line.MoveCursor(m_cursorCol);
    line.Insert(c);
    m_cursorCol++;
}

void spd::Editor::HandleBackspace() {

}

void spd::Editor::HandleEnter() {

}
