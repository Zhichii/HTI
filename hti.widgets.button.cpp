#include "hti.hpp"

namespace hti::widgets {

	Button::Button(Widget* parent, i18n::Text text)
		: Widget(parent), TextWidget(parent, text), SelectableWidget(parent) {
	}

	std::string Button::onRender(bool focus) {
		if (focus) return "[" + this->text().localize(this->app()->languages()) + "]";
		else return "." + this->text().localize(this->app()->languages()) + ".";
	}

	void Button::bind(WidFunc action) {
		this->_action = action;
	}

	bool Button::onKeyPress(char key) {
		if (key == ' ') {
			if (_action) this->_action(this);
			return true;
		}
		else return false;
	}

}