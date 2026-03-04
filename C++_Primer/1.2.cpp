#include <iostream>

int main() {
	// Use std::endl instead of '\n' literal since std::endl will flush the buffer
	std::cout << "Enter two numbers" << std::endl;
	int v1 = 0, v2 = 0;
	std::cin >> v1 >> v2;
	std::cout << "The sum is: " << v1 + v2 << std::endl;
	return 0;
}
