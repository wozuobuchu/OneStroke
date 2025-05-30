#ifndef _THREAD_INPUT_HPP
#define _THREAD_INPUT_HPP

#define DEBUG

namespace device {

class MouseMonitorThread final {
private:
	std::shared_mutex smtx__mouse_state;
	std::pair<int,int> _mouse_pos;
	std::tuple<int,int,int> _mouse_button;
	std::jthread _monit_task_jthread;
	void _monit_task(std::stop_token st) {
		POINT pt;
		while(!st.stop_requested()) {
			// update mouse state
			if(GetCursorPos(&pt)) {
				HWND hWnd = WindowFromPoint(pt);
				if(hWnd) {
					ScreenToClient(hWnd, &pt);
					std::unique_lock<std::shared_mutex> lck(this->smtx__mouse_state);
					_mouse_pos = {pt.x, pt.y};
					_mouse_button = { (GetAsyncKeyState(VK_LBUTTON) & 0x8000), 0, 0 };
				}
			}
		}
		return;
	}
public:
	MouseMonitorThread() {
		this->_mouse_pos = {0, 0};
		this->_mouse_button = {0, 0, 0};
		this->_monit_task_jthread = std::jthread(std::bind(&MouseMonitorThread::_monit_task, this, std::placeholders::_1));
	}
	~MouseMonitorThread() { this->_monit_task_jthread.request_stop(); }

	std::pair<int,int> getMousePosition() {
		std::pair<int,int> ret;
		{
			std::shared_lock<std::shared_mutex> lck(this->smtx__mouse_state);
			ret = _mouse_pos;
		}
		return ret;
	}
	std::tuple<int,int,int> getMouseStateSimple() {
		std::tuple<int,int,int> ret;
		{
			std::shared_lock<std::shared_mutex> lck(this->smtx__mouse_state);
			auto [x,y] = _mouse_pos;
			auto [l,m,r] = _mouse_button;
			ret = {x,y,l};
		}
		return ret;
	}

};

}

#endif // !_THREAD_INPUT_HPP