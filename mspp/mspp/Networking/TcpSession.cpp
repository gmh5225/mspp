#include "mspp/Utils/DebugUtils.hpp"
#include "mspp/Networking/TcpServer.hpp"
#include "mspp/Networking/TcpSession.hpp"

TcpSession::TcpSession(asio::io_service& io_service, TcpServer& server) :
	m_server(server),
	m_socket(io_service)
{
}

tcp::socket& TcpSession::get_socket()
{
	return m_socket;
}

void TcpSession::start_receive()
{
	(void)m_socket.async_read_some(
		asio::buffer(m_read_data),
		std::bind(
			&TcpSession::_handle_read,
			this,
			std::placeholders::_1,
			std::placeholders::_2
		)
	);
}

tcp::endpoint TcpSession::get_endpoint()
{
	return get_socket().remote_endpoint();
}

std::string TcpSession::get_address()
{
	return fmt::format("{}:{}", get_endpoint().address().to_string(), std::to_string(get_endpoint().port()));
}

void TcpSession::handle_read(const asio::error_code& error, size_t bytes_transferred)
{
	if(!error)
	{
		start_receive();
		(void)m_server.post(&TcpServer::handle_incoming_packet, &m_server, shared_from_this(), Buffer(m_read_data.cbegin(), m_read_data.cbegin() + bytes_transferred));
	}
	else if(asio::error::eof == error || asio::error::connection_reset == error)
	{
		DebugPrint("lost connection to {}", get_address());
		(void)m_server.post([this] {
			m_server.m_clients.erase(m_server.m_clients.find(shared_from_this()));
		});
	}
	else
	{
		throw std::runtime_error(error.message());
	}
}

void TcpSession::_handle_read(const asio::error_code& error, size_t bytes_transferred)
{
	DebugPrint("received {} bytes from {}", bytes_transferred, get_address());
	(void)m_server.post(&TcpSession::handle_read, this, error, bytes_transferred);
}

void TcpSession::handle_write(Buffer* write_buffer, const asio::error_code& error, size_t bytes_transferred)
{
	DebugPrint("sent {} bytes to {}", bytes_transferred, get_address());
	const std::unique_ptr<Buffer> auto_free_write_buffer(write_buffer);

	if(error)
	{
		throw std::runtime_error(error.message());
	}
}

void TcpSession::write(const Buffer& buffer)
{
	Buffer* write_buffer = std::make_unique<Buffer>(buffer).release();
	(void)get_socket().async_write_some(
		asio::buffer(*write_buffer, write_buffer->size()),
		std::bind(
			&TcpSession::handle_write,
			this,
			write_buffer,
			std::placeholders::_1,
			std::placeholders::_2
		)
	);
}
