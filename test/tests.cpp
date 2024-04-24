#include <boost/fiber/fiber.hpp>
#include <boost/fiber/operations.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <chrono>
#include <vector>
#include "ClockDomain.hpp"
#include "Register.hpp"
#include "Combinational.hpp"

using namespace std::chrono_literals;

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

void foo(){
	int a = 3;
}
void bar(){
	int b = 4;
}
TEST_CASE("Some combinational logic", "[Combinational]"){
	Register<int> a(6,6);
	Combinational comb([&]() {
		int i = a.get();
		a.set(i+1);
	});
	comb.run();
	a.update();
	REQUIRE(a.get() == 7);
	comb.run();
	a.update();
	REQUIRE(a.get() == 8);
	
	Register<double> b(6.25,6.25);
	Combinational comb2([&]() {
		double i = b.get();
		b.set(i+0.25);
	});
	comb2.run();
	comb2.run();
	b.update();
	REQUIRE(b.get() == 6.5);
}

TEST_CASE("Sleep for 0.1s"){
	boost::this_fiber::sleep_for(100ms);
}

TEST_CASE("I guess we need to put reference in for loop"){
	//std::vector<void (*)()> vec;
	std::vector<Combinational<void (*)()>> vec;
	vec.push_back(foo);
	vec.push_back(bar);
	for(auto &f:vec){
		boost::fibers::fiber([&](){f.run();}).detach();
	}
	boost::this_fiber::sleep_for(100ms);
}

TEST_CASE("A simple ClockDomain", "[ClockDomain]"){
	Register<uint64_t> a(0,0);
	ClockDomain clkdom;
	Combinational comb([&]() {
		uint64_t i = a.get();
		a.set(i+1);
	});
	clkdom.add_register(&a);
	clkdom.add_combinational(&comb);
	clkdom.run();
	boost::this_fiber::sleep_for(10ms);
	clkdom.stop();
	boost::this_fiber::sleep_for(10ms);
	REQUIRE(a.get() > 100);
}

TEST_CASE("Registers stay in sync", "[ClockDomain]"){
	Register<uint64_t> a(0,0);
	Register<uint64_t> b(1,1);
	Register<uint64_t> c(2,2);
	ClockDomain clkdom;
	Combinational comb_a([&]() {
		uint64_t i = a.get();
		a.set(i+1);
	});
	Combinational comb_b([&]() {
		uint64_t i = b.get();
		b.set(i+1);
	});
	Combinational comb_c([&]() {
		uint64_t i = c.get();
		c.set(i+1);
	});
	clkdom.add_register(&a);
	clkdom.add_register(&b);
	clkdom.add_register(&c);
	clkdom.add_combinational(&comb_a);
	clkdom.add_combinational(&comb_b);
	clkdom.add_combinational(&comb_c);
	for(int i = 0; i<20; i++){
		clkdom.run();
		boost::this_fiber::sleep_for(10ms);
		clkdom.stop();
		boost::this_fiber::sleep_for(10ms);
		REQUIRE(a.get()+1 == b.get());
		REQUIRE(b.get()+1 == c.get());
	}
}


TEST_CASE("Fails if add comb while running", "[ClockDomain]"){
	Register<uint64_t> a(0,0);
	ClockDomain clkdom;
	Combinational comb([&]() {
		uint64_t i = a.get();
		a.set(i+1);
	});
	Combinational comb2([&]() {
		uint64_t i = a.get();
		a.set(i+1);
	});
	clkdom.add_register(&a);
	clkdom.add_combinational(&comb);
	clkdom.run();
	boost::this_fiber::sleep_for(10ms);
	REQUIRE_THROWS(clkdom.add_combinational(&comb2));
	boost::this_fiber::sleep_for(10ms);
	clkdom.stop();
	boost::this_fiber::sleep_for(10ms);
	REQUIRE(a.get() > 100);
}

TEST_CASE("Fails if add reg while running", "[ClockDomain]"){
	Register<uint64_t> a(0,0);
	Register<uint64_t> b(0,0);
	ClockDomain clkdom;
	Combinational comb([&]() {
		uint64_t i = a.get();
		a.set(i+1);
	});
	clkdom.add_register(&a);
	clkdom.add_register(&b);
	clkdom.add_combinational(&comb);
	clkdom.run();
	boost::this_fiber::sleep_for(10ms);
	REQUIRE_THROWS(clkdom.add_register(&b));
	boost::this_fiber::sleep_for(10ms);
	clkdom.stop();
	boost::this_fiber::sleep_for(10ms);
	REQUIRE(a.get() > 100);
}

TEST_CASE("Circular ClockDomain", "[ClockDomain]"){
	Register<uint64_t> a(0,0);
	Register<uint64_t> b(0,0);
	Register<uint64_t> c(0,0);
	ClockDomain clkdom;
	Combinational comb_a([&]() {
		uint64_t i = c.get();
		a.set(i+1);
	});
	Combinational comb_b([&]() {
		uint64_t i = a.get();
		b.set(i);
	});
	Combinational comb_c([&]() {
		uint64_t i = b.get();
		c.set(i);
	});
	clkdom.add_register(&a);
	clkdom.add_register(&b);
	clkdom.add_register(&c);
	clkdom.add_combinational(&comb_a);
	clkdom.add_combinational(&comb_b);
	clkdom.add_combinational(&comb_c);
	clkdom.run();
	boost::this_fiber::sleep_for(10ms);
	clkdom.stop();
	boost::this_fiber::sleep_for(10ms);
	REQUIRE(a.get() > 33);
	REQUIRE(a.get() - c.get() <= 1);
	REQUIRE(a.get() - b.get() <= 1);
}

TEST_CASE("Fiber stops itself" "[ClockDomain]"){
	Register<uint64_t> a(0,0);
	ClockDomain clkdom;
	Combinational comb([&]() {
		uint64_t i = a.get();
		a.set(i+1);
		if(i==1000){
			clkdom.stop();
		}
	});
	clkdom.add_register(&a);
	clkdom.add_combinational(&comb);
	clkdom.run();
	boost::this_fiber::sleep_for(10ms);
	REQUIRE(a.get() == 1001);

}
