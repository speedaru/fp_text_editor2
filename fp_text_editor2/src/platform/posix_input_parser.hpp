#pragma once
#include "core/input.hpp"

namespace spd::internal {
    // This helper specifically handles the "Escape Sequence" mess
    KeyEvent ParseEscapeSequence();
    
    // Maps a single byte to a key (handles Ctrl, Backspace, etc.)
    KeyEvent MapByteToKey(char c);
}
