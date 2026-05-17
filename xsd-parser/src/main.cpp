/**
 * This program is intended to parse the MusicXML XML Schema Definition
 * and generate the corresponding classes in TypeScript.
 */

#include <iostream>
#include "xsd_parser.hpp"

int main(int argc, char* argv[]) {

	if(argc < 2) {
		std::cout << "No input specified" << std::endl;
		return -1;
	}

	XSDParser* parser = new XSDParser();
	parser->parse(argv[1]);

	return 0;
}
