#include "Logger.h"


namespace logger
{

	std::ofstream logFile;
	std::ofstream errFile;
	Tee logTee( std::cout, logFile );
	Tee errTee( std::cerr, errFile );

	TeeStream log( logTee );
	TeeStream err( errTee );

	void init(){
		logFile.open("../log.log");
		errFile.open("../err.log");
		time_t rawtime;
		struct tm * timeinfo;
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		asctime (timeinfo);

		log<<asctime (timeinfo)<<"\n";
		err<<asctime (timeinfo)<<"\n";
	}
	void close(){
		logFile.close();
		errFile.close();
	}
}