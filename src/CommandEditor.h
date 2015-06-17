/*
	tutaj klepiemy dedykowane edytory rozkazów i interpolatorów,



*/

class CommandCreator {};

class ICommandEditor {
public:

	virtual bool drawWidget() = 0;
};


/*
	Powinien pozwalac na zmianę interpolatora;
	dzieli się na dwie części:
	- edytor komendy
	- edytor interpolatora (osobne okno)
*/
class MoveCommandEditor : public ICommandEditor {
public:


};



