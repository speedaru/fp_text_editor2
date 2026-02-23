#include "posix_input_parser.hpp"

#ifdef SPD_PLATFORM_LINUX

spd::KeyEvent spd::internal::ParseEscapeSequence() {
    char seq[4]{ 0 };

    // read first 2 bytes after 0x1b
    if (read(STDIN_FILENO, &seq[0], 1) != 1) return { Key::Escape, 0 };
    if (read(STDIN_FILENO, &seq[1], 1) != 1) return { Key::Escape, 0 };

    if (seq[0] == '[') {
        // extended escape sequence that ends with ~
        if (seq[1] >= '0' && seq[1] <= '9') {
            if (read(STDIN_FILENO, &seq[2], 1) != 1) return { Key::Escape, 0 };
            if (seq[2] == '~') {
                switch (seq[1]) {
                    case '3': return { Key::Delete, 0 };
                    case '5': return { Key::PageUp, 0 };
                    case '6': return { Key::PageDown, 0 };
                }
            }
        }
        // simple escape sequence
        else {
            switch (seq[1]) {
                case 'A': return { Key::ArrowUp, 0 };
                case 'B': return { Key::ArrowDown, 0 };
                case 'C': return { Key::ArrowRight, 0 };
                case 'D': return { Key::ArrowLeft, 0 };
                case 'H': return { Key::Home, 0 };
                case 'F': return { Key::End, 0 };
            }
        }
    }
    else{ 
        return { Key::Escape, 0 };
    }

    // somehow invalid
    return { Key::Unknown, 0 };
}

spd::KeyEvent spd::internal::MapByteToKey(char c) {
    if (c == 127) return { Key::Backspace, 0 };
    if (c == '\r' || c == '\n') return { Key::Enter, 0 };
    if (c == '\t') return { Key::Tab, 0 };
    if (c == ('s' & 0x1f)) return { Key::Ctrl_S, 0 };

    if (c >= 32 && c <= 126) return { (Key)c, c };
    return { (Key)c, c };
}

#endif
