#ifndef NETDESIGN1_PASSIVESERVER_H
#define NETDESIGN1_PASSIVESERVER_H

#include "TestConfig.h"
#include <string>
#include <netinet/in.h>
#include <thread>
#include <unistd.h>
#include <sys/fcntl.h>
#include "config.h"

class PassiveServer {
private:
	unsigned short port;

	tc::TestNetworkType testNetworkType;

	unsigned int customDataLength;

	int internal;

	unsigned char *buffer;

	std::thread *thread = nullptr;
	std::vector<std::thread> threads;

	volatile bool shouldClose = false;

public:

	explicit PassiveServer(const TestConfig& testConfig);

	~PassiveServer();

	void start();

	void stop();

	bool checkConfig(TestConfig * testConfig);

	tc::TestNetworkType getTestNetworkType();
};


#endif //NETDESIGN1_PASSIVESERVER_H
