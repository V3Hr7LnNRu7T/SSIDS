// See the file "COPYING" in the main distribution directory for copyright.

#include "zeek/plugin/Plugin.h"
#include "zeek/packet_analysis/Component.h"
#include "zeek/packet_analysis/protocol/can/CAN.h"

namespace zeek::plugin::Zeek_CAN {

class Plugin : public zeek::plugin::Plugin {
public:
	zeek::plugin::Configuration Configure()
		{
		AddComponent(new zeek::packet_analysis::Component("CAN",
		                 zeek::packet_analysis::CAN::CANAnalyzer::Instantiate));

		zeek::plugin::Configuration config;
		config.name = "Zeek::CAN";
		config.description = "CAN packet analyzer";
		return config;
		}

} plugin;

}
