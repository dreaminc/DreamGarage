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
	//if (m_fRunning) {
	//	Stop();
	//}
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

RESULT Websocket::Start() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Websocket::Start");	

	try {
		m_websocketClient.set_access_channels(websocketpp::log::alevel::all);
		m_websocketClient.clear_access_channels(websocketpp::log::alevel::all);

		m_websocketClient.init_asio();
		m_websocketClient.start_perpetual();

		// Set message handler 
		m_websocketClient.set_message_handler(
			std::bind(m_fnOnWebsocketMessageCallback, &m_websocketClient, ::_1, ::_2)
		);

		/*
		m_websocketClient.set_socket_init_handler(
		[](websocketpp::connection_hdl, asio::ssl::stream<asio::ip::tcp::socket> &ssl_stream)
		{
		SSL_set_tlsext_host_name(ssl_stream.native_handle(), "ws.develop.dreamos.com");
		}
		);
		//*/

#ifndef USE_LOCALHOST
		m_websocketClient.set_tls_init_handler([](websocketpp::connection_hdl) ->context_ptr {
			context_ptr ctx = websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);

			try {
				ctx->set_options(asio::ssl::context::default_workarounds |
					asio::ssl::context::no_sslv2 |
					asio::ssl::context::no_sslv3 |
					asio::ssl::context::single_dh_use);

				// client verification for a server trust is not yet supported.

				//ctx->set_verify_mode(asio::ssl::verify_peer);
				//ctx->set_verify_callback(bind(&verify_certificate, hostname, ::_1, ::_2));

				//ctx->load_verify_file("ca-cert.pem");
			}
			catch (std::exception& e) {
				(void)e;
				DOSLOG(INFO, "set_tls_init_handler exception %v", e.what());
				DEBUG_LINEOUT("%s", e.what());
				//ACBM(0, "%s", e.what());
			}
			return ctx;
		});
#endif

		// Handlers
		m_websocketClient.set_open_handler(std::bind(m_fnOnWebsocketConnectionOpenCallback, ::_1));
		m_websocketClient.set_close_handler(std::bind(m_fnOnWebsocketConnectionCloseCallback, ::_1));
		m_websocketClient.set_fail_handler(std::bind(m_fnOnWebsocketConnectionFailCallback, ::_1));

		websocketpp::lib::error_code websocketError;
		m_pWebsocketConnection = m_websocketClient.get_connection(m_strURI, websocketError);

		if (websocketError) {
			DOSLOG(INFO, "websocketError %v", websocketError.message().c_str());
		}

		CBM((!websocketError), "Connection failed with error: %s", websocketError.message().c_str());

		if (m_strToken.size() > 0) {
			m_pWebsocketConnection->append_header("Authorization", m_strToken);
		}

		//m_websocketClient.run();
		m_pWebsockThread.reset(new websocketpp::lib::thread(&WebsocketClient::run, &m_websocketClient));
		
		m_websocketClient.connect(m_pWebsocketConnection);

		m_fRunning = true;
	}
	catch (websocketpp::exception const & e) {
		e;
		DEBUG_LINEOUT("Websocket Exception: %s", e.what());
	}
	
	

Error:
	return r;
}

RESULT Websocket::Stop() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("Websocket::Stop");

	// TODO: This is causing some kind of error on exit still
	m_fRunning = false;

	// Close the connection

	// TODO: Move to shut down
	m_websocketClient.stop_perpetual();
	websocketpp::lib::error_code websocketError;

	CNRM(m_pWebsocketConnection, R_SKIPPED, "Websocket connection already null");

	m_websocketClient.close(
		m_pWebsocketConnection->get_handle(), 
		websocketpp::close::status::going_away,
		"disconnect", 
		websocketError
	);

	if (websocketError) {
		DEBUG_LINEOUT("Error closing connection: %s", websocketError.message().c_str());
	}
	else {
		if (m_pWebsockThread->joinable()) {
			m_pWebsockThread->join();
		}
	}

	m_pWebsockThread = nullptr;

	m_pWebsocketConnection = nullptr;

	m_websocketClient.reset();

Error:
	return r;
}

RESULT Websocket::Send(const std::string & strMessage) {
	RESULT r = R_PASS;

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

Error:
	return r;
}

void Websocket::OnMessage(WebsocketClient* pWebsocketClient, websocketpp::connection_hdl hWebsocketConnection, message_ptr pWebsocketMessage) {
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
}

#pragma warning(disable : 4503)

void Websocket::OnOpen(websocketpp::connection_hdl hWebsocketConnection) {
	DEBUG_LINEOUT("Websocket Connection Opened");

	//scoped_lock guard(m_lock);
	m_fConnectionOpen = true;

	if (m_fnHandleWebsocketConnectionOpenCallback != nullptr) {
		m_fnHandleWebsocketConnectionOpenCallback();
	}
}


void Websocket::OnClose(websocketpp::connection_hdl hWebsocketConnection) {
	DEBUG_LINEOUT("Websocket Connection Closed");

	//scoped_lock guard(m_lock);
	m_fRunning = false;

	if (m_fnHandleWebsocketConnectionCloseCallback != nullptr) {
		m_fnHandleWebsocketConnectionCloseCallback();
	}
}

void Websocket::OnFail(websocketpp::connection_hdl hWebsocketConnection) {
	DEBUG_LINEOUT("Websocket Connection Failed");

	//scoped_lock guard(m_lock);
	m_fRunning = false;

	if (m_fnHandleWebsocketConnectionFailCallback != nullptr) {
		m_fnHandleWebsocketConnectionFailCallback();
	}
}
