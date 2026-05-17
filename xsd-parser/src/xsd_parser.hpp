#include <string>
#include <unordered_map>

#pragma once

// TODO:
// std::ostream& operator<<(std::ostream& os, const XSDToken& xsd_token)
// Should output:
//
//	{tag_name}
//		{attr_key}: {attr_value}
//		...
struct XSDToken {
	std::string tag_name;
	std::unordered_map<std::string, std::string> attributes;
};

enum XSDParserState {
	Default,
	TagAttrs_Name,
	TagAttrs_Value,
};

class XSDParser {

	XSDParserState state = Default;

	public:
		int parse(std::string fpath);
	
	private:
		int parse_default(std::ifstream& input);
		int parse_tag_attr_name(std::ifstream& input);
		int parse_tag_attr_value(std::ifstream& input);
};
