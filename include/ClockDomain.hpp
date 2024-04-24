#ifndef CLOCKDOMAIN
#define CLOCKDOMAIN
#include <boost/fiber/algo/algorithm.hpp>
#include <boost/fiber/operations.hpp>
#include <mutex>
#include <stdexcept>
#include <vector>
#include <memory>
#include <thread>
#include <boost/fiber/fiber.hpp>
#include <boost/fiber/barrier.hpp>
#include <boost/fiber/mutex.hpp>
#include <boost/fiber/algo/shared_work.hpp>
#include "Register.hpp"
#include "Combinational.hpp"

class ClockDomain{
public:
	ClockDomain(){
		boost::fibers::use_scheduling_algorithm< boost::fibers::algo::shared_work >();
		num_threads = std::thread::hardware_concurrency();
	}
	~ClockDomain(){
		stop();
		wait_til_done();
	}
	void add_combinational(Runnable *c){
		if(done){
			logic.push_back(c);
		}
		else{
			throw std::runtime_error("Can't add logic while running");
		}
	}
	void add_register(Updatable* u){
		if(done){
			registers.push_back(u);
		}
		else{
			throw std::runtime_error("Can't add registers while running");
		}
	}
	void set_num_threads(std::uint32_t num_threads){
		this->num_threads = num_threads;
	}
	void run() {
		if(!done){
			throw std::runtime_error("Can't run while already running");
		}
		barrier1 = std::make_unique<boost::fibers::barrier>(logic.size() + 1);
		barrier2 = std::make_unique<boost::fibers::barrier>(logic.size() + 1);
		done = false;
		b_stop = false;
		// create the threads that will run the fibers
		threads.clear();
		for(int i = 0; i < num_threads - 1; i++){
			threads.emplace_back(&ClockDomain::spawn_thread, this);
		}
		// create all the fibers
		for(Runnable *&r : logic){
			boost::fibers::fiber([&]() noexcept{
				while(!done){
					r->run();
					barrier1->wait();
					barrier2->wait();
				}
			}).detach();
		}
		// create the fiber that coordinates and updates
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
			cv.notify_all();
		}).detach();
	}
	void stop(){
		b_stop = true;
	}
	void wait_til_done(){
		/*{
			std::unique_lock<boost::fibers::mutex> lk(stop_lock);
			cv.wait(lk);
		}*/
		for(auto &t : threads){
			t.join();
		}
	}
private:
	bool done = true;
	boost::fibers::mutex stop_lock;
	boost::fibers::condition_variable_any cv;
	
	bool b_stop = false;
	std::uint32_t num_threads;
	std::unique_ptr<boost::fibers::barrier> barrier1;
	std::unique_ptr<boost::fibers::barrier> barrier2;
	std::vector<Runnable*> logic;
	std::vector<Updatable*> registers;
	std::vector<std::thread> threads;
	void spawn_thread(){
		// start work sharing, then wait until notified before ending
		boost::fibers::use_scheduling_algorithm<boost::fibers::algo::shared_work>();
		std::unique_lock<boost::fibers::mutex> lk(stop_lock);
		cv.wait(lk);
	}
};
#endif // CLOCKDOMAIN
