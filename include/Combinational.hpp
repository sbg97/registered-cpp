#ifndef COMBINATIONAL
#define COMBINATIONAL

class Runnable{
public:
	virtual void run() = 0;
};

template <typename TF>
class Combinational : public Runnable{
public:
	Combinational(TF fn)
	:fn(fn){}
	
	void run() {fn();}
private:
	TF fn;
};
#endif // COMBINATIONAL
