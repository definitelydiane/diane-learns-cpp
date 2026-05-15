/**
 * Co-authored by Claude
 */

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include "util.hpp"

// ---------------------------------------------------------------------------
// 1. Basic single delimiter
// ---------------------------------------------------------------------------
TEST_CASE("Reads up to a single delimiter", "[getline]") {
    std::istringstream input("hello world");
    std::string output;

		Util::getline(input, output, " ");

    REQUIRE(output == "hello");
}

// ---------------------------------------------------------------------------
// 2. Delimiter is consumed, next read continues correctly
// ---------------------------------------------------------------------------
TEST_CASE("Delimiter is consumed from the stream", "[getline]") {
    std::istringstream input("hello world");
    std::string first, second;

		Util::getline(input, first, " ");
    Util::getline(input, second, " ");

    REQUIRE(first  == "hello");
    REQUIRE(second == "world");
}

// ---------------------------------------------------------------------------
// 3. Multiple delimiter characters
// ---------------------------------------------------------------------------
TEST_CASE("Stops at any character in the delimiter set", "[getline]") {
    std::istringstream input("foo,bar");
    std::string output;

		Util::getline(input, output, " ,\t");

    REQUIRE(output == "foo");
}

// ---------------------------------------------------------------------------
// 4. EOF sets eofbit
// ---------------------------------------------------------------------------
TEST_CASE("Sets eofbit when EOF is reached before a delimiter", "[getline]") {
    std::istringstream input("hello");
    std::string output;

		Util::getline(input, output, " ");

    REQUIRE(output == "hello");
    REQUIRE(input.eof());
}

// ---------------------------------------------------------------------------
// 5. Empty input sets failbit
// ---------------------------------------------------------------------------
TEST_CASE("Sets failbit when no characters are extracted", "[getline]") {
    std::istringstream input("");
    std::string output;

		Util::getline(input, output, " ");

    REQUIRE(output.empty());
    REQUIRE(input.fail());
}

// ---------------------------------------------------------------------------
// 6. Input starting with a delimiter extracts nothing, sets failbit
// ---------------------------------------------------------------------------
TEST_CASE("Sets failbit when first character is a delimiter", "[getline]") {
    std::istringstream input(",hello");
    std::string output;

		Util::getline(input, output, ",");

    REQUIRE(output.empty());
    REQUIRE(input.fail());
}

// ---------------------------------------------------------------------------
// 7. Output is cleared before reading (no stale data)
// ---------------------------------------------------------------------------
TEST_CASE("Clears output before reading", "[getline]") {
    std::istringstream input("world");
    std::string output = "stale_data";

		Util::getline(input, output, " ");

    REQUIRE(output == "world");
}

// ---------------------------------------------------------------------------
// 8. Input longer than the internal 128-char staging buffer
// ---------------------------------------------------------------------------
TEST_CASE("Correctly reads strings longer than the internal 128-char buffer", "[getline]") {
    const std::string long_token(200, 'a');
    std::istringstream input(long_token + " rest");
    std::string output;

		Util::getline(input, output, " ");

    REQUIRE(output == long_token);
    REQUIRE(output.size() == 200);
}

// ---------------------------------------------------------------------------
// 9. Null byte in stream does not terminate early
// ---------------------------------------------------------------------------
TEST_CASE("Does not stop reading at null byte", "[getline]") {
    std::string data("ab\0cd ", 6);
    std::istringstream input(data);
    std::string output;

		Util::getline(input, output, " ");

    REQUIRE(output.size() == 5);
    REQUIRE(output == std::string("ab\0cd", 5));
}

// ---------------------------------------------------------------------------
// 10. Stream already in failed state — returns immediately without modifying output
// ---------------------------------------------------------------------------
TEST_CASE("Does not modify output when stream is in a failed state", "[getline]") {
    std::istringstream input("hello");
    input.setstate(std::ios_base::failbit);

    std::string output = "unchanged";
		Util::getline(input, output, " ");

    REQUIRE(output == "unchanged");
}
