#include "util.hpp"

#include <iostream>

namespace Util {
	void getline(std::istream& input, std::string& output, char delim, bool consume_delim) {
		std::getline(input, output, delim);
		if(!consume_delim) {
			input.unget();
		}
	}
	// Written with reference to https://gcc.gnu.org/onlinedocs/gcc-4.7.0/libstdc++/api/a01557_source.html#l00627
	// and Claude
	void getline(std::istream& input, std::string& output, std::string_view delims, bool consume_delim) {
		using traits = std::char_traits<char>;
		using int_type = traits::int_type;

		const int_type eof = traits::eof();
		const int BUF_SIZE = 128;

		// https://en.cppreference.com/cpp/io/basic_istream/sentry
		// Check to see if we can read this stream. Don't skip whitespace
		std::istream::sentry can_read(input, true);
		if(!can_read) return;

		output.erase();
		
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
			} else if(consume_delim) {
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
}
