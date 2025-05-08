#include "head.hpp"

static void __game__init() {
	initgraph(WINDOW_WIDTH, WINDOW_HEIGH, EX_NOCLOSE | EX_NOMINIMIZE);
	setbkcolor(LIGHTBLUE);
	game::switch_to_menu();
	return;
}
static void __game__loop() {
	fps_func::FPS_Limiter _fps_limiter(FPS_LIMIT);
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
	__game__init();
	__game__loop();
	std::wcout<<L" [ DEBUG ] Main Thread Joined. "<<std::endl;
    return 0;
}
