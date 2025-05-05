#include "hti.hpp"

namespace hti::widgets {
	
	Widget::Widget(Widget* parent)
		: _parent(parent), _app(parent ? parent->_app : ((Application*)this)) {
		this->_visible = true;
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

	std::list<Widget*>& Widget::children() {
		if (this->app()->isMainThread()) return this->_children;
		throw std::runtime_error("This is not main thread!");
	}

	std::list<Widget*>::iterator Widget::children_begin() { return this->children().begin(); }

	std::list<Widget*>::iterator Widget::children_end() { return this->children().end(); }

	size_t Widget::children_size() {
		if (this->app()->isMainThread()) return this->_children.size();
		std::promise<size_t> prom;
		this->app()->postEvent(std::make_shared<LambdaEvent>([self = this, &prom](Event*) {
			prom.set_value(self->_children.size());
			}));
		return prom.get_future().get();
	}

	const std::list<Widget*> Widget::children_copy() const {
		if (this->app()->isMainThread()) return this->_children;
		std::promise<std::list<Widget*>> prom;
		this->app()->postEvent(std::make_shared<LambdaEvent>([self = this, &prom](Event*) {
			prom.set_value(self->_children);
			}));
		return prom.get_future().get();
	}

	bool Widget::visible() { return this->_visible; }

	void Widget::visible(bool visible_) { this->_visible = visible_; }

	bool Widget::canBeSelected() const { return false; }

	inline std::string Widget::onRender(bool focus) { return ""; }

	bool Widget::onKeyPress(Key key) { return false; }

	void Widget::onChildAdd() {}

	void Widget::onFocusGained() {}

	void Widget::onFocusLost() {}

	SelectableWidget::SelectableWidget(Widget* parent)
		: Widget(parent) {

	}

	bool SelectableWidget::canBeSelected() const { return true; }

}