#include <iostream>

#include <common.h>
#include <stl/vector_unit_tests.hpp>


int main() {
	logging::LoggerInit("logging.txt");

	spd::unit_test::Vector();
	
	logging::LoggerShutdown();
}