#ifndef _HEAD_LEVELS_HPP

namespace levels {

class OriginGrid{
public:
	std::pair<int,int> _start;
	std::pair<int,int> _end;
	std::vector<std::vector<char>> _grid;
	OriginGrid(){}
	OriginGrid(std::pair<int,int> start, std::pair<int,int> end, std::vector<std::vector<char>> grid) :
		_start(start),
		_end(end),
		_grid(grid)
	{}
};

std::unordered_map<int,OriginGrid> levels_umap{
	{1, OriginGrid({0,0},{4,1},std::vector<std::vector<char>>{
		{0, 0, 1, 1, 1},
		{1, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0}
	})},
	{2, OriginGrid({0,0},{2,0},std::vector<std::vector<char>>{
		{0, 0, 1, 1, 1},
		{1, 0, 0, 0, 0},
		{0, 0, 0, 0, 0}
	})},
	{3, OriginGrid({0,0},{4,1},std::vector<std::vector<char>>{
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 1, 1, 0},
		{0, 0, 0, 1, 1, 0},
		{0, 0, 0, 0, 0, 0}
	})},
	{4, OriginGrid({0,0},{4,3},std::vector<std::vector<char>>{
		{0, 0, 1, 0, 0},
		{1, 0, 1, 0, 0},
		{1, 0, 0, 0, 0},
		{0, 0, 0, 1, 0},
		{0, 0, 0, 0, 0}
	})},
	{5, OriginGrid({0,0},{4,4},std::vector<std::vector<char>>{
		{0, 0, 0, 0, 0, 1},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 1, 1, 1, 0},
		{0, 0, 1, 1, 1, 0},
		{0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0}
	})},
};

}

#define _HEAD_LEVELS_HPP
#endif // !_HEAD_LEVELS_HPP
