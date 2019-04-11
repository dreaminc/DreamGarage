#include "Websocket.h"

#include "DreamLogger/DreamLogger.h"

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
	m_fnOnWebsocketInitCallback(std::bind(&Websocket::OnInit, this, ::_1)),
	m_fnHandleWebsocketMessageCallback(fnHandleWebsocketMessageCallback),
	m_fnHandleWebsocketConnectionOpenCallback(fnHandleWebsocketConnectionOpenCallback),
	m_fnHandleWebsocketConnectionFailCallback(fnHandleWebsocketConnectionFailCallback),
	m_fnHandleWebsocketConnectionCloseCallback(fnHandleWebsocketConnectionCloseCallback)
{
	// 
}

Websocket::Websocket(const std::string& strURI) :
	m_fRunning(false),
	m_fConnectionOpen(false),
	m_strURI(strURI),
	m_fnOnWebsocketMessageCallback(std::bind(&Websocket::OnMessage, this, ::_1, ::_2, ::_3)),
	m_fnOnWebsocketConnectionOpenCallback(std::bind(&Websocket::OnOpen, this, ::_1)),
	m_fnOnWebsocketConnectionFailCallback(std::bind(&Websocket::OnFail, this, ::_1)),
	m_fnOnWebsocketConnectionCloseCallback(std::bind(&Websocket::OnClose, this, ::_1)),
	m_fnOnWebsocketInitCallback(std::bind(&Websocket::OnInit, this, ::_1)),
	m_fnHandleWebsocketMessageCallback(nullptr),
	m_fnHandleWebsocketConnectionOpenCallback(nullptr),
	m_fnHandleWebsocketConnectionFailCallback(nullptr),
	m_fnHandleWebsocketConnectionCloseCallback(nullptr)
{
	//
}

RESULT Websocket::SetToken(const std::string& strToken) {
	m_strToken = "Bearer " + strToken;
	return R_PASS;
}

// Should not actually be in a thread like this
/*
RESULT Websocket::ProcessingThread() {
	RESULT r = R_PASS;

	m_fRunning = true;

	//while (m_fRunning) {
		// TODO: Are we ok w/ Exceptions?
		
	//}

	DEBUG_LINEOUT("Websocket Thread Exit");

Error:
	return r;
}
*/

RESULT Websocket::Initialize() {
	RESULT r = R_PASS;

	m_websocketClient.set_access_channels(websocketpp::log::alevel::all);
	m_websocketClient.clear_access_channels(websocketpp::log::alevel::all);
	m_websocketClient.set_error_channels(websocketpp::log::alevel::all);

	// Initialize ASIO
	m_websocketClient.init_asio();
	m_websocketClient.start_perpetual();

	m_pWebsocketThread.reset(new websocketpp::lib::thread(&WebsocketClient::run, &m_websocketClient));
	CNM(m_pWebsocketThread, "Failed to set up websocket thread");
	//m_pWebsocketThread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&WebsocketClient::run, &m_websocketClient);

	// Handlers
	m_websocketClient.set_message_handler(std::bind(m_fnOnWebsocketMessageCallback, &m_websocketClient, ::_1, ::_2));
	//m_websocketClient.set_socket_init_handler(std::bind(m_fnOnWebsocketInitCallback, ::_1));
	m_websocketClient.set_open_handler(std::bind(m_fnOnWebsocketConnectionOpenCallback, ::_1));
	m_websocketClient.set_close_handler(std::bind(m_fnOnWebsocketConnectionCloseCallback, ::_1));
	m_websocketClient.set_fail_handler(std::bind(m_fnOnWebsocketConnectionFailCallback, ::_1));

	/*
	m_websocketClient.set_socket_init_handler(
	[](websocketpp::connection_hdl, asio::ssl::stream<asio::ip::tcp::socket> &ssl_stream)
	{
	SSL_set_tlsext_host_name(ssl_stream.native_handle(), "ws.develop.dreamos.com");
	}
	);
	//*/

#ifndef USE_LOCALHOST
	m_websocketClient.set_tls_init_handler([](websocketpp::connection_hdl) -> context_ptr {
		websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> ctx = 
			websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);

		try {
			ctx->set_options(asio::ssl::context::default_workarounds |
				asio::ssl::context::no_sslv2 |
				asio::ssl::context::no_sslv3 |
				asio::ssl::context::single_dh_use);

			// Client verification for a server trust is not yet supported
			//ctx->set_verify_mode(asio::ssl::verify_peer);
			//ctx->set_verify_callback(bind(&verify_certificate, hostname, ::_1, ::_2));

			//ctx->load_verify_file("ca-cert.pem");
		}
		catch (std::exception& e) {
			(void) e;

			DOSLOG(INFO, "set_tls_init_handler exception %v", e.what());
			DEBUG_LINEOUT("%s", e.what());
		}
		return ctx;
	});
#endif

Error:
	return r;
}

RESULT Websocket::Start() {
	RESULT r = R_PASS;

	websocketpp::lib::error_code websocketError;

	DEBUG_LINEOUT("Websocket::Start");

	try {

		CRM(Initialize(), "Failed to initialize the websocket");

		m_pWebsocketConnection = m_websocketClient.get_connection(m_strURI, websocketError);
		CNM(m_pWebsocketConnection, "Failed to get websocket connection");
		CBM((!websocketError), "Connection failed with error: %s", websocketError.message().c_str());

		if (m_strToken.size() > 0) {
			m_pWebsocketConnection->append_header("Authorization", m_strToken);
		}

		m_websocketClient.connect(m_pWebsocketConnection);

		m_fRunning = true;
	}
	catch (websocketpp::exception const & e) {
		DEBUG_LINEOUT("Websocket Exception: %s", e.what());
	}
	

Error:
	return r;
}

RESULT Websocket::Stop() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Websocket::Stop");

	// Close the connection

	CNRM(m_pWebsocketConnection, R_SKIPPED, "Websocket connection already null");

	{
		// Move to shut down
		m_websocketClient.stop_perpetual();
		websocketpp::lib::error_code websocketError;

		m_websocketClient.close(
			m_pWebsocketConnection->get_handle(),
			websocketpp::close::status::going_away,
			"disconnect",
			websocketError
		);

		if (websocketError) {
			DEBUG_LINEOUT("Error closing connection: %s", websocketError.message().c_str());
		}

		m_pWebsocketThread->join();

		m_pWebsocketThread = nullptr;
		m_pWebsocketConnection = nullptr;

		m_websocketClient.reset();
		
		m_fConnectionOpen = false;
		m_fRunning = false;
	}
	 
Error:
	return r;
}

RESULT Websocket::Send(const std::string & strMessage) {
	RESULT r = R_PASS;

	if(m_fConnectionOpen) {

		DEBUG_LINEOUT("waiting on send");

		auto websocketMessage = m_pWebsocketConnection->get_message(websocketpp::frame::opcode::TEXT, strMessage.length());

		websocketMessage->set_payload(strMessage.c_str());

		websocketpp::lib::error_code websocketError;

		m_websocketClient.send(
			m_pWebsocketConnection->get_handle(),
			websocketMessage->get_payload(),
			websocketMessage->get_opcode(),
			websocketError
		);

		CBM((!websocketError), "Echo failed with message: %s", websocketError.message().c_str());
	}

Error:
	return r;
}

void Websocket::OnInit(websocketpp::connection_hdl hWebsocketConnection) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Websocket Initialized");

	if (m_fnHandleWebsocketInitCallback != nullptr) {
		m_fnHandleWebsocketInitCallback();
	}

Error:
	return;
}

void Websocket::OnMessage(WebsocketClient* pWebsocketClient, websocketpp::connection_hdl hWebsocketConnection, message_ptr pWebsocketMessage) {
	RESULT r = R_PASS;

	CB(m_fRunning);

	DEBUG_LINEOUT("OnMessage called with handle: 0x%p and message: %s", hWebsocketConnection.lock().get(), pWebsocketMessage->get_payload().c_str());
	
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

Error:
	return;
}

#pragma warning(disable : 4503)

void Websocket::OnOpen(websocketpp::connection_hdl hWebsocketConnection) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Websocket Connection Opened");

	m_fConnectionOpen = true;

	if (m_fnHandleWebsocketConnectionOpenCallback != nullptr) {
		m_fnHandleWebsocketConnectionOpenCallback();
	}

Error:
	return;
}


void Websocket::OnClose(websocketpp::connection_hdl hWebsocketConnection) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Websocket Connection Closed");

	m_fRunning = false;
	m_fConnectionOpen = false;

	if (m_fnHandleWebsocketConnectionCloseCallback != nullptr) {
		m_fnHandleWebsocketConnectionCloseCallback();
	}

Error:
	return;
}

void Websocket::OnFail(websocketpp::connection_hdl hWebsocketConnection) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Websocket Connection Failed");

	m_fRunning = false;

	if (m_fnHandleWebsocketConnectionFailCallback != nullptr) {
		m_fnHandleWebsocketConnectionFailCallback();
	}

Error:
	return;
}
