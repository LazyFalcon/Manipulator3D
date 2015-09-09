#define BOOST_CHRONO_HEADER_ONLY 1
#include <boost/chrono.hpp>
#include <string>
// using namespace std::chrono;
using namespace boost::chrono;

// DataType: variable type: double, int32_t, int64_t, etc.
// Ratio: std::ratio<a,b>
// Size: final value is mean of <Size> last samples
// example: Timer<double, 1, 1> measure time in second with floating precision
// Timer<int64_t, 1000, 1> measure time in miliseconds
template <typename DataType, long long int Base, int Size>
class Timer {
	// steady_clocksteady_clock::time_point t1;
	boost::chrono::high_resolution_clock::time_point t1;
	boost::chrono::high_resolution_clock::time_point t2;
	DataType array[Size] = {0};
	int cnt {0};
	DataType val;
	string del;
public:
	Timer(){
			t1 = boost::chrono::high_resolution_clock::now();
			t2 = boost::chrono::high_resolution_clock::now();
	}
	DataType operator ()(){
		t2 = boost::chrono::high_resolution_clock::now();
		duration<DataType, boost::ratio<1, Base>> delta = boost::chrono::duration_cast<duration<DataType, boost::ratio<1, Base>>>(t2 - t1);
		t1 = boost::chrono::high_resolution_clock::now();
		array[cnt] = delta.count();
		DataType sum = 0;
		for(auto it : array)
			sum += it;
		sum /= Size;

		val = (DataType)sum;
		cnt = (cnt + 1)%Size;
		return val;
	}

	string getString(){
		stringstream ss;
		ss<<val;
		return ss.str();
	}
	void start(){
		t1 = boost::chrono::high_resolution_clock::now();
	}
	void end(){
		t2 = boost::chrono::high_resolution_clock::now();
		duration<DataType, boost::ratio<1, Base>> delta = boost::chrono::duration_cast<duration<DataType, boost::ratio<1, Base>>>(t2 - t1);
		array[cnt] = delta.count();
		DataType sum = 0;
		for(auto it : array)
			sum += it;
		sum /= Size;

		val = (DataType)sum;
		cnt = (cnt + 1)%Size;
	}
	DataType get(){
		return val;
	}
};
