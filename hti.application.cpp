#include "hti.hpp"

namespace hti {

	Event::Event(EventType type, EventFunc func) : _func(func), _type(type) {}

	EventFunc Event::func() const { return this->_func; }

	Application::ExitEvent::ExitEvent(Application* app) : Event(EV_EXIT, [&](Event* ev) {
		Application::ExitEvent* eev = (Application::ExitEvent*)ev;
		eev->_app->_should_exit = true;
		}), _app(app) {}

	Application::Application(): _should_exit(false), _displaying(0), _focus(0) {
	}

	Application::~Application() {
	}

	void Application::pushEvent(Event* ev) {
		this->_mutex.lock();
		this->_queue.push(ev);
		this->_mutex.unlock();
	}

	void hti::Application::mainloop() {
		while (!this->_should_exit) {
			if (Event* ev = popEvent()) {
				ev->func()(ev);
				delete ev;
			}
		}
	}

	void Application::exit() {
		this->pushEvent((Event*)new ExitEvent(this));
	}

	Event* Application::popEvent() {
		Event* ev = NULL;
        this->_mutex.lock();
		if (this->_queue.size()) {
			ev = this->_queue.front();
			this->_queue.pop();
		}
        this->_mutex.unlock();
		return ev;
    }

}
