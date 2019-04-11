#ifndef WEBSOCKET_H_
#define WEBSOCKET_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/Websockets/Websocket.h
// The Websocket wrapper for Websocket++

#include <string>
#include <vector>
#include <thread>
#include <iostream>

// Use asio standalone instead of boost
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

// remove worning for asio dev release
#pragma warning(push, 0)

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#pragma warning(pop)

#ifdef USE_LOCALHOST
typedef websocketpp::client<websocketpp::config::asio_client> WebsocketClient;
#else
typedef websocketpp::client<websocketpp::config::asio_tls_client> WebsocketClient;
#endif

typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::placeholders::_3;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_tls_client::message_type::ptr message_ptr;
//typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

class Websocket {
public:

	// Websocket Side callbacks
	typedef std::function<void(WebsocketClient*, websocketpp::connection_hdl, message_ptr)> OnWebsocketMessageCallback;
	typedef std::function<void(websocketpp::connection_hdl)> OnWebsocketInitCallback;
	typedef std::function<void(websocketpp::connection_hdl)> OnWebsocketConnectionOpenCallback;
	typedef std::function<void(websocketpp::connection_hdl)> OnWebsocketConnectionCloseCallback;
	typedef std::function<void(websocketpp::connection_hdl)> OnWebsocketConnectionFailCallback;

	// Client side callbacks
	typedef std::function<void(const std::string&)> HandleWebsocketMessageCallback;
	typedef std::function<void(void)> HandleWebsocketConnectionOpenCallback;
	typedef std::function<void(void)> HandleWebsocketConnectionCloseCallback;
	typedef std::function<void(void)> HandleWebsocketConnectionFailCallback;
	typedef std::function<void(void)> HandleWebsocketInitlCallback;

	Websocket(const std::string& strURI, const HandleWebsocketMessageCallback& fnHandleWebsocketMessageCallback,
			  const HandleWebsocketConnectionOpenCallback&	fnHandleWebsocketConnectionOpenCallback,
			  const HandleWebsocketConnectionCloseCallback& fnHandleWebsocketConnectionCloseCallback,
			  const HandleWebsocketConnectionFailCallback&	fnHandleWebsocketConnectionFailCallback);

	Websocket(const std::string& strURI);
	~Websocket() = default;

	RESULT Initialize();
	RESULT Send(const std::string& strMessage);
	RESULT Start();
	RESULT Stop();

	RESULT SetToken(const std::string& strToken);

	bool IsRunning() {
		return m_fRunning;
	}

	bool IsConnected() {
		return m_fConnectionOpen;
	}

private:
	websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_pWebsocketThread;
	void OnMessage(WebsocketClient* pWebsicketClient, websocketpp::connection_hdl hWebsocketConnection, message_ptr pWebsocketMessage);

	void OnOpen(websocketpp::connection_hdl hWebsocketConnection);
	void OnClose(websocketpp::connection_hdl hWebsocketConnection);
	void OnFail(websocketpp::connection_hdl hWebsocketConnection);
	void OnInit(websocketpp::connection_hdl hWebsocketConnection);

private:
	bool m_fRunning = false;
	bool m_fConnectionOpen = false;

	const std::string m_strURI;

	OnWebsocketMessageCallback			m_fnOnWebsocketMessageCallback = nullptr;
	
	OnWebsocketConnectionOpenCallback	m_fnOnWebsocketConnectionOpenCallback = nullptr;
	OnWebsocketConnectionFailCallback	m_fnOnWebsocketConnectionFailCallback = nullptr;
	OnWebsocketConnectionCloseCallback	m_fnOnWebsocketConnectionCloseCallback = nullptr;
	OnWebsocketInitCallback				m_fnOnWebsocketInitCallback = nullptr;

	HandleWebsocketMessageCallback			m_fnHandleWebsocketMessageCallback = nullptr;
	HandleWebsocketConnectionOpenCallback	m_fnHandleWebsocketConnectionOpenCallback = nullptr;
	HandleWebsocketConnectionFailCallback	m_fnHandleWebsocketConnectionFailCallback = nullptr;
	HandleWebsocketConnectionCloseCallback	m_fnHandleWebsocketConnectionCloseCallback = nullptr;
	HandleWebsocketConnectionCloseCallback	m_fnHandleWebsocketInitCallback = nullptr;

	WebsocketClient::connection_ptr m_pWebsocketConnection;
	WebsocketClient m_websocketClient;

	// TODO: This is repeated
	std::string	m_strToken;
};

#endif	// !WEBSOCKET_H_