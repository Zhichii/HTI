#include "hti.hpp"

namespace hti {

    Key::Key(unsigned int key, unsigned int keycode) {
        this->_key = key;
        this->_keycode = keycode;
    }

    unsigned int Key::key() { return this->_key;  }

    bool Key::isNone() {
        return !this->_key && !this->_keycode;
    }

    bool Key::isPrev() {
        switch (this->_key) {
        case u'w':
        case u'W':
        case u'a':
        case u'A':
            return true;
        }
        switch (this->_keycode) {
        case VK_UP:
        case VK_LEFT:
            return true;
        }
        return false;
    }

    bool Key::isNext() {
        switch (this->_key) {
        case u's':
        case u'S':
        case u'd':
        case u'D':
            return true;
        }
        switch (this->_keycode) {
        case VK_DOWN:
        case VK_RIGHT:
            return true;
        }
        return false;
    }

    bool Key::isPress() {
        switch (this->_key) {
        case u' ':
        case u'\r':
            return true;
        }
        return false;
    }

    bool Key::isUp() {
        switch (this->_key) {
        case u'w':
        case u'W':
            return true;
        }
        switch (this->_keycode) {
        case VK_UP:
            return true;
        }
        return false;
    }

    bool Key::isDown() {
        switch (this->_key) {
        case u's':
        case u'S':
            return true;
        }
        switch (this->_keycode) {
        case VK_DOWN:
            return true;
        }
        return false;
    }

    bool Key::isLeft() {
        switch (this->_key) {
        case u'a':
        case u'A':
            return true;
        }
        switch (this->_keycode) {
        case VK_LEFT:
            return true;
        }
        return false;
    }

    bool Key::isRight() {
        switch (this->_key) {
        case u'd':
        case u'D':
            return true;
        }
        switch (this->_keycode) {
        case VK_RIGHT:
            return true;
        }
        return false;
    }

}