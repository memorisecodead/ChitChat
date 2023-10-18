#include <Network/BeastAttribute.hpp>

namespace GearTests
{
	netAsio::io_context testIO;

	namespace testproperties
	{
		std::string testHost{ "127.0.0.1" };
		uint16_t testPort = 8080;
		std::string testRoot{ "." };
	}

	namespace badproperties
	{
		std::string badHost{ "0" };
		uint16_t badPort = 66666;
		std::string badRoot{ "666" };
	}
}
