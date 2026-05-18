#include "xsd_parser.hpp"
#include "util.hpp"

#include <iostream>
#include <fstream>

constexpr auto MAX_SIZE = std::numeric_limits<std::streamsize>::max();

void XSDParser::parse(const std::string& fpath) {
	std::ifstream input(fpath);

	while(input.good()) {
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
	if(this->open_tags.size()) {
		throw UnexpectedEOF(this->open_tags.top());
	}

	// Debugging / printing
	for(const XSDToken& token : this->tokens) {
		std::cout << token << std::endl;
	}

	// Cleanup
	if(input.is_open()) {
		input.close();
	}

}

void XSDParser::parse_default(std::ifstream& input) {
	char c = input.get();
	switch(c) {
		case '<': {
			// Open tag
			
			// Check to see if this is a close tag
			bool is_close = input.peek() == '/';
			if(is_close) input.ignore(1);

			std::string tag_name;

			// Get line until space or >. Don't consume the delimiter
			Util::getline(input, tag_name, " >", false);

			if(tag_name.compare("?xml") == 0) {
				// expect XML prologue
				// Continue to consume chars until '>'
				input.ignore(MAX_SIZE, '>');
			} else {
				// TODO: This logic can be handled by a separate method.
				// The parser assumes this tag will be Open or Closed
				this->tokens.emplace_back(XSDToken{
						is_close ? XSDToken::Type::Close : XSDToken::Type::Open,
						tag_name,
						{}
				});

				// TODO: this can be a separate method
				if(is_close) {
					if(this->open_tags.empty() || this->open_tags.top() != tag_name) {
						throw UnexpectedCloseTag(tag_name);
					}
					this->open_tags.pop();
				} else {
					this->open_tags.push(tag_name);
				}

				// Unget to check if we consumed a space or a '>'
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
}

void XSDParser::parse_tag_attr_name(std::ifstream& input) {
	char c = input.get();

	// Discard leading whitespace
	if(std::isspace(c)) return;

	switch(c) {
		case '=': {
			// Unexpected char in attr name
			throw UnexpectedChar('=');
		}
		case '/': {
			if(input.peek() != '>') {
				// This is a syntax error. But only if we're not in attribute_value	
				throw UnexpectedChar('/');
			}

			// Per the XML grammar, we expect the next char to be >
			input.ignore(1);

			// Mark this tag as self-closing
			this->tokens.back().type = XSDToken::Type::SelfClose;
			if(!this->open_tags.empty())
				this->open_tags.pop();

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
}

void XSDParser::parse_tag_attr_value(std::ifstream& input) {
	char c = input.get();

	// Ignore whitespace
	if(std::isspace(c)) return;

	switch(c) {
		default:
			std::string attr_val;
			// This ignores the first delimiter. c here is either " or '.
			// TODO: We need to account for these two possible delimiters.
			Util::getline(input, attr_val, "\"");

			// TODO do this in a less hack way.
			this->tokens.back().attributes.back().value = attr_val;
			this->state = TagAttrs_Name;
			break;
	}
}
