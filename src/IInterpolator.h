#pragma once
/**
 *  Ok, czym wiec jest interpolator?
 *  - ma określone parametry,
 *  - na żądanie zwraca kolejny punkt trasy
 *  - można podawać z wyprzedzeniem? czy niech tym zajmie się kontroller? Kontroller
 *
 *  Dobrze byłoby wydzielić sensowny interface
 *  Co prawda Krzywe i tak będą edytowane z informacją o typie, ale zawsze raźniej
 *  Na razie zakładamy same Open
 */


enum class Interpolator : u32
{	
	Empty,
	Open,
	Closed,
	Linear,
	BezierCurve, // always nonUniform
	BSpline,
	NURBS,
	HermiteCardinal,
	HermiteFiniteDifference,
	HermiteFiniteDifferenceClosed
};


extern const std::string sEmpty;
extern const std::string sLinear;
extern const std::string sBezierCurve;
extern const std::string sBSpline;
extern const std::string sNURBS;
extern const std::string sHermiteCardinal;
extern const std::string sHermiteFiniteDifference;
extern const std::string sHermiteFiniteDifferenceClosed;

const std::string& InterpolatorTranslate(Interpolator type);

extern const std::vector<std::pair<std::string, Interpolator>> interpolatorEnumWithName;

class IInterpolator
{
public:
	bool finished = false;
	Interpolator type;
	std::string name;
	std::vector<glm::vec4> points;
	std::vector<glm::vec4> visualisation; // for draving
	IInterpolator(const std::vector<glm::vec4> &points, Interpolator type) : type(type), points(points), name("--empty--") {}
	IInterpolator(const std::vector<glm::vec4> &points, Interpolator type, const std::string &name) : type(type), points(points), name(name) {}
	virtual glm::vec4 firstPoint(){return points[0];};
	virtual glm::vec4 nextPoint() = 0;
	virtual void generatePath() = 0;
	virtual void reset() = 0;
	virtual void drawParams() = 0;
	virtual ~IInterpolator(){
		std::cerr<<"delete IInterpolator"<<std::endl;
	}
};

/**
 *  Interpolacja liniowa, cóż więcej napisać?
 */
class Linear : public IInterpolator
{
public:
	/// editable params
	float singleStepLength {0.01f};

	///

	int currentPoint;
	float steps;
	float step = 0;

	int section;
	int maxSections;
	int sectionMaxSteps = 1;
	glm::vec4 sectionStep;
	glm::vec4 vec;

	Linear(const std::vector<glm::vec4> &p) : IInterpolator(p, Interpolator::Linear){
		maxSections = points.size()-1;
		section = -1;
		nextSection();

		generatePath();
	}
	void generatePath();
	void genNextStep();
	void nextSection();
	glm::vec4 nextPoint();
	void reset();
	void drawParams();

	~Linear(){}
};

/**
 *  Interpolacja krzywą beziera dowolnego stopnia, 8 max,
 *  Standardowo, pierwszy i ostani punkt są interpolowane
 *  W przypadku zamknietym nie wiem co się dzieje, trzeba sprawdzić
 *  Beziery 2 stopnia(4 punkty) mają lepsze zachowanie edytora
 */
class BezierCurve : public IInterpolator
{
public:
	/// editable params
	float singleStepLength {0.0001f};

	///

	std::vector<float> weights;
	double position {0};
	double length {0};
	BezierCurve(const std::vector<glm::vec4> &points, const std::vector<float> &weights) : IInterpolator(points, Interpolator::BezierCurve), weights(weights){
		generatePath();
	}
	BezierCurve(const std::vector<glm::vec4> &points) : IInterpolator(points, Interpolator::BezierCurve), weights(points.size(), 1){
		generatePath();
	}

	glm::vec4 nextPoint();
	glm::vec4 currPoint();
	glm::vec4 eval(double param);
	float Berenstein(float t, int n, int i);
	float factorial(int k);

	void calculateLength();
	void generatePath();
	void reset();
	void drawParams();

	~BezierCurve(){}
};

/**
 *  Kontener na krzywe beziera 2 stopnia, wynik działania BSpline::split
 *
class BezierSpline : public IInterpolator
{
	glm::vec4 nextPoint();
	glm::vec4 currPoint();
	glm::vec4 eval(double param);
	float Berenstein(float t, int n, int i);
	float factorial(int k);

	void calculateLength();
	void generatePath();
	void reset();

	~BezierCurve(){}
};
*/

/**
 *  Krzywa B-Sklejana,
 *  Przy edycji operujemy łamaną którą podliśmy na poczatku
 *  Wstawiany punkt umieszczany jest pomiędzy dwoma, wybranymi, sasiadującymi punktami
 *  Można przekształcić w zbiór bezierów 2 stopnia
 */
class BSpline : public IInterpolator
{
public:
	std::vector<glm::vec4> beziers;
	double position {0};
	double length {0};
	float weight {1};
	int numOfBeziers;

	/// editable params
	float singleStepLength {0.0001f};

	BSpline(const std::vector<glm::vec4> &points) : IInterpolator(points, Interpolator::BSpline) {
		generatePath();
	}

	glm::vec4 nextPoint();
	glm::vec4 currPoint();
	void makeNurbs();
	glm::vec4 eval(double param);
	float Berenstein(float t, int n, int i);
	float factorial(int k);
	void calculateLength();
	void generatePath();
	void reset();
	void drawParams();
	std::vector<BezierCurve> split();


	~BSpline(){}
};

/**
 *  W sumie to ssamo co BSplajn, ale nie wiem jak ogarnąć nierówne odcinki
 */
class NURBS : public IInterpolator
{
public:
	std::vector<float> weights;
	std::vector<BezierCurve> beziers;
	double position {0};
	double length {0};

	/// editable params
	float singleStepLength {0.0001f};

	NURBS(const std::vector<glm::vec4> &points, const std::vector<float> &weights) : IInterpolator(points, Interpolator::NURBS), weights(weights){
		generatePath();
	}

	glm::vec4 nextPoint();
	glm::vec4 currPoint();
	void makeNurbs();
	void calculateLength();
	void generatePath();
	void reset();
	void drawParams();

	~NURBS(){}
};

/**
 *  Krzywa hermita, ładnie interpoluje zadane punkty, poza pierwszym i ostatnim,
 *  te traktowane sa jako elementy kierunkowe
 *  jakie wiec zachowanie wybieramy dla krzywej?
 *  - potraktowanie skrajnych punktów jako kierunkowe
 *  - zmuszenie do dodania nowych punktów
 *  x dodanie nowych punktów?
 */
class HermiteCardinal : public IInterpolator
{
public:

	glm::vec4 nextPoint();
	void reset();
	void generatePath();
	void calculateLength();
	glm::vec4 eval(double);
	void drawParams();

	/// editable params
	float singleStepLength {0.0001f};

	HermiteCardinal(const std::vector<glm::vec4> &_points) :
		IInterpolator(_points, Interpolator::HermiteCardinal),
		numOfSegments(_points.size()-2)
		{
			generatePath();
		}



	double position {0};
	double length {0};
	float tension {0.5};
	float weight {1};
	int numOfSegments {0};

};

/**
 *  Interpoluje wszystkie punkty
 */
class HermiteFiniteDifference : public IInterpolator
{
public:

	glm::vec4 nextPoint();
	void reset();
	void generatePath();
	void calculateLength();
	glm::vec4 eval(double);
	void drawParams();

	/// editable params
	float singleStepLength {0.0001f};

	HermiteFiniteDifference(const std::vector<glm::vec4> &_points) :
		IInterpolator(_points, Interpolator::HermiteFiniteDifference),
		numOfSegments(_points.size()-1)
		{
			generatePath();
		}



	double position {0};
	double length {0};
	double tension {0.5};
	float weight {1};
	int numOfSegments {0};
};

/**
 *  Interpoluje wszystkie punkty
 *  Zamknięta, tyle samo segmentów co punktów
 */
class HermiteFiniteDifferenceClosed : public IInterpolator
{
public:

	glm::vec4 nextPoint();
	void reset();
	void generatePath();
	void calculateLength();
	glm::vec4 eval(double);
	void drawParams();

	/// editable params
	float singleStepLength {0.0001f};

	HermiteFiniteDifferenceClosed(const std::vector<glm::vec4> &_points) :
		IInterpolator(_points, Interpolator::HermiteFiniteDifferenceClosed),
		numOfSegments(_points.size())
		{
			generatePath();
		}

	double position {0};
	double length {0};
	double tension {0.5};
	float weight {1};
	int numOfSegments {0};
};


void addInterpolator(shared_ptr<IInterpolator>&);
shared_ptr<IInterpolator> addInterpolator(const std::string type, vector<glm::vec4> &points);
shared_ptr<IInterpolator> addInterpolator(Interpolator type, const vector<glm::vec4> &points);

void removeInterpolator(shared_ptr<IInterpolator>&);

std::list<shared_ptr<IInterpolator>>& getInterpolators();
shared_ptr<IInterpolator>& getInterpolator(const std::string &name);

class InterpolatorFactory
{
public:

	shared_ptr<IInterpolator> build(const std::string type, vector<glm::vec4> &points);
	shared_ptr<IInterpolator> build(Interpolator type, vector<glm::vec4> &points);
};
