#include "gap_buff.hpp"
#include <stdexcept>

#include "common.h"


spd::GapBuffer::GapBuffer() {
    m_capacity = DEFAULT_SIZE;
	m_data = SPD_ALLOC(char, DEFAULT_SIZE);
    m_gapStart = 0;
    m_gapEnd = m_capacity;
    memset(m_data, '\0', m_capacity * sizeof(CHAR_TYPE));

    LOG_D("ctor\n");
}

spd::GapBuffer::~GapBuffer() {
    if (m_data) {
		SPD_FREE(m_data);
    }

    LOG_D("dtor\n");
}

// copy constructor
spd::GapBuffer::GapBuffer(const GapBuffer& other) {
    // copy data
    m_data = SPD_ALLOC(CHAR_TYPE, other.m_capacity);
    memcpy(m_data, other.m_data, sizeof(CHAR_TYPE) * other.m_capacity);

    m_capacity = other.m_capacity;
    m_gapStart = other.m_gapStart;
    m_gapEnd = other.m_gapEnd;

    LOG_D("copy ctor\n");
}

// copy assign
spd::GapBuffer& spd::GapBuffer::operator=(const GapBuffer& other) {
    // if data alr exists free it
    if (m_data) {
        SPD_FREE(m_data);
    }

    // copy data
    m_data = SPD_ALLOC(CHAR_TYPE, other.m_capacity);
    memcpy(m_data, other.m_data, sizeof(CHAR_TYPE) * other.m_capacity);

    m_capacity = other.m_capacity;
    m_gapStart = other.m_gapStart;
    m_gapEnd = other.m_gapEnd;

    LOG_D("copy assign\n");
    return *this;
}

spd::GapBuffer::GapBuffer(GapBuffer&& other) noexcept {
    // steal data pointer
    m_data = other.m_data;
    other.m_data = nullptr;

    // get capacity & size
    m_capacity = other.m_capacity;
    m_gapStart = other.m_gapStart;
    m_gapEnd = other.m_gapEnd;

    LOG_D("move ctor\n");
}

spd::GapBuffer& spd::GapBuffer::operator=(GapBuffer&& other) noexcept {
    if (m_data) {
        SPD_FREE(m_data);
    }

    // steal data pointer
    m_data = other.m_data;
    other.m_data = nullptr;

    // get capacity & size
    m_capacity = other.m_capacity;
    m_gapStart = other.m_gapStart;
    m_gapEnd = other.m_gapEnd;

    LOG_D("move assign\n");
    return *this;
}

void spd::GapBuffer::Insert(char c) {
	// check if we need more space
    if (m_gapStart == m_gapEnd) {
        // like m_capacity * 2 but faster bcs no float math
        size_t newCapacity = m_capacity + (m_capacity >> 1);
        Grow(newCapacity);
    }

    // place character at the start of the gap
    m_data[m_gapStart++] = c;

    // verify state
    Validate();
}

void spd::GapBuffer::BackSpace() {
	if (m_gapStart > 0) {
        m_gapStart--; // move gap back
        m_data[m_gapStart] = '\0'; // poison it
    }
    
    Validate();
}

void spd::GapBuffer::Delete() {
	if (m_gapEnd < m_capacity) { // check if there is text to right
		m_data[m_gapEnd] = '\0'; // poison the character at the current end
        m_gapEnd++; // move gap end forward to absorb character
    }
    
    Validate();
}

void spd::GapBuffer::MoveCursor(size_t pos) {
    if (m_gapStart != pos) {
		ShiftGap(pos);
    }
}


void spd::GapBuffer::Validate() const {
	// bounds checks
    if (m_gapStart > m_gapEnd || m_gapEnd > m_capacity) {
        LOG_E("VALIDATION FAILED: Pointers out of bounds! Start: %llu, End: %llu, Cap: %llu\n", m_gapStart, m_gapEnd, m_capacity);
        assert(false && "Gap buffer pointers corrupted");
    }

    // ensure gap is poisoned
    for (size_t i = m_gapStart; i < m_gapEnd; ++i) {
        if (m_data[i] != '\0') {
            LOG_E("VALIDATION FAILED: Gap corrupted at index %llu! Value: 0x%02X\n", i, (unsigned char)m_data[i]);
            DumpState(); // Log the visual state of the buffer
            assert(false && "Data found inside gap");
        }
    }
}

void spd::GapBuffer::DumpState() const {
	LOG_D("--- Gap Buffer State ---\n");
    LOG_D("Capacity: %llu | Gap: [%llu - %llu]\n", m_capacity, m_gapStart, m_gapEnd);
    
    // Create a temporary string to visualize the content
    // Warning: Only do this if the line isn't massive!
    char* visual = (char*)malloc(m_capacity + 3); 
    size_t vIdx = 0;
    for (size_t i = 0; i < m_capacity; i++) {
        if (i == m_gapStart) visual[vIdx++] = '[';
        if (i >= m_gapStart && i < m_gapEnd) visual[vIdx++] = '_';
        else visual[vIdx++] = (m_data[i] == '\0' ? '?' : m_data[i]);
        if (i == m_gapEnd - 1) visual[vIdx++] = ']';
    }
    visual[vIdx] = '\0';
    LOG_D("Content: %s\n", visual);
    free(visual);
}

spd::StringView<spd::GapBuffer::CHAR_TYPE> spd::GapBuffer::GetPrefixView() const {
    return spd::StringView<spd::GapBuffer::CHAR_TYPE>(m_data, m_gapStart);
}

spd::StringView<spd::GapBuffer::CHAR_TYPE> spd::GapBuffer::GetSuffixView() const {
    return spd::StringView<spd::GapBuffer::CHAR_TYPE>(m_data + m_gapEnd, m_capacity - m_gapEnd);
}

void spd::GapBuffer::Grow(size_t newCapacity){
	char* newData = SPD_ALLOC(char, newCapacity);
    
    // copy start
    size_t prefixSize = m_gapStart;
    memcpy(newData, m_data, prefixSize * sizeof(CHAR_TYPE));

    // copy end
    size_t suffixSize = m_capacity - m_gapEnd;
    size_t newGapEnd = newCapacity - suffixSize;
    
    memcpy(newData + newGapEnd, m_data + m_gapEnd, suffixSize * sizeof(CHAR_TYPE));

    // fill gap with 0 for debugging
    memset(newData + prefixSize, 0, (newGapEnd - prefixSize) * sizeof(CHAR_TYPE));

    // update state
    SPD_FREE(m_data);
    m_data = newData;
    m_gapEnd = newGapEnd; // gap is now bigger
    m_capacity = newCapacity;

    LOG_D("Buffer Grown: New Capacity: %llu, New Gap: [%llu - %llu]\n", m_capacity, m_gapStart, m_gapEnd);
}

void spd::GapBuffer::ShiftGap(size_t newGapStart){
	// dont move past the possible logical text length
    size_t currentTextLength = GetSize();
    if (newGapStart > currentTextLength) {
        LOG_W("Attempted to shift gap to %llu, but max length is %llu\n", newGapStart, currentTextLength);
        newGapStart = currentTextLength;
    }

    // move gap to the left
    while (newGapStart < m_gapStart) {
        m_gapStart--;
        m_gapEnd--;
        m_data[m_gapEnd] = m_data[m_gapStart];
        
        // poison new empty slot in gap
        m_data[m_gapStart] = '\0'; 
    }

    // move gap to the right
    while (newGapStart > m_gapStart) {
        m_data[m_gapStart] = m_data[m_gapEnd];
        
        // poison new empty slot in gap
        m_data[m_gapEnd] = '\0';
        
        m_gapStart++;
        m_gapEnd++;
    }

    // verify we didn't break anything
    Validate();
}

