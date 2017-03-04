#include <iostream>
#include "ABigNum.h"

int main()
{
	athena::ABigNum num(1), result(1);   /*声明两个大整数类型变量num和result，并赋初值1*/
	std::cout << "第10天有" << result << "个桃子\n";
	for (int i = 0; i < 9; i++)
	{
		result += num;
		result *= result;
		std::cout << "第" << 9 - i << "天有" << result << "个桃子\n";
	}

	return 0;
}