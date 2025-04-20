#include "hti.hpp"

namespace hti::widgets {
	
	Widget::Widget(Widget* parent)
		: _parent(parent), _app(parent ? parent->_app : ((Application*)this)) {
		if (this->_parent) {
			this->app()->tryPostEvent(std::make_shared<LambdaEvent>([self=this](Event*) {
				if (self->_app->children().size() > 0 && self->_parent == self->_app) {
					throw std::runtime_error("Application can only have one root child!");
				}
				self->_app_iter = self->_app->_widgets.insert(
					self->_app->_widgets.end(), self);
				self->_parent_iter = self->_parent->_children.insert(
					self->_parent->_children.end(), self);
				self->_parent->onChildAdd();
				}));
		}
	}

	Widget::~Widget() {
		this->app()->tryPostEvent(std::make_shared<LambdaEvent>([self = this](Event*) {
			while (!self->_children.empty()) {
				delete self->_children.front(); // 会自动从 self->_children 中移除。
			}
			if (self->_parent) { // 否则是 Application，无需移除。
				self->_app->_widgets.erase(self->_app_iter);
				self->_parent->_children.erase(self->_parent_iter);
			}
			}));
	}

	Application* Widget::app() const {
		return this->_app;
	}

	Widget* Widget::parent() const {
		return this->_parent;
	}

	const std::list<Widget*> Widget::children() const {

		if (this->app()->isMainThread()) return this->_children;

		std::promise<std::list<Widget*>> prom;
		this->app()->postEvent(std::make_shared<LambdaEvent>([self=this, &prom](Event*) {
			prom.set_value(self->_children);
			}));
		return prom.get_future().get();
	}

	bool Widget::canBeSelected() const { return false; }

	inline std::string Widget::onRender(bool focus) { return ""; }

	bool Widget::onKeyPress(char key) { return false; }

	void Widget::onChildAdd() {}

	void Widget::onFocusGained() {}

	void Widget::onFocusLost() {}

	SelectableWidget::SelectableWidget(Widget* parent)
		: Widget(parent) {
	}

	bool SelectableWidget::canBeSelected() const { return true; }

}