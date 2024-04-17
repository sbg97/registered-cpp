#include <catch2/catch_test_macros.hpp>
#include "Register.hpp"
#include "Combinational.hpp"

TEST_CASE("Registers register output until update", "[Register]"){
	Register<int> r;
	r.set(5);
	r.update();
	REQUIRE(r.get() == 5);
	r.set(6);
	REQUIRE(r.get() == 5);
	r.update();
	REQUIRE(r.get() == 6);
	r.set(7);
	REQUIRE(r.get() == 6);
	r.update();
	REQUIRE(r.get() == 7);
}

class NoDefaultCstr{
	public:
	NoDefaultCstr(int i){a = i;}
	int a;
};
TEST_CASE("Register of NoDefaultCstr", "[Register]"){
	Register<NoDefaultCstr> r(6);
	REQUIRE(r.get().a == 6);
}

class NoCopy{
public:
	NoCopy(int x): a(x){}
	NoCopy(NoCopy&& rhs): a(rhs.a) {rhs.a = 0;}
	NoCopy(const NoCopy&) = delete;
	NoCopy& operator=(const NoCopy&) = delete;
	NoCopy& operator=(NoCopy&& rhs) = default;
	int a;
};
TEST_CASE("Register of NoCopy", "[Register]"){
	Register<NoCopy> r(NoCopy(6),NoCopy(7));
	REQUIRE(r.get().a == 7);
	r.update();
	REQUIRE(r.get().a == 6);
	r.move_set(NoCopy(5));
	r.update();
	REQUIRE(r.get().a == 5);
}

TEST_CASE("Some combinational logic", "[Combinational]"){
	Register<int> a(0,6);
	Register<double> b(0.0,-4.0);
	Register<int> c(0,6);
	Register<double> d(0.0,6.0);
	Combinational comb({},{},[](Combinational* t) {t-> });
}
