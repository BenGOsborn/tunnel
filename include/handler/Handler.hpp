#pragma once
#include "core/HTTPCommon.hpp"

namespace handler
{
    std::expected<common::HTTPResponse, std::string> Handle(const common::HTTPRequest &req);
}