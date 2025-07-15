#include "misc.h"
#include <string>
#include <ctime>

//Get Current Date Time in String Format "yyyy-mm-dd hh:mm:ss"
string currentDateTimeInString()
{
    char buf[80];
    time_t now = time(0);

    struct tm tmStruct = *localtime(&now);
    memset(buf, 0, sizeof(buf));
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tmStruct);

    return buf;
}

string currentDateLogFileNameInString()
{
    char buf[80];
    time_t now = time(0);

    struct tm tmStruct = *localtime(&now);
    memset(buf, 0, sizeof(buf));
    strftime(buf, sizeof(buf), "%Y%m%d", &tmStruct);

    return buf;
}