#pragma once

#include <string>

class c_requester
{
public:
    c_requester();
    std::string do_request(std::string const& url);
};

