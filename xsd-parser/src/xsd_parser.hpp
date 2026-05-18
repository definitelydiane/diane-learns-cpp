#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#pragma once

struct XMLAttr {
	std::string name;
	std::string value;
};

// TODO:
// std::ostream& operator<<(std::ostream& os, const XSDToken& xsd_token)
// Should output:
//
//	{tag_name}
//		{attr_key}: {attr_value}
//		...
struct XSDToken {
	enum class Type {
		Open,
		Close,
		SelfClose,
	};
	Type type;
	std::string tag_name;
	std::vector<XMLAttr> attributes;

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
		}

		if(t.attributes.size()) {
			std::cout << "ATTIBUTES:" << std::endl;
			for(const XMLAttr& a : t.attributes) {
				std::cout << '\t' << a.name << '=' << a.value << std::endl;
			}
		}
		return os;
	}
};

enum XSDParserState {
	Default,
	TagAttrs_Name,
	TagAttrs_Value,
};

class XSDParser {

	XSDParserState state = Default;
	std::vector<XSDToken> tokens;

	public:
		int parse(std::string fpath);
	
	private:
		int parse_default(std::ifstream& input);
		int parse_tag_attr_name(std::ifstream& input);
		int parse_tag_attr_value(std::ifstream& input);
};
