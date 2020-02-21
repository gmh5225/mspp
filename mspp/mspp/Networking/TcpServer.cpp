#include "mspp/Utils/DebugUtils.hpp"
#include "mspp/Networking/TcpSession.hpp"
#include "mspp/Networking/TcpServer.hpp"

TcpServer::TcpServer(const uint16_t port) :
	m_acceptor(m_io_service, tcp::endpoint(tcp::v4(), port))
{
	_start_accept();
}

asio::io_service& TcpServer::get_io_service()
{
	return m_io_service;
}

void TcpServer::run()
{
	(void)m_io_service.run();
}

void TcpServer::stop()
{
	m_io_service.stop();
}

void TcpServer::handle_accept(const std::shared_ptr<TcpSession>&)
{
}

void TcpServer::_start_accept()
{
	std::shared_ptr<TcpSession> new_session = std::make_shared<TcpSession>(m_io_service, *this);
	(void)m_acceptor.async_accept(
		new_session->get_socket(),
		std::bind(
			&TcpServer::_handle_accept,
			this,
			new_session,
			std::placeholders::_1
		)
	);
	DebugPrint("waiting for new connection");
}

void TcpServer::_handle_accept(const std::shared_ptr<TcpSession>& new_session, const asio::error_code& error)
{
	if(!error)
	{
		DebugPrint("new connection from {}", new_session->get_address());
		m_clients.insert(new_session);
		post(std::bind(&TcpSession::start_receive, new_session.get()));
		post(std::bind(&TcpServer::handle_accept, this, new_session));
	}

	_start_accept();
}
