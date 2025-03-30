#include "hti.hpp"

namespace hti::widgets {

	Widget::Widget(Widget* parent) : _parent(parent) {
		this->_parent = parent;
		if (this->_parent) {
			this->_app = this->_parent->_app;
			// this->_app->addWidget(this);
			this->_parent->_children.push_back(this);
		}
		else throw std::runtime_error("No parent!");
	}

	inline std::string hti::widgets::Widget::onRender() { return ""; }

	TextWidget::TextEvent::TextEvent(TextWidget* widget, i18n::Text text) : Event(EV_TEXT, [&](Event* ev) {
		TextEvent* tev = (TextEvent*)ev;
		tev->_widget->_text = tev->_text;
		}), _widget(widget), _text(text) {
	}

	i18n::Text TextWidget::text() const {
		return this->_text;
	}

	void TextWidget::text(i18n::Text text) {
		this->app()->pushEvent((Event*)new TextEvent(this, text));
	}

}