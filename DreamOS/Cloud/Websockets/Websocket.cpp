#include "Websocket.h"

#include <iostream>

/*
Websocket::Websocket(const std::string& strURI, 
					 const OnWebsocketMessageCallback& fnOnWebsocketMessageCallback,
					 const OnWebsocketConnectionOpenCallback& fnOnWebsocketConnectionOpenCallback,
					 const OnWebsocketConnectionCloseCallback& fnOnWebsocketConnectionCloseCallback,
					 const OnWebsocketConnectionFailCallback& fnOnWebsocketConnectionFailCallback
) :
	m_fRunning(false),
	m_fConnectionOpen(false),
	m_strURI(strURI),
	m_fnOnWebsocketMessageCallback(fnOnWebsocketMessageCallback),
	m_fnOnWebsocketConnectionOpenCallback(fnOnWebsocketConnectionOpenCallback),
	m_fnOnWebsocketConnectionFailCallback(fnOnWebsocketConnectionFailCallback),
	m_fnOnWebsocketConnectionCloseCallback(fnOnWebsocketConnectionCloseCallback),
	m_fnHandleWebsocketMessageCallback(nullptr),
	m_fnHandleWebsocketConnectionOpenCallback(nullptr),
	m_fnHandleWebsocketConnectionFailCallback(nullptr),
	m_fnHandleWebsocketConnectionCloseCallback(nullptr)
{
	//Start();
}
*/

Websocket::Websocket(const std::string& strURI, const HandleWebsocketMessageCallback& fnHandleWebsocketMessageCallback,
					 const HandleWebsocketConnectionOpenCallback&	fnHandleWebsocketConnectionOpenCallback,
					 const HandleWebsocketConnectionCloseCallback& fnHandleWebsocketConnectionCloseCallback,
					 const HandleWebsocketConnectionFailCallback&	fnHandleWebsocketConnectionFailCallback
) :
	m_fRunning(false),
	m_fConnectionOpen(false),
	m_strURI(strURI),
	m_fnOnWebsocketMessageCallback(std::bind(&Websocket::OnMessage, this, ::_1, ::_2, ::_3)),
	m_fnOnWebsocketConnectionOpenCallback(std::bind(&Websocket::OnOpen, this, ::_1)),
	m_fnOnWebsocketConnectionFailCallback(std::bind(&Websocket::OnFail, this, ::_1)),
	m_fnOnWebsocketConnectionCloseCallback(std::bind(&Websocket::OnClose, this, ::_1)),
	m_fnHandleWebsocketMessageCallback(fnHandleWebsocketMessageCallback),
	m_fnHandleWebsocketConnectionOpenCallback(fnHandleWebsocketConnectionOpenCallback),
	m_fnHandleWebsocketConnectionFailCallback(fnHandleWebsocketConnectionFailCallback),
	m_fnHandleWebsocketConnectionCloseCallback(fnHandleWebsocketConnectionCloseCallback)
{
	// Start();
}

Websocket::Websocket(const std::string& strURI) :
	m_fRunning(false),
	m_fConnectionOpen(false),
	m_strURI(strURI),
	m_fnOnWebsocketMessageCallback(std::bind(&Websocket::OnMessage, this, ::_1, ::_2, ::_3)),
	m_fnOnWebsocketConnectionOpenCallback(std::bind(&Websocket::OnOpen, this, ::_1)),
	m_fnOnWebsocketConnectionFailCallback(std::bind(&Websocket::OnFail, this, ::_1)),
	m_fnOnWebsocketConnectionCloseCallback(std::bind(&Websocket::OnClose, this, ::_1)),
	m_fnHandleWebsocketMessageCallback(nullptr),
	m_fnHandleWebsocketConnectionOpenCallback(nullptr),
	m_fnHandleWebsocketConnectionFailCallback(nullptr),
	m_fnHandleWebsocketConnectionCloseCallback(nullptr)
{
	//Start();
}



Websocket::~Websocket() {
	if (m_fRunning) {
		Stop();
	}
}

RESULT Websocket::SetToken(std::string& strToken) {
	m_strToken = strToken;
	return R_PASS;
}

RESULT Websocket::ProcessingThread() {
	RESULT r = R_PASS;

	m_fRunning = true;

	while (m_fRunning) {
		// TODO: Are we ok w/ Exceptions?
		try  {
			m_websocketClient.set_access_channels(websocketpp::log::alevel::all);
			m_websocketClient.clear_access_channels(websocketpp::log::alevel::frame_payload);

			//m_websocketClient.set

			m_websocketClient.init_asio();

			m_websocketClient.set_message_handler(std::bind(m_fnOnWebsocketMessageCallback, &m_websocketClient, ::_1, ::_2));
			
			// Handlers
			m_websocketClient.set_open_handler(std::bind(m_fnOnWebsocketConnectionOpenCallback, ::_1));
			m_websocketClient.set_close_handler(std::bind(m_fnOnWebsocketConnectionCloseCallback, ::_1));
			m_websocketClient.set_fail_handler(std::bind(m_fnOnWebsocketConnectionFailCallback, ::_1));

			//c.set_open_handler(bind(&on_open, &c, ::_1));
			//c.set_fail_handler(bind(&on_fail, &c, ::_1));
			//c.set_close_handler(bind(&on_close, &c, ::_1));

			websocketpp::lib::error_code websocketError;
			m_pWebsocketConnection = m_websocketClient.get_connection(m_strURI, websocketError);
			CBM((!websocketError), "Connection failed with error: %s", websocketError.message().c_str());

			if (m_strToken.size() > 0) {
				m_pWebsocketConnection->append_header("token", m_strToken);
			}

			m_websocketClient.connect(m_pWebsocketConnection);
			m_websocketClient.run();
		}
		catch (websocketpp::exception const & e) {
			e;
			DEBUG_LINEOUT("Websocket Exception: %s", e.what());
		}
	}

	DEBUG_LINEOUT("Websocket Thread Exit");

Error:
	return r;
}

/*
bool Websocket::IsConnected() {
	if (m_pWebsocketConnection == nullptr)
		return false;

	if (m_pWebsocketConnection->get_state() == websocketpp::session::state::value::open)
		return true;
	
	return false;
}
*/

RESULT Websocket::Start() {
	DEBUG_LINEOUT("Websocket::Start");	

	m_thread = std::thread(&Websocket::ProcessingThread, this);
	
	return R_PASS;
}

RESULT Websocket::Stop() {
	DEBUG_LINEOUT("Websocket::Stop");

	// Close the connection
	m_websocketClient.stop_perpetual();
	websocketpp::lib::error_code websocketError;
	m_websocketClient.close(m_pWebsocketConnection->get_handle(), websocketpp::close::status::going_away, "", websocketError);
	
	// TODO: This is causing some kind of error on exit still
	m_fRunning = false;
	m_thread.join();

	return R_PASS;
}

RESULT Websocket::Send(const std::string & strMessage) {
	RESULT r = R_PASS;
	auto websocketMessage = m_pWebsocketConnection->get_message(websocketpp::frame::opcode::TEXT, strMessage.length());

	websocketMessage->set_payload(strMessage.c_str());
	
	//msg->append_payload("efgh");

	websocketpp::lib::error_code websocketError;
	m_websocketClient.send(m_pWebsocketConnection->get_handle(), websocketMessage->get_payload(), websocketMessage->get_opcode(), websocketError);

	CBM((!websocketError), "Echo failed with message: %s", websocketError.message().c_str());

Error:
	return r;
}

void Websocket::OnMessage(WebsocketClient* pWebsocketClient, websocketpp::connection_hdl hWebsocketConnection, message_ptr pWebsocketMessage) {

	DEBUG_LINEOUT("OnMessage called with handle: 0x%p and message: %s", hWebsocketConnection.lock().get(), pWebsocketMessage->get_payload().c_str());
	//DEBUG_LINEOUT("OnMessage called with message: %s", pWebsocketMessage->get_payload().c_str());
	
	/*
	websocketpp::lib::error_code websocketError;
	pWebsocketClient->send(hWebsocketConnection, pWebsocketMessage->get_payload(), pWebsocketMessage->get_opcode(), websocketError);
	if (websocketError) {
		DEBUG_LINEOUT("Echo failed with message: %s", websocketError.message().c_str());
	}
	*/

	if (m_fnHandleWebsocketMessageCallback != nullptr) {
		m_fnHandleWebsocketMessageCallback(pWebsocketMessage->get_payload().c_str());
	}
}

#pragma warning(disable : 4503)

void Websocket::OnOpen(websocketpp::connection_hdl hWebsocketConnection) {
	//m_client.get_alog().write(websocketpp::log::alevel::app, "Connection opened, starting telemetry!");
	DEBUG_LINEOUT("Websocket Connection Opened");

	//scoped_lock guard(m_lock);
	m_fConnectionOpen = true;

	if (m_fnHandleWebsocketConnectionOpenCallback != nullptr) {
		m_fnHandleWebsocketConnectionOpenCallback();
	}
}


void Websocket::OnClose(websocketpp::connection_hdl hWebsocketConnection) {
	//m_client.get_alog().write(websocketpp::log::alevel::app, "Connection closed, stopping telemetry!");
	DEBUG_LINEOUT("Websocket Connection Closed");

	//scoped_lock guard(m_lock);
	m_fRunning = false;

	if (m_fnHandleWebsocketConnectionCloseCallback != nullptr) {
		m_fnHandleWebsocketConnectionCloseCallback();
	}
}

void Websocket::OnFail(websocketpp::connection_hdl hWebsocketConnection) {
	//m_client.get_alog().write(websocketpp::log::alevel::app, "Connection failed, stopping telemetry!");
	DEBUG_LINEOUT("Websocket Connection Failed");

	//scoped_lock guard(m_lock);
	m_fRunning = false;

	if (m_fnHandleWebsocketConnectionFailCallback != nullptr) {
		m_fnHandleWebsocketConnectionFailCallback();
	}
}
