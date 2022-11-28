// ping.cc

#include <assert.h>
#include <iostream>
#include <iostream>
#include <csignal>
#include <time.h>
#include "broker/broker.hh"
#include "broker/zeek.hh"
#include <sys/types.h>
#include <sys/msg.h>
#include <stdio.h>
#include <argp.h>
/* Get definition of MSG_EXCEPT */
#define MAX_MTEXT 1024
#define		CRC_POLY_16		0xA001
#define		CRC_POLY_32		0xEDB88320L
#define		CRC_POLY_CCITT		0x1021
#define		CRC_POLY_DNP		0xA6BC
#define		CRC_POLY_KERMIT		0x8408
#define		CRC_POLY_SICK		0x8005

/*
 * #define CRC_START_xxxx
 *
 * The constants of the form CRC_START_xxxx define the values that are used for
 * initialization of a CRC value for common used calculation methods.
 */

#define		CRC_START_8		0x00
#define		CRC_START_16		0x0000
#define		CRC_START_MODBUS	0xFFFF
#define		CRC_START_XMODEM	0x0000
#define		CRC_START_CCITT_1D0F	0x1D0F
#define		CRC_START_CCITT_FFFF	0xFFFF
#define		CRC_START_KERMIT	0x0000
#define		CRC_START_SICK		0x0000
#define		CRC_START_DNP		0x0000
#define		CRC_START_32		0xFFFFFFFFL
struct mbuf {
long mtype;
char mtext[MAX_MTEXT];
};

/* pcap related */


static int verbose_flag;

typedef unsigned int guint32;
typedef unsigned short int guint16;
typedef int gint32;
typedef short int gint16;
FILE *dest;

/*
                        1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 0 |                   Block Type = 0x0A0D0D0A                     |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 4 |                      Block Total Length                       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 8 |                      Byte-Order Magic                         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
12 |          Major Version        |         Minor Version         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
16 |                                                               |
   |                          Section Length                       |
   |                                                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
24 /                                                               /
   /                      Options (variable)                       /
   /                                                               /
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                      Block Total Length                       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
typedef struct pcapng_shb_s {
	   guint32 Block_Type;
	   guint32 Block_Total_Length;
        guint32 Byte_Order_Magic;   /* magic number */
        guint16 Major_Version ;  /* major version number */
        guint16 Minor_Version;  /* minor version number */
        unsigned long int Section_Length;
} pcapng_shb_t;

typedef struct pcaprec_hdr_s {
        guint32 ts_sec;         /* timestamp seconds */
        guint32 ts_usec;        /* timestamp microseconds */
        guint32 incl_len;       /* number of octets of packet saved in file */
        guint32 orig_len;       /* actual length of packet */
} pcaprec_hdr_t;

typedef struct option_s
{
guint16 OpCode;
guint16 OpLen;
} option_t;
/*                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|      Option Code              |         Option Length         |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/                       Option Value                            /
/              variable length, padded to 32 bits               /
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
/                                                               /
/                 . . . other options . . .                     /
/                                                               /
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   Option Code == opt_endofopt |   Option Length == 0          |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/



 /*                     1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 0 |                    Block Type = 0x00000001                    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 4 |                      Block Total Length                       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 8 |           LinkType            |           Reserved            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
12 |                            SnapLen                            |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
16 /                                                               /
   /                      Options (variable)                       /
   /                                                               /
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                      Block Total Length                       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   */


typedef struct pcapng_idb_s {
	   guint32 Block_Type;
	   guint32 Block_Total_Length;
        guint16 LinkType ;  /* major version number */
        guint16 Reserved;  /* minor version number */
        guint32 SnapLen;
} pcapng_idb_t;

/*
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
0 |                    Block Type = 0x00000006                    |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
4 |                      Block Total Length                       |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
8 |                         Interface ID                          |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
12 |                        Timestamp (High)                       |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
16 |                        Timestamp (Low)                        |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
20 |                    Captured Packet Length                     |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
24 |                    Original Packet Length                     |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
28 /                                                               /
  /                          Packet Data                          /
  /              variable length, padded to 32 bits               /
  /                                                               /
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  /                                                               /
  /                      Options (variable)                       /
  /                                                               /
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                      Block Total Length                       |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

typedef struct pcapng_epb_s {
	   guint32 Block_Type;
	   guint32 Block_Total_Length;
    guint32 IfID ;  /* major version number */
    guint32 TsH;  /* minor version number */
    guint32 TsL;  /* minor version number */
    guint32 CaptPackLen;
    guint32 OrigPackLen;
} pcapng_epb_t;
using namespace broker;


pcapng_shb_t shb;
pcapng_idb_t idb;
pcapng_epb_t epb;


void signalHandler( int signum ) {
   std::cout << "Interrupt signal (" << signum << ") received.\n";

   // cleanup and close up stuff here
   // terminate program
fclose(dest);
   exit(signum);
}

void pcpng(char *filename)
{
/* standard option processing */

option_t option;
	guint32 value;



shb.Block_Type = 0x0A0D0D0A;
shb.Byte_Order_Magic=0x1A2B3C4D;
shb.Major_Version=1;
shb.Minor_Version=0;
shb.Section_Length=0xFFFFFFFFFFFFFFFF;
shb.Block_Total_Length= 28; /* 28 + Options len */


idb.Block_Type =0x00000001;
idb.Block_Total_Length= 20;
idb.LinkType=147;
idb.Reserved=0;
idb.SnapLen=0;





char buffer[256];
char inputfile[256];



dest=fopen(filename,"w+");
    if(dest==NULL)
        {
            perror("cannot open output file\n");
            exit(1);
        }
    /* write pcap header*/
  //  printf("Hello world!\n");


fwrite(&shb,(24),1,dest);
fwrite(&shb.Block_Total_Length,(4),1,dest);
idb.Block_Total_Length= 28;
fwrite(&idb,(16),1,dest);
	option.OpCode=9;
	option.OpLen=1;
  fwrite(&option,4,1,dest);
  value=9;
  fwrite(&value,4,1,dest);
//    option.OpCode=0;
//	option.OpLen=0;
// fwrite(&option,4,1,dest);
fwrite(&idb.Block_Total_Length,(4),1,dest);
epb.Block_Type=0x00000006;
epb.IfID=0;
}

static bool             crc_tab16_init          = false;
static uint16_t         crc_tab16[256];

static void init_crc16_tab( void ) {

	uint16_t i;
	uint16_t j;
	uint16_t crc;
	uint16_t c;

	for (i=0; i<256; i++) {

		crc = 0;
		c   = i;

		for (j=0; j<8; j++) {

			if ( (crc ^ c) & 0x0001 ) crc = ( crc >> 1 ) ^ CRC_POLY_16;
			else                      crc =   crc >> 1;

			c = c >> 1;
		}

		crc_tab16[i] = crc;
	}

	crc_tab16_init = true;

}  /* init_crc16_tab */

uint16_t crc_modbus( const unsigned char *input_str, size_t num_bytes ) {

	uint16_t crc;
	uint16_t tmp;
	uint16_t short_c;
	const unsigned char *ptr;
	size_t a;

	if ( ! crc_tab16_init ) init_crc16_tab();

	crc = CRC_START_MODBUS;
	ptr = input_str;

	if ( ptr != NULL ) for (a=0; a<num_bytes; a++) {

		short_c = 0x00ff & (uint16_t) *ptr;
		tmp     =  crc       ^ short_c;
		crc     = (crc >> 8) ^ crc_tab16[ tmp & 0xff ];

		ptr++;
	}

	return crc;

}  /* crc_modbus */

/* command line options   */

struct arguments
{

  char *broker;
  char *logger;
  char *queue;    
  char *good_topic;
  char *bad_topic;            /* arg1 & arg2 */
  char *file_name;
  bool pcap ;
  bool pub;
};
struct arguments arguments;

const char *argp_program_version =   "modbus_rtu_publisher - 1.0";
const char *argp_program_bug_address =   "<stephane.mocanu@imag.fr>";

/* Program documentation. */
static char doc[] =   "Pcap converter and Zeek Broker Publisher of Modbus frames captured by a probe. Checks Frame CRC";

/* A description of the arguments we accept. */
static char args_doc[] = "-p PUB -s GBRO -l BBRO -g GOOD -b BAD -w PCAP -f FILE -q QUEUE";

/* The options we understand. */
static struct argp_option options[] = {
  {"verbose",  'v', 0,      0,  "Produce verbose output" },
  {"publish", 'p', "PUB", 0, "Enables publisher y/n, default y"},
  {"server",	's', "GBRO", 0 ,  "Broker  address for good CRC messages publishing, default 127.0.0.1:9999"},
  {"log",	's', "BBRO", 0 ,  "Broker  address for bad CRC messages publishing, default 127.0.0.1:9999"},
  {"good_topic",    'g', "GOOD", 0 ,  "Topic name for good CRC messages publishing, default /modbus_rtu_good"},
  {"bad_topic",    'b', "BAD", 0 ,  "Topic name for bad CRC messages publishing, default /modbus_rtu_bad"},
  {"wireshark", 'w', "PCAP", 0, "Enables pcapng conversion y/n, default y"},
  {"file", 'f', "FILE", 0, "File name for pcapng conversion, default output.pcapng"},
  {"queue",   'q', "QUEUE", 0 ,  "Message queue for incoming captured Modbus frames from the probe. Has to be the same as for the probe and shall exist. Default ./msg " },
  { 0 }
};

/* Used by main to communicate with parse_opt. */


/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = (struct arguments * ) state->input;

  switch (key)
    {
    case 'q':
      arguments->queue = arg;
      break;
    case 'p':
        if ((arg[0]!='y')&&(arg[0]!='n'))
        {
        printf("invalid PUB choice y/n \n");
      	exit(1);
        }
      arguments->pub = (arg[0]=='y');
      break;
      case 'w':
        if ((arg[0]!='y')&&(arg[0]!='n'))
        {
        printf("invalid pcap conversion choice y/n \n");
      	exit(1);
        }
      arguments->pcap = (arg[0]=='y');
      break;
    case 's':
      arguments->broker = (arg);
      break;
      case 'l':
      arguments->logger = (arg);
      break;
      case 'g':
      arguments->good_topic = (arg);
      break;
      case 'b':
      arguments->bad_topic = (arg);
      break;
      case 'f':
      arguments->file_name = (arg);
      break;
    

    case ARGP_KEY_ARG:
      if (state->arg_num > 0)
        /* Too many arguments. */
        argp_usage (state);


      break;

    case ARGP_KEY_END:
//      if (state->arg_num < 2)
        /* Not enough arguments. */
//        argp_usage (state);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };




/* command line options   */


int main(int argc, char* argv[]) {
  signal(SIGINT, signalHandler);
  std::cout << "Modbus_RTU publisher";
  unsigned int byte_size,speed;
  key_t key;
  int flags, msqid;
  FILE *p;
  struct mbuf msg;


arguments.broker=(char *)malloc(1024);
arguments.logger=(char *)malloc(1024);
arguments.good_topic=(char *)malloc(1024);
arguments.bad_topic=(char *)malloc(1024);
arguments.file_name=(char *)malloc(1024);;
arguments.queue=(char *)malloc(1024);;


strcpy(arguments.broker,"127.0.0.1:9999");
strcpy(arguments.logger,"127.0.0.1:9999");
arguments.pub=true;
arguments.pcap=true;
strcpy(arguments.good_topic,"/modbus_rtu_good");
strcpy(arguments.bad_topic,"/modbus_rtu_bad");
strcpy(arguments.file_name,"./output.pcapng");
strcpy(arguments.queue,"./msg");
   /* Parse our arguments; every option seen by parse_opt will
      be reflected in arguments. */
   argp_parse (&argp, argc, argv, 0, 0, &arguments);


 p=fopen(arguments.queue,"w+");
//      printf("msg.type size : %ld\n",sizeof(msg.mtype));
  if(p==NULL)
  perror("fopen'");
  if((key=ftok(arguments.queue,'x'))==-1)
  {
    perror("ftok\n");
    exit(1);
  }

  printf("queue key %d\n", key);
  msqid = msgget(key, IPC_CREAT |0666);
  if(msqid==-1)
  perror("queue creation");

printf("Reading Modbus frames from %s message queue, key %d\n", arguments.queue, key);
if(arguments.pub)
{
printf("Publishing good CRC Moodbus Frames on %s topic %s\n",arguments.broker,arguments.good_topic);
printf("Publishing bad CRC Moodbus Frames on %s topic %s\n",arguments.logger,arguments.bad_topic);
}
if(arguments.pcap)
{
    printf("Exporting Modbus frames to %s\n",arguments.file_name);
    pcpng(arguments.file_name);
}





	
struct broker_options opt1;   // Setup endpoint and connect to Zeek.
	opt1=broker_options();
//	opt1.disable_ssl=true;
configuration conf=broker::configuration(opt1);
//conf=configuration::configuration(opt1);
endpoint ep_good=broker::endpoint(broker::configuration(opt1));
endpoint ep_bad=broker::endpoint(broker::configuration(opt1));

//ep.options.disable_ssl=true;






if(arguments.pub)
{
    char *b_ip=(char*)malloc(1024);
    char *b_port=(char*)malloc(1024);
    char *l_ip=(char*)malloc(1024);
    char *l_port=(char*)malloc(1024);
   
    b_port=(strchr(arguments.broker,':')+1);
    
    l_port=strchr(arguments.logger,':')+1;
     b_ip=strtok(arguments.broker,":");
    l_ip=strtok(arguments.logger,":");
    ep_good.peer(b_ip,atoi(b_port));
    ep_bad.peer(l_ip,atoi(l_port));
}


// Wait until connection is established.

uint16_t crc_mb;
timespec ts;
  while(1){

  int diag=msgrcv(msqid,&msg, 1024  ,101, 0);
  /*
  printf("diag rcv %d ",diag-24);
  for(int j=0;j<diag-24;j++)
  printf("%2x ",(unsigned char)(msg.mtext+16+j)[0]);
 printf("\n"); */
  if(diag==-1)
  perror("qrecv");

if(arguments.pcap)
{
  unsigned long TsH;
  unsigned long TsL;
   memcpy(&TsH,msg.mtext,8);
  memcpy(&TsL,msg.mtext+8,8);
    unsigned long time=TsH*1000000000+TsL;
	epb.TsH=time>>32;
  epb.TsL=(time&0x00000000FFFFFFFF);
  epb.CaptPackLen=diag-24;
  epb.OrigPackLen=diag-24;
  int WrBytes=(diag-24)/4;
  if (((diag-24)%4)>0) WrBytes++;
  epb.Block_Total_Length=32+WrBytes*4;
  fwrite(&epb,28,1,dest);
  fwrite(msg.mtext+16,WrBytes*4,1,dest);
  int dig=fwrite(&epb.Block_Total_Length,4,1,dest);
}

if(arguments.pub)
{
 memcpy(&ts,msg.mtext,16);

crc_mb= crc_modbus( (unsigned char*)(msg.mtext+16), diag-24 );
// printf("%x \n",crc_mb );
vector packet;
  auto duration = std::chrono::seconds{ts.tv_sec}   + std::chrono::nanoseconds{ts.tv_nsec};
 // broker::timestamp tp= std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>{std::chrono::duration_cast<system_clock::duration>(duration)};
broker::timestamp tp= timestamp(duration); 
packet.push_back(tp);
printf("diag %d\n", diag);
for(int j=0;j<diag-24;j++)
packet.push_back((unsigned char)(msg.mtext+16+j)[0]);    
        // Send event "ping(n)".
        if(crc_mb==0){
        printf("good\n");
        zeek::Event mbusrtu("modbus_good",packet);
        ep_good.publish(arguments.good_topic, mbusrtu);
        }
        else
        {
        printf("bad\n");
        zeek::Event mbusrtu("modbus_bad",packet);
        ep_good.publish(arguments.bad_topic, mbusrtu);
        }

}   
   
}
 return 0;
}
