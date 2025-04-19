#include "hti.hpp"

namespace hti::widgets {

	TextWidget::TextWidget(Widget* parent, i18n::Text text)
		: Widget(parent), _text(text) {
	}

	i18n::Text TextWidget::text() const {
		return this->_text;
	}

	void TextWidget::text(i18n::Text text) {
		this->app()->postEventWithCheck(std::make_shared<LambdaEvent>([self=this, text](Event*) {
			self->_text = text;
			}));
	}

	Label::Label(Widget* parent, i18n::Text text)
		: TextWidget(parent, text) {
	}

	std::string Label::onRender(bool focus) {
		return this->text().localize(this->app()->languages());
	}

}