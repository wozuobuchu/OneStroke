#ifndef _GAME_LOGIC_HPP
#define _GAME_LOGIC_HPP

namespace game {

bool game_loop_flag = true;

void switch_to_menu();
void switch_to_game();

class Button {
protected:
	std::pair<int,int> _position;
	std::pair<int,int> _coord_up_left;
	std::pair<int,int> _coord_low_right;
	std::wstring _text;
	int _half_width;
	int _half_height;
	COLORREF _color;
	COLORREF _default_color;
	COLORREF _text_color;
	bool _clicked;

	virtual void _when_click() = 0;
public:
	Button(std::pair<int,int> pos, std::pair<int,int> half_size, COLORREF color, std::wstring text) : 
		_position(pos),
		_color(color),
		_default_color(color),
		_text(text),
		_text_color(WHITE),
		_clicked(false)
	{
		auto [mx,my] = _position;
		auto [hw,hh] = half_size;
		_half_width = hw;
		_half_height = hh;
		_coord_up_left = {mx-hw, my-hh};
		_coord_low_right = {mx+hw, my+hh};
	}

	void _draw_this() const {
		auto [x1,y1] = _coord_up_left;
		auto [x2,y2] = _coord_low_right;
		setlinecolor(_color);
		setfillcolor(_color);
		fillrectangle(x1,y1,x2,y2);

		if(_text.size()==0) return;

		settextcolor(_text_color);
		settextstyle(_half_height*2/3, 0, __T("Arial"));
		auto [tx,ty] = _coord_up_left;
		ty += _half_height*2/3;
		outtextxy(tx,ty,_text.c_str());

		return;
	}

	bool _judge_mouse_postion(int x,int y) const {
		auto [x1,y1] = _coord_up_left;
		auto [x2,y2] = _coord_low_right;
		return ((x1<=x && x<=x2) && (y1<=y && y<=y2));
	}

	bool _judge_click(int x, int y, int c) {
		if(!c) return false;
		if(_clicked) return false;
		if(_judge_mouse_postion(x,y)) {
			_clicked = true;
			this->_when_click();
			return true;
		}
		return false;
	}

	void _reset_click() {
		this->_clicked = false;
		return;
	}

	void _set_color(COLORREF color) {
		this->_color = color;
		return;
	}

	void _reset_color() {
		this->_color = this->_default_color;
		return;
	}

	void _set_text(std::wstring text) {
		this->_text = text;
		return;
	}

};

class PlayButton : public Button {
protected:
	void _when_click() override {
		std::cout<<" [ DEBUG ] PLAY_BUTTON CLICK "<<std::endl;
		switch_to_game();
		return;
	}
public:
	PlayButton() : Button({280,370},{120,60},LIGHTCYAN,__T("PLAY")) {}
};

class ExitButton : public Button {
protected:
	void _when_click() override {
		std::cout<<" [ DEBUG ] EXIT_BUTTON CLICK "<<std::endl;
		game_loop_flag = false;
		return;
	}
public:
	ExitButton() : Button({280,500},{120,60},LIGHTCYAN,__T("EXIT")) {}
};

class MainTitle : public Button {
protected:
	void _when_click() override {
		return;
	}
public:
	MainTitle() : Button({280,200},{160,80},LIGHTMAGENTA,__T("ONE STROKE")){}
};

class GameButton : public Button {
protected:
	void _when_click() override { return; }
public:
	const std::pair<int,int> _grid_pos;
	const std::pair<int,int> _paint_pos;
	GameButton(std::pair<int,int> pos, std::pair<int,int> grid_pos) :
		Button(pos,{45,45},WHITE,__T("")),
		_grid_pos(grid_pos),
		_paint_pos(pos)
	{}
};

void reset_game_interface();
class ResetButton : public Button {
protected:
	void _when_click() override {
		reset_game_interface();
		this->_reset_click();
		return;
	}
public:
	ResetButton() : Button({100,680},{60,30},LIGHTCYAN,__T("RESET")) {}
};

class ResumeButton : public Button {
protected:
	void _when_click() override {
		switch_to_menu();
		return;
	}
public:
	ResumeButton() : Button({240,680},{60,30},LIGHTCYAN,__T("RESUME")) {}
};

void show_hint();
class HintButton : public Button {
protected:
	void _when_click() override {
		show_hint();
		this->_reset_click();
		return;
	}
public:
	HintButton() : Button({380,680},{60,30},LIGHTCYAN,__T("HINT")) {}
};

/**
* @brief Launch mouse monitor thread
*/
device::MouseMonitorThread mouse_thread;

class Logic {
public:
	Logic(){}
	virtual ~Logic(){}
	virtual void execute() = 0;
};

class MainMenuLogic final : public Logic{
public:
	std::deque< std::shared_ptr<Button> > _button_deque;
	void execute() override {
		auto [x,y,c] = mouse_thread.getMouseStateSimple();
		for(auto& bptr : _button_deque) {
			if(bptr->_judge_mouse_postion(x,y)) bptr->_set_color(LIGHTGRAY);
			else bptr->_reset_color();
			bptr->_judge_click(x,y,c);
			bptr->_draw_this();
		}
		return;
	}

	MainMenuLogic() {
		_button_deque.push_front(std::static_pointer_cast<Button>(std::make_shared<ExitButton>()));
		_button_deque.push_front(std::static_pointer_cast<Button>(std::make_shared<PlayButton>()));
		_button_deque.push_front(std::static_pointer_cast<Button>(std::make_shared<MainTitle>()));
	}
};

class GameInterfaceLogic final : public Logic{
private:
	std::deque<std::pair<int,int>> _solution;
	std::vector<std::vector<std::shared_ptr<game::GameButton>>> _grid;
	std::pair<int,int> _start;
	std::pair<int,int> _end;
	std::pair<int,int> _size;

	std::pair<int,int> _last_pos;
	int _total_node_num;
	int _current_node_num;

	int _directs[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};
	bool isClickValid(int cur_x, int cur_y) const {
		auto [sx,sy] = _start;
		auto [ex,ey] = _end;

		if(cur_x==sx && cur_y==sy) return true;

		auto [last_x,last_y] = _last_pos;
		auto [edge_x,edge_y] = _size;
		for(int i=0; i<4; ++i) {
			std::pair<int,int> tar_pos{cur_x+_directs[i][0],cur_y+_directs[i][1]};
			auto [tar_x,tar_y] = tar_pos;
			if(!(0<=tar_x && tar_x<edge_x && 0<=tar_y && tar_y<edge_y)) continue;
			if(_grid[tar_x][tar_y]==nullptr) continue;
			if(last_x==tar_x&&last_y==tar_y) return true;
		}

		return false;
	}

public:
	bool game_hint_signal;
	std::deque< std::shared_ptr<Button> > _button_deque;

	void execute() override {
		auto [m,n] = _size;
		auto [x,y,c] = mouse_thread.getMouseStateSimple();
		for(int i=0; i<m; ++i) {
			for(int j=0; j<n; ++j) {
				if(_grid[i][j]==nullptr) continue;
				if(_grid[i][j]->_judge_click(x,y,c)) {
					if(isClickValid(i,j)){
						_grid[i][j]->_set_color(LIGHTGRAY);
						_last_pos = {i,j};
						++_current_node_num;
					}
					else{
						_grid[i][j]->_reset_click();
					}
				}
				_grid[i][j]->_draw_this();
			}
		}

		for(auto& bptr : _button_deque) {
			if(bptr->_judge_mouse_postion(x,y)) bptr->_set_color(LIGHTGRAY);
			else bptr->_reset_color();
			bptr->_judge_click(x,y,c);
			bptr->_draw_this();
		}

		if(game_hint_signal) {
			auto _last_paint = _solution[0];
			for(int k=1; k<_solution.size(); ++k) {
				auto [i1,j1] = _last_paint;
				auto [i2,j2] = _solution[k];
				setlinecolor(RED);
				line(
					_grid[i1][j1]->_paint_pos.first,
					_grid[i1][j1]->_paint_pos.second,
					_grid[i2][j2]->_paint_pos.first,
					_grid[i2][j2]->_paint_pos.second
				);
				_last_paint = _solution[k];
			}
		}

		if(_current_node_num>=_total_node_num && _last_pos==_end) {
			MessageBox(NULL, __T("Clearance !"), __T("Info"), MB_OK | MB_ICONASTERISK);
			game::switch_to_menu();
		}

		return;
	}

	void reSet() {
		auto [m,n] = _size;
		for(int i=0; i<m; ++i) {
			for(int j=0; j<n; ++j) {
				if(_grid[i][j]==nullptr) continue;
				_grid[i][j]->_reset_click();
				_grid[i][j]->_reset_color();
			}
		}
		_last_pos = {-1,-1};
		_current_node_num = 0;
		game_hint_signal = false;
		return;
	}

	void reGenerate() {
		std::mt19937 RandE(std::random_device{}());
		std::uniform_int_distribution<int> randint(0, (int)levels::levels_umap.size()-1);
		int _level = 1;
		_level = randint(RandE);

		// DEBUG
		//_level = levels::levels_umap.size()-1;

		_solution.clear();
		_grid.clear();
		graph_algo::DfsHamiltonPath _dfs(
			levels::levels_umap[_level]._start,
			levels::levels_umap[_level]._end,
			levels::levels_umap[_level]._grid
		);
		_solution = _dfs.getSolution();
		if(!_solution.size()) {
			MessageBox(NULL, __T("Unsolvable Level  "), __T("Warn"), MB_OK | MB_ICONERROR);
			exit(1);
		}

		_start = _dfs._start;
		_end = _dfs._end;
		_size = _dfs._size;
		_total_node_num = _dfs.getNodeNum();
		
		_current_node_num = 0;
		_last_pos = {-1,-1};

		auto _origin_grid = _dfs.getOriginGrid();
		auto [m,n] = _size;
		
		_grid.resize(m,std::vector<std::shared_ptr<game::GameButton>>(n,std::shared_ptr<game::GameButton>()));
		for(int i=0; i<m; ++i) {
			for(int j=0; j<n; ++j) {
				if(_origin_grid[i][j]==0) {
					_grid[i][j].reset();
					_grid[i][j] = std::make_shared<game::GameButton>(
						std::make_pair(BASE_POINT_X+BUTTON_GAP*j,BASE_POINT_Y+BUTTON_GAP*i),
						std::make_pair(i,j)
					);
					if(i==_start.first && j==_start.second) _grid[i][j]->_set_text(__T("S"));
					else if(i==_end.first && j==_end.second) _grid[i][j]->_set_text(__T("E"));
				}
			}
		}

		return;
	}

	GameInterfaceLogic() : _current_node_num(0), _total_node_num(0), game_hint_signal(false) {
		_button_deque.push_front(std::static_pointer_cast<Button>(std::make_shared<ResumeButton>()));
		_button_deque.push_front(std::static_pointer_cast<Button>(std::make_shared<ResetButton>()));
		_button_deque.push_front(std::static_pointer_cast<Button>(std::make_shared<HintButton>()));
	}

};

std::shared_ptr<Logic> current_logic;
std::shared_ptr<MainMenuLogic> main_menu_logic = std::make_shared<MainMenuLogic>();
std::shared_ptr<GameInterfaceLogic> game_interface_logic = std::make_shared<GameInterfaceLogic>();

void switch_to_menu() {
	current_logic.reset();
	current_logic = std::static_pointer_cast<game::Logic>(main_menu_logic);
	
	for(auto& bptr : main_menu_logic->_button_deque) bptr->_reset_click();

	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	return;
}
void switch_to_game() {
	current_logic.reset();
	current_logic = std::static_pointer_cast<game::Logic>(game_interface_logic);

	for(auto& bptr : game_interface_logic->_button_deque) bptr->_reset_click();
	game_interface_logic->reGenerate();
	game_interface_logic->reSet();

	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	return;
}

void reset_game_interface() {
	game_interface_logic->reSet();
	return;
}

void show_hint() {
	game_interface_logic->game_hint_signal = true;
	return;
}

}

#endif // !_GAME_LOGIC_HPP
