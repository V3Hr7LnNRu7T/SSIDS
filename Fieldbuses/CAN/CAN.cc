// See the file "COPYING" in the main distribution directory for copyright.

#include "zeek/zeek-config.h"
#include "zeek/packet_analysis/protocol/can/CAN.h"


#include "zeek/Event.h"
#include "zeek/Val.h"
#include "zeek/packet_analysis/protocol/can/events.bif.h"

using namespace zeek::packet_analysis::CAN;

CANAnalyzer::CANAnalyzer()
	: zeek::packet_analysis::Analyzer("CAN")
	{
	}

// Argh! FreeBSD and Linux have almost completely different net/if_arp.h .
// ... and on Solaris we are missing half of the ARPOP codes, so define
// them here as necessary:

#ifndef ARPOP_REQUEST
#define ARPOP_REQUEST    1 // ARP request.
#endif
#ifndef ARPOP_REPLY
#define ARPOP_REPLY      2 // ARP reply.
#endif
#ifndef ARPOP_PREQUEST
#define ARPOP_RREQUEST   3 // RARP request.
#endif
#ifndef ARPOP_RREPLY
#define ARPOP_RREPLY     4 // RARP reply.
#endif
#ifndef ARPOP_InREQUEST
#define ARPOP_InREQUEST  8 // InARP request.
#endif
#ifndef ARPOP_InREPLY
#define ARPOP_InREPLY    9 // InARP reply.
#endif
#ifndef ARPOP_NAK
#define ARPOP_NAK       10 // (ATM)ARP NAK.
#endif




bool CANAnalyzer::AnalyzePacket(size_t len, const uint8_t* data, Packet* packet)
	{
	RecordTypePtr pay_t;
	auto CAN_payload=id::find_type<RecordType>("CAN_payload");
	uint32_t CANID;
	uint8_t Frlen;
	Frlen=data[20];
	CANID=(((uint16_t *)(data+16))[0]);
	uint8_t RTR=(data[18] & 0x04);
	uint8_t Error=(data[18] & 0x02);
	uint8_t Extended=(data[18] & 0x08);
	uint32_t Reserved;
	memcpy(&Reserved, (data+21),3);
	uint8_t payload[8];
	memcpy(payload , (data+24),8);
	//RecordType pay={payload[0],payload[1],payload[2],payload[3],payload[4],payload[5],payload[6],payload[7]};
	DBG_LOG(DBG_LOGGING, "CANID %x %x %x %x %x %x %x %x %x %x %x %x %x\n", CANID,Frlen,RTR,Error,Extended,payload[0],payload[1],payload[2],payload[3],payload[4],payload[5],payload[6],payload[7]);
	
	auto pay_val=make_intrusive<zeek::RecordVal>(CAN_payload);
	pay_val->Assign(0,payload[0]);
	pay_val->Assign(1,payload[1]);
	pay_val->Assign(2,payload[2]);
	pay_val->Assign(3,payload[3]);
	pay_val->Assign(4,payload[4]);
	pay_val->Assign(5,payload[5]);
	pay_val->Assign(6,payload[6]);
	pay_val->Assign(7,payload[7]);
	
	event_mgr.Enqueue(can_request,val_mgr->Count(CANID),val_mgr->Count(Frlen),val_mgr->Count(RTR),val_mgr->Count(Error),val_mgr->Count(Extended),pay_val);
	
	return true;
	}

zeek::AddrValPtr CANAnalyzer::ToAddrVal(const void* addr)
	{
	//Note: We only handle IPv4 addresses.
	return zeek::make_intrusive<zeek::AddrVal>(*(const uint32_t*) addr);
	}

zeek::StringValPtr CANAnalyzer::ToEthAddrStr(const u_char* addr)
	{
	char buf[1024];
	snprintf(buf, 16, "%02x%02x%02x:%02x%02x%02x",
			 addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
	return zeek::make_intrusive<zeek::StringVal>(buf);
	}

void CANAnalyzer::BadCANEvent(const uint8_t* hdr, const char* fmt, ...)
	{
	if ( ! bad_can )
		return;

	char msg[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(msg, sizeof(msg), fmt, args);
	va_end(args);

	event_mgr.Enqueue(bad_can,
			ToEthAddrStr((const u_char*)(hdr)), 
			zeek::make_intrusive<zeek::StringVal>(msg));
	}

void CANAnalyzer::RequestReplyEvent(EventHandlerPtr e, const u_char *src, const u_char *dst,
		const char *spa, const char *sha, const char *tpa, const char *tha)
	{
	event_mgr.Enqueue(e,ToEthAddrStr((const u_char*) (src)));

	}
