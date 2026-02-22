#include "string.hpp"

size_t spd::strlen(const char* str) {
	size_t len = 0ull;
	while (*str++ != '\0') {
		++len;
	}

	return len;
}
