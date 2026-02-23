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

        // if Key > 2000 it means control is pressed
        Ctrl = 2000,

        Unknown = 0xffff
    };

    typedef DWORD KeyModifier;
    namespace KeyModifiers {
		constexpr KeyModifier Ctrl = 0x1;
    }

    struct KeyEvent {
        Key key;
        char c; // valid if key < 1000
        DWORD keyModifiers; // control pressed or wtv
    };

    const char* KeyToStr(KeyEvent ke);
}
