#include "pch.h"
#include "../Core/Core.h"
#include "CLI11.hpp"

#include <iostream>
#include <string>

int main(int argc, const char** argv)
{
	using std::cout;
	using std::cerr;
	using std::endl;
	using std::string;

	CLI::App app{ "Word Count" };

	std::string in = "default";
	std::string out = "default";
	bool useWidth = false;
	size_t pharseLength = 0;
	size_t exportWordLines = 10;
	app.add_option("-i", in, "Input file")->required(true);
	app.add_option("-o", out, "Output file")->required(true);
	app.add_option("-w", useWidth, "Use width");
	app.add_option("-m", pharseLength, "Pharse");
	app.add_option("-n", exportWordLines, "Top x words");

	CLI11_PARSE(app, argc, argv);


	auto a = CalculateWordCount(in.c_str());
	if (a.errorCode == WORDCOUNTRESULT_SUCCEED) {
		cout << "characters: " << a.characters << endl;
		cout << "words: " << a.words << endl;
		cout << "lines: " << a.lines << endl;
		for (size_t i = 0; i < min(a.uniqueWords, exportWordLines); i++) {
			cout << a.wordAppears[i].word << ": " << a.wordAppears[i].count << endl;
		}
	}
	else {
		switch (a.errorCode) {
		case WORDCOUNTRESULT_OPEN_FILE_FAILED:
			cerr << "Open file failed";
			exit(255);
			break;
		}
	}
	ClearWordAppear(&a);

	return 0;
}