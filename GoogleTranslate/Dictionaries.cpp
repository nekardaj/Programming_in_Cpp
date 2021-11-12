#include "Dictionaries.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
using  namespace  std;

enum command {add, del, del_all, find, find_pr, find_rev  };
RequestProcessor::RequestProcessor(std::istream& _stream, Dictionary& dictionary_) : stream(_stream), dictionaries(dictionary_)
{

}
/*
RequestProcessor::RequestProcessor(std::istream& _stream, Dictionary dictionary_) : stream(_stream), dictionaries(dictionary_)
{
	
}
*/
void RequestProcessor::ProcessStream()
{
	for (;;)
	{
		if(stream.fail())
		{
			break;
		}
		ProcessRequest();
	}
}
void RequestProcessor::ProcessRequest()
{
	string line;
	getline(stream, line);
	//cout << line << endl;
	if (stream.fail())
	{
		return;
	}
	stringstream linestream(line);
	string cmd;
	linestream >> cmd;
	if(stream.fail()) //no command
	{
		return;
	}
	command command;
	//only add may have any number of args
	if(cmd == "add")
	{
		string word;
		linestream >> word;
		if(linestream.fail())
		{
			return;
		}
		vector<string> translations;
		do
		{
			string translation;
			
			linestream >> translation;
			translations.push_back(translation);
		}
		while (!linestream.fail()); //read translations until stream fails
		if(!translations.empty())
		{
			dictionaries.Add(word, translations);
		}
		command = add;
	}

	string cmd1;
	linestream >> cmd1;

	/*
	switch (cmd1)
	{
		case "del":
			break; 
	}
	*/
	//split
	//get word(prefix)

	// *get translations

	//call function to process cmd
}

void Dictionary::Pfind(const string& prefix)
{
	//auto it_pair = BackwardsDict.equal_range(prefix);
	//auto it_begin = it_pair.first;
	auto it_begin = BackwardsDict.lower_bound(prefix);
	for (;;++it_begin)
	{
		string key = it_begin->first;
		if(key.substr(0,prefix.size())==prefix) //contains prefix-> we want this
		{
			
		}


	}
}
void Dictionary::Add(const string& word, const vector<string>& translations)
{
	
}
void Dictionary::Find(const string& word)
{
	
}
void Dictionary::Remove(const string& word, const string& transl_tb_removed)
{
	
}




