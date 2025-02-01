#include <vector>
#include <iostream>

int	main(void)
{
	std::vector<int>	vec1(1);
	std::cout << &vec1.at(0) << std::endl;
	std::vector<int>	vec2(vec1);

	std::cout << &vec2.at(0) << std::endl;
	// std::cout << &vec1 << std::endl;
	// std::cout << &vec2 << std::endl;
	
	// std::cout << vec1.cbegin().base() << std::endl;
	// std::cout << NULL << std::endl;
}