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

	for(const XSDToken& token : this->tokens) {
		std::cout << token << std::endl;
	}

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
				input.ignore(MAX_SIZE, '>');
			} else {
				// The parser assumes this tag will be Open or Closed
				this->tokens.emplace_back(XSDToken{
						is_close ? XSDToken::Type::Close : XSDToken::Type::Open,
						tag_name,
						{}
				});

				// Unget to check if we consumed a space or a '>'
				input.unget();
				if(std::isspace(input.peek()))
					this->state = TagAttrs_Name;
				else {
					// The tag has closed
					// TODO: if this tag can contain text node, then move to text node.
					input.ignore(1);
				}
			}
			break;
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
			std::cout << "Unexpected '=' char. Expected attr_name" << std::endl;
			throw -1;
		}
		case '/': {
			if(input.peek() != '>') {
				// This is a syntax error. But only if we're not in attribute_value	
				std::cout << "Unexpected backslash" << std::endl;
				throw -1;
			}

			// Per the XML grammar, we expect the next char to be >
			input.ignore(1);

			// Mark this tag as self-closing
			this->tokens.back().type = XSDToken::Type::SelfClose;

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
			// Read the attribute name. Account for if there is whitespace between
			// the '=' char.
			Util::getline(input, attr_name, " =");
			this->tokens.back().attributes.push_back(XMLAttr{attr_name, ""});
			this->state = TagAttrs_Value;
			break;
		}
	}
	return 0;
}

int XSDParser::parse_tag_attr_value(std::ifstream& input) {
	char c = input.get();

	// Ignore whitespace
	if(std::isspace(c)) return 0;

	switch(c) {
		default:
			std::string attr_val;
			// This ignores the first delimiter. c here is either " or '.
			// TODO: We need to account for these two possible delimiters.
			Util::getline(input, attr_val, "\"");
			this->tokens.back().attributes.back().value = attr_val;
			this->state = TagAttrs_Name;
			break;
	}
	return 0;
}
