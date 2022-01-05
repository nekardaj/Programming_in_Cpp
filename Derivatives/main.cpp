#include "Derivatives.h"
#include <istream>
#include <sstream>
#include <fstream>
using namespace std;

int main()
{
	istream& str = cin;
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
		stringstream sstr(s);
		try
		{
			NodePtr root = ParseInfix(sstr);
			//root->Print();
			//cout << endl;
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
			//cout << e.what();
			cout << "Input error" << endl;
		}
	}
	
	
	return 0;
}
