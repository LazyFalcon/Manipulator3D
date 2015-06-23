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
	punkt niech bdzie poruszany przez pointController z edytora, czemu wiec nie przeniesc by teachingu do interpolatorów?
	
==== 
	

