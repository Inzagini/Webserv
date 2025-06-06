#include "utils.hpp"

void	logPrint(std::string type, std::string msg){
	std::time_t now = std::time(NULL);
	std::tm* localTime = std::localtime(&now);
	std::cout << "["
			<< (1900 + localTime->tm_year) << "-"
			<< std::setw(2) << std::setfill('0') << (1 + localTime->tm_mon) << "-"
			<< std::setw(2) << std::setfill('0') << localTime->tm_mday
			<< " "
			<< std::setw(2) << std::setfill('0') << localTime->tm_hour << "-"
			<< std::setw(2) << std::setfill('0') << localTime->tm_min << "-"
			<< std::setw(2) << std::setfill('0') << localTime->tm_sec
			<< "] ";
	std::cout << "[" << type << "]\t";
	std::cout << msg;
}
