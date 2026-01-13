#pragma once
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <expected>

namespace json
{
    struct Object;

    using Null = std::monostate;
    using Bool = bool;
    using String = std::string;
    using Number = double;
    using Array = std::vector<Object>;
    using Map = std::unordered_map<std::string, Object>;

    struct Object
    {
        std::variant<Null, Bool, String, Number, Array, Map> data_;
    };

    class JSON
    {
    public:
        static std::expected<Object, std::string> FromString(const std::string &str);
        static std::expected<std::string, std::string> ToString(const Object &obj);
    };
}