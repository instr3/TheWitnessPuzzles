#include "Generate.h"
#include "pch.h"
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include "json.hpp"
using Json = nlohmann::json;

std::string exportSymmetry(Panel::Symmetry symmetry)
{
	switch (symmetry)
	{
	case Panel::Symmetry::None:
		return "none"; break;
	case Panel::Symmetry::Horizontal:
		return "horizontal"; break;
	case Panel::Symmetry::Vertical:
		return "vertical"; break;
	case Panel::Symmetry::Rotational:
		return "rotational"; break;
	default:
		return "special"; break;
	}
}

const char* exportColor(int symbol)
{
	int color = symbol & 0xf;
	if (color == Decoration::Black)
		return "black";
	else if (color  == Decoration::White)
		return "white";
	else if (color == Decoration::Red)
		return "red";
	else if (color == Decoration::Purple)
		return "purple";
	else if (color == Decoration::Green)
		return "green";
	else if (color == Decoration::Cyan)
		return "cyan";
	else if (color == Decoration::Magenta)
		return "magenta";
	else if (color == Decoration::Yellow)
		return "yellow";
	else if (color == Decoration::Blue)
		return "blue";
	else if (color == Decoration::Orange)
		return "orange";
	else
		return "";
}

const char* exportDotColor(int symbol)
{
	if ((symbol & IntersectionFlags::DOT_IS_BLUE) == IntersectionFlags::DOT_IS_BLUE)
		return "first";
	else if ((symbol & IntersectionFlags::DOT_IS_ORANGE) == IntersectionFlags::DOT_IS_ORANGE)
		return "second";
	else
		return "black";
}

Json exportSymbol(int symbol)
{
	int decoration_type = symbol & 0x700;
	if (decoration_type == Decoration::Stone)
		return Json{ {"type","square"},{"color",exportColor(symbol)} };
	else if (decoration_type == Decoration::Star)
		return Json{ {"type","star"},{"color",exportColor(symbol)} };
	else if (decoration_type == Decoration::Eraser)
		return Json{ {"type","eraser"},{"color",exportColor(symbol)} };
	else if (decoration_type == Decoration::Poly)
		return Json{ {"type",(symbol & Decoration::Negative) ? "ylop" : "poly"},{"color",exportColor(symbol)},
			{"rotation",(symbol & Decoration::Can_Rotate) ? 1 : 0},{"shape",symbol >> 16} };
	else if (decoration_type == Decoration::Triangle)
	{
		if ((symbol & Decoration::Triangle3) == Decoration::Triangle3)
			return Json{ {"type","triangle"},{"color",exportColor(symbol)},{"number",3 } };
		else if ((symbol & Decoration::Triangle2) == Decoration::Triangle2)
			return Json{ {"type","triangle"},{"color",exportColor(symbol)},{"number",2 } };
		else if ((symbol & Decoration::Triangle1) == Decoration::Triangle1)
			return Json{ {"type","triangle"},{"color",exportColor(symbol)},{"number",1 } };
	}
	else if (decoration_type == Decoration::Arrow)
	{
		if ((symbol & Decoration::Arrow3) == Decoration::Arrow3)
			return Json{ {"type","arrow"},{"color",exportColor(symbol)},{"number",3 } };
		else if ((symbol & Decoration::Arrow2) == Decoration::Arrow2)
			return Json{ {"type","arrow"},{"color",exportColor(symbol)},{"number",2 } };
		else if ((symbol & Decoration::Arrow1) == Decoration::Arrow1)
			return Json{ {"type","arrow"},{"color",exportColor(symbol)},{"number",1 } };
	}
	else if ((symbol & Decoration::Dot) == Decoration::Dot)
		return Json{ {"type","dot"},{"color", "black"} };
	else if ((symbol & Decoration::Gap) == Decoration::Gap)
		return Json{ {"type","gap"} };
	return Json{ {"type","empty"} };
}

Json exportGrid(std::vector<std::vector<int> > grid, int height, int width, std::vector<Point> startPoints,
	std::vector<Endpoint> endpoints)
{
	std::vector<Json> result = std::vector<Json>();
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			if ((grid[x][y] & 0x700) > 0 && (x % 2 == 0 || (y % 2 == 0)))
				result.push_back(Json{ {"type","dot"},{"color",exportDotColor(grid[x][y])} });
			else
				result.push_back(exportSymbol(grid[x][y]));
		}
	}
	for (Point point : startPoints)
		result[point.first * height + point.second]["start"] = 1;
	for (Endpoint point : endpoints)
		result[point.GetX() * height + point.GetY()]["end"] = 1;
	return Json(result);
}

/*Json exportPointList(std::vector<Point> points)
{
	std::vector<Json> result = std::vector<Json>();
	for (Point point : points)
	{
		result.push_back(Json{ {"x",point.first},{"y",point.second} });
	}
	return Json(result);
}

Json exportEndPointList(std::vector<Endpoint> endpoints)
{
	std::vector<Json> result = std::vector<Json>();
	for (Endpoint endpoint : endpoints)
	{
		result.push_back(Json{ {"x",endpoint.GetX()},{"y",endpoint.GetY()},{"dir",endpoint.GetDir()} });
	}
	return Json(result);
}*/

std::string Generate::exportPanel()
{
	Json json = Json{
		{"height", _panel->_height},
		{"width", _panel->_width},
		{"symmetry",exportSymmetry(_panel->symmetry)},
		{"grid",exportGrid(_panel->_grid, _panel->_height, _panel->_width, _panel->_startpoints, _panel->_endpoints)},
		// {"starts",exportPointList(_panel->_startpoints)},
		// {"ends",exportEndPointList(_panel->_endpoints)},
	};
	return json.dump();
}

PuzzleSymbols Generate::importPanelSetting(std::string json_string)
{
	Json json = Json::parse(json_string);
	std::vector<std::pair<int, int> > symbols;
	return PuzzleSymbols(symbols);
}


int randomSize(int symmetry)
{
	double num = rand() % 100 / 100.0;
	if (symmetry == -1)
		return num > 0.3 ? (num > 0.33 ? (num > 0.87 ? (num > 0.9 ? 7 : 6) : 5) : 4) : 3;
	else if(symmetry>0)
		return num > 0.3 ? (num > 0.75 ? (num > 0.87 ? (num > 0.9 ? 7 : 6) : 5) : 4) : 3;
	else
		return num > 0.05 ? (num > 0.3 ? (num > 0.75 ? (num > 0.9 ? 6 : 5) : 4) : 3) : 2;
}

bool withP(double threshold)
{
	double num = rand() % 1000 / 1000.;
	return num < threshold;
}

int selectP(double t1, int v1, double t2, int v2, int v3)
{
	double num = rand() % 1000 / 1000.;
	if (num < t1) return v1;
	if (num - t1 < t2) return v2;
	return v3;
}

int selectP(double t1, int v1, double t2, int v2, double t3, int v3, int v4)
{
	double num = rand() % 1000 / 1000.;
	if (num < t1) return v1;
	if (num - t1 < t2) return v2;
	if (num - t1 - t2 < t3) return v3;
	return v4;
}

int randomInt(int low, int high)
{
	return rand() % (high - low + 1) + low;
}

void randomColor(int* colors, int colorCount)
{
	for (int i = 0; i < colorCount; ++i)
	{
		while (colors[i] == -1)
		{
			colors[i] = rand() % 8 + 1;
			if (colors[i] >= Decoration::Red) colors[i] += 1;
			for (int j = 0; j < i; ++j)
			{
				if (colors[i] == colors[j])
				{
					colors[i] = -1;
					break;
				}
			}
		}
	}
}

const int MAX_COLORS = 5;
int dotColors[] = { Decoration::Color::Black,Decoration::Color::Blue,Decoration::Color::Yellow };
void RandomAssignColor(std::vector<std::pair<int,int> > &symbols, int type, int totalCount, int *colors, int colorCount, int forceColor=-1)
{
	if (totalCount > 0)
	{
		int colorArray[MAX_COLORS] = {};
		for (int i = 0; i < totalCount; ++i)
		{
			int colorID = forceColor == -1 ? rand() % colorCount : forceColor;
			colorArray[colorID]++;
		}
		for (int i = 0; i < colorCount; ++i)
		{
			if (colorArray[i] > 0)
			{
				symbols.push_back(std::make_pair(type | colors[i], colorArray[i]));
			}
		}
	}
}
void Generate::generateRandom(int seed, bool debug)
{
	this->seed(seed);
	while (true)
	{
		bool fullDot = withP(0.1);
		bool forcePolyColor = withP(0.8);
		int symmetry = fullDot ? 0 : selectP(0.7, 0, 0.15, 1, 2);
		int width = randomSize(symmetry == 2 ? -1 : symmetry), height;
		if (withP(0.5)) height = width; else height = randomSize(symmetry);
		int colors[MAX_COLORS];
		int sqrtArea = int(round(sqrt(width * height)));
		int startCount;
		int endCount;
		int colorCount;
		int typeCount;
		int estimatedComplexity;
		int squareCount;
		int starCount;
		int polyCount;
		int ylopCount;
		int triangleCount;
		int eliminatorCount;
		int dotCount;
		int failedCount = 0;
		do
		{
			this->resetConfig();
			this->setGridSize(width, height);
			if (symmetry == 2)
				this->setSymmetry(Panel::Symmetry::Vertical);
			else if (symmetry == 1)
				this->setSymmetry(Panel::Symmetry::Rotational);
			estimatedComplexity = fullDot ? 100 : 10;
			if (withP(fullDot ? 0.7 : 0.3)) colorCount = 1;
			else if (withP(0.6)) colorCount = 2;
			else if (withP(0.6)) colorCount = 3;
			else colorCount = 4;
			for (int i = 0; i < colorCount; ++i) colors[i] = -1;
			if (withP(fullDot? 0.5 : 0.1)) typeCount = 1;
			else if (withP(0.4)) typeCount = 2;
			else if (withP(0.8)) typeCount = 3;
			else typeCount = 4;
			int currentTypeCount = typeCount;
			// Starts
			// startCount = withP(0.8) ? 1 : withP(0.85) ? 2 : 3;
			startCount = selectP(0.7, 1, 0.2, 2, 0.18, 3, 4);
			//Ends
			endCount = selectP(0.75, 1, 0.16, 2, 0.08, 3, 4);

			if (!fullDot)
			{
				if (withP(0.01))
					this->setFlag(Generate::ShortPath);
				else if (withP(0.05))
					this->setFlag(Generate::LongPath);
			}

			// Squares
			if (withP(currentTypeCount / 6.0))
			{
				if (colorCount == 1) continue;
				currentTypeCount -= 1;
				if (withP(0.5)) squareCount = randomInt(1, sqrtArea);
				else if (withP(0.9)) squareCount = randomInt(1, 2 * sqrtArea);
				else squareCount = randomInt(1, 3 * sqrtArea);
			}
			else squareCount = 0;
			// Stars
			if (withP(currentTypeCount / 5.0))
			{
				currentTypeCount -= 1;
				if (withP(0.6)) starCount = randomInt(1, sqrtArea);
				else if (withP(0.9)) starCount = randomInt(1, 2 * sqrtArea);
				else starCount = randomInt(1, 3 * sqrtArea);
			}
			else starCount = 0;
			// Eliminator
			if (withP(currentTypeCount / 4.0))
			{
				if (withP(0.9)) eliminatorCount = 1;
				else if (withP(0.9)) eliminatorCount = 2;
				else eliminatorCount = 3;
				if (fullDot && withP(0.3))
					this->setFlag(Generate::FalseParity);
			}
			else eliminatorCount = 0;
			// Poly
			if (withP(currentTypeCount / 3.0))
			{
				currentTypeCount -= 1;
				if (withP(0.9)) polyCount = randomInt(1, sqrtArea);
				else polyCount = randomInt(1, 2 * sqrtArea);
				if (forcePolyColor)
					colors[0] = Decoration::Yellow;
				if (withP(0.15))
					this->setFlag(Generate::RequireCombineShapes);
				else if (withP(0.1))
					this->setFlag(Generate::SmallShapes);
				else if (withP(0.11))
					this->setFlag(Generate::BigShapes);
				// Ylop
				if (withP(0.3))
				{
					int maxYLopCount = polyCount + 3;
					if (withP(0.9)) ylopCount = randomInt(1, min(maxYLopCount, sqrtArea));
					else ylopCount = randomInt(1, min(maxYLopCount, 2 * sqrtArea));
					if (forcePolyColor)
						colors[1] = Decoration::Blue;
				}
				else ylopCount = 0;
			}
			else
			{
				polyCount = 0; ylopCount = 0;
			}
			// Triangle
			if (withP(currentTypeCount / 2.0))
			{
				currentTypeCount -= 1;
				if (withP(0.9)) triangleCount = randomInt(1, sqrtArea);
				else triangleCount = randomInt(1, 2 * sqrtArea);
			}
			else triangleCount = 0;
			// Dot
			if (withP(currentTypeCount / 1.0) || (symmetry && withP(0.5)))
			{
				if (fullDot) continue;
					currentTypeCount -= 1;
					if (withP(0.5)) dotCount = randomInt(1, sqrtArea);
					else if (withP(0.8)) dotCount = randomInt(1, 2 * sqrtArea);
					else dotCount = randomInt(1, 3 * sqrtArea);
					if (dotCount > (width + 1) * (height + 1))
						continue;
			}
			else dotCount = 0;
				if (fullDot) dotCount = (width + 1) * (height + 1);
			// End of elements
			if (triangleCount + ylopCount + polyCount + squareCount + starCount + eliminatorCount > width * height)
				continue;
			int score = triangleCount * 3 + ylopCount * 6 + polyCount * 5 + squareCount * 2 + starCount * 3 + eliminatorCount * 5 + (dotCount * 3 + 1) / 4;
			if (score < width * height)
				continue;

			std::vector<std::pair<int, int> > symbols;
			symbols.push_back(std::make_pair(Decoration::Start, startCount));
			symbols.push_back(std::make_pair(Decoration::Exit, endCount));
			randomColor(colors, colorCount);
			RandomAssignColor(symbols, Decoration::Stone, squareCount, colors, colorCount);
			RandomAssignColor(symbols, Decoration::Star, starCount, colors, colorCount);
			RandomAssignColor(symbols, Decoration::Eraser, eliminatorCount, colors, colorCount);
			RandomAssignColor(symbols, Decoration::Poly, polyCount, colors, colorCount, forcePolyColor ? 0 : -1);
			RandomAssignColor(symbols, Decoration::Poly | Decoration::Negative, ylopCount, colors, colorCount, forcePolyColor ? 1 : -1);
			RandomAssignColor(symbols, Decoration::Triangle, triangleCount, colors, colorCount);
			if (fullDot) symbols.push_back(std::make_pair(Decoration::Dot_Intersection | Decoration::Color::Black, dotCount));
			else if (dotCount > 0)
			{
				RandomAssignColor(symbols, Decoration::Dot, dotCount, dotColors, 3, symmetry ? -1 : 0);
			}

			if (generate(0, PuzzleSymbols(symbols), debug))
				return;
			failedCount += 1;
			if (failedCount >= 50)
				break;
		} while (true);
	}
}