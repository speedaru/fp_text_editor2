#pragma once
#include "input.hpp"

#ifdef SPD_PLATFORM_LINUX
#include <termios.h>
#include <unistd.h>
#endif


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

	typedef DWORD TermColor;
	namespace TermColors {
        constexpr TermColor Light = 0x1;

        // normal colors
		constexpr TermColor DarkRed = 0x2;
		constexpr TermColor DarkGreen = 0x4;
		constexpr TermColor DarkBlue = 0x8;

        constexpr TermColor LightGray = DarkRed | DarkGreen | DarkBlue;
        constexpr TermColor White = DarkRed | DarkGreen | DarkBlue | Light;
        constexpr TermColor Black = 0;

        // light normal colors
		constexpr TermColor LighRed = DarkRed | Light;
		constexpr TermColor LighGreen = DarkGreen | Light;
		constexpr TermColor LighBlue = DarkBlue | Light;

        // light mix of colors
		constexpr TermColor Beige = DarkRed | DarkGreen | Light;
		constexpr TermColor Pink = DarkRed | DarkBlue | Light;
		constexpr TermColor Cyan = DarkGreen | DarkBlue | Light;

        // dark mix of colors
		constexpr TermColor Yellow = DarkRed | DarkGreen;
		constexpr TermColor Magenta = DarkRed | DarkBlue;
		constexpr TermColor Turquoise = DarkGreen | DarkBlue;
	}

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
        size_t Write(const char* str);
        size_t Write(const char* buff, size_t len);
        void Flush();

        // colors
        void SetForeColor(SMALL_RECT rect, TermColor color) const;
        void SetBgColor(SMALL_RECT rect, TermColor color) const;
        void SetColor(SMALL_RECT rect, spd::TermColor foreColor, spd::TermColor bgColor) const;

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