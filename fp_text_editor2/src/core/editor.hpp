#pragma once
#include "stl/vector.hpp"
#include "stl/gap_buff.hpp"
#include "terminal.hpp"


namespace spd {
	class Editor {
	public:
		using CHAR_TYPE = spd::GapBuffer::CHAR_TYPE;
        using Line = spd::GapBuffer;

		Editor();

		void Run();

        void GetData(spd::Vector<uint8_t>& outData) const;

    private:
		void ProcessInput();
        void Render();
        
        // Command Helpers
        void HandleArrow(spd::Key arrowKey);
        void InsertChar(char c);
        void HandleBackspace();
        void HandleDelete();
        void HandleEnter();

        void MergeLineUp();
        void MergeLineDown();

    private:
        Terminal m_terminal;
        spd::Vector<Line> m_lines;
        
        size_t m_cursorRow{ 0 };
        size_t m_cursorCol{ 0 };
        bool m_isRunning{ true };
        
        // so we can clear empty lines after deleting them
        size_t m_prevLineCount{ 0ull };
	};
}