class GlobalProperties {
public:
	map<string, float> m_cnt;
	
	
	
	
	
	void save(){
		ofstream file;
		file.open("cache\\GlobalProperties.txt", ios::out | ios::trunc);
		for(auto it = m_cnt.begin(); it != m_cnt.end(); it++){
			file<<it->first<<" "<<it->second<<"\n";
		}
		file.close();
	}
	void load(){
		ifstream file;
		file.open("cache\\GlobalProperties.txt", ios::in);
		string s;
		float f;
		while(file>>s>>f){
			m_cnt[s] = f;
		}
		file.close();
	}
	void editor(){
		
	}
	
}gp;


