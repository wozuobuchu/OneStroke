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

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	__game__init();
	__game__loop();
	return 0;
}
