#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp> 

#include <Network/Listener.hpp>
#include <Network/WebSocket.hpp>

#include <future>

#if 1
TEST_CASE("Listener Test", "[Listener]")
{
    const std::string localhost{ "127.0.0.1" };
    const uint16_t port = 8080;
    const std::string localroot{ "." };
    auto state = std::make_shared<Shared_state>(localroot);

    asio::io_context io;

    auto listener = std::make_shared<Listener>(
        io, tcp::endpoint{ asio::ip::make_address(localhost), port }, state);

    std::future<void> listenerResult = std::async(std::launch::async, [&listener, &io]()
        {
            CHECK_NOTHROW(listener->run());
            io.run();
        });

    boost::beast::websocket::stream<tcp::socket> ws(io);

    tcp::resolver resolver(io);
    auto const results = resolver.resolve(localhost, std::to_string(port));

    asio::connect(ws.next_layer(), results.begin(), results.end());

    SECTION("Listener run behavior")
    {
        std::string message{ "Hello, WebSocket!" };

        ws.handshake(localhost, state.get()->doc_root());
        ws.write(asio::buffer(message));

        boost::beast::flat_buffer buffer;
        ws.read(buffer);

        REQUIRE(boost::beast::buffers_to_string(buffer.data()).c_str() == message);

        ws.close(boost::beast::websocket::close_code::normal);
    }

    io.stop();
    listenerResult.get();
}
#endif

#if 1
TEST_CASE("Shared_state Test", "[Shared_state]")
{
    const std::string root{ "/" };
    auto shared_state = std::make_shared<Shared_state>(root);

    asio::io_context io;

    SECTION("Root state")
    {
        REQUIRE(shared_state->doc_root().c_str() == root);
    }

    SECTION("Join && Leave states")
    {
        const std::string dummyRoot{ "." };
        auto dummyState = 
            std::make_shared<Shared_state>(dummyRoot);

        WebSocket dummySocket(tcp::socket(io), dummyState);

        CHECK_NOTHROW(shared_state->join(dummySocket));
        REQUIRE(shared_state->doc_root().c_str() != dummyRoot);
        CHECK_NOTHROW(shared_state->leave(dummySocket));
    }

    SECTION("Send function")
    {
        CHECK_NOTHROW(shared_state->send("DummyMessage"));
    }
}
#endif
