#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include <Network/Listener.hpp>
#include <Network/WebSocket.hpp>

#include <future>

TEST_CASE("Listener Test", "[Listener]")
{
    std::string host{ "127.0.0.1" };
    uint16_t port = 8080;
    std::string doc_root{ "." };
    auto state = std::make_shared<Shared_state>(doc_root);

    netAsio::io_context io;

    auto listener = std::make_shared<Listener>(
        io, tcp::endpoint{ netAsio::ip::make_address(host), port }, state);

    std::future<void> listenerResult = std::async(std::launch::async, [&]()
        {
            listener->run();
            io.run();
        });

    boost::beast::websocket::stream<tcp::socket> ws(io);

    tcp::resolver resolver(io);
    auto const results = resolver.resolve(host, std::to_string(port));

    netAsio::connect(ws.next_layer(), results.begin(), results.end());

    SECTION("Listner run behavior")
    {
        std::string message{ "Hello, WebSocket!" };

        ws.handshake(host, state.get()->doc_root());
        ws.write(netAsio::buffer(message));

        boost::beast::flat_buffer buffer;
        ws.read(buffer);

        REQUIRE(boost::beast::buffers_to_string(buffer.data()).c_str() == message);

        ws.close(boost::beast::websocket::close_code::normal);
    }

    io.stop();
    listenerResult.get();
}

#if 0
TEST_CASE("Shared_state Test", "[Shared_state]")
{
    SECTION("Join function")
    {

    }

    SECTION("Leave function")
    {

    }

    SECTION("Send function")
    {

    }
}
#endif

#if 0
TEST_CASE("WebSocket Test", "[WebSocket]")
{
    SECTION("Send Message")
    {
        //const std::shared_ptr<const std::string> message = 
//    std::make_shared<const std::string>("Hello, WebSocket!");

                //REQUIRE(websocket.queue().size() == 1);
        //REQUIRE(*(websocket.queue()[0]) == "Hello, WebSocket!");
    }

    SECTION("Send Fail Message")
    {

    }

    SECTION("Check queue size")
    {

    }
}
#endif 
