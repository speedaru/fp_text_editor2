#pragma once
#include "input.hpp"


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
        DWORD m_originalInMode;
#else
        struct termios m_originalTermios;
#endif
    };
}