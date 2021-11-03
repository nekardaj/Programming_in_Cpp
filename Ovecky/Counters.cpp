#include "Counters.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

NumberCounter::NumberCounter(LineProcessor& lineProcessor_) : lineProcessor(lineProcessor_)
{
}

void NumberCounter::process(char c)
{	
	if (isdigit(c) && number)
	{
		curr = 10 * curr + (int)c - (int)'0';
		digit = true;
		return;
	}
	if (!isdigit(c))
	{
		if (digit) //if we read at least one digit there is number
		{
			sum += curr;
			count += 1;
			lineProcessor.targetDetected();
			digit = false; //gets true when we read digit and switches to false at end of num
		}
		number = !isalnum(c); //number can begin after nonalnum
		curr = 0;
		return;
	}
	return;
}
StreamProcessor::StreamProcessor()
{
	results = { 0,0,0,0,0,0 };
}
void StreamProcessor::process(istream& str)
{
	LineProcessor lineProcessor;
	LexicalCounter lexCounter(lineProcessor);
	NumberCounter numCounter(lineProcessor);
	for (;;)
	{
		char c = str.get();
		if (str.fail())
		{
			c = '\0'; //wont create new num/word
			lexCounter.process(c);
			numCounter.process(c);
			lineProcessor.process(c);
			results[0] += chars;
			results[1] +=  lexCounter.getWords();
			results[2] += lexCounter.getSentences();
			results[3] += lineProcessor.getLines();
			results[4] += numCounter.getCount();
			results[5] += numCounter.getSum();
			return;
		}
		lexCounter.process(c);
		numCounter.process(c);
		lineProcessor.process(c);
		chars += 1;

	}	
}
const vector<int>& StreamProcessor::getResults()
{
	return results;
}

void LexicalCounter::process(char c)
{
	
	if (word)
	{
		if (isalpha(c))
		{
			word = false;
			words += 1;
			lineProcessor.targetDetected();
			if (sentence)
			{
				sentences += 1;
				sentence = false;
			}
			return;
		}
		if (isdigit(c)) //alnums began with digit -> not word
		{
			word = false;
			return;
		}
	}
	if (!isalnum(c))
	{
		word = true; //new word can start
		sentence |= (c == sentenceSep[0] || c == sentenceSep[1] || c == sentenceSep[2]);
		//if there is sentence sep next word should increment sentences
	}

}

LexicalCounter::LexicalCounter(LineProcessor& lineProcessor_) : lineProcessor(lineProcessor_)
{
}


void LineProcessor::process(char c)
{
	if (c == '\n')
	{
		newline = true;
	}
}

void LineProcessor::targetDetected()
{
	if (newline) //line didnt have word/num before -> increment lines
	{
		lines += 1;
		newline = false;
	}
}
