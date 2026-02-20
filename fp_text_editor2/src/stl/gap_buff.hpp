#pragma once
#include "string_view.hpp"


namespace spd {
	class GapBuffer {
    public:
        using CHAR_TYPE = char;

		GapBuffer();
		~GapBuffer();

        // copy constructor
        GapBuffer(const GapBuffer& other);
        GapBuffer& operator=(const GapBuffer& other);

        // move constructor
        GapBuffer(GapBuffer&& other) noexcept;
        GapBuffer& operator=(GapBuffer&& other) noexcept;

        void Insert(char c);
        void MoveCursor(size_t pos);
        void BackSpace(); // simulate backspace
        void Delete(); // simulate delete key

        void TruncateAtGap();

        // ensure state is not corrupted
        void Validate() const;
        void DumpState() const;

        StringView<CHAR_TYPE> GetPrefixView() const;
        StringView<CHAR_TYPE> GetSuffixView() const;

        // get logical text length
        size_t GetSize() const { Validate(); return m_capacity - (m_gapEnd - m_gapStart); }
        CHAR_TYPE* GetBuff() const { return m_data; }
        size_t GetGapStart() const { return m_gapStart; }
        size_t GetGapEnd() const { return m_gapEnd; }

    private:
        // grow gap
        void Grow(size_t newCapacity);
        void ShiftGap(size_t newGapStart);

    private:
        static const size_t DEFAULT_SIZE{ 64ull };

        CHAR_TYPE* m_data{ nullptr };
        size_t m_capacity{ 0 };
        size_t m_gapStart{ 0 }; // Cursor usually lives here
        size_t m_gapEnd{ 0 };
	};
}