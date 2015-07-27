
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
	
void MoveCommand::init(RobotController &rc){
	isRuning = true;
	solver->solve(Point{ interpolator->firstPoint(), glm::quat(0, 0, 0, 1) }, *rc.robot);
	targetJointPosition = solver->result;
	rc.robot->isReady = false;
	std::cout<<"Init command"<<std::endl;


	previousPoint = rc.robot->endEffector.position;
	rc.robot->insertVariables(targetJointPosition);
}
double MoveCommand::calculateRequiredDistance(float dt){
	return dt*velocity;
}
glm::vec4 MoveCommand::calculateNextPoint(float dt){
	requiredDistance = calculateRequiredDistance(dt);

	glm::vec4 newTarget;

	while(requiredDistance > 0.0 && (not interpolator->finished)){
		newTarget = interpolator->nextPoint();
		requiredDistance -= glm::distance(previousPoint, newTarget);
		previousPoint = newTarget;
	}

	return newTarget;
}
bool MoveCommand::update(RobotController &rc, float dt){

	if(not rc.robot->isReady){
		rc.robot->goTo(dt);
		previousPoint = rc.robot->endEffector.position;
		return false;
	}
	if(rc.robot->isReady && interpolator->finished){
		interpolator->reset();
		std::cout<<"job done"<<endl;
		return false;
	}
	glm::vec4 newTarget = calculateNextPoint(dt);

	solver->solve(Point{ newTarget, glm::quat(0, 0, 0, 1) }, *rc.robot);
	targetJointPosition = solver->result;
	rc.robot->goTo(targetJointPosition);

	return false;
}
