/**
 * This program is intended to parse the MusicXML XML Schema Definition
 * and generate the corresponding classes in TypeScript.
 */

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <stack>
#include <string>

constexpr auto MAX_SIZE = std::numeric_limits<std::streamsize>::max();

struct XSDToken {
	std::string tag_name;
	std::unordered_map<std::string, std::string> attributes;
};

// TODO:
// std::ostream& operator<<(std::ostream& os, const XSDToken& xsd_token)
// Should output:
//
//	{tag_name}
//		{attr_key}: {attr_value}
//		...

enum XSDParserState {
	Default,
	TagAttrs,
};

class XSDParser {
	std::stack<std::string> open_tags;
	XSDParserState state = Default;

	public:
		int parse(std::string fpath);
	
	private:
		int parse_default(std::ifstream& input);
		int parse_tag_attrs(std::ifstream& input);
};

int XSDParser::parse(std::string fpath) {
	std::ifstream input(fpath);

	while(!input.eof()) {
		switch(this->state) {
			case Default:
				this->parse_default(input);
				break;
			case TagAttrs:
				this->parse_tag_attrs(input);
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

			std::getline(input, tag_name, ' ');

			if(tag_name.compare("?xml") == 0) {
				// expect XML prologue
				// Continue to consume chars until '>'
				std::cout << "[XML PROLOGUE]" << std::endl;
				input.ignore(MAX_SIZE, '>');
			} else {
				std::cout << (is_close ? "CLOSE " : "OPEN ") << "TAG: " << tag_name << std::endl;
				this->state = TagAttrs;
			}
			break;
		}
		default: {
			std::cout << c;
	 }
	}
	return 0;
}

int XSDParser::parse_tag_attrs(std::ifstream& input) {
	this->state = Default;
	return 0;
}




int main(int argc, char* argv[]) {

	if(argc < 2) {
		std::cout << "No input specified" << std::endl;
		return -1;
	}

	XSDParser* parser = new XSDParser();
	parser->parse(argv[1]);

	return 0;
}
