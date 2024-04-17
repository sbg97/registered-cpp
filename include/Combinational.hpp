#ifndef COMBINATIONAL
#define COMBINATIONAL
#include <functional>
#include <vector>
#include "Register.hpp"

class Combinational{
public:
	Combinational(std::vector<void*> input,
	       std::vector<void*> output,
	       std::function<void(Combinational*)> fn)
	:input(input), output(output), fn(fn){}
	
	void run() {fn(this);}
private:
	std::vector<void*> input;
	std::vector<void*> output;
	std::function<void(Combinational*)> fn;
};
#endif // COMBINATIONAL
