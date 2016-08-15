#pragma once

#include <string>
#include <vector>
#include <thread>

// Use asio standalone instead of boost
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::placeholders::_3;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

class Websocket
{
public:
	typedef std::function<void(client*, websocketpp::connection_hdl, message_ptr)> Callback;

	Websocket(const std::string& uri, const Callback& callback);
	Websocket(const std::string& uri);
	~Websocket();

	void Send(const std::string& message);

	void Start();
	void Stop();

private:
	void ProcessingThread();

	void OnMessage(client* c, websocketpp::connection_hdl hdl, message_ptr msg);

private:
	std::thread	m_thread;
	bool	m_isRunning;

	const std::string m_uri;
	Callback m_callback;

	client::connection_ptr m_connection;
	client m_client;
};
