#include "include.h"
#include "macros.h"

#ifndef MAIN_CONFIG
	#define MAIN_CONFIG
		struct Pattern
		{
			std::vector<std::vector<sf::Color>> color;
			std::vector<std::vector<std::vector<sf::Color>>> top;
			std::vector<std::vector<std::vector<sf::Color>>> bottom;
			std::vector<std::vector<std::vector<sf::Color>>> left;
			std::vector<std::vector<std::vector<sf::Color>>> right;
		};

		enum class EVENT_CODE
		{
			NONE,
			CLOSE,
			RESTART
		};
#endif