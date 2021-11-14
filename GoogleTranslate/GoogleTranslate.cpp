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
	Dictionary dict;
	//ifstream str;
	RequestProcessor processor(cin,dict);
	//RequestProcessor processor(str, dict);
	processor.ProcessStream();
}
