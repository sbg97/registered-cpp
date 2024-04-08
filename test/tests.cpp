#include <catch2/catch_test_macros.hpp>
#include "const.hpp"

TEST_CASE("function returns 26", "[get_number]"){
	REQUIRE(get_number() == 26);
}
