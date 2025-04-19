#include "hti.hpp"

int main() {
	auto* app = new hti::Application;
	auto* test = new hti::widgets::Label(app, "What?");
	time_t t = time(NULL);
	std::thread n([&]() {
		while (true) {
			sleep_ms(100);
			time_t t2 = time(NULL);
			test->text(std::to_string(t2-t));
		}
		});
	n.detach();
	app->mainloop();
	delete app;
	return 0;
}