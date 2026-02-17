#include "common.h"
#include "stl/vector_unit_tests.hpp"
#include "stl/gap_buff_unit_tests.hpp"

#include "core/editor.hpp"


int main() {
	logging::LoggerInit("logging.txt");

	//spd::unit_test::Vector();
	//spd::unit_test::GapBufferTests();

	spd::Editor editor;
	editor.Run();
	
	logging::LoggerShutdown();
}