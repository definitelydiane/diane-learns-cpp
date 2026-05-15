build: src/main.cpp
	rm -rf build
	mkdir build
	g++ -std=c++11 -o build/xsd-parser src/main.cpp

.PHONY: test
test: build/tests
	./build/tests

build/tests: test/getline.test.cpp thirdparty/catch2/catch.hpp
	rm -rf build/tests
	g++ -std=c++11 \
		-I thirdparty \
		-I src \
		-o build/tests \
		test/getline.test.cpp \
		src/util.cpp

.PHONY: clean
clean:
	rm -rf build
