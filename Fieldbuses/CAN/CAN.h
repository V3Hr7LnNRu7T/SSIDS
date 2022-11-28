// See the file "COPYING" in the main distribution directory for copyright.

#pragma once

#include <sys/types.h>

#include "zeek/packet_analysis/Analyzer.h"
#include "zeek/packet_analysis/Component.h"


namespace zeek::packet_analysis::CAN {

class CANAnalyzer : public Analyzer {
public:
	CANAnalyzer();
	~CANAnalyzer() override = default;

	bool AnalyzePacket(size_t len, const uint8_t* data, Packet* packet) override;

	static zeek::packet_analysis::AnalyzerPtr Instantiate()
		{
		return std::make_shared<CANAnalyzer>();
		}

private:
	zeek::AddrValPtr ToAddrVal(const void* addr);
	zeek::StringValPtr ToEthAddrStr(const u_char* addr);

	void BadCANEvent(const uint8_t* hdr, const char* fmt, ...)
			__attribute__((format(printf, 3, 4)));
	void RequestReplyEvent(EventHandlerPtr e, const u_char* src, const u_char* dst,
			const char* spa, const char* sha, const char* tpa, const char* tha);
};

}
