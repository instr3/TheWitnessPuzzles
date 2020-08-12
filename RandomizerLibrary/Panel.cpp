#include "Panel.h"
#include "Memory.h"
#include <sstream>
#include <fstream>
#include <memory>
#include <Windows.h>
#include "pch.h"

int Point::pillarWidth = 0;
std::vector<Panel> Panel::generatedPanels;

template <class T>
int find(const std::vector<T> &data, T search, size_t startIndex = 0) {
	for (size_t i=startIndex ; i<data.size(); i++) {
		if (data[i] == search) return static_cast<int>(i);
	}
	return -1;
}



void Panel::SetSymbol(int x, int y, Decoration::Shape symbol, Decoration::Color color)
{
	int gridx = x * 2 + (symbol & IntersectionFlags::COLUMN ? 0 : 1);
	int gridy = y * 2 + (symbol & IntersectionFlags::ROW ? 0 : 1);
	if (symbol & IntersectionFlags::DOT) {
		if (color == Decoration::Color::Blue || color == Decoration::Color::Cyan)
			color = static_cast<Decoration::Color>(IntersectionFlags::DOT_IS_BLUE);
		else if (color == Decoration::Color::Orange || color == Decoration::Color::Yellow)
			color = static_cast<Decoration::Color>(IntersectionFlags::DOT_IS_ORANGE);
		else color = Decoration::Color::None;
		if (symmetry) {
			Point sp = get_sym_point(gridx, gridy);
			SetGridSymbol(sp.first, sp.second, static_cast<Decoration::Shape>(symbol & ~Decoration::Dot), Decoration::Color::None);
		}
	}
	else if (symbol & IntersectionFlags::ROW || symbol & IntersectionFlags::COLUMN)
		color = Decoration::Color::None;
	SetGridSymbol(gridx, gridy, symbol, color);
}

void Panel::SetShape(int x, int y, int shape, bool rotate, bool negative, Decoration::Color color)
{
	if (!shape) return;
	int symbol = Decoration::Shape::Poly;
	while (!(shape & 0xf)) shape >>= 4;
	while (!(shape & 0x1111)) shape >>= 1;
	shape <<= 16;
	if (rotate) shape |= Decoration::Shape::Can_Rotate;
	else shape &= ~Decoration::Shape::Can_Rotate;
	if (negative) shape |= Decoration::Shape::Negative;
	else shape &= ~Decoration::Shape::Negative;
	_grid[x * 2 + 1][y * 2 + 1] = symbol | shape | color;
}

void Panel::ClearSymbol(int x, int y)
{
	ClearGridSymbol(x * 2 + 1, y * 2 + 1);
}

void Panel::SetGridSymbol(int x, int y, Decoration::Shape symbol, Decoration::Color color)
{
	if (symbol == Decoration::Start) _startpoints.push_back({ x, y });
	if (symbol == Decoration::Exit) {
		Endpoint::Direction dir;
		if (y == 0) dir = Endpoint::Direction::UP;
		else if (y == _height - 1) dir = Endpoint::Direction::DOWN;
		else if (x == 0) dir = Endpoint::Direction::LEFT;
		else dir = Endpoint::Direction::RIGHT;
		if (id == 0x033D4 || id == 0x0A3B5) {
			if (x == 0) dir = Endpoint::Direction::LEFT;
			else dir = Endpoint::Direction::RIGHT;
		}
		if (symmetry == Symmetry::ParallelH || symmetry == Symmetry::ParallelHFlip) {
			if (x == 0) dir = Endpoint::Direction::LEFT;
			if (x == _width - 1) dir = Endpoint::Direction::RIGHT;
		}
		_endpoints.push_back(Endpoint(x, y, dir, IntersectionFlags::ENDPOINT | 
			(dir == Endpoint::Direction::UP || dir == Endpoint::Direction::DOWN ?
				IntersectionFlags::COLUMN : IntersectionFlags::ROW)));
	}
	else _grid[x][y] = symbol | color;
}

void Panel::ClearGridSymbol(int x, int y)
{
	_grid[x][y] = 0;
}

void Panel::Resize(int width, int height)
{
	for (Point &s : _startpoints) {
		if (s.first == _width - 1) s.first = width - 1;
		if (s.second == _height - 1) s.second = height - 1;
	}
	for (Endpoint &e : _endpoints) {
		if (e.GetX() == _width - 1) e.SetX(width - 1);
		if (e.GetY() == _height - 1) e.SetY(height - 1);
	}
	if (_width != _height || width != height) {
		float maxDim = max(maxx - minx, maxy - miny);
		float unitSize = maxDim / max(width - 1, height - 1);
		minx = 0.5f - unitSize * (width - 1) / 2;
		maxx = 0.5f + unitSize * (width - 1) / 2;
		miny = 0.5f - unitSize * (height - 1) / 2;
		maxy = 0.5f + unitSize * (height - 1) / 2;
	}
	_width = width;
	_height = height;
	_grid.resize(width);
	for (auto& row : _grid) row.resize(height);
	_resized = true;
}