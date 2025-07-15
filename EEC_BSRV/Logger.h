#ifndef LOG_HDR_FILE_
#define LOG_HDR_FILE_

#include <string>

using namespace std;

enum LogLevel { DEBUG, INFO, WARNING, ERRORS, CRITICAL };

class logger
{
public:

	void checkLogDirectory();
	void writeToLog(LogLevel level, char* message);

private:
	string getAvailableFileName();
};

#endif
