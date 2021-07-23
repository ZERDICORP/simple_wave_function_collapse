#include "config.h"
#include "tools.h"

int init(sf::RenderWindow& window);
int loop(sf::RenderWindow& window, std::map<std::string, float>& cfg)
{
	bool bNeedToUpdateConsole = true;
	bool bCollapse = true;

	int iSqW = 10;
	int iRows = mWH / iSqW;
	int iCols = mWW / iSqW;

	std::string sFontPath = "./static/fonts/Roboto.ttf";
	std::string sInputImagePath = "./input.png";

	sf::Image inputImage;
	if (!inputImage.loadFromFile(sInputImagePath))
	{
		std::cout << "\n[error]:\n\tinput image not found (should be \"" << sInputImagePath << "\").." << std::endl;
		system("pause > nul");
		return -1;
	}
	
	std::vector<Pattern> patterns;

	int iImageHeight = inputImage.getSize().y;
	int iImageWidth = inputImage.getSize().x;

	/*
		Parsing patterns.
	*/
	for (int iY = 0; iY < iImageHeight; iY += iSqW)
	{
		for (int iX = 0; iX < iImageWidth; iX += iSqW)
		{
			std::vector<sf::Color> row(iSqW);
			std::vector<std::vector<sf::Color>> area(iSqW, row);

			for (int y = 0; y < iSqW; ++y)
				for (int x = 0; x < iSqW; ++x)
					area[y][x] = inputImage.getPixel(iX + x, iY + y);

			int iIndex = -1;
			for (int i = 0; i < patterns.size(); ++i)
				if (patterns[i].color == area)
				{
					iIndex = i;
					break;
				}

			if (iIndex < 0)
			{
				Pattern pattern;
				pattern.color = area;
				
				patterns.push_back(pattern);
				
				iIndex = patterns.size() - 1;
			}

			if (zer::athm::inRange2D(iImageHeight, iImageWidth, iY - iSqW, iX))
			{
				std::vector<std::vector<sf::Color>> top(iSqW, row);
				for (int y = 0; y < iSqW; ++y)
					for (int x = 0; x < iSqW; ++x)
						top[y][x] = inputImage.getPixel(iX + x, iY - iSqW + y);
				if (!zer::athm::vectorHas(patterns[iIndex].top, top))
					patterns[iIndex].top.push_back(top);
			}

			if (zer::athm::inRange2D(iImageHeight, iImageWidth, iY + iSqW, iX))
			{
				std::vector<std::vector<sf::Color>> bottom(iSqW, row);
				for (int y = 0; y < iSqW; ++y)
					for (int x = 0; x < iSqW; ++x)
						bottom[y][x] = inputImage.getPixel(iX + x, iY + iSqW + y);
				if (!zer::athm::vectorHas(patterns[iIndex].bottom, bottom))
					patterns[iIndex].bottom.push_back(bottom);
			}

			if (zer::athm::inRange2D(iImageHeight, iImageWidth, iY, iX - iSqW))
			{
				std::vector<std::vector<sf::Color>> left(iSqW, row);
				for (int y = 0; y < iSqW; ++y)
					for (int x = 0; x < iSqW; ++x)
						left[y][x] = inputImage.getPixel(iX - iSqW + x, iY + y);
				if (!zer::athm::vectorHas(patterns[iIndex].left, left))
					patterns[iIndex].left.push_back(left);
			}

			if (zer::athm::inRange2D(iImageHeight, iImageWidth, iY, iX + iSqW))
			{
				std::vector<std::vector<sf::Color>> right(iSqW, row);
				for (int y = 0; y < iSqW; ++y)
					for (int x = 0; x < iSqW; ++x)
						right[y][x] = inputImage.getPixel(iX + iSqW + x, iY + y);
				if (!zer::athm::vectorHas(patterns[iIndex].right, right))
					patterns[iIndex].right.push_back(right);
			}
		}
	}

	std::vector<std::vector<sf::Color>> behind(iRows, std::vector<sf::Color>(iCols, sf::Color(255, 0, 0)));
	
	for (int i = 0; i < patterns.size(); ++i)
	{
		patterns[i].top.push_back(behind);
		patterns[i].bottom.push_back(behind);
		patterns[i].left.push_back(behind);
		patterns[i].right.push_back(behind);
	}

	std::vector<std::vector<std::vector<int>>> superpositionMap(iRows, std::vector<std::vector<int>>(iCols, std::vector<int>()));

	for (int r = 0; r < iRows; ++r)
		for (int c = 0; c < iCols; ++c)
		{
			superpositionMap[r][c].resize(0);
			for (int i = 0; i < patterns.size(); ++i)
				superpositionMap[r][c].push_back(i);
		}

	std::vector<uint8_t> pixmap(mWH * mWW * 4, 0);
	for (int i = 0; i < mWH * mWW; ++i)
		tools::setPixelToPixmap(pixmap, i, sf::Color::Black);

	sf::Texture texture;
	texture.create(mWW, mWH);

	sf::Font font;
	font.loadFromFile(sFontPath);

	sf::Text text;
	text.setFont(font);
	text.setString(std::to_string(patterns.size()));
	text.setCharacterSize(iSqW - 2);

	int iLastTileRow = -1;
	int iLastTileCol = -1;

	while (window.isOpen())
	{
		/*
			WFC algorithm body.
		*/
		if (bCollapse)
		{
			int iMinEntropy = patterns.size();
			int iNotCollapsingTiles = 0;
			int iTileRow = zer::athm::rand_int(iRows);
			int iTileCol = zer::athm::rand_int(iCols);

			for (int iRow = 0; iRow < iRows; ++iRow)
				for (int iCol = 0; iCol < iCols; ++iCol)
				{
					int iEntropy = superpositionMap[iRow][iCol].size();
					if (!iEntropy)
						return init(window);

					if (iEntropy > 1)
					{
						if (iEntropy < iMinEntropy)
						{
							iMinEntropy = iEntropy;
							iTileRow = iRow;
							iTileCol = iCol;
						}
						iNotCollapsingTiles++;
					}
				}

			int iRandomIndex = zer::athm::rand_int(superpositionMap[iTileRow][iTileCol].size());
			int iItemIndex = superpositionMap[iTileRow][iTileCol][iRandomIndex];

			superpositionMap[iTileRow][iTileCol].resize(0);
			superpositionMap[iTileRow][iTileCol].push_back(iItemIndex);

			if (zer::athm::inRange2D(iRows, iCols, iTileRow - 1, iTileCol) &&
				!(iTileRow - 1 == iLastTileRow && iTileCol == iLastTileCol))
				for (int i = 0; i < superpositionMap[iTileRow - 1][iTileCol].size(); ++i)
					if (!zer::athm::vectorHas(patterns[iItemIndex].top, patterns[superpositionMap[iTileRow - 1][iTileCol][i]].color))
					{
						superpositionMap[iTileRow - 1][iTileCol].erase(superpositionMap[iTileRow - 1][iTileCol].begin() + i);
						i--;
					}

			if (zer::athm::inRange2D(iRows, iCols, iTileRow + 1, iTileCol) &&
				!(iTileRow + 1 == iLastTileRow && iTileCol == iLastTileCol))
				for (int i = 0; i < superpositionMap[iTileRow + 1][iTileCol].size(); ++i)
					if (!zer::athm::vectorHas(patterns[iItemIndex].bottom, patterns[superpositionMap[iTileRow + 1][iTileCol][i]].color))
					{
						superpositionMap[iTileRow + 1][iTileCol].erase(superpositionMap[iTileRow + 1][iTileCol].begin() + i);
						i--;
					}

			if (zer::athm::inRange2D(iRows, iCols, iTileRow, iTileCol - 1) &&
				!(iTileRow == iLastTileRow && iTileCol - 1 == iLastTileCol))
				for (int i = 0; i < superpositionMap[iTileRow][iTileCol - 1].size(); ++i)
					if (!zer::athm::vectorHas(patterns[iItemIndex].left, patterns[superpositionMap[iTileRow][iTileCol - 1][i]].color))
					{
						superpositionMap[iTileRow][iTileCol - 1].erase(superpositionMap[iTileRow][iTileCol - 1].begin() + i);
						i--;
					}

			if (zer::athm::inRange2D(iRows, iCols, iTileRow, iTileCol + 1) &&
				!(iTileRow == iLastTileRow && iTileCol + 1 == iLastTileCol))
				for (int i = 0; i < superpositionMap[iTileRow][iTileCol + 1].size(); ++i)
					if (!zer::athm::vectorHas(patterns[iItemIndex].right, patterns[superpositionMap[iTileRow][iTileCol + 1][i]].color))
					{
						superpositionMap[iTileRow][iTileCol + 1].erase(superpositionMap[iTileRow][iTileCol + 1].begin() + i);
						i--;
					}

			for (int y = 0; y < iSqW; ++y)
				for (int x = 0; x < iSqW; ++x)
				{
					tools::setPixelToPixmap(pixmap, mWW * (iTileRow * iSqW + y) + (iTileCol * iSqW + x),
						patterns[superpositionMap[iTileRow][iTileCol][0]].color[y][x]);
				}

			iLastTileRow = iTileRow;
			iLastTileCol = iTileCol;

			if (iNotCollapsingTiles == 1)
				bCollapse = false;
		}

		texture.update(&pixmap[0]);
		window.draw(sf::Sprite(texture));

		/*
			Drawing in each cell the number of remaining possible patterns.
		*/
		for (int iRow = 0; iRow < iRows; ++iRow)
			for (int iCol = 0; iCol < iCols; ++iCol)
			{
				int iEntropy = superpositionMap[iRow][iCol].size();
				if (iEntropy > 1)
				{
					text.setString(std::to_string(iEntropy));
					text.setPosition(iCol * iSqW + (iSqW / 2 - text.getLocalBounds().width / 2), iRow * iSqW);
					text.setFillColor(iEntropy == patterns.size() ? sf::Color::Yellow : sf::Color::Green);

					window.draw(text);
				}
			}

		window.display();

		if (bNeedToUpdateConsole)
		{
			tools::displayConsoleInformation(cfg);
			bNeedToUpdateConsole = false;
		}

		switch (tools::eventListener(window))
		{
			case EVENT_CODE::CLOSE:
				window.close();
				break;

			case EVENT_CODE::RESTART:
				return init(window);
		}
	}

	return 0;
}

int init(sf::RenderWindow& window)
{
	std::map<std::string, float> cfg = tools::readConfig(msConfigPath);
	
	return loop(window, cfg);
}

int main()
{
	zer::athm::rand_init();

	sf::RenderWindow window(sf::VideoMode(mWW, mWH), msTitle);
	
	return init(window);
}