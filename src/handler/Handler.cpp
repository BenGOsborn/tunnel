#include "handler/Handler.hpp"

namespace handler
{
    std::expected<common::HTTPResponse, std::string> Handle(const common::HTTPRequest &req)
    {
        common::HTTPResponse resp{
            common::HTTPVersion::V1_1,
            200,
            "OK",
            common::HTTPHeaders{},
            "Path: " + req.path};
        return resp;
    }
}