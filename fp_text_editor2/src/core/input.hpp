#pragma once
#include "common.h"

namespace spd {
	enum class Key : uint32_t {
        None = 0,

		// control keys
        ControlKeyStart = 1000,
        ArrowUp, ArrowDown, ArrowLeft, ArrowRight,
        Home, End, PageUp, PageDown,
        Insert, Delete, Backspace, Enter, Tab, Escape,

        // modifier combos (optional but helpful)
        Ctrl_S, Ctrl_Q, Ctrl_F,

        Unknown = 0xffff
    };

    struct KeyEvent {
        Key key;
        char c; // valid if key < 1000
    };

    const char* KeyToStr(KeyEvent ke);
}
