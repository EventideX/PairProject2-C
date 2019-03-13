// WordCount.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <string>
using namespace std;

int times[100005] = {};
string word[100005] = {};
bool w = false;
int n=10;
int m = 1;

int countChar(FILE *file) {
	fseek(file, 0, SEEK_SET);
	int cnt = 0;
	int line = 1;
	char c;
	while (fscanf_s(file, "%c", &c, sizeof(char)) != EOF) {	
		if (c == '\n') {
			if (line == 1) {
				cnt-=8;
				line++;
				continue;
			}
			else if (line == 2) {
				cnt -= 10;
				line++;
			}
			else if (line == 3) {
				line++;
			}
			else if (line == 4) {
				line++;
				continue;
			}
			else {
				line = 1;
				continue;
			}
		}
		cnt++;
	}
	return cnt;
}

int countWord(FILE *file) {
	fseek(file, 0, SEEK_SET);
	int cnt = 0;
	int alacnt = 0;
	int line = 1;
	char c, ignore;
	while (fscanf_s(file, "%c", &c, sizeof(char)) != EOF) {
		if (c == '\n') {
			if (line == 1) {
				for (int i = 0; i < 7; i++) fscanf_s(file, "%c", &ignore, sizeof(char));
				line++;
				continue;
			}
			else if (line == 2) {
				for (int i = 0; i < 10; i++) fscanf_s(file, "%c", &ignore, sizeof(char));
				line++;
			}
			else if (line == 3) {
				line++;
			}
			else if (line == 4) {
				line++;
				continue;
			}
			else {
				line = 1;
				continue;
			}
		}
		if (isalpha(c) || isdigit(c) && alacnt >= 4) alacnt++;
		if (alacnt < 4 && !isalpha(c)) alacnt = 0;
		if (alacnt >= 4 && !(isalpha(c) || isdigit(c))) alacnt = 0;
		if (alacnt == 4) cnt++;
	}
	return cnt;
}

int countLine(FILE *file) {
	fseek(file, 0, SEEK_SET);
	int cnt = 0;
	int line = 1;
	char c, ignore;
	bool isline = false;
	while (fscanf_s(file, "%c", &c, sizeof(char)) != EOF) {
		if (c == '\n') {
			if (line == 1) {
				for (int i = 0; i < 7; i++) fscanf_s(file, "%c", &ignore, sizeof(char));
				line++;
				continue;
			}
			else if (line == 2) {
				for (int i = 0; i < 10; i++) fscanf_s(file, "%c", &ignore, sizeof(char));
				line++;
				isline = false;
			}
			else if (line == 3) {
				line++;
				isline = false;
			}
			else if (line == 4) {
				line++;
				continue;
			}
			else {
				line = 1;
				continue;
			}
		}
		if (!(c >= 0 && c <= 32 || c == 127) && !isline&&line!=1) {
			isline = true;
			cnt++;
		}
	}
	return cnt;
}

int countFre(FILE *file) {
	fseek(file, 0, SEEK_SET);
	int line = 1;
	char c,ignore;
	string temp = "";
	int alacnt = 0;
	int w_cnt = 0;
	int m_cnt = 0;
	while (fscanf_s(file, "%c", &c, sizeof(char)) != EOF) {
		if (c == '\n') {
			if (line == 1) {
				for (int i = 0; i < 7; i++) fscanf_s(file, "%c", &ignore, sizeof(char));
				line++;
				continue;
			}
			else if (line == 2) {
				for (int i = 0; i < 10; i++) fscanf_s(file, "%c", &ignore, sizeof(char));
				line++;
			}
			else if (line == 3) {
				line++;
			}
			else if (line == 4) {
				line++;
				continue;
			}
			else {
				line = 1;
				continue;
			}
		}
		if (isalpha(c) || isdigit(c) && alacnt >= 4) {
			temp += c;
			alacnt++;
		}
		else if (m_cnt > 0 && alacnt == 0 && !isalpha(c) && !isdigit(c)) {
			temp += c;
		}
		else if (alacnt < 4 && !isalpha(c)) {
			temp = "";
			m_cnt = 0;
			alacnt = 0;
		}
		else if (alacnt >= 4 && !(isalpha(c) || isdigit(c))) {
			m_cnt++;
			if (m_cnt == m) {
				bool found = false;
				for (int i = 0; i < (int)temp.length(); i++)
					temp[i] = tolower(temp[i]);
				for (int i = 0; i < w_cnt; i++) {
					if (temp == word[i]) {
						if (w && (line == 2 || line == 3 && c == '\n')) times[i] += 10;
						else times[i]++;
						found = true;
						break;
					}
				}
				if (!found) {
					word[w_cnt] = temp;
					if (w && (line == 2 || line == 3 && c == '\n')) times[w_cnt] += 10;
					else times[w_cnt]++;
					w_cnt++;
				}
				int pos = 0;
				for (int i = 0; i < (int)temp.length(); i++) {
					if (!isdigit(temp[i]) && !isalpha(temp[i])) {
						pos = i;
						break;
					}
				}
				temp = string(temp, pos + 1);
				m_cnt--;
			}
			temp += c;
			alacnt = 0;
		}
		if (line == 3 && c == '\n') {
			m_cnt = 0;
			temp = "";
		}
	}
	if (alacnt >= 4) {
		m_cnt++;
		if (m_cnt == m) {
			bool found = false;
			for (int i = 0; i < (int)temp.length(); i++)
				temp[i] = tolower(temp[i]);
			for (int i = 0; i < w_cnt; i++) {
				if (temp == word[i]) {
					if (w && (line == 2 || line == 3 && c == '\n')) times[i] += 10;
					else times[i]++;
					found = true;
					break;
				}
			}
			if (!found) {
				word[w_cnt] = temp;
				if (w && (line == 2 || line == 3 && c == '\n')) times[w_cnt] += 10;
				else times[w_cnt]++;
				w_cnt++;
			}
		}
	}
	for (int i = 0; i < w_cnt - 1; i++) {
		for (int j = 0; j < w_cnt - 1 - i; j++) {
			if (times[j] > times[j + 1] || times[j] == times[j + 1] && word[j] < word[j + 1]) {
				int temp;
				temp = times[j];
				times[j] = times[j + 1];
				times[j + 1] = temp;
				string s_temp;
				s_temp = word[j];
				word[j] = word[j + 1];
				word[j + 1] = s_temp;
			}
		}
	}
	return w_cnt;
}

int main(int argc, char* argv[])
{
	FILE *in, *out;
	errno_t err;
	char *From=nullptr, *To=nullptr,*Weight=nullptr,*Number=nullptr,*M=nullptr;
	for (int i = 1; i < argc; i += 2) {
		if (argv[i][1] == 'i') {
			From = argv[i + 1];
		}
		else if (argv[i][1] == 'o') {
			To = argv[i + 1];
		}
		else if (argv[i][1] == 'w') {
			Weight = argv[i + 1];
			if (strlen(argv[i + 1]) > 1 || argv[i + 1][0] != '0'&&argv[i + 1][0] != '1') printf("Error!\n");
			else if (argv[i + 1][0] == '1') w = true;
		}
		else if (argv[i][1] == 'n') {
			Number = argv[i + 1];
			if (sscanf_s(Number, "%d", &n) == -1) printf("Error!\n");
		}
		else if (argv[i][1] == 'm') {
			M = argv[i + 1];
			if (sscanf_s(M, "%d", &m) == -1) printf("Error!\n");
		}
		else printf("Error!\n");
	}
	if ((err = fopen_s(&in, From, "r")) != 0) {
		printf("Open file failed!");
		exit(0);
	}
	if ((err = fopen_s(&out, To, "w")) != 0) {
		printf("Open file failed!");
		exit(0);
	}
	int w_cnt = countFre(in);
	fprintf(out, "characters: %d\n", countChar(in));
	fprintf(out, "words: %d\n", countWord(in));
	fprintf(out, "lines: %d\n", countLine(in));
	for (int i = w_cnt - 1; i >= 0; i--) {
		fprintf(out, "<%s>: %d\n", word[i].c_str(), times[i]);
		if (i == w_cnt - n) break;
	}
	fclose(in);
	return 0;
}

