

#ifndef NETWORKMANAGER_HPP_
#define NETWORKMANAGER_HPP_

#include "asio.hpp"
#include <map>

class NetworkManager {
  public:

    NetworkManager(int port);
    ~NetworkManager();
    void run();

    private:
      char data[1024];
      asio::io_context context;
      asio::ip::udp::socket socket;
      asio::ip::udp::endpoint clientEndpoint;
      std::map<asio::ip::udp::endpoint, int> clientMap;
      int clientCounter = 1;

      void server_receive();
      void handle_receive(const asio::error_code& ec, std::size_t bytes_received);
      void connect();
      void disconnect();
};

#endif //NETWORKMANAGER_HPP
