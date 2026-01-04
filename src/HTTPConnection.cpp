#include "HTTPConnection.hpp"
#include "HTTPRequest.hpp"
#include <format>

#include <iostream>

namespace server::connection
{
    HTTPConnection::HTTPConnection(Connection &&connection) : connection_(std::move(connection))
    {
    }

    std::expected<bool, std::string> HTTPConnection::Handle()
    {
        while (true)
        {
            std::string req = "";
            while (!req.contains(request::HEADER_END.c_str()))
            {
                std::expected<std::optional<SocketData>, std::string> __data = connection_.Read();
                if (!__data)
                {
                    return std::unexpected(std::format("failed to read data, err={}", __data.error()));
                }
                std::optional<SocketData> _data = *__data;
                if (!_data)
                {
                    return true;
                }
                SocketData data = *_data;
                req += std::string(data.data.begin(), data.data.begin() + data.size);
            }
            std::expected<request::HTTPRequest, std::string> _httpReq = request::ParseHTTPRequest(req);
            if (!_httpReq)
            {
                std::cout << "We need to return a 400 here" << std::endl;
            }
            for (auto const &[key, val] : (*_httpReq).headers)
            {
                std::cout << key << ": " << val << std::endl;
            }
        }
    }
}