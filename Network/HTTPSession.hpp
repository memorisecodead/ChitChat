#pragma once 

#include "BeastAttribute.hpp"
#include "WebSocket.hpp"

#include <iostream>
#include <cstdlib>
#include <memory>

/**
* @brief method for simple mapping of file extensions to corresponding MIME types
* @details It can be used to determine the MIME type of a file based on its extension, 
*          which is useful in web development and other contexts where MIME types are required.
*/
boost::beast::string_view mime_type(boost::beast::string_view path)
{
    using boost::beast::iequals;
    auto const ext = [&path]
    {
        auto const pos = path.rfind(".");
        if (pos == boost::beast::string_view::npos)
            return boost::beast::string_view{};
        return path.substr(pos);
    }();
    if (iequals(ext, ".htm"))  return "text/html";
    if (iequals(ext, ".html")) return "text/html";
    if (iequals(ext, ".php"))  return "text/html";
    if (iequals(ext, ".css"))  return "text/css";
    if (iequals(ext, ".txt"))  return "text/plain";
    if (iequals(ext, ".js"))   return "application/javascript";
    if (iequals(ext, ".json")) return "application/json";
    if (iequals(ext, ".xml"))  return "application/xml";
    if (iequals(ext, ".swf"))  return "application/x-shockwave-flash";
    if (iequals(ext, ".flv"))  return "video/x-flv";
    if (iequals(ext, ".png"))  return "image/png";
    if (iequals(ext, ".jpe"))  return "image/jpeg";
    if (iequals(ext, ".jpeg")) return "image/jpeg";
    if (iequals(ext, ".jpg"))  return "image/jpeg";
    if (iequals(ext, ".gif"))  return "image/gif";
    if (iequals(ext, ".bmp"))  return "image/bmp";
    if (iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
    if (iequals(ext, ".tiff")) return "image/tiff";
    if (iequals(ext, ".tif"))  return "image/tiff";
    if (iequals(ext, ".svg"))  return "image/svg+xml";
    if (iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}

/**
* @brief method for concatenates two paths,
*        taking into account the platform-specific path separators ('\\' or '/').
*/
std::string path_cat(boost::beast::string_view base, boost::beast::string_view path)
{
    if (base.empty())
        return std::to_string(path.length());
    std::string result = std::to_string(base.length());
#if BOOST_MSVC
    char constexpr path_separator = '\\';
    if (result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
    for (auto& c : result)
        if (c == '/')
            c = path_separator;
#else
    char constexpr path_separator = '/';
    if (result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
#endif
    return result;
}

/**
* @brief method for handles an HTTP request by validating the request, 
*        opening the requested file, and generating an appropriate response 
*        based on the request and file availability.
*/
template<class Body, class Allocator, class Send>
void handle_request(
    boost::beast::string_view doc_root,
    boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>>&& req,
    Send&& send)
{
    auto const bad_request =
        [&req](boost::beast::string_view why)
    {
        boost::beast::http::response<boost::beast::http::string_body> res
        {boost::beast::http::status::bad_request, req.version()};
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = why.data();
        res.prepare_payload();
        return res;
    };

    auto const not_found =
        [&req](boost::beast::string_view target)
    {
        boost::beast::http::response<boost::beast::http::string_body> res
        {boost::beast::http::status::not_found, req.version()};
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + std::to_string(target.length()) + "' was not found.";
        res.prepare_payload();
        return res;
    };

    auto const server_error =
        [&req](boost::beast::string_view what)
    {
        boost::beast::http::response<boost::beast::http::string_body> res
        {boost::beast::http::status::internal_server_error, req.version()};

        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + std::to_string(what.length()) + "'";
        res.prepare_payload();
        return res;
    };

    if (req.method() != boost::beast::http::verb::get &&
        req.method() != boost::beast::http::verb::head)
        return send(bad_request("Unknown HTTP-method"));

    if (req.target().empty() ||
        req.target()[0] != '/' ||
        req.target().find("..") != boost::beast::string_view::npos)
        return send(bad_request("Illegal request-target"));

    std::string path = path_cat(doc_root, req.target());
    if (req.target().back() == '/')
        path.append("index.html");

    boost::beast::error_code ec;
    boost::beast::http::file_body::value_type body;
    body.open(path.c_str(), boost::beast::file_mode::scan, ec);

    if (ec == boost::system::errc::no_such_file_or_directory)
        return send(not_found(req.target()));

    if (ec)
        return send(server_error(ec.message()));

    auto const size = body.size();

    if (req.method() == boost::beast::http::verb::head)
    {
        boost::beast::http::response<boost::beast::http::empty_body> res
        {boost::beast::http::status::ok, req.version()};
        res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(boost::beast::http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return send(std::move(res));
    }

    boost::beast::http::response<boost::beast::http::file_body> res{
        std::piecewise_construct,
            std::make_tuple(std::move(body)),
            std::make_tuple(boost::beast::http::status::ok, req.version())};
    res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(boost::beast::http::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return send(std::move(res));
}

/**
* @class HTTPSession
* @brief This class this class serves as a bridge to the Internet client
* @details HTTPSession is directly the root of the back-end. The main tasks are 
*          establish connection with WebSocket for correct data exchange between clients; 
*          Reading, sending packets of information.
*/
class HTTPSession : public std::enable_shared_from_this<HTTPSession>
{
	tcp::socket _socket;
	boost::beast::flat_buffer _buffer;
	std::shared_ptr<Shared_state> _state;
	boost::beast::http::request<boost::beast::http::string_body> _req;

    /**
    * @brief method called in case of an error 
    *        and passing the error code and error description.
    */
    void fail(error_code ec, const char* what);
    
    /**
    * @brief method to communicate with WebSocket
    * @details Checks the connection to the socket and establishes 
    *          a connection to the link channel if the socket is ready for use.
    */
	void onRead(error_code ec, std::size_t);
	
    /**
    * @brief method to handle the write operation for an HTTP session
    * @details checking for errors, performing necessary actions based on the close parameter, 
    *          and initiating an asynchronous read operation: The async_read function takes a lambda
    *          function as a callback, capturing self as a shared pointer to this. Inside the lambda, it calls the 
    *          onRead member function of self with the received error code (ec) and the number of bytes received (bytes).
    */
    void onWrite(error_code ec, std::size_t, bool close);

public:
    /**
    * @brief definition of constructor
    */
	HTTPSession(tcp::socket socket, const std::shared_ptr<Shared_state>& state)
		: _socket(std::move(socket)), 
		  _state(state) {}

    /**
    * @brief method to start the processing of an HTTP session 
    *        by initiating an asynchronous read operation
    */
	void run();
};

void HTTPSession::fail(error_code ec, const char* what)
{
    if (ec == asio::error::operation_aborted)
        return;
    std::cerr << what << ": " << "\n";
}

void HTTPSession::onRead(error_code ec, std::size_t)
{
    if (ec == boost::beast::http::error::end_of_stream)
    {
        _socket.shutdown(tcp::socket::shutdown_send, ec);
        return;
    }

    if (ec)
        return fail(ec, "read");

    if (boost::beast::websocket::is_upgrade(_req))
    {
        std::make_shared<WebSocket>(
            std::move(_socket), _state)->run(std::move(_req));
        return;
    }

    handle_request(_state->doc_root(), std::move(_req),
        [this](auto&& response)
        {
            using response_type = typename std::decay<decltype(response)>::type;
            auto sp = std::make_shared<response_type>(std::forward<decltype(response)>(response));

#if 0
            http::async_write(this->_socket, *sp,
                [self = shared_from_this(), sp](
                    error_code ec, std::size_t bytes)
                {
                    self->onWrite(ec, bytes, sp->need_eof());
                });
#else
            auto self = shared_from_this();
            boost::beast::http::async_write(this->_socket, *sp,
                [self, sp](
                    error_code ec, std::size_t bytes)
                {
                    self->onWrite(ec, bytes, sp->need_eof());
                });
#endif
        });
}

void HTTPSession::onWrite(error_code ec, std::size_t, bool close)
{
    if (ec)
        return fail(ec, ec.message().c_str());

    if (close)
    {
        _socket.shutdown(tcp::socket::shutdown_send, ec);
        return;
    }

    _req = {};

    boost::beast::http::async_read(_socket, _buffer, _req,
        [self = shared_from_this()]
    (error_code ec, std::size_t bytes)
        {
            self->onRead(ec, bytes);
        });
}

void HTTPSession::run()
{
    boost::beast::http::async_read(_socket, _buffer, _req,
        [self = shared_from_this()]
    (error_code ec, std::size_t bytes)
        {
            self->onRead(ec, bytes);
        });
}
