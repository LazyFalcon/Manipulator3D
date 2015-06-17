#pragma once
/* flow
	┌──────────────────┐
	├	move             │
	├	move             │
	├	waitOnKey        │
	├	conditional(key) │
	│	 ├ move          │
	│	 └ useGripper    │
	├	move             │
	├ useGripper       │
	└──────────────────┘
*/


enum class CommandStatus : int {


};

class RobotController;

class ICommand {
	//uint32_t flags;
public:
	//ICommand() : flags(0){}
	//ICommand(uint32_t f) : flags(f){}
	virtual bool update(RobotController &rc, float dt) = 0;
	virtual vector<glm::vec4>& getPath() = 0;
	virtual ~ICommand(){
	}

	std::string name = "--empty--";
};

class MoveCommand : public ICommand {
public:
	IInterpolator *m_interpolator;
	IIK *m_solver;
	
	MoveCommand(IInterpolator *interpolator) : m_interpolator(interpolator){}

	bool update(RobotController &rc, float dt);
	vector<glm::vec4>& getPath() {
		return m_interpolator->visualisation;
	}
	MoveCommand& time(float time);
	// Move& interpolator(IInterpolator *interpolator);
	MoveCommand& interpolator(IInterpolator *interpolator);
	MoveCommand& solver(IIK *solver){
		m_solver = solver;
		return *this;
	}
	MoveCommand& points();
	~MoveCommand(){
		std::cerr << "delete move command\n";
	}

	MoveCommand& velocity();
	MoveCommand& acceleration();
	MoveCommand& precision();

};

class WaitCommand : public ICommand {
	vector <glm::vec4> fakePath{};
	float timeLeft = 0.f;
public:
	WaitCommand(float time) : timeLeft(time){}

	bool update(RobotController &rc, float dt);
	vector<glm::vec4>& getPath() {
		return fakePath;
	}
};
class UseEffectorCommand : public ICommand {};
// niech trzyma zespawnowane commandy, żeby mozna bylo je usunac
// albo spawnowanym flagę ustawić, i je usunać przy stopie :D
class ConditionalCommand : public ICommand {};
class FunctionCommand : public ICommand {};

enum class RCStates {
	Run, Pause, Stop,

}; 

class RobotController {
public:
	Robot *robot;
	IIK *solver;
	std::list<std::unique_ptr<ICommand>> commands;
	std::list<std::unique_ptr<ICommand>>::iterator commandIter;
	RCStates state = RCStates::Pause;

	RobotController(){
		commandIter = commands.end();
	}

	bool update(float dt);

	MoveCommand& move(IInterpolator *interpolator, const std::string &name);
	WaitCommand& wait(float time);

	void run();
	void pause();
	void stop();
	void next();
	void prev();

};

void RCTest(RobotController &rc);

