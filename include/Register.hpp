#ifndef REGISTER
#define REGISTER
#include <utility>

class Updatable{
public:
	virtual void update() = 0;
};


template<typename T>
class Register: Updatable{
public:
	Register(){}
	Register(T val):val1(val),val2(val){}
	Register(T nextVal, T currentVal): val1(std::move(nextVal)),val2(std::move(currentVal)){}

	T& get() {return val1_current?val1:val2;}
	void set(T newVal) {
		if(val1_current) val2 = newVal;
		else val1 = newVal;
	}
	void move_set(T newVal) {
		if(val1_current) val2 = std::move(newVal);
		else val1 = std::move(newVal);
	}
	void update() {val1_current ^= 1;}
private:
	bool val1_current = false;
	bool updated = false;
	T val1{};
	T val2{};
};
#endif // REGISTER
