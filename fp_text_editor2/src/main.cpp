#include "common.h"
#include <fstream>
#include <filesystem>
#include <iterator>
namespace fs = std::filesystem;

#include "stl/vector_unit_tests.hpp"
#include "stl/gap_buff_unit_tests.hpp"

#include "core/editor.hpp"
#include "core/renderer.hpp"


static void DumpEditor(const spd::Editor& editor, const fs::path& path);

static void LoadEditorData(spd::Editor& editor, const fs::path& path);


int main(int argc, char** argv) {
	logging::LoggerInit("logging.txt");

	//spd::unit_test::Vector();
	//spd::unit_test::GapBufferTests();

	spd::Editor editor;
	
	// load file
	fs::path savePath("editor_text.txt");
	if (argc > 1) {
		savePath = fs::path(argv[1]);
		LoadEditorData(editor, savePath);
	}

	editor.Run();

	DumpEditor(editor, savePath);

	logging::LoggerShutdown();
}


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

void LoadEditorData(spd::Editor& editor, const fs::path& path) {
	if (!fs::exists(path)) {
		return;
	}

	std::ifstream file(path);
	if (!file.is_open()) {
		LOG_E("failed to open %s\n", path.string().c_str());
		return;
	}

	// get number of chars
	size_t charCount = std::distance(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	file.seekg(std::ios::beg);

	spd::Vector<char> fileData(charCount);
	fileData.Resize(charCount);

	file.read(fileData.Data(), charCount);
	editor.LoadData(fileData);
}
