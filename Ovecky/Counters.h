#ifndef HEADER_P
#define HEADER_P
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
//using namespace std;
class LineProcessor
{
public:
	void process(char c);
	int getLines() { return lines; }
	void targetDetected();
private:
	bool newline = true;
	int lines = 0;
};
class NumberCounter
{
public:
	NumberCounter(LineProcessor& lineProcessor);
	void process(char c);
	int getSum() { return sum;}
	int getCount() { return count;}

private:
	LineProcessor& lineProcessor;
	int sum = 0;
	int count = 0;
	int curr = 0;
	bool number = true; //holds whether next char can be in number
	bool digit = false; //true if we added >0 digit into curr
};

class LexicalCounter //counts words sentences..
{
public:
	void process(char c); //add to lines after first word is found(doesnt fail on last line)
	int getWords() { return words; }
	int getSentences() { return sentences; }
	//int getLines() { return lines; } //line processor
	LexicalCounter(LineProcessor& lineProcessor);
private:
	const char sentenceSep[3] {'.','!','?'};
	LineProcessor& lineProcessor;
	int words = 0;
	int sentences = 0;
	int lines = 0;
	bool word = true; //true -> if we read letter it is start of word
	bool sentence = true; //if true reading a word means new sentence
};
class StreamProcessor
{
public:
	StreamProcessor();
	void process(std::istream& str);
	const std::vector<int>& getResults();
private:
	std::vector<int> results;
	int chars = 0;
};

#endif
