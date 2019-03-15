#include "pch.h"
#include <iostream>
#include<string>
#include<fstream>
#include<map>
#include<algorithm>
#include<cctype>
#include<cstdio>
using namespace std;

int countCharacter(string f) {
	int ascii = 0;
	//	int space = 0, enter = 0, tab = 0;
	ifstream read;
	read.open(f, ios::in);

	char c;
	while (!read.eof()) {
		read >> c;
		if (c >= 0 && c <= 127)
			ascii++;
		//可以扩展为一次读取判断其余 空格，水平制表符，换行符，等字符的计算
/*		switch (c)
		{
		case ' ':space++; break;
		case'\n':enter++; break;
		case '\t':tab++; break;
		}
		*/

	}
	read.close();
	return ascii;



}

int countLine(string f) {
	ifstream input(f, ios::in);
	string eachline;
	int line = 0;
	while (getline(input, eachline))
	{
		if (!eachline.empty())
			line++;
	}
	input.close();
	return line;
}
//isalpha(s[0]) && isalpha(s[1]) && isalpha(s[2]) && isalpha(s[3])Expression: c >= -1 && c <= 255
bool isLetter(char s) {
	if ((s >= 'a'&&s <= 'z') || (s >= 'A'&&s <= 'Z'))
		return true;
	else
		return false;
}
//转化为小写并删除除了数字字母外的ascii
void tolowerString(string &s) {
	for (int i = 0; i < s.size(); ++i) {
		if (isLetter(s[i]))
			s[i] = tolower(s[i]);
		else if (s[i]<'0' || s[i]>'9') {
			s.erase(i, 1);
		}
	}
}

bool isword(string s) {
	if (s.size() >= 4) {
		if (isLetter(s[0]) && isLetter(s[1]) && isLetter(s[2]) && isLetter(s[3]))//isalpha(s[0]) && isalpha(s[1]) && isalpha(s[2]) && isalpha(s[3])Expression: c >= -1 && c <= 255
			return true;
	}

	else
		return false;
}
bool isTitle(string s) {
	if (s._Equal("Title:"))
		return true;
	else
		return false;
}
bool isAbstract(string s) {
	if (s._Equal("Abstract:"))
		return true;
	else
		return false;
}
int countWords(string f) {
	int wordNum = 0;
	ifstream input;
	input.open(f, ios::in);
	string aline;
	string content;
	string::size_type start = 0;
	string::size_type end = aline.find_first_of(" ");//空格作为单词分隔符

	while (getline(input, aline))
	{
		//为了避免溢出，保存一个string对象size的最安全的方法就是使用标准库类型string：：size_type
		string::size_type start = 0;
		string::size_type end = aline.find_first_of(" ");//空格作为单词分隔符
		while (end != string::npos) //npos就是这一行到头啦；
		{
			string content = aline.substr(start, end - start);
			if (isword(content))//这个单词从未出现
				wordNum++;
			start = end + 1;
			end = aline.find_first_of(" ", start);//空格作为单词分隔符
		}

	}


	input.close();
	return wordNum;
}
void sortwordCount(string f, string resultTxt) {

	ofstream out(resultTxt, ios::app);
	ifstream input;

	input.open(f, ios::in);
	string eachline;
	map<string, int> mapA; //第一个存单词,第二个存单词出现的次数;

	while (getline(input, eachline))
	{
		//为了避免溢出，保存一个string对象size的最安全的方法就是使用标准库类型string：：size_type
		string::size_type start = 0;
		string::size_type end = eachline.find_first_of(" ");//空格作为单词分隔符
				///不统计title和abstract
		if (isAbstract(eachline.substr(start, end - start)) || isTitle(eachline.substr(start, end - start))) {
			start = end + 1;
			end = eachline.find_first_of(" ", start);//空格作为单词分隔符
		}
		while (end != string::npos) //npos就是这一行到头啦；
		{
			string content = eachline.substr(start, end - start);
			if (isword(content)) {
				tolowerString(content);//把content内容转换为小写 便于输出和统计

				//if (!isLetter(content[end])&&!isdigit(content[end]))
				//	content.erase(content.end());
				map<string, int>::iterator it = mapA.find(content);
				if (it == mapA.end())//这个单词从未出现
					mapA.insert(pair<string, int>(content, 1));//赋值的时候只接受pair类型；
				else
					++it->second;//单词存在

			}
			start = end + 1;
			end = eachline.find_first_of(" ", start);//空格作为单词分隔符
		}

	}

	multimap<int, string, greater<int> > mapB;//按int排序的multimap

//转移mapA
	for (map<string, int>::iterator it1 = mapA.begin(); it1 != mapA.end(); ++it1)
	{
		mapB.insert(pair<int, string>(it1->second, it1->first));
	}


	//界面输出前十
	int i = 0;
	for (map<int, string>::iterator it2 = mapB.begin(); i < 10 && it2 != mapB.end(); ++it2, ++i)
		cout << "<" << it2->second << ">:" << it2->first << endl;
	//输出排序好的map

	for (map<int, string>::iterator it2 = mapB.begin(); it2 != mapB.end(); ++it2)
	{
		//		if ((it2->first) > 1)
		out << "<" << it2->second << ">:" << it2->first << endl;
	}

	out.close();
	input.close();
}
void countWordsWithWeight(string f, string resultTxt, int w) {
	ofstream out(resultTxt, ios::app);
	ifstream input;
	bool flag = false;
	input.open(f, ios::in);
	string eachline;
	map<string, int> mapA; //第一个存单词,第二个存单词出现的次数;

	while (getline(input, eachline))
	{

		//为了避免溢出，保存一个string对象size的最安全的方法就是使用标准库类型string：：size_type
		string::size_type start = 0;
		string::size_type end = eachline.find_first_of(" ");//空格作为单词分隔符
		flag = isTitle(eachline.substr(start, end - start));
		///不统计title和abstract
		if (isAbstract(eachline.substr(start, end - start)) || isTitle(eachline.substr(start, end - start))) {
			start = end + 1;
			end = eachline.find_first_of(" ", start);//空格作为单词分隔符
		}
		while (end != string::npos) //npos就是这一行到头啦；
		{
			string content = eachline.substr(start, end - start);

			if (isword(content)) {
				tolowerString(content);//把content内容转换为小写 便于输出和统计

				//if (!isLetter(content[end])&&!isdigit(content[end]))
				//	content.erase(content.end());
				map<string, int>::iterator it = mapA.find(content);
				if (it == mapA.end())//这个单词从未出现
				{
					mapA.insert(pair<string, int>(content, 1));//赋值的时候只接受pair类型；
				}

				else {
					if (w == 0 || flag == false)
						++it->second;//单词存在
					else if (w == 1 && flag == true)
					{
						it->second += 10;//单词存在+= 10;
					}

				}




			}
			start = end + 1;
			end = eachline.find_first_of(" ", start);//空格作为单词分隔符
		}

	}

	multimap<int, string, greater<int> > mapB;//按int排序的multimap

//转移mapA
	for (map<string, int>::iterator it1 = mapA.begin(); it1 != mapA.end(); ++it1)
	{
		mapB.insert(pair<int, string>(it1->second, it1->first));
	}


	//界面输出前十
	int i = 0;
	for (map<int, string>::iterator it2 = mapB.begin(); i < 10 && it2 != mapB.end(); ++it2, ++i)
		cout << "<" << it2->second << ">:" << it2->first << endl;
	//输出排序好的map

	for (map<int, string>::iterator it2 = mapB.begin(); it2 != mapB.end(); ++it2)
	{
		//		if ((it2->first) > 1)
		out << "<" << it2->second << ">:" << it2->first << endl;
	}

	out.close();
	input.close();
}

void countGroupWordsWithLength(string f, string resultTxt, int n) {
	ofstream out(resultTxt, ios::app);
	ifstream input;
	string content;
	input.open(f, ios::in);
	string eachline;
	map<string, int> mapA; //第一个存单词,第二个存单词出现的次数;
	int cntNum = 0;
	int i = 0;
	while (getline(input, eachline))
	{
		//为了避免溢出，保存一个string对象size的最安全的方法就是使用标准库类型string：：size_type
		string::size_type start = 0;
		string::size_type end = eachline.find_first_of(" ");//空格作为单词分隔符
						///不统计title和abstract
		if (isAbstract(eachline.substr(start, end - start)) || isTitle(eachline.substr(start, end - start))) {
			start = end + 1;
			end = eachline.find_first_of(" ", start);//空格作为单词分隔符
		}

		content = eachline.substr(start, end - start);
		for (i = 0; i < content.size() && cntNum < n; ++i) {
			if (content[i] == ' ')
				cntNum++;
		}
		end = end + i;



		while (end != string::npos) //npos就是这一行到头啦；
		{
			content = eachline.substr(start, end - start);
			if (isword(content)) {
				tolowerString(content);//把content内容转换为小写 便于输出和统计

				//if (!isLetter(content[end])&&!isdigit(content[end]))
				//	content.erase(content.end());
				map<string, int>::iterator it = mapA.find(content);
				if (it == mapA.end())//这个单词从未出现
					mapA.insert(pair<string, int>(content, 1));//赋值的时候只接受pair类型；
				else
					++it->second;//单词存在

			}
			start = end + 1;
			for (i = 0; i < content.size() && cntNum < n; ++i) {
				if (content[i] == ' ')
					cntNum++;
			}
			end = end + i;
		}

	}

	multimap<int, string, greater<int> > mapB;//按int排序的multimap

//转移mapA
	for (map<string, int>::iterator it1 = mapA.begin(); it1 != mapA.end(); ++it1)
	{
		mapB.insert(pair<int, string>(it1->second, it1->first));
	}


	//界面输出前十
	i = 0;
	for (map<int, string>::iterator it2 = mapB.begin(); i < 10 && it2 != mapB.end(); ++it2, ++i)
		cout << "<" << it2->second << ">:" << it2->first << endl;
	//输出排序好的map

/*	for (map<int, string>::iterator it2 = mapB.begin(); it2 != mapB.end(); ++it2)
	{
		//		if ((it2->first) > 1)
		out << "<" << it2->second << ">:" << it2->first << endl;
	}

	out.close();*/
	input.close();
}
string inputTxt = "input.txt";
string resultTxt = "result.txt";
int main(int argc, char*argv[])
{

	string inputTxt = "";
	string resultTxt = "";
	bool ism = false;
	int length = 0;
	for (int i = 0; i < argc; ++i) {
		if (string(argv[i]) == "-i") {
			cout << "命令为-i" << endl;
			inputTxt = argv[i + 1];
			cout << "input路径被修改为" << inputTxt << endl;

		}
		else if (string(argv[i]) == "-o") {

			cout << "命令为-o" << endl;
			resultTxt = argv[i + 1];
			cout << "result路径被修改为" << resultTxt << endl;

		}
		else if (string(argv[i]) == "-w") {

			if (string(argv[i + 1]) == "1") {
				cout << "命令为-w" << argv[i + 1] << endl;
				countWordsWithWeight(inputTxt, resultTxt, 1);//排序词频 屏幕输出前十
			}
			else if (string(argv[i + 1]) == "0") {
				sortwordCount(inputTxt, resultTxt);//排序词频 屏幕输出前十 文件写入词频统计
			}

		}
		else if (string(argv[i]) == "-m") {
			ism = true;
		}
	}
	if (ism == false) {
		cout << "characters:" << countCharacter(inputTxt) << endl;
	}
	/*
		string file = inputTxt;
		ofstream out(resultTxt);


		cout << "characters:" << countCharacter(file) << endl;

		cout << "words:" << countWords(file) << endl;

		cout << "lines:" << countLine(file) << endl;
		out << "characters:" << countCharacter(file) << endl;
		out << "words:" << countWords(file) << endl;
		out << "lines:" << countLine(file) << endl;
		sortwordCount(file, resultTxt);//排序词频 屏幕输出前十 文件写入词频统计
		cout << endl;
		cout << endl;
		cout << endl;
		cout << "开启权重统计的结果如下" << endl;
		//	countWordsWithWeight (file, resultTxt,1);//排序词频 屏幕输出前十
		cout << endl;
		cout << endl;
		cout << endl;
		//	countGroupWordsWithLength(file, 2);
		*/
	return 0;
}

