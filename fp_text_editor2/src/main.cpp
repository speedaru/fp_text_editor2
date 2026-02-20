#include "common.h"
#include "stl/vector_unit_tests.hpp"
#include "stl/gap_buff_unit_tests.hpp"

#include "core/editor.hpp"
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;


static void DumpEditor(const spd::Editor& editor, const fs::path& path) {
	std::ofstream file(path, std::ios::binary);
	if (!file) {
		LOG_E("failed to open file: %s\n", path.string().c_str());
		return;
	}

	// get editor data
	spd::Vector<uint8_t> editorData;
	editor.GetData(editorData);

	// write editor data to file
	file.write((char*)editorData.Data(), editorData.Size());
	LOG_D("wrote editor data to in file: %s\n", path.string().c_str());

	// write editor data to console
	std::cout.write((char*)editorData.Data(), editorData.Size());
}

int main() {
	logging::LoggerInit("logging.txt");

	spd::unit_test::Vector();
	spd::unit_test::GapBufferTests();

	spd::Editor editor;
	editor.Run();

	DumpEditor(editor, "editor_text.txt");

	logging::LoggerShutdown();
}