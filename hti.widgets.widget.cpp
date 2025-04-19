#include "hti.hpp"

namespace hti::widgets {
	
	Widget::Widget(Widget* parent)
		: _parent(parent), _app(parent ? parent->_app : ((Application*)this)) {
		if (this->_parent) {
			this->app()->postEventWithCheck(std::make_shared<LambdaEvent>([self=this](Event*) {
				if (self->_app->children().size() != 0) {
					throw std::runtime_error("Application can only have one child!");
				}
				self->_app_iter = self->_app->_widgets.insert(
					self->_app->_widgets.end(), self);
				self->_parent_iter = self->_parent->_children.insert(
					self->_parent->_children.end(), self);
				}));
		}
	}

	Widget::~Widget() {
		this->app()->postEventWithCheck(std::make_shared<LambdaEvent>([self = this](Event*) {
			while (!self->_children.empty()) {
				auto child = self->_children.front();
				delete child; // 会自动从self->_children中移除。
			}
			self->_parent->_children.erase(self->_parent_iter);
			self->_app->_widgets.erase(self->_app_iter);
			}));
	}

	Application* Widget::app() const {
		return this->_app;
	}

	Widget* Widget::parent() const {
		return this->_parent;
	}

	const std::list<Widget*> Widget::children() const {
		std::promise<std::list<Widget*>> prom;
		this->app()->postEventWithCheck(std::make_shared<LambdaEvent>([self=this, &prom](Event*) {
			prom.set_value(self->_children);
			}));
		return prom.get_future().get();
	}

	inline std::string Widget::onRender(bool focus) {
		return "";
	}

	bool Widget::onKeyPress(char key) {
		return false;
	}

}