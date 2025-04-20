#include "hti.hpp"

namespace hti::widgets {

	List::List(Widget* parent)
		: Widget(parent), SelectableWidget(parent), _index(0) { }

	std::string List::onRender(bool focus) {

		std::ostringstream output;
		const auto& childList = this->children();
		const size_t itemCount = childList.size();

		if (itemCount == 0) return "";

		if (this->_index >= itemCount) {
			this->_index = itemCount - 1;
		}

		const std::string separator = (_style == STYLE_VERTICAL) ? "\n" : " ";

		size_t i = 0;
		for (Widget* child : childList) {
			if (i > 0) output << separator;
			output << child->onRender(focus && (this->_index == i));
			i++;
		}

		return output.str();
	}

	bool List::onKeyPress(char key) {

		const auto& childList = this->children();
		const size_t itemCount = childList.size();

		if (itemCount == 0) return false;

		auto selectedItem = childList.begin();
		std::advance(selectedItem, std::min(this->_index, itemCount - 1));
		if ((*selectedItem)->onKeyPress(key)) {
			return true;
		}

		size_t idx = this->_index;
		// 回退机制
		if (key == 'w' || key == 'W') {
			while (this->_index > 0) {
				selectedItem--;
				this->_index--;
				if ((*selectedItem)->canBeSelected()) return true;
			}
			// 寻找失败。
			this->_index = idx;
		}
		else if (key == 's' || key == 'S') {
			while (this->_index + 1 < itemCount) {
				// 原 (this->_index < itemCount - 1)，
				// 但 itemCount 减一可能会变成 4294967295。
				selectedItem++;
				this->_index++;
				if ((*selectedItem)->canBeSelected()) return true;
			}
			// 寻找失败。
			this->_index = idx;
		}

		return false;
	}

	void List::onChildAdd() {
		const auto& childList = this->children();
		const size_t itemCount = childList.size();
		auto selectedItem = childList.begin();
		std::advance(selectedItem, std::min(this->_index, itemCount - 1));
		if ((*selectedItem)->canBeSelected()) return;
		else {
			size_t idx = this->_index;
			while (this->_index + 1 < itemCount) {
				selectedItem++;
				this->_index++;
				if ((*selectedItem)->canBeSelected()) return;
			}
			// 寻找失败。
			this->_index = idx;
		}
	}

}