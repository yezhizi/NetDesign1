#include "PassiveServer.h"

PassiveServer::PassiveServer(const TestConfig& testConfig) : port(testConfig.getDestinationPort()), testNetworkType(testConfig.getTestNetworkType()), customDataLength(testConfig.getCustomDataLength()) {
	this->internal = socket(AF_INET, tc::convertNetworkType(testNetworkType), 0);
	if (this->internal == -1)
		throw std::runtime_error("Failed to create socket at port " + std::to_string(port));
	this->buffer = new unsigned char[testConfig.getCustomDataLength()];
	int option = 1;
	if (setsockopt(this->internal, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
		throw std::runtime_error("Failed to set socket options at port " + std::to_string(port));
	if (testNetworkType == tc::TestNetworkType::TCP) {
		option = customDataLength;
		if (setsockopt(this->internal, SOL_SOCKET, SO_RCVBUF, &option, sizeof(option)) == -1)
			throw std::runtime_error("Failed to set socket size options at port " + std::to_string(port));
	}
	struct sockaddr_in addr {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->internal, (struct sockaddr *) &addr, sizeof(addr)) == -1)
		throw std::runtime_error("Failed to bind socket at port " + std::to_string(port));
	if (testNetworkType == tc::TestNetworkType::TCP) {
#ifdef WIN32
		ioctlsocket(this->internal, FIONBIO, (unsigned long *) &option);
#endif
		listen(this->internal, testConfig.getSingleTestCount());
	}
}

void PassiveServer::start() {
	this->thread = new std::thread([this]() {
		while (true) {
			if (this->shouldClose)
				break;
			struct sockaddr_in addr {};
			socklen_t addrlen = sizeof(addr);
			if (this->testNetworkType == tc::TestNetworkType::TCP) {
				int client = accept(this->internal, (struct sockaddr *) &addr, &addrlen);
				if (client == -1)
					continue;
				this->threads.emplace_back([this, client]() {
					unsigned int length;
					while (!this->shouldClose) {
						length = recv(client, this->buffer, customDataLength, MSG_DONTWAIT);
						if (length != -1)
							send(client, this->buffer, length, MSG_DONTWAIT);
						else if (errno != 0x23)
							break;
					}
					shutdown(client, SHUT_RDWR);
					::close(client);
				});
			} else {
				unsigned int length = recvfrom(this->internal, this->buffer, customDataLength, MSG_DONTWAIT, (struct sockaddr *) &addr, &addrlen);
				if (length != 0)
					sendto(this->internal, this->buffer, length, MSG_DONTWAIT, (struct sockaddr *) &addr, addrlen);
			}
		}
	});
}

void PassiveServer::stop() {
	if (this->thread == nullptr)
		return;
	if (this->shouldClose)
		return;
	::close(this->internal);
	this->shouldClose = true;
	this->thread->join();
	for (auto &thread : this->threads)
		thread.join();
	delete this->thread;
	this->thread = nullptr;
}

PassiveServer::~PassiveServer() {
	this->stop();
	delete[] this->buffer;
}

bool PassiveServer::checkConfig(TestConfig *testConfig) {
	return testConfig->getDestinationPort() == this->port && testConfig->getTestNetworkType() == this->testNetworkType && testConfig->getCustomDataLength() == this->customDataLength;
}

tc::TestNetworkType PassiveServer::getTestNetworkType() {
	return this->testNetworkType;
}


