#pragma once
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/tee.hpp>

#include <fstream>
#include <iostream>

namespace logger
{
	typedef boost::iostreams::tee_device<std::ostream, std::ofstream> Tee;
	typedef boost::iostreams::stream<Tee> TeeStream;

	extern std::ofstream logFile;
	extern std::ofstream errFile;
	extern Tee logTee;
	extern Tee errTee;

	extern TeeStream log;
	extern TeeStream err;

	void init();
	void close();

}
