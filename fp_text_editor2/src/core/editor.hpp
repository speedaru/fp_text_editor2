#pragma once
#include "stl/vector.hpp"
#include "stl/gap_buff.hpp"
#include "terminal.hpp"


namespace spd {
	class Editor {
	public:
		Editor();
		~Editor();

		void Run();

    private:
		void ProcessInput();
        void Render();
        
        // Command Helpers
        void InsertChar(char c);
        void HandleBackspace();
        void HandleEnter();

    private:
        Terminal m_terminal;
        spd::Vector<spd::GapBuffer> m_lines;
        
        size_t m_cursorRow{ 0 };
        size_t m_cursorCol{ 0 };
        bool m_isRunning{ true };
	};
}