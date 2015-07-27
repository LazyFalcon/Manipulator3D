
	vector <glm::vec4> fakePath;
	u32 globalFlags = 0;
	
	
	void WaitCommand::init(RobotController &rc){
		isRuning = true;
	};
	bool WaitCommand::enter(RobotController &rc){}
	bool WaitCommand::update(RobotController &rc, float dt){
	  if(releaseTime <= 0.f || releaseFlag&globalFlags || (releaseFuction && releaseFuction()))
      return true;
	  
	  releaseTime -= dt;
    time += dt;
    return false;
	}
	bool WaitCommand::exit(RobotController &rc){}
	vector<glm::vec4>& WaitCommand::getPath(){
		return fakePath;
	}
	vector<glm::vec4>& WaitCommand::getPolyline(){
		return fakePath;
	}
	
	void ExecuteCommand::init(RobotController &rc){
		isRuning = true;
	};
	bool ExecuteCommand::enter(RobotController &rc){
	  if(enterCallback) enterCallback();
	}
	bool ExecuteCommand::update(RobotController &rc, float dt){
	  if(func) func();
	}
	bool ExecuteCommand::exit(RobotController &rc){
	  if(exitCallback) exitCallback();
	}
	vector<glm::vec4>& ExecuteCommand::getPath(){
		return fakePath;
	}
	vector<glm::vec4>& ExecuteCommand::getPolyline(){
		return fakePath;
	}
	
	
