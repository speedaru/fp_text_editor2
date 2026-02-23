#include "core/terminal.hpp"


#ifdef SPD_PLATFORM_WINDOWS

static WORD s_clearForeground = ~(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
static WORD s_clearBackground = ~(BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY);
static WORD s_clearColor = s_clearForeground & s_clearBackground;

static WORD GetWin32Color(spd::TermColor color, bool foreground, bool background) {
    WORD attribs = 0u;

    if (foreground) {
        if (color & spd::TermColors::DarkRed) {
            attribs |= FOREGROUND_RED;
        }
        if (color & spd::TermColors::DarkGreen) {
            attribs |= FOREGROUND_GREEN;
        }
        if (color & spd::TermColors::DarkBlue) {
            attribs |= FOREGROUND_BLUE;
        }
        if (color & spd::TermColors::Light) {
            attribs |= FOREGROUND_INTENSITY;
        }
    }

    if (background) {
        if (color & spd::TermColors::DarkRed) {
            attribs |= BACKGROUND_RED;
        }
        if (color & spd::TermColors::DarkGreen) {
            attribs |= BACKGROUND_GREEN;
        }
        if (color & spd::TermColors::DarkBlue) {
            attribs |= BACKGROUND_BLUE;
        }
        if (color & spd::TermColors::Light) {
            attribs |= BACKGROUND_INTENSITY;
        }
    }

    return attribs;
}


spd::Terminal::Terminal() {
    m_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    m_hIn = GetStdHandle(STD_INPUT_HANDLE);
    
    DWORD dwMode = 0;
    GetConsoleMode(m_hOut, &dwMode);
    m_originalOutMode = dwMode;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(m_hOut, dwMode);
}

spd::Terminal::~Terminal() {
    DisableRawMode();
    SetConsoleMode(m_hOut, m_originalOutMode);
}

void spd::Terminal::EnableRawMode() {
    DWORD dwMode = 0;
    GetConsoleMode(m_hIn, &dwMode);
    m_originalInMode = dwMode;
    dwMode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
    SetConsoleMode(m_hIn, dwMode);
}

void spd::Terminal::DisableRawMode() {
    SetConsoleMode(m_hIn, m_originalInMode);
}

spd::TermSize spd::Terminal::GetSize() const {
    TermSize size{};
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(m_hOut, &csbi);
    size.cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    size.rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return size;
}

spd::KeyEvent spd::Terminal::ReadKey() const {
    DWORD read;
    INPUT_RECORD ir;
    while (true) {
        ReadConsoleInput(m_hIn, &ir, 1, &read);
        if (read && ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
            auto& ke = ir.Event.KeyEvent;
            KeyEvent result = { Key::None, 0 };

            if (ke.uChar.AsciiChar >= 32 && ke.uChar.AsciiChar <= 126) {
                result.key = (Key)ke.uChar.AsciiChar;
                result.c = ke.uChar.AsciiChar;
                return result;
            }

            switch (ke.wVirtualKeyCode) {
                case VK_UP:     result.key = Key::ArrowUp;      break;
                case VK_DOWN:   result.key = Key::ArrowDown;    break;
                case VK_LEFT:   result.key = Key::ArrowLeft;    break;
                case VK_RIGHT:  result.key = Key::ArrowRight;   break;
                case VK_BACK:   result.key = Key::Backspace;    break;
                case VK_RETURN: result.key = Key::Enter;        break;
                case VK_DELETE: result.key = Key::Delete;       break;
                case VK_ESCAPE: result.key = Key::Escape;       break;
                case VK_HOME:   result.key = Key::Home;         break;
                case VK_END:    result.key = Key::End;          break;
            }

            // check control pressed
            if (ke.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) {
                result.keyModifiers |= KeyModifiers::Ctrl;
            }

            if (result.key != Key::None) return result;
        }
    }
}

static void SetTerminalColor(HANDLE hOut, SMALL_RECT rect, WORD attrib) {
    // iterate through each row in the rectangle
    for (SHORT y = rect.Top; y <= rect.Bottom; ++y) {
        COORD startCoord = { rect.Left, y };
        DWORD numCellsToFill = (DWORD)(rect.Right - rect.Left);
        DWORD cellsWritten = 0;

        // apply the attribute to the specific row
        if (!FillConsoleOutputAttribute(
                hOut,           // handle to console screen buffer
                attrib,           // color attribute to write
                numCellsToFill,   // number of cells to write to
                startCoord,       // starting coordinate
                &cellsWritten     // receive number of cells written
            )) 
        {
            LOG_E("Failed to set background color for row %d", y);
        }
    }
}

void spd::Terminal::SetForeColor(SMALL_RECT rect, spd::TermColor color) const {
    WORD attrib = GetWin32Color(color, true, false);
    SetTerminalColor(m_hOut, rect, attrib);
}

void spd::Terminal::SetBgColor(SMALL_RECT rect, spd::TermColor color) const {
    WORD attrib = GetWin32Color(color, false, true);
    SetTerminalColor(m_hOut, rect, attrib);
}

void spd::Terminal::SetColor(SMALL_RECT rect, spd::TermColor foreColor, spd::TermColor bgColor) const {
    WORD attrib = GetWin32Color(foreColor, true, false) | GetWin32Color(bgColor, false, true);
    SetTerminalColor(m_hOut, rect, attrib);
}

#endif // SPD_PLATFORM_WINDOWS