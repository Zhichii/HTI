#include "hti.hpp"

namespace hti::widgets {

	Widget::TextEvent::TextEvent(Widget* widget, i18n::Text text) : Event(EV_TEXT, [&](Event* ev) {
		Widget::TextEvent* tev = (Widget::TextEvent*)ev;
		tev->_widget->_text = tev->_text;
		}), _widget(widget), _text(text) {}

	Widget::Widget(Widget* parent) : _parent(parent) {
		this->_focus = false;
		this->_parent = parent;
		if (this->_parent) {
			this->_app = this->_parent->_app;
			// this->_app->addWidget(this);
			this->_parent->_children.push_back(this);
		}
		else throw std::runtime_error("No parent!");
	}

	inline std::string hti::widgets::Widget::onRender() { return ""; }

	i18n::Text Widget::text() const {
		return this->_text;
	}

	void Widget::text(i18n::Text text) {
		this->_app->pushEvent((Event*)new TextEvent(this, text));
	}

}