// cos jakby issues, bliższe todo list, do issues nie chce mi sie patrzec
- bullet
- lua
- joint interpolated movement
- teaching

- dodać czas do komend
- uogólnić interpolatory na zestawy {glm::vec4}, {vec4, quat}, {vector<double>}, najlepiej templatkowo, a odpowiednim
	kontenerom zapewnić odpowiednie operatory


==== joint interpolated movement ====
	konstruktor ma dwie opcje:
	- dostajemy zestaw wspóżednych zlaczowych i jestemy happy
	- dostajemy zestaw punktów, zlaczowe musimy sobie wyliczyc
	dodatkowo dostajemy zestaw modyfiaktorów predkosci
	interpolator sobie wybieramy
	
==== teaching =====
	co klatke zbieramy pozycje robota: vec4, quat i joint, w sumie to wystarczy joint, reszta jest wyliczalna
	przy pushu należy pamitać o tym żeby wygadzić punkty, albo można wykorzystać solver z lerpem do ledzenia pozycji
	punkt niech bdzie poruszany przez pointController z edytora, czemu wiec nie przeniesc by teachingu do 
	interpolatorów?
	
==== sledzenie trasy ====
	w kazdym frame wyliczamy o ile trzeba sie przemiecić, nastpnie zdejmujemy z interpolatora punkty aż przemieszczenie nie bedzie wieksze niz zakadane, potem można zinterpolować pomidzy punktami, ale po chuj?

==== commands ====
	wydzielic:
	- move<glm::vec4> interpolacja jedynie trasy kartezjańskiej, to co teraz jest
	- move<Point> interpolacja trasy 3D i kontrolowanie orientacji efektora
	- move<Joints> interpolacja zlaczowa
		Każdy z inna implementacja, tryzmajacy inny interpolator
	- goTo<vec4>
	- goTo<Point>
	- goTo<Joints>
	- wait - przerywa dziaanie na okrelony czas
	- waitOnSignal - przerywa czekajc na sygnal
	- waitIfSignal - czeka na czas lub sygnal
	- executeOnSignal - wykonuje rozkaz po dostaniu sygnalu, sygnaly sa w globalnym obiekcie Signals, sygnal może istniec do odebrania, badz jedna klatke
	- executeOnValue - definiujemu wasny operator porównania executeOn([&value]()bool->{return value > 20}, []{...})
	- 
	
	commands sa tworzone przez builder na podstawie enuma/stringa, nastpnie parametryzowane, można je też jako do lua  wepchać()
==== Signals ====
	pierwsze 32 bity sa resetowane co klatke, pozostae sa czyszczone po odczycie
	{
		uint64_t data;
		void update(){
			data &= 0xffffffff00000000;
		}
		bool operator [] (uint64_t i){
			bool out = data & i;
			if(i > 0xFFFFFFFF)
				data &= ~i;
			return out;
		}
	}
	
==== Ksztaty ====
	w sumie to po co?
	Przydalyby sie jakies predefiniowane ksztalty typu circle, arc, 
	- circle - (punkt, normalna, promień), (trzy punkty), (dwa punkty, normalna, promień)
	- arc (dwa punkty, normalna, promień, która czesc), (trzy punkty), (punkt, normalna, uk w katach)

==== Edytor ====
	zrobic obracanie grup punktów

==== ogólne ====
	graf ma defaultowy tryb LastX, trzeba mu zrobić opcje exportu danych dla matlaba
	dać możliwoć logowania wiekszej iloci danych
	
==== punkty ====
	mozliwosc zapisania pozycji robota {vec4, quat, vector<float>} do mapy/vectora, z menu, komendy savePoint()
	punkty te maja generyczne nazwy Px, 
	vector bedzie lepszy
	zrobic tak by dalo sie punkty wywietlic na 3D, jakos korzystac w interpolatorach, np goTo(point("P123")+glm::vec4(0,0,13));
	
==== zrobić w końcu klase Label bedaca const charem[] o staym rozmiarze, np. 30 znaków, 
	struct InfoLabel {
		char word[31];
		char size;
		InfoLabel(const char* _word){
			std::copy(_word, strlen(_word), word);
		}
		InfoLabel(const string _word){
			std::copy(_word.begin(), _word.end(), word);
		}
		InfoLabel(const InfoLabel&) = delete;
		InfoLabel(const InfoLabel&&) = delete;
		InfoLabel& operator = (const InfoLabel&) = delete;  
	};
	powinno sie ladnie alignować.

==== 
	

