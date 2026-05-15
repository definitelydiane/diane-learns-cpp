/**
 * This program is intended to parse the MusicXML XML Schema Definition
 * and generate the corresponding classes in TypeScript.
 */

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cstring>
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

/**
 * Reads the input stream and writes to the output string until one of the delimiters
 * is reached. Then, consume the delimiter.
 */
void getline(std::istream& input, std::string& output, std::string_view delims);


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

// Written with reference to https://gcc.gnu.org/onlinedocs/gcc-4.7.0/libstdc++/api/a01557_source.html#l00627
// and Claude
void getline(std::istream& input, std::string& output, std::string_view delims) {
	using traits = std::char_traits<char>;
	using int_type = traits::int_type;

	output.erase();
	const int_type eof = traits::eof();
	const int BUF_SIZE = 128;

	// https://en.cppreference.com/cpp/io/basic_istream/sentry
	// Check to see if we can read this stream. Don't skip whitespace
	std::istream::sentry can_read(input, true);
	if(!can_read) return;
	
	try {
		char buf[BUF_SIZE];
		size_t len = 0;
		size_t extracted = 0;

		// We're handling the istream's readbuf manually so we'll need to
		// update the state based on the result of the internal readbuf
		std::ios_base::iostate err = std::ios_base::goodbit; 

		int_type c = input.rdbuf()->sgetc(); // Drop into the underlying istreambuf
		
		// We check that c is not EOF, and that c does not exist in the string_view
		while(!traits::eq_int_type(c, eof)
				&& delims.find(traits::to_char_type(c)) == std::string_view::npos
			) {
			if(len == BUF_SIZE) {
				output.append(buf, len);
				len = 0;
			}

			// postfix increment; we'll read the current value first, then increment
			buf[len++] = traits::to_char_type(c);
			++extracted; // Prefix per libc++ impl
			c = input.rdbuf()->snextc(); // Advance, then read
		}

		// append remaining chars in buffer
		output.append(buf, len);

		// Error handling
		if(traits::eq_int_type(c, eof)) {
			err |= std::ios_base::eofbit; // |= (bitwise OR assignment) is equivalent to a = a | b
		} else {
			input.rdbuf()->sbumpc(); // consume delimiter
		}

		// Failed to read
		if(!extracted) {
			err|= std::ios_base::failbit; 
		}

		if(err) {
			input.setstate(err);
		}
	} catch(...) {
		input.setstate(std::ios_base::badbit);
		throw;
	}
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
