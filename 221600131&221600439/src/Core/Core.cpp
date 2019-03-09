// Core.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Core.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <list> 


#define EXTERN extern "C" __declspec(dllexport)

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
	return c == ' ' || !isLetter(c);
}

constexpr bool isCharacter(const char c) {
	return (isLetter(c) || isCr(c) || isLf(c) || c == ' ' || c == '\t');
}

std::string getPharse(std::list<std::string> &list) {
	std::string ret = "";
	for (auto &it : list) {
		ret += it + " ";
	}
	ret.erase(ret.end() - 1);
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
	READ_INVALID_WORD,
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
	std::ifstream file(config.in);
	std::string token = ""; // 不想做动态分配内存，std::string省事 
	size_t wordLength = 0; // <= wordAtLeastCharacterCount，超过则不再计数
	auto map = std::map<std::string, size_t>();
	file >> std::noskipws;

	if (!file.good()) {
		ret.errorCode = WORDCOUNTRESULT_OPEN_FILE_FAILED;
		return ret;
	}

	ReadingStatus currentStatus = ALREADY;
	WordStatus wordStatus = NONE;
	std::list<std::string> pharse;

	while (runStateMachine) {
		prev = c;
		file >> c;
		if (file.eof()) {
			runStateMachine = false; // 文件读取结束，不立即退出，处理一下之前未整理干净的状态
			c = 0;
		}

		if (c >= 'A' && c <= 'Z') {
			c = c - 'A' + 'a';
		}

		bool switchStatusInCurrentToken = true;
		// 直接把token和parse做在一起，懒得拆开了
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
				else if (isSeparator(c)) { // 正常， do nothing
				}
				else { // 此处要抛错
				}
				break;
			case ReadingStatus::READING_PAPER_INDEX:
				if (isNumber(c)) {
					token += c;
				}
				else if (isSeparator(c)) { // 编号读完，状态转换开始
					token = ""; // 这个编号数据没啥用，我也不知道读了干啥
					currentStatus = WAITING_FOR_TITLE;
				}
				else { // 此处要抛错
				}
				break;
			case WAITING_FOR_TITLE:
				if (isSeparator(c) && c != ':') { // 可能是还没读完Title，也可能是已经读完了
					if (token == "title:") { // 读完了
						currentStatus = FINDING_WORD_START;
						wordStatus = TITLE;
						token = "";
					}
				}
				else {
					token += c; // 暂不判断title:是否完全正确，假设其规范；之后加入错误提示
				}
				break;
			case WAITING_FOR_ABSTRACT:
				if (isSeparator(c) && c != ':') { // 同title
					if (token == "abstract:") { // 读完了
						currentStatus = FINDING_WORD_START;
						wordStatus = ABSTRACT;
						token = "";
					}
				}
				else {
					token += c;
				}
				break;
			case FINDING_WORD_START:
				if (isCharacter(c)) {
					ret.characters++;
					if (isLetter(c)) {
						token += c;
						if (wordLength <= wordAtLeastCharacterCount) {
							if (!isAlphabet(c)) {// abc123不算单词
								token = "";
								wordLength = 0;
								currentStatus = READ_INVALID_WORD;
							}
							else {
								wordLength++;
							}
						}
						else {
							currentStatus = READ_WORD;
						}
					}
					else {
						token = "";
						wordLength = 0;
						currentStatus = READ_WORD_END;
						switchStatusInCurrentToken = true;
						continue;
					}
				}
				else {
					token = "";
					wordLength = 0;
					currentStatus = READ_INVALID_WORD;
				}
				break;
			case READ_INVALID_WORD: // 处理不算单词的单词，等待下一个单词
				if (isCharacter(c)) {
					ret.characters++;
				}
				if (isSeparator(c)) {
					currentStatus = READ_WORD_END;
					switchStatusInCurrentToken = true;
					continue;
				}
				if (isLf(c)) {
					ret.lines++;
				}
				break;
			case READ_WORD: // 确定已经是单词了，继续搞
				if (isCharacter(c)) {
					ret.characters++;
				}
				if (isLetter(c)) { // 仍然是字母的情况下，继续读
					token += c;
				}
				else { // 不是字母了，开始处理剩下的了
					currentStatus = READ_WORD_END;
					switchStatusInCurrentToken = true;
					continue;
				}
				break;
			case READ_WORD_END:
				if (isLetter(c)) {  // 判定当前是否是个单词，是单词就不处于END状态
					currentStatus = FINDING_WORD_START;
					switchStatusInCurrentToken = true;
					continue;
				}

				bool noNextWord = isLf(c) || !runStateMachine;
				bool shouldPharseRunTwice = noNextWord; // abstract和title不共用pharse，暂时以Lf为其分隔符。如果pharse要清空，就再运行一次。

				if (token != "") {
					if (config.statByPharse) {
						while (true) {
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
							pharse.push_back(token);
							if (shouldPharseRunTwice) {
								shouldPharseRunTwice = false;
							}
							else {
								break;
							}
						}
					}
					else {

						// 略微重复代码，建议抽象成宏
						if (map.find(token) == map.end()) {
							map[token] = 0;
							ret.uniqueWordsOrPharses++;
						}

						if (config.useDifferentWeight) {
							if (wordStatus == TITLE) {
								map[token] += titleWeight;
							}
							else {
								map[token] += 1;
							}
						}
						else {
							map[token]++;
						}
					}

				}

				ret.words++;
				token = "";
				wordLength = 0;

				if (noNextWord) { // 如果确定单词读完了，后续没单词了，就切换状态
					ret.lines++;
					pharse.clear();
					if (wordStatus == TITLE) {
						currentStatus = WAITING_FOR_ABSTRACT;
					}
					else {
						currentStatus = ALREADY;
					}
				}
				break;
			}
		}
	}

	file.close();

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
