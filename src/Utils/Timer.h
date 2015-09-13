#include <chrono>
#include <string>
using namespace std::chrono;

// Base: variable type: double, int32_t, int64_t, etc.
// Ratio: std::ratio<a,b>
// Size: final value is mean of <Size> last samples
// example: Timer<double, std::ratio<1,1>, 1> measure time in second with floating precision
// Timer<int64_t, std::ratio<1, 1000>, 1> measure time in miliseconds
template <typename Base, typename Ratio, int Size>
class Timer {
	steady_clock::time_point t1;
	steady_clock::time_point t2;
	Base array[Size] = {0};
	int cnt {0};
	Base val;
	string del;
public:
	Timer(){
			t1 = chrono::steady_clock::now();
			t2 = chrono::steady_clock::now();
	}
	Base operator ()(){
		t2 = chrono::steady_clock::now();
		duration<Base, Ratio> delta = chrono::duration_cast<duration<Base, Ratio>>(t2 - t1);
		t1 = chrono::steady_clock::now();
		array[cnt] = delta.count();
		Base sum = 0;
		for(auto it : array)
			sum += it;
		sum /= Size;
		
		val = (Base)sum;
		cnt = (cnt + 1)%Size;
		return val;
	}
	
	string getString(){
		stringstream ss;
		ss<<val;
		return ss.str();
	}
	void start(){
			t1 = chrono::steady_clock::now();
	}
	void end(){
		t2 = chrono::steady_clock::now();
		duration<Base, Ratio> delta = chrono::duration_cast<duration<Base, Ratio>>(t2 - t1);
		array[cnt] = delta.count();
		Base sum = 0;
		for(auto it : array)
			sum += it;
		sum /= Size;
		
		val = (Base)sum;
		cnt = (cnt + 1)%Size;
	}
	Base get(){
		return val;
	}
};
