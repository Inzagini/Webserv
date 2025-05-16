
#include "config.hpp"

//will be deleted
void printTokens(const std::vector<std::string>& tokens) {
	std::cout << "[Tokens]" << std::endl;
    for (std::vector<std::string>::const_iterator it = tokens.begin(); it != tokens.end(); ++it) {
        std::cout << *it << std::endl;
    }
	std::cout << std::endl;
}
