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

Json exportColor(int symbol)
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
			{"rotation",(symbol & Decoration::Can_Rotate) ? 1 : 0} };
	else if (decoration_type == Decoration::Triangle)
	{
		if ((symbol & Decoration::Triangle1) == Decoration::Triangle1)
			return Json{ {"type","triangle"},{"color",exportColor(symbol)},{"number",1 } };
		else if ((symbol & Decoration::Triangle2) == Decoration::Triangle2)
			return Json{ {"type","triangle"},{"color",exportColor(symbol)},{"number",2 } };
		else if ((symbol & Decoration::Triangle3) == Decoration::Triangle3)
			return Json{ {"type","triangle"},{"color",exportColor(symbol)},{"number",3 } };
	}
	else if (decoration_type == Decoration::Arrow)
	{
		if ((symbol & Decoration::Arrow1) == Decoration::Arrow1)
			return Json{ {"type","arrow"},{"color",exportColor(symbol)},{"number",1 } };
		else if ((symbol & Decoration::Arrow2) == Decoration::Arrow2)
			return Json{ {"type","arrow"},{"color",exportColor(symbol)},{"number",2 } };
		else if ((symbol & Decoration::Arrow3) == Decoration::Arrow3)
			return Json{ {"type","arrow"},{"color",exportColor(symbol)},{"number",3 } };
	}
	else if ((symbol & Decoration::Dot) == Decoration::Dot)
		return Json{ {"type","dot"},{"color",exportColor(symbol)} };
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