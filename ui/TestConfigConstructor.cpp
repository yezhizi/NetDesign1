#include "TestConfigConstructor.h"
#include "iostream"

void TestConfigConstructor::setTotalTestCount(const QString& q) {
	this->totalTestCount = q.toUInt();
}

void TestConfigConstructor::setSingleTestCount(const QString &q) {
	this->singleTestCount = q.toUInt();
}

void TestConfigConstructor::setTotalTestInterval(const QString &q) {
	this->totalTestInterval = std::chrono::microseconds(q.toUInt());
}

void TestConfigConstructor::setSingleTestInterval(const QString &q) {
	this->singleTestInterval = std::chrono::microseconds(q.toUInt());
}

void TestConfigConstructor::setNetworkType(int index) {
	this->testNetworkType = (tc::TestNetworkType) index;
	std::cout<<"changed to "<<index<<std::endl;
}

void TestConfigConstructor::setSourcePort(const QString &q) {
	this->sourcePort = q.toUShort();
}

void TestConfigConstructor::setDestinationAddress(const QString &q) {
	this->destinationAddress = q.toStdString();
}

void TestConfigConstructor::setDestinationPort(const QString &q) {
	this->destinationPort = q.toUShort();
}

void TestConfigConstructor::setCustomData(const QString& q) {
	this->customData = q;
}

void TestConfigConstructor::setCustomDataLength(const QString &q) {
	this->customDataLength = q.toUInt();
}

TestConfig *TestConfigConstructor::build() {
	if (this->customData.isEmpty())
		return new TestConfig(this->singleTestCount, this->totalTestCount, this->singleTestInterval, this->totalTestInterval,
		                      this->testNetworkType, this->sourcePort, this->destinationAddress, this->destinationPort, nullptr, this->customDataLength);
	else {
		auto * _customData = new unsigned char[this->customDataLength];
		std::memcpy(_customData, this->customData.toStdString().c_str(), this->customDataLength);
		return new TestConfig(this->singleTestCount, this->totalTestCount, this->singleTestInterval, this->totalTestInterval,
		                      this->testNetworkType, this->sourcePort, this->destinationAddress, this->destinationPort, _customData, this->customDataLength);
	}
}

TestConfig *TestConfigConstructor::construct() {
	return build();
}

bool TestConfigConstructor::loadFromFile(const QString& path) {

}

TestConfigConstructor::TestConfigConstructor() = default;

TestConfigConstructor::~TestConfigConstructor() = default;
