#include "tools.h"

void tools::setPixelToPixmap(std::vector<uint8_t>& pixmap, int iIndex, sf::Color color)
{
	pixmap[iIndex * 4 + 0] = color.r;
	pixmap[iIndex * 4 + 1] = color.g;
	pixmap[iIndex * 4 + 2] = color.b;
	pixmap[iIndex * 4 + 3] = color.a;
}

void tools::displayConsoleInformation(std::map<std::string, float>& cfg)
{
	system("cls");

	std::cout << "# " << msTitle << " #" << std::endl;
	std::cout << "\n[!] keyboard buttons for control:" << std::endl;
	std::cout << "\t [ ESC ] - exit;" << std::endl;
	std::cout << "\t [ R ] - restart;" << std::endl;
	std::cout << "\n[!] note: visit a \"" << msConfigPath << "\" file to change configuration;" << std::endl;
	std::cout << "\n[!] current configuration:" << std::endl;
	
	for (std::map<std::string, float>::iterator p = cfg.begin(); p != cfg.end(); p++)
		std::cout << "\t" << p -> first << " = " << p -> second << ";" << std::endl;
}

std::map<std::string, float> tools::readConfig(std::string sConfigPath)
{
	std::map<std::string, float> cfg;

	zer::File file(sConfigPath);
	file.read({zer::file::Modifier::lines});

	for (int i = 0; i < file.linesLen(); ++i)
	{
		std::string sLine = file.lineAt(i);
		if (sLine.find(" = ") != std::string::npos)
		{
			std::vector<std::string> lineParts = zer::athm::split(sLine, " = ");
			cfg[lineParts[0]] = stof(lineParts[1]);
		}
	}

	return cfg;
}

EVENT_CODE tools::eventListener(sf::RenderWindow& window)
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			return EVENT_CODE::CLOSE;
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Escape)
				return EVENT_CODE::CLOSE;
			if (event.key.code == sf::Keyboard::R)
				return EVENT_CODE::RESTART;
		}
	}
	return EVENT_CODE::NONE;
}