#include "head.hpp"

namespace fps_func {
class FPS_Limiter {
public:
    const std::chrono::duration< double,std::ratio<1,1> > targetFrameTime;
    std::chrono::high_resolution_clock::time_point start,end;
    long long targetDelayMS;
    FPS_Limiter(const double targetFPS):
        targetFrameTime(1.0/targetFPS),
        start(std::chrono::high_resolution_clock::now()),
        targetDelayMS(std::chrono::duration_cast<std::chrono::milliseconds>(targetFrameTime).count())
    {}
    void limit(){
        end=std::chrono::high_resolution_clock::now();
        std::chrono::duration< double,std::ratio<1,1> > eplased(end-start);
        if(eplased<targetFrameTime) std::this_thread::sleep_for(targetFrameTime-eplased);
        start=std::chrono::high_resolution_clock::now();
        return;
    }
};
}

static void __async__iostream() {
	std::ios::sync_with_stdio(false);
	std::cin.tie(nullptr);
	std::cout.tie(nullptr);
	return;
}

static void __game__init() {
	initgraph(WINDOW_WIDTH, WINDOW_HEIGH, EX_NOCLOSE | EX_NOMINIMIZE);
	setbkcolor(LIGHTBLUE);
	game::switch_to_menu();
	return;
}

static void __game__loop() {
	fps_func::FPS_Limiter _fps_limiter(240.0);
	BeginBatchDraw();
	while(game::game_loop_flag) {
		cleardevice();
		game::current_logic->execute();
		FlushBatchDraw();
		_fps_limiter.limit();
	}
	EndBatchDraw();
	return;
}

int main() {
	__async__iostream();

	__game__init();

	__game__loop();

	std::wcout<<" [ DEBUG ] EasyX Ver : "<<std::wstring(GetEasyXVer())<<"  \n";

	std::cout<<" [ DEBUG ] Main _main_thread JOINED \n";

    return 0;
}
