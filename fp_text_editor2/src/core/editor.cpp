#include "editor.hpp"
#include "mem.h"


spd::Editor::Editor() {
	// start with one empty line
    m_lines.EmplaceBack();
    
    LOG_I("Editor initialized with 1 line.\n");
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

void spd::Editor::GetData(spd::Vector<uint8_t>& outData) const {
    // calc size
    size_t dataSize = 0ull;
    for (spd::iterator it = m_lines.begin(); it != m_lines.end(); it++) {
        dataSize += it->GetSize() + 1; // +1 for new line
    }

    // reserve data
    outData.Resize(dataSize);

    // set data
    spd::iterator outIt = outData.begin();
    for (spd::iterator it = m_lines.begin(); it != m_lines.end(); it++) {
        size_t lineSize = it->GetSize();
        spd::StringView<CHAR_TYPE> linePrefix = it->GetPrefixView();
        spd::StringView<CHAR_TYPE> lineSuffix = it->GetSuffixView();

        // for each line add line data
        for (size_t i = 0ull; i < lineSize; i++) {
            size_t relativeIdx = i;
            spd::StringView<CHAR_TYPE>* strView = &linePrefix;
            if (i >= linePrefix.GetLength()) {
                relativeIdx -= linePrefix.GetLength();
                strView = &lineSuffix;
            }
            
            *outIt++ = strView->GetData()[relativeIdx];
        }

        *outIt++ = '\n';
    }

    // trim last \n
    outData.PopBack();
}


void spd::Editor::ProcessInput() {
	KeyEvent e = m_terminal.ReadKey();
    
    // Debug log every key
	LOG_D("Input: Char='%s' KeyID=%d, \n", KeyToStr(e), (int)e.key);

    switch (e.key) {
        case Key::Escape: 
            m_isRunning = false; 
            break;

        // arrow keys
        case Key::ArrowUp:
        case Key::ArrowDown:
        case Key::ArrowLeft:
        case Key::ArrowRight:
            HandleArrow(e.key);
            break;

        case Key::Delete:       HandleDelete(); break;
        case Key::Backspace:    HandleBackspace(); break;
        case Key::Enter:        HandleEnter(); break;

        case Key::Home: HandleHome(); break;
        case Key::End:  HandleEnd(); break;

        default:
            if ((uint32_t)e.key < 1000 && e.c != 0) {
                InsertChar(e.c);
            }
            break;
    }
}

void spd::Editor::Render() {
    // hide cursor and place at origin
	m_terminal.HideCursor();
	m_terminal.SetCursorPos(0, 0);

    // Simple Render: Draw every line
    //for (size_t i = 0; i < m_lines.Size(); i++) {
    for (spd::iterator<spd::GapBuffer> it = m_lines.begin(); it != m_lines.end(); it++) {
        spd::StringView<char> prefix = it->GetPrefixView();
        m_terminal.Write(prefix.GetData(), prefix.GetLength());

        spd::StringView<char> suffix = it->GetSuffixView();
        m_terminal.Write(suffix.GetData(), suffix.GetLength());

        m_terminal.Write(ANSI_CLEAR_TO_EOL "\r\n");
    }

    // clear deleted lines
    for (size_t i = m_lines.Size(); i < m_prevLineCount; i++) {
        m_terminal.Write(ANSI_CLEAR_TO_EOL "\n");
    }
    m_prevLineCount = m_lines.Size(); // reset prev line count

    // set cursor pos
    m_terminal.SetCursorPos((int)m_cursorRow, (int)m_cursorCol);
    m_terminal.ShowCursor();
    m_terminal.Flush();
}

void spd::Editor::HandleArrow(spd::Key arrowKey) {
    auto clampCol = [&]() {
        return spd::min(m_cursorCol, m_lines[static_cast<int>(m_cursorRow)].GetSize());
	};

    switch (arrowKey) {
    case Key::ArrowUp:
		if (m_cursorRow > 0) {
			m_cursorRow--;
			m_cursorCol = clampCol();
		}
        break;
    case Key::ArrowDown:
		if (m_cursorRow < m_lines.Size() - 1) {
			m_cursorRow++;
			m_cursorCol = clampCol();
		}
        break;
    case Key::ArrowRight:
		if (m_cursorCol < m_lines[static_cast<int>(m_cursorRow)].GetSize()) {
			m_cursorCol++;
		}
        break;
    case Key::ArrowLeft:
		if (m_cursorCol > 0) {
			m_cursorCol--;
		}
        break;
	}
}

void spd::Editor::HandleBackspace() {
    Line& currentLine = m_lines[static_cast<int>(m_cursorRow)];
    LOG_D("pressed backspace at col %llu\n", m_cursorCol);
    currentLine.MoveCursor(m_cursorCol);

    if (m_cursorCol == 0) {
        MergeLineUp();
        return;
    }

    currentLine.BackSpace();
    m_cursorCol--;
}

void spd::Editor::HandleDelete() {
    Line& currentLine = m_lines[static_cast<int>(m_cursorRow)];
    LOG_D("pressed delet at col %llu\n", m_cursorCol);
    currentLine.MoveCursor(m_cursorCol);

    if (m_cursorCol == currentLine.GetSize()) {
        MergeLineDown();
        return;
    }

    currentLine.Delete();
}

void spd::Editor::HandleEnter() {
    Line& currentLine = m_lines[static_cast<int>(m_cursorRow)];
    currentLine.MoveCursor(m_cursorCol);

    // get everything after cursor and insert it into a new line
    Line newLine;
    newLine.InsertRange(currentLine.GetSuffixView());

    // remove suffix
    currentLine.TruncateAtGap();

    //m_lines.Insert(m_cursorRow + 1, std::move(newLine));
    m_lines.Insert(m_cursorRow + 1, std::move(newLine));

    // go to begining of new line
    m_cursorRow++;
    m_cursorCol = 0;

    LOG_D("handled enter, editor now has %llu lines\n", m_lines.Size());
    LOG_D("lines len:\n");
    for (int i = 0; i < m_lines.Size(); i++) {
        logging::LogOutput(logging::LOG_DEBUG, __RELATIVE_FILE__, __LINE__, __FUNCTION__, "row %llu: %llu\n", i, m_lines[i].GetSize());
    }
}

void spd::Editor::HandleHome() {
    m_cursorCol = 0;
    LOG_D("pressed HOME. moved cursor to beginning of line\n");
}

void spd::Editor::HandleEnd() {
    m_cursorCol = m_lines[static_cast<int>(m_cursorRow)].GetSize();
    LOG_D("pressed END. moved cursor to end of line: %llu\n", m_cursorCol);
}


void spd::Editor::InsertChar(char c) {
	Line& currentLine = m_lines[static_cast<int>(m_cursorRow)];
    currentLine.MoveCursor(m_cursorCol);
    currentLine.Insert(c);
    LOG_D("inserted char: %c at %llu:%llu\n", c, m_cursorRow, m_cursorCol);
    m_cursorCol++;
}


void spd::Editor::MergeLineUp() {
    Line& currentLine = m_lines[static_cast<int>(m_cursorRow)];

    // can't merge up because alr at line 0
    if (m_cursorRow == 0) {
        return;
    }

    // get everything after cursor to merge up and insert it into line above
    Line& lineAbove = m_lines[static_cast<int>(m_cursorRow) - 1];
    size_t originalLineAboveLen = lineAbove.GetSize(); // editor cursor needs to be at pos before line merge
    lineAbove.MoveCursor(originalLineAboveLen); // so it inserts the range at the end
    lineAbove.InsertRange(currentLine.GetSuffixView());

    // position editor cursor at end of line
    m_cursorCol = originalLineAboveLen;

    // delete current line and move editor cursor up
    m_lines.RemoveAt(m_cursorRow--);
}

void spd::Editor::MergeLineDown() {
    Line& currentLine = m_lines[static_cast<int>(m_cursorRow)];

    // can't merge down because alr at last line
    if (m_cursorRow + 1 == m_lines.Size()) {
        return;
    }

    // get next line string view and insert it into next line
    Line& nextLine = m_lines[static_cast<int>(m_cursorRow) + 1];
    nextLine.MoveCursor(nextLine.GetSize());
    currentLine.InsertRange(nextLine.GetPrefixView());

    // delete next line bcs merged
    m_lines.RemoveAt(m_cursorRow + 1);
}
