/**
 * Chapter 1.6: Bookstore Program
 *
 * Requirements:
 * Read a file of sales transactions and produce a report that
 * for each book shows:
 * - number of copies sold
 * - total revenue
 * - average sales price  
 *
 * Transactions are serialized in comma-separated values:
 * ISBN QTY PRICE
 *
 *
 * This solution is a bit different from the example in the book:
 * - This solution is written to take an input passed in char* argv[]
 * - Results are stored in a hashmap that maps ISBN to total sales and revenue
 *
 * To run:
 * 1. Compile this file i.e. `g++ -std=c++11 -o 1.6 1.6.cpp`
 * 2. Execute the program `$1.6 book_sales.csv`
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

struct SalesSummary {
	int total_sales;
	double total_revenue;
};

typedef std::vector<std::vector<std::string> > CSV;

CSV parseCSV(std::string &fname);
std::vector<std::string> parseLine(std::string &line);


int main(int argc, char* argv[]) {
	if(argc < 1) {
		std::cout << "specify input file" << std::endl;
		return -1;
	}

	// process file
	std::unordered_map<std::string, SalesSummary> sales;
	std::string fname = argv[1];
	const CSV csv = parseCSV(fname);

	// summarize
	for(const auto &row : csv) {
			SalesSummary &summary = sales[row.at(0)];
			summary.total_sales += std::stoi(row.at(1));
			summary.total_revenue += std::stod(row.at(2));
	}

	//print results
	std::cout << "Isbn\t\tTotal Sold\tRevenue\t\tAvg. Price" << std::endl;
	for(const auto &it : sales) {
		double avg_price = it.second.total_revenue / double(it.second.total_sales);
		std::cout << it.first << "\t" << it.second.total_sales << "\t\t" 
			<< it.second.total_revenue << "\t\t" << avg_price << std::endl;
	}

	
	return 0;
}

std::vector<std::vector<std::string> > parseCSV(std::string &fname) {
	std::ifstream input(fname);
	std::vector<std::vector<std::string> > result;
	std::string line;

	while(getline(input, line)) {
		std::vector<std::string> data = parseLine(line);
		result.push_back(data);
	}

	if(input.is_open()) {
		input.close();
	}

	return result;
}

std::vector<std::string> parseLine(std::string &line) {
	std::vector<std::string> tokens;

	int pos = 0;
	std::string token;
	// Find the position of the delimiter
	while((pos = line.find(",")) != std::string::npos) {
		// Extract the substring from the start of the string to the ','
		token = line.substr(0, pos);
		// Add the token to our return value
		tokens.push_back(token);
		line.erase(0, pos + 1);
	}
	tokens.push_back(line);

	return tokens;
}

