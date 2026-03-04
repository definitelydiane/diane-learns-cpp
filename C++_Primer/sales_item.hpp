// Header file for 1.5.3; wasn't sure where to find this so I wrote it myself.
// *.hpp naming convention to differentiate between C header files (*.h) and Cpp header files

// #pragma preprocessor directive means include this file one time; don't include this file multiple times
// See also: ifndef (if not defined)
#pragma once
#include <iostream>
#include <string>
// Sales_item class def

class SalesItem {
	private:
		std::string ISBN;
		double price;
	public:
		SalesItem(std::string ISBN, double price);
		std::string getISBN() const;
		double getPrice() const;
		friend std::ostream& operator<<(std::ostream& os, const SalesItem& salesItem);
		friend double operator+(const SalesItem& r, const SalesItem& l);
};
