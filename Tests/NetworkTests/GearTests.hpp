#include <Network/BeastAttribute.hpp>

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
}
