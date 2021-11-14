#include "Dictionaries.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
using  namespace  std;

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
	command command = del;//gets overwritten or function returns before reading
	//only add may have any number of args
	if(!ParseCommand(command,cmd))
	{
		return;
	}
	
	switch (command)
	{

	case add:
	{
		string word;
		vector<string> translations;
		linestream >> word;
		if (linestream.fail())
		{
			return;
		}

		do
		{
			string translation;
			linestream >> translation;
			if(linestream.fail())
			{
				break;
			}
			translations.push_back(translation);
		} while (true); //read translations until stream fails
		if (!translations.empty())
		{
			dictionaries.Add(word, translations);
		}
		break;
	}
	case del:
	{
		string word;
		linestream >> word;
		if(linestream.fail())
		{
			return;
		}
		string translation;
		linestream >> translation;
		if(linestream.fail()) //no word given - delete whole word, pass empty string
		{
			dictionaries.Remove(word, "");
		}
		else
		{
			dictionaries.Remove(word, translation);
		}
		break;
	}
	case  command::find:
	{
		string word;
		linestream >> word;
		if(linestream.fail())
		{
			return;
		}
		dictionaries.Find(word);
		break;
	}
	
	case find_pr:
	{
		string prefix;
		linestream >> prefix;
		if (linestream.bad())
		{
			return;
		}
		dictionaries.Pfind(prefix);
		break;
	}
	case find_rev:
	{
		string translation;
		linestream >> translation;
		if(linestream.bad())
		{
			return;
		}
		dictionaries.Rfind(translation);
		break;
	}
		/*default:
			break;*/
	}
}

bool RequestProcessor::ParseCommand(command& command,const string& cmd) 
{
	if (cmd == "add")
	{
		command = add;
		return true;
	}
	if (cmd == "del")
	{
		command = del;
		return true;
	}
	if (cmd == "find")
	{
		command = command::find;
		return true;
	}
	if (cmd == "pfind")
	{
		command = find_pr;
		return true;
	}
	if (cmd == "rfind")
	{
		command = find_rev;
		return true;
	}
	return false;
}

void Dictionary::PrintWordTransl(const transl_map::iterator& entry)
{
	//cout << entry->first << ' ';
	for (auto&& tr : entry->second)
	{
		cout << tr << ' ';
	}
	cout << endl;
}

void Dictionary::Pfind(const string& prefix)
{
	//auto it_pair = BackwardDict.equal_range(prefix);
	//auto it_begin = it_pair.first;
	auto begin = ForwardDict.lower_bound(prefix);
	if(begin == ForwardDict.end())
	{
		return;
	}
	for (;begin != ForwardDict.end();++begin)
	{
		string key = begin->first;
		if(key.substr(0,prefix.size())==prefix) //word contains prefix
		{
			cout << key << ' ';
			PrintWordTransl(begin);
		}
		else //key > pref in first chars nothing after can be valid 
		{
			break;
		}

	}
}
void Dictionary::Add(const string& word, const vector<string>& translations)
{
	//TODO pass by value?
	//check whether there were translations before
	auto entry = ForwardDict.find(word);
	if(entry != ForwardDict.end())
	{
		entry->second.insert(entry->second.end(), translations.begin(), translations.end());
	}
	else
	{
		ForwardDict.insert(make_pair(word, translations));
	}
	for (auto&& tr : translations)
	{
		auto it = BackwardDict.find(tr);
		//create new enty in translations or add transl to existing ones
		if(it == BackwardDict.end())
		{
			BackwardDict.insert(make_pair(tr, vector<string>(1,word)));
		}
		else
		{
			it->second.push_back(word);
		}
	}
	
	
}
void Dictionary::Find(const string& word)
{
	auto transl = ForwardDict.find(word);
	if(transl != ForwardDict.end())
	{
		PrintWordTransl(transl);
	}
}
void Dictionary::Remove(const string& word, const string& transl_tb_removed)
{
	auto it = ForwardDict.find(word);
	if(it == ForwardDict.end()) //key wasnt in dict
	{
		return;
	}
	if(transl_tb_removed.empty()) //delete whole word
	{
		for (auto && tr : it->second)
		{
			auto transl = BackwardDict.find(tr);
			if (transl->second.size() == 1) //no other word translates to this - rm entry
			{
				BackwardDict.erase(transl);
			}
			else
			{
				for (auto i = transl->second.begin(); i < transl->second.end();++i)
				{
					if(*i == word)
					{
						transl->second.erase(i);
						break;
					}
				}
			}
		}
		ForwardDict.erase(it);
	}
	else //delete one transl
	{
		bool tr_missing = true;
		for (auto i = it->second.begin(); i != it->second.end();++i)
		{
			if(*i == transl_tb_removed) 
			{
				if (it->second.size() == 1) //last transl - remove word
				{
					ForwardDict.erase(it);
				}
				else
				{
					it->second.erase(i);
				}
				tr_missing = false;

				break;
			}
		}
		if(tr_missing) //translation is not present
		{
			return;
		}
		auto it_tr = BackwardDict.find(transl_tb_removed); //remove from second dict
		for (auto i = it_tr->second.begin(); i != it_tr->second.end();++i)
		{
			if (*i == word)
			{
				it_tr->second.erase(i);
				break;
			}
		}
		if(it_tr->second.empty()) //no words translate as this
		{
			BackwardDict.erase(it_tr);
		}
		
	}
	
}

void Dictionary::Rfind(const string& transl)
{
	auto it = BackwardDict.find(transl);
	if(it == BackwardDict.end())
	{
		return;
	}
	for (auto&& word : it->second)
	{
		cout << word << ' ';
	}
	cout << endl;
}




