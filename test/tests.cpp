#include <catch2/catch_test_macros.hpp>
#include "Register.hpp"

TEST_CASE("Register registers output unitl update", "[Register]"){
	Register<int> r;
	r.set(5);
	r.update();
	REQUIRE(r.get() == 5);
	r.set(6);
	REQUIRE(r.get() == 5);
	r.update();
	REQUIRE(r.get() == 6);
}

class NoDefaultCstr{
	public:
	NoDefaultCstr(int i){a = i;}
	int a;
};
TEST_CASE("Register of no default constructor", "[Register]"){
	Register<NoDefaultCstr> r(6);
	REQUIRE(r.get().a == 6);
}
/*
class NoCopy{
public:
	NoCopy(int x): a(x){}
	NoCopy(NoCopy&& rhs): a(rhs.a) {rhs.a = 0;}
	NoCopy(const NoCopy&);
	NoCopy& operator=(const NoCopy&);
	int a;
};
TEST_CASE("No copy", "[Register]"){
	Register<NoCopy> r(NoCopy(6),NoCopy(7));
	REQUIRE(r.get().a == 7);
	r.update();
	REQUIRE(r.get().a == 6);
	r.set(NoCopy(5));
	r.update();
	REQUIRE(r.get().a == 5);
}*/
