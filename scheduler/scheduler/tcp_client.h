#pragma once

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <iostream>

typedef boost::asio::ip::tcp tcp;

class TCPClient
{
    typedef boost::function< void(const boost::system::error_code &error) > ConnectedCallback_t;
    typedef boost::function< void() > DisconnectedCallback_t;
    ConnectedCallback_t m_connected_callback;
    DisconnectedCallback_t m_disconnected_callback;

public:
    TCPClient();
    ~TCPClient();

    void set_connected_callback(ConnectedCallback_t cb)
    {
        m_connected_callback = cb;
    }
    void set_disconnected_callback(DisconnectedCallback_t cb)
    {
        m_disconnected_callback = cb;
    }

    void connect_to(const std::string &host, int16_t port);
    void disconnect();

    boost::asio::io_service &get_io_service()
    {
        return m_io_service;
    }

private:
    void handle_connect(const boost::system::error_code &error);
    void doDisconnect();

    boost::asio::io_service m_io_service;
    boost::asio::io_service::work m_work;
    boost::shared_ptr< tcp::socket > m_socket;
    boost::thread m_worker;
};
