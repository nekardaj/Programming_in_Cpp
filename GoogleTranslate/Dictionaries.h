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


class Dictionary
{
public:
	Dictionary() = default;
	void Pfind(const string& prefix);
	void Find(const string& word);
	void Add(const string& word, const vector<string>& translations);
	void Remove(const string& word, const string& transl_tb_removed = nullptr); //if we pass nullptr delet whole word

private:
	transl_map BackwardsDict;
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
	//static constexpr int max_cmd_size = 5;

};



#endif
