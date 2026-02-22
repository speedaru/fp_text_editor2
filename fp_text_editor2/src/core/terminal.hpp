#pragma once
#include "input.hpp"


#define ANSI_CLEAR_TO_EOL       "\x1b[K"
#define ANSI_CLEAR_SCREEN       "\x1b[2J"
#define ANSI_SET_CURSOR_HOME    "\x1b[H"
#define ANSI_SET_CURSOR_POS     "\x1b[%d;%dH"
#define ANSI_HIDE_CURSOR        "\x1b[?25l"
#define ANSI_SHOW_CURSOR        "\x1b[?25h"

namespace spd {
    struct TermSize {
        int rows{};
        int cols{};
    };

    class Terminal {
    public:
        Terminal();
        ~Terminal();

        // Screen Management
        void Clear();
        void SetCursorPos(int row, int col);
        void HideCursor();
        void ShowCursor();
        TermSize GetSize() const;

        // Mode Management
        void EnableRawMode();  // Turns off "Enter to send" and "Echo"
        void DisableRawMode(); // Returns terminal to normal

        // input
        KeyEvent ReadKey() const;

        // Output
        void Write(const char* str);
        void Write(const char* buff, size_t len);
        void Flush();

    private:
#ifdef SPD_PLATFORM_WINDOWS
        HANDLE m_hOut;
        HANDLE m_hIn;
        DWORD m_originalOutMode;
        DWORD m_originalInMode{};
#else
        struct termios m_originalTermios;
#endif
    };
}