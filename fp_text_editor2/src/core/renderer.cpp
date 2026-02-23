#include "renderer.hpp"


void spd::Renderer::Render(const spd::Vector<GapBuffer>& lines, size_t cursorRow, size_t cursorCol) {
    UpdateScroll(cursorRow, cursorCol);

    // hide cursor and place at origin
	m_term.HideCursor();
	m_term.SetCursorPos(0, 0);

    for (size_t row = m_offY; row - m_offY <= m_rows && row <= m_prevLineCount; row++) {
		// clear deleted lines
        if (row >= lines.Size()) {
			m_term.Write(ANSI_CLEAR_TO_EOL "\r\n");
            continue;
        }
        auto& line = lines[(int)row];

        size_t colsLeft = m_cols;
        colsLeft -= RenderStringView(line.GetPrefixView(), colsLeft);
        colsLeft -= RenderStringView(line.GetSuffixView(), colsLeft);

        if (colsLeft > 0) {
			m_term.Write(ANSI_CLEAR_TO_EOL);
        }
        m_term.Write("\r\n");
    }
    m_prevLineCount = lines.Size(); // reset prev line count

    // set terminal color
    spd::TermColor foreColor = spd::TermColors::LightGray;
    spd::TermColor bgColor = spd::TermColors::Magenta;
	m_term.SetColor({ 0, 0, (SHORT)m_cols, (SHORT)m_rows }, foreColor, bgColor);

    // set cursor pos in terminal
    m_term.SetCursorPos((int)cursorRow - (int)m_offY, (int)cursorCol - (int)m_offX);
    m_term.ShowCursor();
    m_term.Flush();
}

void spd::Renderer::VerticalScroll(int delta) {
    // if delta negative ensure it wont overflow when we subtract
    if (delta < 0 && m_offY >= -delta) {
        size_t absDelta = -delta;
		m_offY += delta;
        LOG_D("scrolled up: to %llu\n", m_offY);
    }
    //else if (delta < 0 && m_offY < -delta) {
    //    LOG_D("cant scroll up, m_offY too small %llu\n", m_offY);
    //}
    else if (delta > 0) {
        m_offY += delta;
        LOG_D("scrolled down: to %llu\n", m_offY);
    }
}

void spd::Renderer::HorizontalScroll(int delta) {
    // if delta negative ensure it wont overflow when we subtract
    if (delta < 0 && m_offX >= -delta) {
        size_t absDelta = -delta;
		m_offX += delta;
        LOG_D("scrolled left: to %llu\n", m_offX);
    }
    else if (delta > 0) {
        m_offX += delta;
        LOG_D("scrolled right: to %llu\n", m_offX);
    }
}

void spd::Renderer::UpdateScroll(size_t cursorRow, size_t cursorCol) {
    TermSize termSize = m_term.GetSize();
    m_cols = termSize.cols;
    m_rows = termSize.rows - 2; // -1 for 0 indexing. -1 for status bar

    // vertical scrolling
    if (cursorRow < m_offY) { // scroll up
        m_offY = cursorRow;
    }
    else if (cursorRow >= m_offY + m_rows) { // scroll down
        m_offY = cursorRow - m_rows;
    }

    // horizontal scrolling
    if (cursorCol < m_offX) { // scroll left
        m_offX = cursorCol;
    }
    else if (cursorCol >= m_offX + m_cols) { // scroll right
        m_offX = cursorCol - m_cols + 1;
    }
}

size_t spd::Renderer::RenderStringView(const spd::StringView<CHAR_TYPE>& strView, size_t colsLeft) {
    const CHAR_TYPE* buff = strView.GetData();
    size_t buffLen = strView.GetLength();

    // line invisible
    if (!colsLeft || buffLen < m_offX) {
        return 0;
    }

    const CHAR_TYPE* start = buff + m_offX;
    size_t toWrite = spd::min(buffLen - m_offX, colsLeft);

    m_term.Write(start, toWrite);
    return toWrite;
}

