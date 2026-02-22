#include "core/terminal.hpp"


#ifdef SPD_PLATFORM_LINUX
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

spd::Terminal::Terminal() {
    tcgetattr(STDIN_FILENO, &m_originalTermios);
}

spd::Terminal::~Terminal() {
    DisableRawMode();
}

void spd::Terminal::EnableRawMode() {
    struct termios raw = m_originalTermios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void spd::Terminal::DisableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &m_originalTermios);
}

spd::TermSize spd::Terminal::GetSize() const {
    TermSize size{};
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        size.cols = 80; size.rows = 24; // Default fallback
    }
    else {
        size.cols = ws.ws_col;
        size.rows = ws.ws_row;
    }
    return size;
}

spd::KeyEvent spd::Terminal::ReadKey() const {
    char c;
    int nread;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) return { Key::None, 0 };
    }

    if (c == '\x1b') {
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return { Key::Escape, 0 };
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return { Key::Escape, 0 };

        if (seq[0] == '[') {
            switch (seq[1]) {
            case 'A': return { Key::ArrowUp, 0 };
            case 'B': return { Key::ArrowDown, 0 };
            case 'C': return { Key::ArrowRight, 0 };
            case 'D': return { Key::ArrowLeft, 0 };
            case 'H': return { Key::Home, 0 };
            case 'F': return { Key::End, 0 };
            }
        }

        LOG_D("read 0x1b + 0x%02x 0x%02x 0x%02x\n", seq[0], seq[1], seq[2]);
        return { Key::Escape, 0 };
    }

    if (c == 127) return { Key::Backspace, 0 };
    if (c == '\r' || c == '\n') return { Key::Enter, 0 };
    if (c == ('s' & 0x1f)) return { Key::Ctrl_S, 0 };

    return { (Key)c, c };
}

#endif
