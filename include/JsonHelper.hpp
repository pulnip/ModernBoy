#pragma once

#define RAPIDJSON_PARSE_ERROR_NORETURN(parseErrorCode, offset) \
    throw ParseException(parseErrorCode, #parseErrorCode, offset)

#include <optional>
#include <stdexcept>
#include <string>
#include <rapidjson/error/error.h>

struct ParseException: std::runtime_error, rapidjson::ParseResult{
    ParseException(rapidjson::ParseErrorCode code, const char* msg, size_t offset)
    :std::runtime_error(msg), ParseResult(code, offset){}
};

#include <rapidjson/document.h>
#include "Math.hpp"

namespace ModernBoy{
    class JsonHelper{
      private:
        const rapidjson::Value& object;

      public:
        JsonHelper(const rapidjson::Value& object);
        std::optional<int> getInt(const std::string& propertyName);
        std::optional<double> getDouble(const std::string& propertyName);
        std::optional<std::string> getString(const std::string& propertyName);
        std::optional<JsonHelper> subset(const std::string& propertyName);
        std::optional<point2<>> getPoint2(const std::string& propertyName);
        std::optional<ipoint2> getPoint2i(const std::string& propertyName);
    };

    class Json{
      private:
        rapidjson::Document doc;

      public:
        Json(const std::string& fileName);
        explicit operator JsonHelper();
    };
}
