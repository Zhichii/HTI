#include "hti.hpp"

namespace hti::widgets {

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

	inline void hti::widgets::Widget::onRender() {
	}

}