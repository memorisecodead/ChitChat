#pragma once

#include <boost/beast.hpp>
#include <boost/asio.hpp>

namespace netAsio = boost::asio;
using tcp = netAsio::ip::tcp;
using error_code = boost::system::error_code;
