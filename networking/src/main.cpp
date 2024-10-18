// standard inclides
#include <iostream>

// boost includes
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

int main()
{
    asio::error_code ec;
    asio::io_context context;
    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1", ec), 80);
    asio::ip::tcp::socket socket(context);

    socket.connect(endpoint, ec);

    if (!ec)
    {
        std::cout << "Socket connected to endpoint" << std::endl;
    }
    else
    {
        std::cout << "Failed to connect socket to endpoint" << std::endl;
    }


    return 0;
}