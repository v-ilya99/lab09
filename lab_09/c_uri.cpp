

#include "c_uri.h"

c_uri::c_uri(std::string uri)
    : m_uri(uri)
{
    UriParserStateA state_;
    state_.uri = &m_uri_parse;
    m_is_valid = uriParseUriA(&state_, m_uri.c_str()) == URI_SUCCESS;
}

c_uri::~c_uri()
{
    uriFreeUriMembersA(&m_uri_parse);
}

bool c_uri::isValid() const
{
    return m_is_valid;
}

std::string c_uri::scheme() const
{
    return fromRange(m_uri_parse.scheme);
}

std::string c_uri::host() const
{
    return fromRange(m_uri_parse.hostText);
}

std::string c_uri::port() const
{
    return fromRange(m_uri_parse.portText);
}

std::string c_uri::path() const
{
    return fromList(m_uri_parse.pathHead, "/");
}

std::string c_uri::query() const
{
    return fromRange(m_uri_parse.query);
}

std::string c_uri::fragment() const
{
    return fromRange(m_uri_parse.fragment);
}

std::string c_uri::fromRange(const UriTextRangeA &rng) const
{
    return std::string(rng.first, rng.afterLast);
}

std::string c_uri::fromList(UriPathSegmentA *xs, const std::string &delim) const
{
    UriPathSegmentStructA * head(xs);
    std::string accum;

    while (head)
    {
        accum += delim + fromRange(head->text);
        head = head->next;
    }

    return accum;
}
