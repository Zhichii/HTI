#include "hti.hpp"

namespace hti::i18n {

    Language::Language() = default;

    Language::Language(const std::string& content) {
        Json::Value root = chh::parseJson(content);
        if (!root.isObject()) {
            throw std::runtime_error("`root` is not an object.");
        }
        for (const auto& i : root.getMemberNames()) {
            this->_content[i] = root[i].asString();
        }
    }

    std::string Language::localize(const std::string& key) const {
        if (this->_content.count(key)) {
            return this->_content.at(key);
        }
        else {
            return key;
        }
    }

    LanguageManager::LanguageManager() {
        this->_languages["default"] = Language("{}");
        this->current("default");
    }

    void LanguageManager::load(const std::string& name, const std::string& content) {
        this->_languages[name] = Language(content);
    }

    void LanguageManager::current(std::string name) {
        if (this->_languages.count(name)) {
            this->_current = name;
        }
    }

    std::string LanguageManager::localize(std::string key) const {
        return this->_languages.at(this->_current).localize(key);
    }

    std::string LanguageManager::current() const {
        return this->_current;
    }

    LocalizingString::LocalizingString(std::string key) : _key(key) {}

    std::string LocalizingString::localize(const LanguageManager& languages) const {
        return languages.localize(this->_key);
    }

    Text::Text() = default;

    Text::Text(const Text& text) = default;

    Text::Text(const LocalizingString& string) : _parts({ string }) {}

    Text::Text(const std::string& string) : _parts({ string }) {}

    Text::Text(const char* string) : _parts({ string }) {}

    Text Text::operator+(const Text& that) const {
        Text result = *this;
        result._parts.insert(result._parts.end(), that._parts.begin(), that._parts.end());
        return result;
    }

    Text& Text::operator+=(const Text& that) {
        this->_parts.insert(this->_parts.end(), that._parts.begin(), that._parts.end());
        return *this;
    }

    std::string Text::localize(const LanguageManager& languages) const {
        if (this->_cache_language != languages.current()) {
            this->_cache_language = languages.current();
            std::ostringstream oss;
            for (auto& part : this->_parts) {
                std::visit([&oss, &languages](auto& arg) {
                    if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, LocalizingString>) {
                        oss << arg.localize(languages);
                    }
                    else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::string>) {
                        oss << arg;
                    }
                    }, part);
            }
            this->_cache = oss.str();
        }
        return this->_cache;
    }

    size_t Text::size() const {
        return this->_parts.size();
    }

}