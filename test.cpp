#include "hti.hpp"

int main() {
	auto* app = new hti::Application;
	auto* cont = new hti::widgets::List(app);
	auto* cont_title = new hti::widgets::Label(cont, "<What>");
	long long n = 0;
	auto* cont_num = new hti::widgets::Label(cont, "0");
	auto* cont_sel1 = new hti::widgets::Button(cont, "Prev");
	auto* cont_sel2 = new hti::widgets::Button(cont, "Next");
	cont_sel1->bind([&](hti::widgets::Widget*) {
		n--;
		cont_num->text(std::to_string(n));
		});
	cont_sel2->bind([&](hti::widgets::Widget*) {
		n++;
		cont_num->text(std::to_string(n));
		});
	app->mainloop();
	delete app;
	return 0;
}