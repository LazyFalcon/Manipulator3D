
template <typename T>
struct DataCell
{
	void up(){
		data.push_back(*ptr);
	}

	std::string name;
	T *ptr;
	std::vector<double> data;
};

template <typename T>
class DataRecorder
{
public:
	void up(){

	}
	void set(T &p){

	}

	std::vector
};