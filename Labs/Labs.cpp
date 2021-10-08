#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

void process( istream& str)
{
	char c;
	int sum = 0;
	int start = 0;
	bool number = true; //holds whether string from start till current char(excl) can be a number
	char whitespace[] = {'\n', ' ', };
	string curr = "";

	for (;;)
	{
		c = str.get();
		if (str.fail())
		{
			//checks for number ending next to eof
			if (number && curr != "")
			{
				sum += stoi(curr);
			}
			cout << sum;
			return;

		}
		
		
		if (isdigit(c) && number) //current printable seq contains nonnumber we dont do anything until we hit whitespace
		{
			curr += c;
			continue;
		}
		if (iswspace(c) || c == '.' || c == '?' || c == '!' )
		{
			if (number && curr != "") //there might be a number
			{
				sum += stoi(curr);
			}
			number = true; //number can begin after wspace
			curr = "";
			continue;
		}
		if (isprint(c)) //nums and !?. already taken care of
		{
			number = false; //cant be number until next wspace
		}
	}
	return;
}

int main(int argc, char ** argv)
{
	//string inp;
	//cin >> inp;
	/*
	ifstream f;
	f.open("testinp.txt");
	if (f.good()) //debuging with testfile
	{
		process(f);
	}
	*/
	process(cin);

}

