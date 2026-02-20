#include "input.hpp"

const char* spd::KeyToStr(KeyEvent ke) {
	static char ch[2]{ 0 };

	switch (ke.key) {
	case Key::ArrowUp: return "ArrowUp";
	case Key::ArrowDown: return "ArrowDown";
	case Key::ArrowRight: return "ArrowRight";
	case Key::ArrowLeft: return "ArrowLeft";
	case Key::Home: return "Home";
	case Key::End: return "End";
	case Key::PageUp: return "PageUp";
	case Key::Insert: return "Insert";
	case Key::Delete: return "Delete";
	case Key::Backspace: return "Backspace";
	case Key::Enter: return "Enter";
	case Key::Tab: return "Tab";
	case Key::Escape: return "Escape";

		// temporarily store char in static buffer
	default: assert(ke.c < (int)Key::ControlKeyStart); ch[0] = ke.c; return ch;
	}
}