#ifndef HEADER_T
#define HEADER_T
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
using namespace std;
using transl_map = map<string, vector<string>>;

enum command { add, del, find, find_pr, find_rev };
class Dictionary
{
public:
	Dictionary() = default;
	void Pfind(const string& prefix);
	void Find(const string& word);
	void Add(const string& word, const vector<string>& translations);
	void Remove(const string& word, const string& transl_tb_removed); //if we pass empty str(not allowed in dict) delete whole word
	void Rfind(const string& transl);
	void PrintWordTransl(const transl_map::iterator& entry);

private:
	transl_map BackwardDict;
	transl_map ForwardDict;
	// we use wrapper that makes the operation on the other dict to ensure correctness
};

class RequestProcessor
{
public:
	RequestProcessor(std::istream& _stream, Dictionary& dictionary_);
	void ProcessStream();
private:
	std::istream& stream;
	Dictionary& dictionaries;
	void ProcessRequest();
	//returns true if parsed succesfully
	static bool ParseCommand(command& command,const string& cmd);
	//static constexpr int max_cmd_size = 5;

};



#endif
