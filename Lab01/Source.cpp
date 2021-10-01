#include <iostream>
#include <string>
#include <vector>

void nasobilka(int a)
{
	std::cout << "Nasobilka " << a << ":" << std::endl;
	//printf("Nasobilka","");
	for (int i = 0; i <= 10; i++)
	{
		std::cout << i << " * " << a << " = " << a*i << std::endl;
	}
}

int main(int argc, char * argv[])
{
	//std::cout << "Hello World\n";
	for (int i = 1; i < argc; i++)
	{
		//nasobilka();
	}
	nasobilka(7);
}
