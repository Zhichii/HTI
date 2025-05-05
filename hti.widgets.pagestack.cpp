#include "hti.hpp"

namespace hti::widgets {

	Pages::Pages(Widget* parent)
		: Widget(parent), SelectableWidget(parent) {
		this->_index = this->children_end();
	}

	std::string Pages::onRender(bool focus) {
		if (this->_index == this->children_end()) return "";
		else return (*this->_index)->onRender(focus);
	}

	bool Pages::onKeyPress(Key key) {
		if (this->_index == this->children_end()) return false;
		else return (*this->_index)->onKeyPress(key);
	}

	void Pages::onChildAdd() {
		if (this->_index == this->children_end()) {
			this->_index = this->children_begin();
		}
	}

	bool Pages::selPrev() {
		if (this->_index != this->children_begin()) {
			this->_index--;
			return true;
		}
		else return false;
	}

	bool Pages::selNext() {
		if (std::next(this->_index) != this->children_end()) {
			this->_index++;
			return true;
		}
		else return false;
	}

	void Pages::selBegin() {
		this->_index = this->children_begin();
	}

	void Pages::selEnd() {
		this->_index = this->children_end();
	}

	void Pages::select(size_t index) {
		this->selBegin();
		for (size_t i = 0; i < index; i++) {
			if (!this->selNext()) break;
		}
	}

	bool Pages::canBeSelected() const {
		return (*this->_index)->canBeSelected();
	}

	PageStack::PageStack(Widget* parent, i18n::Text title, Style style)
		: Widget(parent), TextWidget(parent, title), SelectableWidget(parent) {
		this->app()->tryPostEvent(std::make_shared<LambdaEvent>([self = this, style](Event* ev) {
			self->_pages = self->add<Pages>();
			self->_style = style;
			self->_it = self->_navigation.end();
			}));
	}

	PageStack::~PageStack() {
		delete this->_pages;
	}

	bool PageStack::onKeyPress(Key key) {
		// 依次定义：
		// 切换到导航栏、切换到内容、还有导航栏改页面（左、右）。
		bool nav = 0, con = 0, swp = 0, swn = 0;
		if (this->_style == STYLE_UP_DOWN) {
			nav = key.isUp();
			con = key.isDown();
			swp = key.isLeft();
			swn = key.isRight();
		}
		//if (this->_style == STYLE_LEFT_RIGHT) {
		//	nav = 'a'; con = 'd'; swu = 'w'; swd = 'd';
		//}
		if (this->_pos == 0) { // 在导航栏。
			if (swp && this->_it != this->_navigation.begin()) {
				this->_it--; this->_pages->selPrev();
				return true;
			}
			if (swn && std::next(this->_it) != this->_navigation.end()) {
				this->_it++; this->_pages->selNext();
				return true;
			}
		}
		if (this->_pos == 1) { // 在内容。
			if (this->_pages->onKeyPress(key)) {
				return true;
			}
		}
		if (nav && (this->_pos != 0)) { this->_pos = 0; return true; }
		if (con && (this->_pos != 1) && this->_pages->canBeSelected()) {
			this->_pos = 1; return true;
		}
		return false;
	}

	std::string PageStack::onRender(bool focus) {
		std::ostringstream oss;
		std::string title = this->text().localize(this->app()->languages());
		if (title != "") {
			if (focus) oss << "<" << title << ">\n";
			else oss << "." << title << ".\n";
		}
		if (this->_style == STYLE_UP_DOWN) {
			for (auto i = this->_navigation.begin(); i != this->_navigation.end(); i++) {
				if (i == this->_it) {
					if (focus && this->_pos == 0) oss << "[";
					else oss << ".";
				}
				else oss << " ";
				oss << (*i).first.localize(this->app()->languages());
				if (i == this->_it) {
					if (focus && this->_pos == 0) oss << "]";
					else oss << ".";
				}
				else oss << " ";
			}
		}
		oss << "\n";
		oss << this->_pages->onRender(focus && this->_pos == 1);
		return oss.str();
	}

}