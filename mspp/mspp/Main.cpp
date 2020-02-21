#include "mspp/Networking/TcpServer.hpp"
#include "mspp/Utils/IterableUtils.hpp"

class EchoServer : public TcpServer
{
public:
	explicit EchoServer(const uint16_t port) :
		TcpServer(port)
	{
	}
	virtual ~EchoServer() = default;
	EchoServer(const EchoServer&) = delete;
	EchoServer(EchoServer&& other) = delete;
	EchoServer& operator=(const EchoServer&) = delete;
	EchoServer& operator=(EchoServer&&) = delete;

	virtual void handle_incoming_packet(const std::shared_ptr<TcpSession>& client, const Buffer& packet) override
	{
		std::string packet_as_string(packet.cbegin(), packet.cend());
		if(IterableUtils::starts_with(packet_as_string, std::string("quit")))
		{
			stop();
			return;
		}
		client->write(packet);
	}
};

int main(int argc, char* argv[])
{
	EchoServer(1337).run();

	return 0;
}
