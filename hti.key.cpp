#include "hti.hpp"

namespace hti {

    Key::Key(unsigned short key) {
        this->_key = key;
    }

    bool Key::isPrev() {
        switch (this->_key) {
        case u'w':
        case u'W':
        case u'a':
        case u'A':
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
        return false;
    }

    bool Key::isPress() {
        switch (this->_key) {
        case u' ':
        case u'\n':
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
        return false;
    }

    bool Key::isDown() {
        switch (this->_key) {
        case u's':
        case u'S':
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
        return false;
    }

    bool Key::isRight() {
        switch (this->_key) {
        case u'd':
        case u'D':
            return true;
        }
        return false;
    }

}