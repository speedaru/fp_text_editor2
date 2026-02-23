#pragma once
#include "stl/vector.hpp"
#include "stl/gap_buff.hpp"
#include "terminal.hpp"


namespace spd {
	class Renderer {
	public:
		using CHAR_TYPE = spd::GapBuffer::CHAR_TYPE;

		Renderer(Terminal& term) : m_term(term), m_prevLineCount(m_term.GetSize().rows) {}

		// handles scrolling etc
        void Render(const spd::Vector<GapBuffer>& lines, size_t cursorRow, size_t cursorCol);

		void VerticalScroll(int delta);
		void HorizontalScroll(int delta);

    private:
		void UpdateScroll(size_t cursorRow, size_t cursorCol);

		// returns number of chars written
		size_t RenderStringView(const spd::StringView<CHAR_TYPE>& strView, size_t colsLeft);

	private:
		Terminal& m_term;
		size_t m_offX{ 0 }; // Horizontal scroll
		size_t m_offY{ 0 }; // Vertical scroll
		int m_cols{ 0 };
		int m_rows{ 0 };

        // so we can clear empty lines after deleting them
        size_t m_prevLineCount;
	};
}