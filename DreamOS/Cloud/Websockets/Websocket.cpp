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
	if (m_fRunning) {
		Stop();
	}
}

RESULT Websocket::SetToken(const std::string& strToken) {
	m_strToken = "Bearer " + strToken;
	return R_PASS;
}

// The following is a client verification for a trusted server.
// currently being unsupported. for more details: https://github.com/zaphoyd/websocketpp/tree/develop/examples/print_client_tls
/* 
/// Verify that one of the subject alternative names matches the given hostname
bool verify_subject_alternative_name(const char * hostname, X509 * cert) {
     STACK_OF(GENERAL_NAME) * san_names = NULL;
     
     san_names = (STACK_OF(GENERAL_NAME) *) X509_get_ext_d2i(cert, NID_subject_alt_name, NULL, NULL);
     if (san_names == NULL) {
         return false;
     }
     
     int san_names_count = sk_GENERAL_NAME_num(san_names);
     
     bool result = false;
     
     for (int i = 0; i < san_names_count; i++) {
         const GENERAL_NAME * current_name = sk_GENERAL_NAME_value(san_names, i);
         
         if (current_name->type != GEN_DNS) {
             continue;
         }
         
         char * dns_name = (char *) ASN1_STRING_data(current_name->d.dNSName);
         
         // Make sure there isn't an embedded NUL character in the DNS name
         if (ASN1_STRING_length(current_name->d.dNSName) != strlen(dns_name)) {
             break;
         }
         // Compare expected hostname with the CN
         result = (strcmp(hostname, dns_name) == 0);
     }
     sk_GENERAL_NAME_pop_free(san_names, GENERAL_NAME_free);
     
     return result;
 }
 
 /// Verify that the certificate common name matches the given hostname
 bool verify_common_name(const char * hostname, X509 * cert) {
     // Find the position of the CN field in the Subject field of the certificate
     int common_name_loc = X509_NAME_get_index_by_NID(X509_get_subject_name(cert), NID_commonName, -1);
     if (common_name_loc < 0) {
         return false;
     }
     
     // Extract the CN field
     X509_NAME_ENTRY * common_name_entry = X509_NAME_get_entry(X509_get_subject_name(cert), common_name_loc);
     if (common_name_entry == NULL) {
         return false;
     }
     
     // Convert the CN field to a C string
     ASN1_STRING * common_name_asn1 = X509_NAME_ENTRY_get_data(common_name_entry);
     if (common_name_asn1 == NULL) {
         return false;
     }
     
     char * common_name_str = (char *) ASN1_STRING_data(common_name_asn1);
     
     // Make sure there isn't an embedded NUL character in the CN
     if (ASN1_STRING_length(common_name_asn1) != strlen(common_name_str)) {
         return false;
     }
     
     // Compare expected hostname with the CN
     return (strcmp(hostname, common_name_str) == 0);
 }

bool verify_certificate(const char * hostname, bool preverified, asio::ssl::verify_context& ctx) {
    // The verify callback can be used to check whether the certificate that is
	// being presented is valid for the peer. For example, RFC 2818 describes
	// the steps involved in doing this for HTTPS. Consult the OpenSSL
	// documentation for more details. Note that the callback is called once
	// for each certificate in the certificate chain, starting from the root
	// certificate authority.
	
	// Retrieve the depth of the current cert in the chain. 0 indicates the
	// actual server cert, upon which we will perform extra validation
	// (specifically, ensuring that the hostname matches. For other certs we
	// will use the 'preverified' flag from Asio, which incorporates a number of
	// non-implementation specific OpenSSL checking, such as the formatting of
	// certs and the trusted status based on the CA certs we imported earlier.
	int depth = X509_STORE_CTX_get_error_depth(ctx.native_handle());
	
	// if we are on the final cert and everything else checks out, ensure that
	// the hostname is present on the list of SANs or the common name (CN).
	if (depth == 0 && preverified) {
	X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
	
	if (verify_subject_alternative_name(hostname, cert)) {
	return true;
	
		}
			else if (verify_common_name(hostname, cert)) {
				return true;
				
			}
			else {
				return false;
				
			}

	}
	
	return preverified;
	
}
*/

RESULT Websocket::ProcessingThread() {
	RESULT r = R_PASS;

	m_fRunning = true;

	while (m_fRunning) {
		// TODO: Are we ok w/ Exceptions?
		try  {
			m_websocketClient.set_access_channels(websocketpp::log::alevel::all);
			m_websocketClient.clear_access_channels(websocketpp::log::alevel::all);

			m_websocketClient.init_asio();

			m_websocketClient.set_message_handler(std::bind(m_fnOnWebsocketMessageCallback, &m_websocketClient, ::_1, ::_2));

//			m_websocketClient
			
			/*
			m_websocketClient.set_socket_init_handler([](websocketpp::connection_hdl, asio::ssl::stream<asio::ip::tcp::socket> &ssl_stream)
			{
				SSL_set_tlsext_host_name(ssl_stream.native_handle(), "ws.develop.dreamos.com");
			});
			//*/

			//TODO: NEED TO REENABLE FOR PRODUCTION
			//TODO: ALSO SWITCH TYPEDEF BACK TO asio_client
			/*
			m_websocketClient.set_tls_init_handler([] (websocketpp::connection_hdl) ->context_ptr {
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
			//*/

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
	
	m_pWebsocketConnection.reset();
	
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
