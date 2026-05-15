#include <string>

/**
 * Reads the input stream and writes to the output string until one of the delimiters
 * is reached. Then, consume the delimiter.
 */
namespace Util {
	void getline(std::istream& input, std::string& output, std::string_view dleims);
}
