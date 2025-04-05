#ifndef _THREAD_INPUT_HPP
#define _THREAD_INPUT_HPP

#define DEBUG

namespace device {

class DeviceMonitorThread {
public:
	DeviceMonitorThread() : _break_flag(false){}
	virtual ~DeviceMonitorThread(){}
protected:
	const int _flush_elapsed_time = 100;

	std::condition_variable _cv;
	std::mutex mtx__break_flag;
	bool _break_flag;

	std::thread _monit_task_thread;
	virtual void _monit_task() = 0;

	bool stopCheck() {
        std::unique_lock<std::mutex> ulck(this->mtx__break_flag);
		return _cv.wait_for(ulck,std::chrono::microseconds(_flush_elapsed_time),[this](){return this->_break_flag;});
	}

    void stopThread() {
        {
            std::unique_lock<std::mutex> ulck(this->mtx__break_flag);
            this->_break_flag = true;
        }
		_cv.notify_all();
        if (_monit_task_thread.joinable()) _monit_task_thread.join();
		return;
    }
};

class MouseMonitorThread final : public DeviceMonitorThread {
public:
	MouseMonitorThread() : DeviceMonitorThread() {
		this->_mouse_pos = {0, 0};
		this->_mouse_button = {0, 0, 0};
		this->_monit_task_thread = std::thread(std::bind(&MouseMonitorThread::_monit_task, this));
	}
	~MouseMonitorThread() override {
		this->stopThread();

		#ifdef DEBUG
		std::cout<<" [ DEBUG ] MouseMonitor _monit_tsak_thread JOINED \n"<<std::endl;
		#endif // DEBUG
	}

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
	std::tuple<int,int,int,int, int> getMouseState() {
		std::tuple<int,int,int,int,int> ret;
		{
			std::shared_lock<std::shared_mutex> lck(this->smtx__mouse_state);
			auto [x,y] = _mouse_pos;
			auto [l,m,r] = _mouse_button;
			ret = {x,y,l,m,r};
		}
		return ret;
	};

private:
	void _monit_task() override {
		#ifdef DEBUG
		std::cout<<" [ DEBUG ] MouseMonitorThread _monit_thread LAUNCH \n";
		#endif // DEBUG
		ExMessage msg;
		while(true) {
			// check if this class is destructed
			if(this->stopCheck()) return;

			// update mouse state
			if( peekmessage(&msg,EX_MOUSE,true) ) {
				
				{
					std::unique_lock<std::shared_mutex> lck(this->smtx__mouse_state);
					this->_mouse_pos = {msg.x, msg.y};
					this->_mouse_button = {msg.lbutton, msg.mbutton, msg.rbutton};
				}
			}

		}
		return;
	}

	std::shared_mutex smtx__mouse_state;
	std::pair<int,int> _mouse_pos;
	std::tuple<int,int,int> _mouse_button;
};

}

#endif // !_THREAD_INPUT_HPP