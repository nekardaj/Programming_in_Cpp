#include "Derivatives.h"
#include <istream>
#include <sstream>
#include <fstream>
using namespace std;

int main()
{
	cout << "Hello there" << endl;

	ifstream str;
	str.open(R"(C:\Users\Honza\Desktop\ukoly\C++_husak\nekardaj\Derivatives\out\build\x64-Debug\test.in.txt)" );
	std::string s;
	while (true)
	{
		getline(str, s);
		if(str.fail())
		{
			break;
		}
		if(s.empty())
		{
			cout << endl;
			continue;
		}
		cout << s << endl;
		stringstream sstr(s);
		try
		{
			NodePtr root = ParseInfix(sstr);
			root->Print();
			cout << endl;
			auto copy = root->Clone();
			root->DeriveSelf(copy->SelfToOperator());
			NodePtr newRoot = root->SimplifySelf();
			if(newRoot != nullptr)
			{
				root = move(newRoot);
			}
			root->Print();
			cout << endl;
		}
		catch (exception& e)
		{
			cout << e.what();
			cout << "Input error" << endl;
		}
	}
	
	
	return 0;
}
