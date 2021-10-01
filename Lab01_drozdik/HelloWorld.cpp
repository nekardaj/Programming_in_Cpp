#include <iostream>
#include <vector>

int multiply(int x, int y)
{
	return x * y;
}

int gcd(int x, int y)
{
	if (x == 0)
	{
		return y;
	}
	if (y == 0)
	{
		return x;
	}
	if (x < y)
	{
		int temp = x;
		x = y;
		y = temp;
	}
	while (true)
	{
		if (x % y == 0)
		{
			return y;
		}
		else
		{
			int temp = x % y;
			x = y;
			y = temp;
		}
	}


}

void swap(int* a, int* b)
{
	int temp = *a;
	*a = *b;
	* b = temp;
}

void bubblesort(std::vector<int> &vec)
{
	for (size_t i = 0; i < &vec.size(); i++)
	{
		for (size_t j = i; i < &vec.size(); j++)
		{
			if (vec[j] < vec[j + 1])
			{
				swap(&vec[j], &vec[j + 1])
			}
		}
	}
}

int main(int argc, char * argv[])
{
	/*
	for (int i = 0; i < argc; i++)
	{
		std::cout << argv[i] << std::endl;
	}
    */
	//int x, y;
	//std::cin >> x;
	//std::cin >> y;
	//std::cout << x << std::endl;
	//std::cout << y << std::endl;
	//std::cout << gcd(x, y) << std::endl;
	/*
	std::vector<int> v;
	for (int i = -10; i <= 10; i++)
	{
		v.push_back(i);
	}
	for (int i = 0; i < v.size(); i++)
	{
		std::cout << v[i] << std::endl;
	}
	*/
	int a = 23;
	int b = 65;
	swap(&a, &b);
	std::cout << a << std::endl;
	std::cout << b << std::endl;
}