#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>

#include "c_requester.h"
#include "c_uri.h"

using boost::asio::ip::tcp;
namespace http = boost::beast::http;

c_requester::c_requester()
{

}

std::string c_requester::do_request(const std::string &url)
{
    boost::asio::io_service ios;
    tcp::resolver resolver_{ios};
    c_uri u{url};

    if (!u.isValid())
        return std::string();

    auto const lookup = resolver_.resolve( tcp::resolver::query(u.host(), u.scheme()) );

    // Set up an HTTP GET request message
    tcp::socket socket{ios};
    boost::asio::connect(socket, lookup);

    http::request<http::string_body> req{http::verb::get, u.path(), 11};
    req.keep_alive(true);

    req.set(http::field::host, u.host());
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    std::cout << "Target: " << url << "\n";
    std::cout << req << "\n";

    try
    {

        http::write(socket, req);
        boost::beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(socket, buffer, res);

        switch(res.base().result_int())
        {
            case 301:
                std::cout << "Redirecting.....\n";
                do_request(res.base()["Location"].to_string());
                break;
            case 200:
                std::cout << res << "\n";
                return (boost::beast::buffers_to_string(res.body().data()));
                break;
            default:
                std::cout << "Unexpected HTTP status " << res.result_int() << "\n";
                break;
        }

        return std::string();
    }
    catch (std::exception &ex)
    {
        std::cerr << "Error! Trans exception: " << ex.what() << std::endl;
    }
}
