#pragma once
template <typename T>
struct DataCell
{
	void up(){
		if(ptr)
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
		for(auto &it : dataset)
			it.second.up();
	}
	void set(const std::string &name, T &p){
		dataset[name] = DataCell<double> {name, &p};
	}

	std::unordered_map<std::string, DataCell<T>> dataset;
};