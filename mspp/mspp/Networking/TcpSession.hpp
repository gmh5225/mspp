#pragma once

#include <functional>
#include "mspp/Utils/third_party.hpp"

using asio::ip::tcp;
using Buffer = std::vector<uint8_t>;

class TcpServer;

class TcpSession : public std::enable_shared_from_this<TcpSession>
{
public:
	explicit TcpSession(asio::io_service& io_service, TcpServer& server);
	tcp::socket& get_socket();
	void start_receive();

	tcp::endpoint get_endpoint();
	std::string get_address();

private:
	friend class TcpServer;

private:
	void handle_read(const asio::error_code& error, size_t bytes_transferred);
	void _handle_read(const asio::error_code& error, size_t bytes_transferred);
	void handle_write(Buffer* write_buffer, const asio::error_code& error, size_t bytes_transferred);

public:
	void write(const Buffer& buffer);
	template <typename TWriteHandler>
	void write(const Buffer& buffer, TWriteHandler&& write_handler);

private:
	static constexpr size_t MAX_LENGTH = 1024 * 1024; // 1MB

	TcpServer& m_server;
	tcp::socket m_socket;
	Buffer m_read_data = Buffer(MAX_LENGTH);
};

template<typename TWriteHandler>
void TcpSession::write(const Buffer& buffer, TWriteHandler&& write_handler)
{
	Buffer* write_buffer = std::make_unique<Buffer>(buffer).release();
	(void)get_socket().async_write_some(
		asio::buffer(*write_buffer, write_buffer->size()),
		std::bind(
			std::forward<TWriteHandler>(write_handler),
			write_buffer,
			std::placeholders::_1,
			std::placeholders::_2
		)
	);
}
