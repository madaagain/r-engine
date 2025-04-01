

#ifndef CONNECTION_HPP_
#define CONNECTION_HPP_

#include "Queue.hpp"
#include "Message.hpp"
#include <asio.hpp>

namespace net
{
    template <typename T>
    class Connection : public std::enable_shared_from_this<Connection<T>> {

    public:
        Connection(asio::ip::udp::endpoint endpoint)
            : client_endpoint(endpoint), id(0)
        {
        }

        virtual ~Connection()
        {
        }

        asio::ip::udp::endpoint getEndpoint() const
        {
            return client_endpoint;
        }

        uint32_t getID() const
        {
            return id;
        }

        void setID(uint32_t uid)
        {
            id = uid;
        }

    private:
        asio::ip::udp::endpoint client_endpoint;
        uint32_t id;
    };
}

#endif /* !CONNECTION_HPP_ */
