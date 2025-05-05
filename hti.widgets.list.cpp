#include "hti.hpp"

namespace hti::widgets {

	List::List(Widget* parent, Style style)
		: Widget(parent), SelectableWidget(parent) {
		this->app()->tryPostEvent(std::make_shared<LambdaEvent>([self = this, style](Event* ev) {
			self->_index = self->children_end();
			self->_style = style;
			}));
	}

	std::string List::onRender(bool focus) {
		std::ostringstream output;
		if ((children_size() == 0) || this->_index == this->children_end()) return "";
		const std::string separator = (_style == STYLE_VERTICAL) ? "\n" : " ";

		bool first = true;
		for (auto i = this->children_begin(); i != this->children_end(); i++) {
			auto child = *i;
			if (!first) {
				output << separator;
			}
			else first = false;
			if (child->visible()) {
				bool is_selected = (i == _index);
				output << child->onRender(focus && is_selected);
			}
		}

		return output.str();
	}

	bool List::onKeyPress(Key key) {
		if (this->children_size() == 0) return false;
		if ((*this->_index)->onKeyPress(key)) {
			return true;
		}

		// 回退机制，如果选中的项无法被调用则回退到列表操作。
		auto old_index = this->_index;
		if (key.isPrev()) {
			while (this->_index != this->children_begin()) {
				this->_index--;
				if ((*this->_index)->canBeSelected() &&
					(*this->_index)->visible()) {
					return true;
				}
			}
			// 寻找失败，恢复旧位置。
			this->_index = old_index;
		}
		else if (key.isNext()) {
			while (std::next(this->_index) != this->children_end()) {
				this->_index++;
				if ((*this->_index)->canBeSelected() &&
					(*this->_index)->visible()) {
					return true;
				}
			}
			// 寻找失败，恢复旧位置。
			this->_index = old_index;
		}

		return false;
	}

	void List::onChildAdd() {
		if (this->_index != this->children_end() &&
			(*this->_index)->canBeSelected() &&
			(*this->_index)->visible()) {
			// 如果当前选中的项仍然有效（可选中且可见），则保持不变
			return;
		}
		auto last_item = children_end(); last_item--;
		// 现在指向最后一个元素。
		if ((*last_item)->canBeSelected() && (*last_item)->visible()) {
			this->_index = last_item;
		}
	}

}