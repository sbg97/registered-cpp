#ifndef CLOCKDOMAIN
#define CLOCKDOMAIN
#include <stdexcept>
#include <vector>
#include <boost/fiber/fiber.hpp>
#include <boost/fiber/barrier.hpp>
#include <boost/fiber/mutex.hpp>
#include "Register.hpp"
#include "Combinational.hpp"

class ClockDomain{
public:
	ClockDomain(){}
	~ClockDomain(){
		if(barrier1 == nullptr) delete barrier1;
		if(barrier2 == nullptr) delete barrier2;
	}
	void add_combinational(Combinational c){
		if(done){
			logic.push_back(c);
		}
		else{
			throw std::runtime_error("Can't add logic while running");
		}
	}
	void run() {
		if(!done){
			throw std::runtime_error("Can't run while already running");
		}
		if(barrier1 == nullptr) delete barrier1;
		if(barrier2 == nullptr) delete barrier2;
		barrier1 = new boost::fibers::barrier(logic.size() + 1);
		barrier2 = new boost::fibers::barrier(logic.size() + 1);
		done = false;
		b_stop = false;
		// create all the fibers
		for(Combinational c : logic){
			boost::fibers::fiber([&]() noexcept{
				while(!done){
					c.run();
					barrier1->wait();
					barrier2->wait();
				}
			}).detach();
		}
		boost::fibers::fiber([&]() noexcept{
			while(!b_stop|| !done){
				barrier1->wait();
				for (Updatable *u : registers) {
					u->update();
				}
				if(b_stop){
					done = true;
				}
				barrier2->wait();
			}
		}).detach();
	}
	void stop(){
		b_stop = true;
	}
private:
	bool done = true;
	bool b_stop = false;
	boost::fibers::barrier *barrier1 = nullptr;
	boost::fibers::barrier *barrier2 = nullptr;
	std::vector<Combinational> logic;
	std::vector<Updatable*> registers;
};
#endif // CLOCKDOMAIN
