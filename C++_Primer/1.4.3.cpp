// Handling an unknown number of inputs
// use CTRL+D to end input
// CTRL+C aborts
// CTRL+Z sends to background
#include <iostream>

int main() {
	int sum = 0, value = 0;
	std::cout << "Enter a set of numbers separated by whitespace. Press CTRL+D to finish." << std::endl;
	// Read until EOF
	while (std::cin >> value) {
		sum += value;
	}
	std::cout << "Sum: " << sum << std::endl;
	return 0;
}

