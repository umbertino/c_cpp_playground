// standard inclides
#include <iostream>
#include <thread>
#include <chrono>

// boost includes
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

std::vector<char> vBuffer(20 * 1024);

void GrabSomeData(asio::ip::tcp::socket& socket)
{
    socket.async_read_some(
        asio::buffer(vBuffer.data(), vBuffer.size()),
        [&](std::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                std::cout << "\n\nRead " << length << " bytes\n\n";

                for (int i = 0; i < length; i++)
                {
                    std::cout << vBuffer[i];
                }

                GrabSomeData(socket);
            }
        }
    );
}

int main()
{
    asio::error_code ec;

    // create a 'context' - essentially the platform specific interface
    asio::io_context context;

    // Give some fake tasks to asio so the context doesn't finish
    asio::io_context::work idleWork(context);

    // Start the context
    std::thread thrContext = std::thread([&]() {context.run(); });

    // the address of somwhere we want to connect to
    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec), 80); // example.com 82.165.229.87 2606:2800:21f:cb07:6820:80da:af6b:8b2c

    // create a socket, the context will deliver the implementation
    asio::ip::tcp::socket socket(context);

    // Tell socket to try and connect
    socket.connect(endpoint, ec);

    if (!ec)
    {
        std::cout << "Socket connected to endpoint" << std::endl;
    }
    else
    {
        std::cout << "Failed to connect socket to endpoint" << std::endl;
    }

    if (socket.is_open())
    {
        // prome asio context to work before sending data
        GrabSomeData(socket);

        std::string sRequest =
            "GET /index.html HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "Connection: close\r\n\r\n";

        socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

        // program does something else, while asio handles data transfer in the background
        std::this_thread::sleep_for(std::chrono::seconds(5));

        context.stop();
        if (thrContext.joinable())
        {
            thrContext.join();
        }
    }

    return 0;
}

