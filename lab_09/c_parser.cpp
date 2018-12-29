#include <gumbo.h>
#include <boost/regex.hpp>
#include "c_parser.h"

static void search_for_links(GumboNode* node, std::vector<std::string> &list_links)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }
    GumboAttribute* href;
    if (node->v.element.tag == GUMBO_TAG_A && (href = gumbo_get_attribute(&node->v.element.attributes, "href")))
    {
        list_links.push_back(std::string(href->value));
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i)
    {
        search_for_links(static_cast<GumboNode*>(children->data[i]), list_links);
    }
}

c_parser::c_parser(std::string &content)
    : m_content(content)
{

}

void c_parser::get_links(std::vector<std::string> &list_links)
{
    GumboOutput* output = gumbo_parse(m_content.c_str());
    search_for_links(output->root, list_links);
    gumbo_destroy_output(&kGumboDefaultOptions, output);
}

void c_parser::get_images(std::vector<std::string> &list_images)
{
    boost::regex r("<img[^>]*src=['|\"](.*?)['|\"].*?>");
    //boost::smatch results;

    boost::sregex_iterator it(m_content.begin(), m_content.end(), r);
    boost::sregex_iterator end;

    while (it != end)
    {
        list_images.push_back((*it)[1].str());
        ++it;
    }
}
