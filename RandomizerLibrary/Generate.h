#pragma once
#include "Panel.h"
#include "PuzzleSymbols.h"
#include <stdlib.h>
#include <string>
#include <time.h>
#include <set>
#include <algorithm>
#include <vector>
#include <memory>
#include <Windows.h>

typedef std::set<Point> Shape;

//The main class for generating puzzles.
class Generate
{
public:
	Generate() {
		_width = _height = 0;
		_areaTotal = _genTotal = _totalPuzzles = _areaPuzzles = 0;
		_handle = NULL;
		_panel = NULL;
		_parity = -1;
		_seed = rand();
		arrowColor = backgroundColor = successColor = { 0, 0, 0, 0 };
		resetConfig();
	}
	enum Config { //See configinfo.txt for explanations of config flags.
		None = 0, FullGaps = 0x1, StartEdgeOnly = 0x2, DisableWrite = 0x4, PreserveStructure = 0x8, MakeStonesUnsolvable = 0x10, SmallShapes = 0x20, DisconnectShapes = 0x40, ResetColors = 0x80,
		DisableCancelShapes = 0x100, RequireCancelShapes = 0x200, BigShapes = 0x400, SplitShapes = 0x800, RequireCombineShapes = 0x1000, TreehouseLayout = 0x2000, TreehouseColors = 0x4000,
		AlternateColors = 0x8000, WriteColors = 0x10000, Write2Color = 0x20000, FixBackground = 0x40000, CombineErasers = 0x80000, LongPath = 0x100000, ShortPath = 0x200000, SplitStones = 0x400000,
		DecorationsOnly = 0x800000, FalseParity = 0x1000000, DisableDotIntersection = 0x2000000, WriteDotColor = 0x4000000, WriteDotColor2 = 0x8000000, LongestPath = 0x10000000, WriteInvisible = 0x20000000,
		DisableReset = 0x40000000, MountainFloorH = 0x80000000
	};
	
	void generate(int id) { PuzzleSymbols symbols({ }); while (!generate(id, symbols, false)); }
	void generate(int id, int symbol, int amount);
	void generate(int id, int symbol1, int amount1, int symbol2, int amount2);
	void generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3);
	void generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4);
	void generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4, int symbol5, int amount5);
	void generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4, int symbol5, int amount5, int symbol6, int amount6);
	void generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4, int symbol5, int amount5, int symbol6, int amount6, int symbol7, int amount7);
	void generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4, int symbol5, int amount5, int symbol6, int amount6, int symbol7, int amount7, int symbol8, int amount8);
	void generate(int id, int symbol1, int amount1, int symbol2, int amount2, int symbol3, int amount3, int symbol4, int amount4, int symbol5, int amount5, int symbol6, int amount6, int symbol7, int amount7, int symbol8, int amount8, int symbol9, int amount9);
	void generate(int id, std::vector<std::pair<int, int>> symbolVec);
	void generateRandom(int seed, bool debug);
	// void generateMulti(int id, std::vector<std::shared_ptr<Generate>> gens, std::vector<std::pair<int, int>> symbolVec);
	// void generateMulti(int id, int numSolutions, std::vector<std::pair<int, int>> symbolVec);
	void generateMaze(int id);
	void generateMaze(int id, int numStarts, int numExits);
	void initPanel(int id);
	void setPath(std::set<Point> path) {
		customPath = path;
		for (Point p : path) setSymbol(IntersectionFlags::PATH, p.first, p.second); }
	void setObstructions(std::vector<Point> walls) { _obstructions = { walls }; }
	void setObstructions(std::vector<std::vector<Point>> walls) { _obstructions = walls; }
	void setSymbol(Decoration::Shape symbol, int x, int y);
	void setSymbol(IntersectionFlags symbol, int x, int y) { setSymbol(static_cast<Decoration::Shape>(symbol), x, y); }
	void setVal(int val, int x, int y) { _panel->_grid[x][y] = val; }
	void setGridSize(int width, int height);
	void setSymmetry(Panel::Symmetry symmetry);
	// void write(int id);
	void setLoadingHandle(HWND handle) { _handle = handle; }
	void setLoadingData(int totalPuzzles) { _totalPuzzles = totalPuzzles; _genTotal = 0; }
	void setLoadingData(std::wstring areaName, int numPuzzles) { _areaName = areaName; _areaPuzzles = numPuzzles; _areaTotal = 0; }
	void setFlag(Config option) { _config |= option; };
	void setFlagOnce(Config option) { _config |= option; _oneTimeAdd |= option; };
	bool hasFlag(Config option) { return _config & option; };
	void removeFlag(Config option) { _config &= ~option; };
	void removeFlagOnce(Config option) { _config &= ~option; _oneTimeRemove |= option; };
	void resetConfig();
	void seed(long seed) { srand(seed); _seed = rand(); }
	void incrementProgress();
	std::string exportPanel();
	PuzzleSymbols importPanelSetting(std::string json_string);

	float pathWidth; //Controls how thick the line is on the puzzle
	std::vector<Point> hitPoints; //The generated path will be forced to hit these points in order
	std::set<Point> openPos; //Custom set of points that can have symbols placed on
	std::set<Point> blockPos; //Point that must be left open
	std::set<Point> customPath; 
	Color arrowColor, backgroundColor, successColor; //For the arrow puzzles

private:

	int get(Point pos) { return _panel->_grid[pos.first][pos.second]; }
	void set(Point pos, int val) { _panel->_grid[pos.first][pos.second] = val; }
	int get(int x, int y) { return _panel->_grid[x][y]; }
	void set(int x, int y, int val) { _panel->_grid[x][y] = val; }
	int get_symbol_type(int flags) { return flags & 0x700; }
	void set_path(Point pos);
	Point get_sym_point(Point pos) { return _panel->get_sym_point(pos); }
	int get_parity(Point pos) { return (pos.first / 2 + pos.second / 2) % 2; }
	void clear();
	void resetVars();
	void init_treehouse_layout();
	template <class T> T pick_random(std::vector<T>& vec) { return vec[rand() % vec.size()]; }
	template <class T> T pick_random(std::set<T>& set) { auto it = set.begin(); std::advance(it, rand() % set.size()); return *it; }
	template <class T> T pop_random(std::vector<T>& vec) { int i = rand() % vec.size(); T item = vec[i]; vec.erase(vec.begin() + i); return item; }
	template <class T> T pop_random(std::set<T>& set) { T item = pick_random(set); set.erase(item); return item; }
	bool on_edge(Point p) { return (Point::pillarWidth == 0 && (p.first == 0 || p.first + 1 == _panel->_width) || p.second == 0 || p.second + 1 == _panel->_height); }
	bool off_edge(Point p) { return (p.first < 0 || p.first >= _panel->_width || p.second < 0 || p.second >= _panel->_height); }
	static std::vector<Point> _DIRECTIONS1, _8DIRECTIONS1, _DIRECTIONS2, _8DIRECTIONS2, _SHAPEDIRECTIONS, _DISCONNECT;
	bool generate_maze(int id, int numStarts, int numExits);
	bool generate(int id, PuzzleSymbols symbols, bool debug); //************************************************************
	bool place_all_symbols(PuzzleSymbols& symbols);
	bool generate_path(PuzzleSymbols& symbols);
	bool generate_path_length(int minLength);
	bool generate_path_regions(int minRegions);
	bool generate_longest_path();
	bool generate_special_path();
	void erase_path();
	Point adjust_point(Point pos);
	std::set<Point> get_region(Point pos);
	std::vector<int> get_symbols_in_region(Point pos);
	std::vector<int> get_symbols_in_region(std::set<Point> region);
	bool place_start(int amount);
	bool place_exit(int amount);
	bool can_place_gap(Point pos);
	bool place_gaps(int amount);
	bool can_place_dot(Point pos);
	bool place_dots(int amount, int color, bool intersectionOnly);
	bool can_place_stone(std::set<Point>& region, int color);
	bool place_stones(int color, int amount);
	Shape generate_shape(std::set<Point>& region, std::set<Point>& bufferRegion, Point pos, int maxSize);
	Shape generate_shape(std::set<Point>& region, Point pos, int maxSize) { std::set<Point> buffer; return generate_shape(region, buffer, pos, maxSize); }
	int make_shape_symbol(Shape shape, bool rotated, bool negative, int rotation, int depth);
	int make_shape_symbol(Shape shape, bool rotated, bool negative) { return make_shape_symbol(shape, rotated, negative, -1, 0); }
	bool place_shapes(std::vector<int> colors, std::vector<int> negativeColors, int amount, int numRotated, int numNegative);
	int count_color(std::set<Point>& region, int color);
	bool place_stars(int color, int amount);
	bool has_star(std::set<Point>& region, int color);
	bool place_triangles(int color, int amount, int targetCount);
	int count_sides(Point pos);
	bool place_arrows(int color, int amount, int targetCount);
	int count_crossings(Point pos, Point dir);
	bool place_erasers(std::vector<int> colors, std::vector<int> eraseSymbols);
	bool combine_shapes(std::vector<Shape>& shapes);

	std::shared_ptr<Panel> _panel;
	std::vector<std::vector<int>> _custom_grid;
	int _width, _height;
	Panel::Symmetry _symmetry;
	std::set<Point> _starts, _exits;
	std::set<Point> _gridpos, _openpos;
	std::set<Point> _path, _path1, _path2;
	bool _fullGaps, _bisect;
	int _stoneTypes;
	int _config;
	int _oneTimeAdd, _oneTimeRemove;
	long _seed;
	std::vector<Point> _splitPoints;
	bool _allowNonMatch; //Used for multi-generator
	int _parity;
	std::vector<std::vector<Point>> _obstructions;

	HWND _handle;
	int _areaTotal, _genTotal, _areaPuzzles, _totalPuzzles;
	std::wstring _areaName;

	friend class PuzzleList;
	friend class Special;
	friend class MultiGenerate;
};

