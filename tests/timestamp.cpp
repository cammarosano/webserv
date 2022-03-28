#include <ctime>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>

std::string get_timestamp() 
{
    static const char *wday_name[]
        = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    static const char *mon_name[]
        = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
            "Aug", "Sep", "Oct", "Nov", "Dec"};
    time_t now = time(NULL);
    tm *t = gmtime(&now);
    std::ostringstream timestamp;
	timestamp << std::setfill('0');
    timestamp << wday_name[t->tm_wday] << ", ";
    timestamp << std::setw(2) << t->tm_mday << ' ';
	timestamp << mon_name[t->tm_mon] << ' ';
	timestamp << 1900 + t->tm_year << ' ';
	timestamp << std::setw(2) << t->tm_hour << ':';
	timestamp << std::setw(2) << t->tm_min << ':';
	timestamp << std::setw(2) << t->tm_sec << ' ';
	timestamp << "GMT";
	return (timestamp.str());
}

int main()
{
	std::cout << get_timestamp() << std::endl;

}
