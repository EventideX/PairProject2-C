// Core.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Core.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <list> 


#define EXTERN extern "C" __declspec(dllexport)

struct WordInPharse {
	std::string word;
	std::string separator;
};

constexpr size_t wordAtLeastCharacterCount = 4;
constexpr int titleWeight = 10;

constexpr bool isAlphabet(const char c) {
	return (c >= 'a' && c <= 'z');
}

constexpr bool isNumber(const char c) {
	return (c >= '0' && c <= '9');
}

constexpr bool isLetter(const char c) {
	return (isAlphabet(c) || isNumber(c));
}

constexpr bool isLf(const char c) {
	return (c == '\n');
}

constexpr bool isCr(const char c) {
	return (c == '\r');
}

constexpr bool isSeparator(const char c) {
	return !isLetter(c);
}

constexpr bool isEmptyChar(const char c) {
	return c <= 32 && c != 127; // 当>128时，溢出，c < 0，于是被视为empty char
}

constexpr bool isCharacter(const char c) {
	return !isEmptyChar(c) || c == ' ' || c == '\t' || c == '\n';
}

std::string getPharse(std::list<WordInPharse> &list) {
	std::string ret = "";
	size_t i = 0;
	for (auto &it : list) {
		if (i == 0) {
			ret += it.word;
		}
		else {
			ret += it.separator + it.word;
		}
		i++;
	}
	// ret.erase(ret.end() - 1);
	return ret;
}

typedef std::pair<std::string, size_t> WordCountPair;

enum ReadingStatus {
	ALREADY,
	DONE,
	READING_PAPER_INDEX,
	WAITING_FOR_TITLE,
	FINDING_WORD_START,
	READ_WORD,
	READ_WORD_END,
	WAITING_FOR_ABSTRACT,
};

enum WordStatus {
	NONE,
	TITLE,
	ABSTRACT
};

EXTERN WordCountResult CalculateWordCount(struct WordCountConfig config)
{
	auto ret = WordCountResult();
	bool runStateMachine = true;
	char prev = 0, c = 0;
	std::string word = ""; // 不想做动态分配内存，std::string省事 
	std::string separator = "";
	std::string token = "";
	size_t wordLength = 0; // <= wordAtLeastCharacterCount，超过则不再计数
	auto map = std::unordered_map<std::string, size_t>();
	bool isValidWordStart = false;

	FILE* f;
	if (fopen_s(&f, config.in, "rb") != 0) {
		ret.errorCode = WORDCOUNTRESULT_OPEN_FILE_FAILED;
		return ret;
	}
	fseek(f, 0, SEEK_END);
	long fileLength = ftell(f);
	fseek(f, 0, SEEK_SET);
	char * string = (char*)malloc(fileLength + 1);
	fread(string, fileLength, 1, f);
	fclose(f);
	string[fileLength] = 0;

	ReadingStatus currentStatus = ALREADY;
	WordStatus wordStatus = NONE;
	std::list<WordInPharse> pharse;

	size_t currentPosition = 0;
	while (runStateMachine) {
		prev = c;
		c = string[currentPosition];
		if (currentPosition == fileLength) {
			runStateMachine = false; // 文件读取结束，不立即退出，处理一下之前未整理干净的状态
			c = 0;
		}
		else {
			currentPosition++;
		}

		if (c >= 'A' && c <= 'Z') {
			c = c - 'A' + 'a';
		}

		bool switchStatusInCurrentToken = true;
		// 直接把read token和parse做在一起，懒得拆开了
		while (switchStatusInCurrentToken) {
			switchStatusInCurrentToken = false;
			// 避免这个大switch的方法是把这个状态转换写成一个类
			// 不过没啥必要，不考虑后续维护
			switch (currentStatus) {
			case ALREADY:
				if (isNumber(c)) {
					currentStatus = READING_PAPER_INDEX;
					switchStatusInCurrentToken = true;
					continue;
				}
				// else if (isEmptyChar(c)) { // 正常， do nothing
				// }
				else { // @TODO: 此处要抛错
				}
				break;
			case READING_PAPER_INDEX:
				if (isNumber(c)) {
					token += c;
				}
				else if (isEmptyChar(c)) { // 编号读完，状态转换开始
					token = ""; // 这个编号数据没啥用，我也不知道读了干啥
					currentStatus = WAITING_FOR_TITLE;
				}
				else { // @TODO: 此处要抛错
				}
				break;
			case WAITING_FOR_TITLE:
				if (isEmptyChar(c) && c != ':') { // 可能是还没读完Title，也可能是已经读完了
					if (token == "title:") { // 读完了
						isValidWordStart = true;
						currentStatus = FINDING_WORD_START;
						wordStatus = TITLE;
						token = "";
					}
					else {  // @TODO: 此处要抛错
					}
				}
				else {
					token += c; // 暂不判断title:是否完全正确，假设其规范；之后加入错误提示
				}
				break;
			case WAITING_FOR_ABSTRACT:
				if (isEmptyChar(c) && c != ':') { // 同title
					if (token == "abstract:") { // 读完了
						isValidWordStart = true;
						currentStatus = FINDING_WORD_START;
						wordStatus = ABSTRACT;
						token = "";
					}
					else {  // @TODO: 此处要抛错
					}
				}
				else {
					token += c;
				}
				break;
			case FINDING_WORD_START:
				if (isLetter(c)) {
					if (wordLength == 0) {
						separator = token;
						token = "";
					}
					// 后半部分判断是为了处理01abcdefg这种情况
					if ((wordLength > 0 && wordLength < wordAtLeastCharacterCount) || (wordLength == 0 && isValidWordStart)) {
						if (isAlphabet(c)) {
							wordLength++;
							if (wordLength == wordAtLeastCharacterCount) {
								currentStatus = READ_WORD;
								switchStatusInCurrentToken = true;
							}
							else {
								ret.characters++;
								word += c;
							}
							continue;
						}
					}
				}

				if (config.statByPharse) {// 单词长度不达标则清空词组
					if (wordLength > 0) {
						pharse.clear();
					}
				}

				isValidWordStart = false;
				word = "";
				wordLength = 0;
				currentStatus = READ_WORD_END;
				switchStatusInCurrentToken = true;
				continue;
				break;
			case READ_WORD: // 确定已经是单词了，继续搞
				if (isLetter(c)) { // 仍然是字母的情况下，继续读
					word += c;
					ret.characters++; // 非单词的情况下字符统计交给READ_WORD_END
				}
				else { // 不是字母了，开始处理剩下的了
					currentStatus = READ_WORD_END;
					switchStatusInCurrentToken = true;
					continue;
				}
				break;
			case READ_WORD_END:

				if (word != "") {
					ret.words++; // 这个时候就能确定读到了一个完整的单词了
					if (config.statByPharse) {
						pharse.push_back(WordInPharse{
							word = word,
							separator = separator
						}); 
						if (pharse.size() == config.pharseSize) {
							auto pharseString = getPharse(pharse);
							if (map.find(pharseString) == map.end()) {
								map[pharseString] = 0;
								ret.uniqueWordsOrPharses++;
							}
							if (config.useDifferentWeight) {
								if (wordStatus == TITLE) {
									map[pharseString] += titleWeight;
								}
								else {
									map[pharseString] += 1;
								}
							}
							else {
								map[pharseString]++;
							}

							pharse.pop_front();
						}
					}
					else {

						// 略微重复代码，建议抽象成宏
						if (map.find(word) == map.end()) {
							map[word] = 0;
							ret.uniqueWordsOrPharses++;
						}

						if (config.useDifferentWeight) {
							if (wordStatus == TITLE) {
								map[word] += titleWeight;
							}
							else {
								map[word] += 1;
							}
						}
						else {
							map[word]++;
						}
					}

					isValidWordStart = false;
				}

				word = "";
				wordLength = 0;

				if (isLf(c) || !runStateMachine) { // 如果是个换行符，就可以切换状态是读TITLE还是读ABSTRACT了
					ret.lines++;
					pharse.clear();
					token = "";
					if (wordStatus == TITLE) {
						currentStatus = WAITING_FOR_ABSTRACT;
					}
					else {
						currentStatus = ALREADY;
					}
					if (isLf(c)) {
						ret.characters++;
					}
				}
				else { // 单词处理完成了，该等新的单词了。
					if (!isValidWordStart) {
						if (isSeparator(c)) {
							isValidWordStart = true;
							token += c;
						}
					}
					if (isCharacter(c)) {
						ret.characters++;
					}
					currentStatus = FINDING_WORD_START;
				}

				break;
			}
		}
	}

	auto sortedMap = std::vector<WordCountPair>(map.begin(), map.end());
	std::sort(sortedMap.begin(), sortedMap.end(), [](const WordCountPair& lhs, const WordCountPair& rhs) {
		if (lhs.second == rhs.second) {
			return lhs.first < rhs.first;
		}
		return lhs.second > rhs.second;
	});
	ret.wordAppears = new WordCountWordAppear[ret.uniqueWordsOrPharses];
	size_t i = 0;
	for (auto &it : sortedMap) {
		ret.wordAppears[i].word = new char[it.first.length() + 1];
		strcpy_s(ret.wordAppears[i].word, it.first.length() + 1, it.first.c_str());
		ret.wordAppears[i].count = it.second;
		i++;
	}

	return ret;

}


EXTERN void ClearWordAppear(WordCountResult * resultStruct)
{
	for (size_t i = 0; i < resultStruct->uniqueWordsOrPharses; i++) {
		delete resultStruct->wordAppears[i].word;
	}
	delete resultStruct->wordAppears;
}
