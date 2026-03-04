#include "sales_item.hpp"

// Constructor definitions
SalesItem::SalesItem(std::string isbn, double price) : ISBN(isbn), price(price) {}
// Also
// SalesItem::SalesItem(std::string isbn) {
// 	this->ISBN = isbn;
// }

std::ostream& operator<<(std::ostream& os, const SalesItem& salesItem) {
	os << "{\"isbn\":\"" << salesItem.ISBN << "\","
		<< "\"price\":" << salesItem.price
		<< "}";
	return os;
}

std::string SalesItem::getISBN() const {
	// SalesItem *this, i.e. a pointer
	return this->ISBN;
}

double SalesItem::getPrice() const {
	return this->price;
}

double operator+(const SalesItem& l, const SalesItem& r) {
	return l.getPrice() + r.getPrice();
}
