#include <Network/BeastAttribute.hpp>
#include <boost/beast/http.hpp>

#include <memory>

namespace GearTests
{
	inline netAsio::io_context testIO;

	namespace testproperties
	{
		const std::string testHost{ "127.0.0.1" };
		const uint16_t testPort = 8080;
		const std::string testRoot{ "." };
	}

	namespace badproperties
	{
		const std::string badHost{ "0" };
		const uint16_t badPort = 66666;
		const std::string badRoot{ "666" };
	}
	
	using namespace boost::beast;

	class HTTPClient : public std::enable_shared_from_this<HTTPClient>
	{
		tcp::resolver _resolver;
		boost::beast::tcp_stream _stream;
		boost::beast::flat_buffer _buffer;
		http::request<http::empty_body> _req;
		http::response<http::string_body> _res;
	public:
		explicit HTTPClient(boost::asio::io_context& ioc)
			: _resolver(boost::asio::make_strand(ioc))
			, _stream(boost::asio::make_strand(ioc)) {}

		void run()
		{
			uint16_t port{8080};
			std::string address{ "127.0.0.1" };

			_req.version(11);
			_req.method(http::verb::get);
			_req.target("/");
			_req.set(http::field::host, testproperties::testHost);
			_req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

			_resolver.async_resolve(testproperties::testHost, 
				boost::beast::bind_front_handler(&HTTPClient::on_resolve, shared_from_this()));

		}
	private:
		void fail(boost::beast::error_code ec, std::string what)
		{
			std::cerr << what.c_str() << " :" << ec.message() << "\n";
		}

		void on_resolve(boost::beast::error_code ec, tcp::resolver::results_type results)
		{
			if (ec)
				return fail(ec, "resolve");

			_stream.expires_after(std::chrono::seconds(30));
			_stream.async_connect(results,
				boost::beast::bind_front_handler(&HTTPClient::on_connect, shared_from_this()));
		}

		void on_connect(boost::beast::error_code ec, tcp::resolver::results_type::endpoint_type)
		{
			if (ec)
				return fail(ec, "connect");

			_stream.expires_after(std::chrono::seconds(30));
			http::async_write(_stream, _req,
				boost::beast::bind_front_handler(&HTTPClient::on_write, shared_from_this()));
		}

		void on_write(boost::beast::error_code ec, std::size_t bytes_transferred)
		{
			boost::ignore_unused(bytes_transferred);

			if (ec)
				return fail(ec, "write");

			http::async_read(_stream, _buffer, _res,
				boost::beast::bind_front_handler(&HTTPClient::on_read, shared_from_this()));
		}

		void on_read(boost::beast::error_code ec, std::size_t bytes_transferred)
		{
			boost::ignore_unused(bytes_transferred);

			if (ec)
				return fail(ec, "read");

			std::cout << _res << std::endl;

			_stream.socket().shutdown(tcp::socket::shutdown_both, ec);

			if (ec && ec != boost::beast::errc::not_connected)
				return fail(ec, "shutdown");
		}
	};
}
