#include <iostream>
#include "sales_item.hpp"
int main() {
	SalesItem book1("some-isbn-14235-235", 24.99);
	SalesItem book2("some-isbn-98984-235", 10.99);
	std::cout << book1 << std::endl;
	std::cout << book1 + book2 << std::endl;
	return 0;
}
