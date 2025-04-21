#include "hti.hpp"

// 继承示例。
class TitleList : public hti::widgets::TextWidget, public hti::widgets::List {
public:
	TitleList(hti::widgets::Widget* parent, hti::i18n::Text text = {}) : Widget(parent), TextWidget(parent, text), List(parent) {}
	std::string onRender(bool focus) override {
		std::ostringstream oss;
		if (focus) oss << "<" + this->text().localize(this->app()->languages()) + ">\n";
		else oss << " " + this->text().localize(this->app()->languages()) + " \n";
		oss << List::onRender(focus);
		return oss.str();
	}
};

int main() {
	auto* app = new hti::Application;
	auto* cont = new TitleList(app, "Yahoo!1");
	auto* cont_test = new hti::widgets::Button(cont, "Test");
	long long n = 0;
	auto* cont_c2 = new TitleList(cont, "Yahoo!");
	auto* cont_c2_num = new hti::widgets::Label(cont_c2, "0");
	auto* cont_c2_sel1 = new hti::widgets::Button(cont_c2, "Prev");
	auto* cont_c2_sel2 = new hti::widgets::Button(cont_c2, "Next");
	cont_c2_sel1->bind([&](hti::widgets::Widget*) {
		n--;
		cont_c2_num->text(std::to_string(n));
		});
	cont_c2_sel2->bind([&](hti::widgets::Widget*) {
		n++;
		cont_c2_num->text(std::to_string(n));
		});
	app->mainloop();
	delete app;
	return 0;
}