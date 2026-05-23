#include <string>

/**
 * Reads the input stream and writes to the output string until one of the delimiters
 * is reached. Then, consume the delimiter.
 */
namespace Util {
	void getline(std::istream& input, std::string& output, std::string_view delims, bool consume_delim = true);
	void getline(std::istream& input, std::string& output, char delim, bool consume_delim = true);
}
