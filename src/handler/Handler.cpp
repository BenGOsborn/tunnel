#include "handler/Handler.hpp"

#include <iostream>

namespace handler
{
    std::expected<common::HTTPResponse, std::string> Handle(const common::HTTPRequest &req)
    {
        common::HTTPResponse resp{
            req.version,
            200,
            "OK",
            common::HTTPHeaders{},
            "Path: " + req.path};
        std::cout << req.body << std::endl;
        return resp;
    }
}