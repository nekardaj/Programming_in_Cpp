#include "Counters.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
using namespace std;

int main(int argc,char * argv[])
{
	/**/
	ifstream f;
	
	//f.open("testinp.txt");
	StreamProcessor processor;
	if (argc > 1)
	{
		for (int i = 1; i < argc; ++i)
		{
			f.open(argv[i]);
			processor.process(f);
			f.clear();
			//f.close();
		}
	}
	else
	{
		processor.process(cin);
	}
	
	vector<int> results = processor.getResults();
	//TODO pricitat k vectoru inic 6x nulou a nedelat clear
	string subjects[6]{"znaku","slov", "vet", "radku", "cisel", "soucet"};
	for (int i = 0; i < results.size(); i++)
	{
		cout << subjects[i] << ": " << results[i] << endl;
	}
	/**/
}

