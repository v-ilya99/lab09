#pragma once

#include <string>
#include <vector>

class c_parser
{
public:
    c_parser(std::string &content);
    void get_links(std::vector<std::string> &list_links);
    void get_images(std::vector<std::string> &list_images);
private:
    std::string m_content;
};

