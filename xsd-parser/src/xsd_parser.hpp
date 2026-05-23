#include <iostream>
#include <string>
#include <vector>

#pragma once

struct XMLAttr {
	std::string name;
	std::string value;
};

struct XSDToken {
	enum class Type {
		Open,
		Close,
		SelfClose,
		TextContent,
	};
	Type type;
	std::string tag_name;
	std::vector<XMLAttr> attributes;
	std::string text_content;

	friend std::ostream& operator<<(std::ostream& os, const XSDToken& t) {
		switch(t.type) {
			case Type::Open:
				std::cout << "OPEN TAG: " << t.tag_name << std::endl;
				break;
			case Type::Close:
				std::cout << "CLOSE TAG: " << t.tag_name << std::endl;
				break;
			case Type::SelfClose:
				std::cout << "SELF CLOSING TAG: " << t.tag_name << std::endl;
				break;
			case Type::TextContent:
				break;
		}

		if(t.attributes.size()) {
			std::cout << "ATTIBUTES:" << std::endl;
			for(const XMLAttr& a : t.attributes) {
				std::cout << '\t' << a.name << '=' << a.value << std::endl;
			}
		}
		if(t.type == Type::TextContent) {
			std::cout << "TEXT CONTENT:" << std::endl << t.text_content << std::endl;
		}
		return os;
	}
};

enum XSDParserState {
	Default,
	TagAttrs_Name,
	TagAttrs_Value,
	TextContent,
};

class XSDParser {

	XSDParserState state = Default;
	std::vector<XSDToken> tokens;
	std::stack<std::string> open_tags;

	public:
		void parse(const std::string& fpath);
	
	private:
		void parse_default(std::ifstream& input);
		void parse_tag_attr_name(std::ifstream& input);
		void parse_tag_attr_value(std::ifstream& input);
		void parse_text_content(std::ifstream& input);
		void parse_comment(std::ifstream& input);
};

// Parser Errors
struct UnexpectedEOF : public std::runtime_error {
    UnexpectedEOF(const std::string& tag_name)
        : std::runtime_error("Expected closing tag for element: " + tag_name)
    {}
};

struct UnexpectedCloseTag : public std::runtime_error {
    UnexpectedCloseTag(const std::string& tag_name)
        : std::runtime_error(tag_name)
    {}
};

struct UnexpectedChar : public std::runtime_error {
		UnexpectedChar(const std::string& msg)
			: std::runtime_error(msg)
		{}
    UnexpectedChar(char c)
        : std::runtime_error(std::string("") + c)
    {}
};
