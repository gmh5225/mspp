#pragma once

#include <optional>
#include <unordered_set>
#include "mspp/Networking/TcpSession.hpp"

class TcpServer
{
public:
	explicit TcpServer(const uint16_t port);
	virtual ~TcpServer() = default;
	TcpServer(const TcpServer&) = delete;
	TcpServer& operator=(const TcpServer&) = delete;
	TcpServer(TcpServer&&) = delete;
	TcpServer& operator=(TcpServer&&) = delete;

	asio::io_service& get_io_service();
	void run();
	void stop();
	template <typename TCallable, typename... TInvokeArgs>
	auto post(TCallable&& callable, TInvokeArgs... args);

	/**
	 * \brief Handler for incoming packets.
	 * Should be implemented in every server.
	 */
	virtual void handle_incoming_packet(const std::shared_ptr<TcpSession>& client, const Buffer& packet) = 0;
	/**
	 * \brief Client acceptance handler.
	 * Default implementation does nothing, but can be overriden for sending banners to new clients.
	 */
	virtual void handle_accept(const std::shared_ptr<TcpSession>& new_session);

private:
	friend class TcpSession;

private:
	void _start_accept();
	void _handle_accept(const std::shared_ptr<TcpSession>& new_session, const asio::error_code& error);
	template <typename TImmediateCallable, typename = std::enable_if<std::is_invocable<TImmediateCallable>::value>>
	auto _post(TImmediateCallable&& callable);

private:
	asio::io_service m_io_service;
	tcp::acceptor m_acceptor;
	std::unordered_set<std::shared_ptr<TcpSession>> m_clients;

	asio::thread_pool m_thread_pool = asio::thread_pool(std::min(std::thread::hardware_concurrency() - 1, 1U));
};

template<typename TCallable, typename ...TInvokeArgs>
inline auto TcpServer::post(TCallable&& callable, TInvokeArgs ...args)
{
	return _post(std::bind(std::forward<TCallable>(callable), std::forward<TInvokeArgs>(args)...));
}

template<typename TImmediateCallable, typename>
inline auto TcpServer::_post(TImmediateCallable&& callable)
{
	return asio::post(m_thread_pool, std::forward<TImmediateCallable>(callable));
}
