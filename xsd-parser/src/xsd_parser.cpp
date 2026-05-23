#include "xsd_parser.hpp"
#include "util.hpp"

#include <iostream>
#include <fstream>

constexpr auto MAX_SIZE = std::numeric_limits<std::streamsize>::max();

constexpr const char* Attr_TextContent = "#text";

void XSDParser::parse(const std::string& fpath) {
	std::ifstream input(fpath);

	try {
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
				case TextContent:
					this->parse_text_content(input);
					break;
			}
		}
		if(this->open_tags.size()) {
			throw UnexpectedEOF(this->open_tags.top());
		}
	} catch(...) {
		// Debugging / printing
		for(const XSDToken& token : this->tokens) {
			std::cout << token << std::endl;
		}
		throw;
	}

	// Debugging / printing
	for(const XSDToken& token : this->tokens) {
		std::cout << token << std::endl;
	}
}

void XSDParser::parse_default(std::ifstream& input) {
	char c = input.get();
	switch(c) {
		case '<': {
			// Possible meanings: open tag, close tag, comment?

			// Not good way to do this, but ¯\_(ツ)_/¯
			if(input.peek() == '!') {
				input.ignore(MAX_SIZE, '>');
				return;
			}
			// Check to see if this is a close tag
			bool is_close = input.peek() == '/';
			if(is_close) input.ignore(1);


			std::string tag_name;

			// Get line until space or >. Don't consume the delimiter
			Util::getline(input, tag_name, " >", false);

			if(tag_name == "?xml") {
				// expect XML prologue
				// Continue to consume chars until '?>'
				input.ignore(MAX_SIZE, '?');
				input.ignore(1);
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

				if(std::isspace(input.peek()))
					this->state = TagAttrs_Name;
				else if(tag_name == "xs:documentation" || tag_name == "xs:appinfo") {
					// Util::getline will end up on the closing '>',
					// so we assume the tag has closed.
					input.ignore(1);

					this->tokens.emplace_back(XSDToken{
						XSDToken::Type::TextContent
					});

					this->state = TextContent;
				} else {
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
			// TODO: do this in a sane way
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
		case '\'':
		case '"':
		{
			char delim = c;
			std::string attr_val;

			// TODO: We need to account for these two possible delimiters.
			// Check to see if this is an empty string
			if(input.peek() != delim) {
				// This ignores the first delimiter. c here is either " or '.
				Util::getline(input, attr_val, delim);
			} else {
				// Otherwise just consume the '"'
				input.ignore(1);
			}

			// TODO do this in a less hack way.
			this->tokens.back().attributes.back().value = attr_val;
			this->state = TagAttrs_Name;
			break;
		}
		default:
			throw UnexpectedChar("Expected char ' or \" but found " + std::string(1, c));
	}
}

void XSDParser::parse_text_content(std::ifstream& input) {
	int c = input.peek();

	// TODO create a method for this
	// Itnogre whitespace
	if(std::isspace(c) || c == EOF) {;
		input.ignore(1);
		return;
	}

	std::string content;

	switch(c) {
		case '<':
			this->state = Default;
			break;
		default:
			// TODO: do this less naive
			// Get all the text until the next angle bracket. Don't consume.
			Util::getline(input, content, "<", false);
			this->tokens.back().text_content = content;
			this->state = Default;
			break;
	}
}

