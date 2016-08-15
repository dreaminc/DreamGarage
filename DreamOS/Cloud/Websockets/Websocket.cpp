#include "Websocket.h"

#include <iostream>

Websocket::Websocket(const std::string& uri, const Callback& callback) :
	m_isRunning(false),
	m_uri(uri),
	m_callback(callback)
{
	Start();
}

Websocket::Websocket(const std::string& uri) :
	m_isRunning(false),
	m_uri(uri),
	m_callback(std::bind(&Websocket::OnMessage, this, ::_1, ::_2, ::_3))
{
	Start();
}

Websocket::~Websocket()
{
	Stop();
}

void Websocket::ProcessingThread()
{
	m_isRunning = true;

	while (m_isRunning)
	{
		try 
		{
			m_client.set_access_channels(websocketpp::log::alevel::all);
			m_client.clear_access_channels(websocketpp::log::alevel::frame_payload);

			m_client.init_asio();

			m_client.set_message_handler(bind(m_callback, &m_client, ::_1, ::_2));
			//c.set_open_handler(bind(&on_open, &c, ::_1));
			//c.set_fail_handler(bind(&on_fail, &c, ::_1));
			//c.set_close_handler(bind(&on_close, &c, ::_1));

			websocketpp::lib::error_code ec;
			m_connection = m_client.get_connection(m_uri, ec);
			if (ec) {
				std::cout << "could not create connection because: " << ec.message() << std::endl;
				return;
			}

			m_client.connect(m_connection);
			m_client.run();
		}
		catch (websocketpp::exception const & e) {
			std::cout << e.what() << std::endl;
		}
	}
}

void Websocket::Start()
{
	std::cout << "Websocket::Start" << std::endl;

	m_thread = std::thread(&Websocket::ProcessingThread, this);
}

void Websocket::Stop()
{
	std::cout << "Websocket::Stop" << std::endl;

	m_isRunning = false;
	m_thread.join();
}

void Websocket::Send(const std::string & message)
{
	auto msg = m_connection->get_message(websocketpp::frame::opcode::TEXT, message.length());

	msg->set_payload(message.c_str());
	//msg->append_payload("efgh");

	websocketpp::lib::error_code ec;
	m_client.send(m_connection->get_handle(), msg->get_payload(), msg->get_opcode(), ec);
	if (ec) {
		std::cout << "Echo failed because: " << ec.message() << std::endl;
	}
}

void Websocket::OnMessage(client* c, websocketpp::connection_hdl hdl, message_ptr msg)
{
	std::cout << "OnMessage called with hdl: " << hdl.lock().get()
		<< " and message: " << msg->get_payload()
		<< std::endl;

	websocketpp::lib::error_code ec;

	c->send(hdl, msg->get_payload(), msg->get_opcode(), ec);
	if (ec) {
		std::cout << "Echo failed because: " << ec.message() << std::endl;
	}
}
