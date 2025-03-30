#include "hti.hpp"

int main() {
	auto* app = new hti::Application;
	auto* test = new hti::widgets::Widget;
	std::thread n([&]() {
		sleep(3000);
		app->exit();
		});
	n.detach();
	app->mainloop();
	delete app;
	return 0;
}