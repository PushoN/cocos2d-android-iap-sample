#ifndef __common_JsonParser__
#define __common_JsonParser__

#include "cocos2d.h"
#include "dropbox/json11/json11.hpp"

namespace common {
    class JsonParser {
    public:
        /**
         * Create a JSON object from a string
         * 
         * @param jsonString json format string
         * @return Returns an instance of json11::Json.
         *         Returns nullptr in case of error
         */
        static json11::Json parseFromString(const std::string& jsonString);

        /**
         * Create a JSON object from a file
         *
         * @return Returns an instance of json11::Json.
         *         Returns nullptr in case of error
         */
        static json11::Json parseFromFile(const std::string& path);
    };
}

#endif