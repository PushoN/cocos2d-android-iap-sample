#include "JsonParser.h"

USING_NS_CC;
using namespace common;

json11::Json JsonParser::parseFromString(const std::string& jsonString) {
    std::string err;
    json11::Json json = json11::Json::parse(jsonString, err);
    if (!err.empty()) {
        CCLOG("[JsonParser] parse failed: error=%s", err.c_str());
        return nullptr;
    }
    return json;
}

json11::Json JsonParser::parseFromFile(const std::string& path) {
    auto fileUtils = FileUtils::getInstance();
    if (!fileUtils->isFileExist(path)) {
        CCLOG("[JsonParser] file not found: path=%s", path.c_str());
        return nullptr;
    }

    std::string jsonString = FileUtils::getInstance()->getStringFromFile(path);

    std::string err;
    json11::Json json = json11::Json::parse(jsonString, err);
    if (!err.empty()) {
        CCLOG("[JsonParser] parse failed: error=%s, path=%s", err.c_str(), path.c_str());
        return nullptr;
    }

    return json;
}