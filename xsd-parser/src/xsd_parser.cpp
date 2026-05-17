#include "xsd_parser.hpp"
#include "util.hpp"

#include <iostream>
#include <fstream>

constexpr auto MAX_SIZE = std::numeric_limits<std::streamsize>::max();

int XSDParser::parse(std::string fpath) {
	std::ifstream input(fpath);

	while(!input.eof()) {
		switch(this->state) {
			case Default:
				this->parse_default(input);
				break;
			case TagAttrs_Name:
				this->parse_tag_attr_name(input);
				break;
			case TagAttrs_Value:
				this->parse_tag_attr_value(input);
				break;
		}
	}

	std::cout << std::endl;

	if(input.is_open()) {
		input.close();
	}

	return 0;
}

int XSDParser::parse_default(std::ifstream& input) {
	char c = input.get();
	switch(c) {
		case '<': {
			// Open tag
			
			// Check to see if this is a close tag
			bool is_close = input.peek() == '/';
			if(is_close) input.ignore(1);

			std::string tag_name;

			// Get line until space or >
			Util::getline(input, tag_name, " >");

			if(tag_name.compare("?xml") == 0) {
				// expect XML prologue
				// Continue to consume chars until '>'
				std::cout << "[XML PROLOGUE]" << std::endl;
				input.ignore(MAX_SIZE, '>');
			} else {
				std::cout << (is_close ? "CLOSE " : "OPEN ") << "TAG: " << tag_name << std::endl;
				input.unget();
				if(std::isspace(input.peek()))
					this->state = TagAttrs_Name;
				else {
					// std::cout << char(input.peek());
					input.ignore(1);
				}
			}
			break;
		}
		default: {
			std::cout << c;
	 }
	}
	return 0;
}

int XSDParser::parse_tag_attr_name(std::ifstream& input) {
	char c = input.get();

	// Discard leading whitespace
	if(std::isspace(c)) return 0;

	switch(c) {
		case '=': {
			if(input.peek() != '"') {
				std::cout << "Expected '=\"' after attribute name" << std::endl;
				throw -1;
			} else {
				this->state = TagAttrs_Value;
			}
			break;
		}
		case '/': {
			// This is a syntax error. But only if we're not in attribute_value	
			if(input.peek() != '>') {
				std::cout << "Unexpected backslash" << std::endl;
				throw -1;
			}

			// Per the XML grammar, we expect the next char to be >
			input.ignore(1);

			// Debugging
			std::cout << std::endl << std::endl;
			this->state = Default;
			break;
		}
		case '>': {
			this->state = Default;
			break;
		}
		default: {
			input.unget();
			std::string attr_name;
			Util::getline(input, attr_name, " =");
			std::cout << "ATTR_NAME: " << attr_name << std::endl;
			this->state = TagAttrs_Value;
			break;
		}
	}
	return 0;
}

int XSDParser::parse_tag_attr_value(std::ifstream& input) {
	char c = input.get();
	if(std::isspace(c)) return 0;
	switch(c) {
		default:
			std::string attr_val;
			Util::getline(input, attr_val, "\"");
			std::cout << "ATTR_VAL: " << attr_val <<std::endl;
			this->state = TagAttrs_Name;
			break;
	}
	return 0;
}
