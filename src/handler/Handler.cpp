#include "handler/Handler.hpp"
#include <format>

namespace handler
{
    common::HTTPResponse Handle(const common::HTTPRequest &req)
    {
        common::HTTPResponse resp{req.version, 200, "OK", common::HTTPHeaders{}, std::format("Path: {}, body: {}", req.path, req.body)};
        return resp;
    }
}