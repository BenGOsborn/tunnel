#pragma once
#include "core/HTTPCommon.hpp"

namespace handler
{
    common::HTTPResponse Handle(const common::HTTPRequest &req);
}