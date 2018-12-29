#pragma once

#include <string>

#include <uriparser/Uri.h>

class c_uri
{
public:
    c_uri(std::string uri);
    ~c_uri();
    bool isValid() const;
    std::string scheme()   const;
    std::string host()     const;
    std::string port()     const;
    std::string path()     const;
    std::string query()    const;
    std::string fragment() const;
private:
    std::string fromRange(const UriTextRangeA & rng) const;
    std::string fromList(UriPathSegmentA * xs, const std::string & delim) const;
private:
    std::string m_uri;
    UriUriA     m_uri_parse;
    bool        m_is_valid;
};
