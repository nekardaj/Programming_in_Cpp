#include "Dictionaries.h"
#ifndef HEADER_T
#define HEADER_T
#endif

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

using namespace std;

int main()
{
	/*
	map<string, int> dict;
	dict.insert(make_pair("aaa", 9));
	dict.insert(make_pair("aba", 9));
	dict.insert(make_pair("aab", 9));
	dict.insert(make_pair("bbaa", 9));
	for (auto&& x : dict)
	{
		cout << x.first << " : " << x.second;
	}

	auto it = dict.find("aaa");
	if (it != dict.end())
	{
	
	}
	*/
	Dictionary dict;
	RequestProcessor processor(cin,dict);
	processor.ProcessStream();


}
