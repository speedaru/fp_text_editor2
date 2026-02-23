#include "editor.hpp"
#include "mem.h"


spd::Editor::Editor() : m_renderer(m_terminal) {
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

void spd::Editor::LoadData(const spd::Vector<CHAR_TYPE>& data) {
    m_lines.Clear();

	auto current = data.begin();
    const auto end = data.end();
    bool endsWithNewLine = false; // file ends with newline

    while (current != end) {
        // find next newline or end of data
        auto lineEnd = std::find(current, end, '\n');

        // create line and insert line data
        Line newLine;
        newLine.InsertRange(spd::StringView<CHAR_TYPE>(current, lineEnd));
        m_lines.PushBack(std::move(newLine));

        // put next line start after newline char if we're not at end of data
        if (lineEnd != end) {
			current = lineEnd + 1;
            endsWithNewLine = (current == end); // if current is now end it means file ends with newline
        }
        else {
            current = lineEnd;
        }
    }

    if (endsWithNewLine) {
        m_lines.EmplaceBack();
    }

    // ensure there is always at least one empty line
    if (m_lines.Empty()) {
        m_lines.PushBack(Line());
    }
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
            HandleArrow(e);
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
    m_renderer.Render(m_lines, m_cursorRow, m_cursorCol);
}

void spd::Editor::HandleArrow(spd::KeyEvent event) {
    auto clampCol = [&]() {
        return spd::min(m_cursorCol, m_lines[static_cast<int>(m_cursorRow)].GetSize());
	};

    // if control pressed
    if (event.keyModifiers & KeyModifiers::Ctrl) {
        switch (event.key) {
        case Key::ArrowUp:
            LOG_D("scrolling up\n");
            m_renderer.VerticalScroll(-1);
            break;
        case Key::ArrowDown:
            LOG_D("scrolling down\n");
            m_renderer.VerticalScroll(1);
            break;
        case Key::ArrowLeft:
            LOG_D("scrolling left\n");
            m_renderer.HorizontalScroll(-1);
            break;
        case Key::ArrowRight:
            LOG_D("scrolling right\n");
            m_renderer.HorizontalScroll(1);
            break;
        }
    }

    switch (event.key) {
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
    Line& currentLine = m_lines[(int)m_cursorRow];

    // can't merge down because alr at last line
    if (m_cursorRow + 1 == m_lines.Size()) {
        return;
    }

    // get next line string view and insert it into next line
    Line& nextLine = m_lines[(int)m_cursorRow + 1];
    nextLine.MoveCursor(nextLine.GetSize());
    currentLine.InsertRange(nextLine.GetPrefixView());

    // delete next line bcs merged
    m_lines.RemoveAt(m_cursorRow + 1);
}
