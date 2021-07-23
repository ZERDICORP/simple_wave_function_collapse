#include "config.h"

namespace tools
{
	void setPixelToPixmap(std::vector<uint8_t>& pixmap, int iIndex, sf::Color color);
	void displayConsoleInformation(std::map<std::string, float>& cfg);

	std::map<std::string, float> readConfig(std::string sConfigPath);

	EVENT_CODE eventListener(sf::RenderWindow& window);	
}