#pragma once
#include "stl/vector.hpp"
#include "stl/gap_buff.hpp"
#include "terminal.hpp"
#include "renderer.hpp"


namespace spd {
	class Editor {
	public:
		using CHAR_TYPE = spd::GapBuffer::CHAR_TYPE;
        using Line = spd::GapBuffer;

		Editor();

		void Run();

        void LoadData(const spd::Vector<CHAR_TYPE>& data);
        void GetData(spd::Vector<uint8_t>& outData) const;

    private:
		void ProcessInput();
        void Render();
        
        // handle keys
        void HandleArrow(spd::KeyEvent event);
        void HandleBackspace();
        void HandleDelete();
        void HandleEnter();
        void HandleHome();
        void HandleEnd();

        // Command Helpers
        void InsertChar(char c);

        void MergeLineUp();
        void MergeLineDown();

    private:
        Terminal m_terminal;
        Renderer m_renderer;
        spd::Vector<Line> m_lines;
        
        size_t m_cursorRow{ 0 };
        size_t m_cursorCol{ 0 };
        bool m_isRunning{ true };
	};
}