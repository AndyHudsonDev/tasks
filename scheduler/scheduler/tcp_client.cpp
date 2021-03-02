#include "tcp_client.h"

TCPClient::TCPClient()
    : m_connected_callback(0)
    , m_disconnected_callback(0)
    , m_work(m_io_service)
    , m_socket(new tcp::socket(m_io_service))
    , m_worker([&] { return m_io_service.run(); })
{
}

TCPClient::~TCPClient()
{
    doDisconnect();
    m_io_service.stop();
    if (m_worker.joinable())
        m_worker.join();
}

void TCPClient::connect_to(const std::string &host, int16_t port)
{
    tcp::resolver resolver(m_io_service);
    tcp::resolver::query query(host, boost::lexical_cast< std::string >(port));
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    m_socket->async_connect(*endpoint_iterator,
                            boost::bind(&TCPClient::handle_connect, this, boost::asio::placeholders::error));
}

void TCPClient::handle_connect(const boost::system::error_code &error)
{
    if (0 != m_connected_callback) {
        m_connected_callback(error);
    }
}

void TCPClient::disconnect()
{
    m_io_service.post(boost::bind(&TCPClient::doDisconnect, this));
}

void TCPClient::doDisconnect()
{
    m_io_service.reset();
    if (m_socket->is_open()) {
        m_socket->close();
    }
    m_socket.reset(new tcp::socket(m_io_service));
    m_disconnected_callback();
}
