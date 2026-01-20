#include <iostream>
#include "common.h"

int main() {
	logging::LoggerInit("logging.txt");

	int* ptr = SPD_ALLOC(int, 3);
	printf("hello: %s\n", __RELATIVE_FILE__);
	SPD_FREE(ptr);

	logging::LoggerShutdown();
}