/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2017
	 *
 * serial_modbus is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
	 *
 * serial_modbus is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define _GNU_SOURCE


#include <stdio.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/epoll.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <argp.h>
#include <malloc.h>

#define STATE_WAIT  1
#define STATE_READ  2
#define STATE_TIMING  3
#define STATE_WAIT_EOF  4
#define STATE_ERROR 5
#define TIMER_SIG SIGRTMAX-1

extern int errno;
struct termios ttysetting;   /* structure for tty-setting */
FILE *p;
int Device_fd;
int hasdata;
int state, once,msqid;
unsigned char frame[1024];
unsigned short i,j;
struct itimerspec ts_r,ts_15, ts_35,ts_15o,ts_35o;
timer_t tidlist[2];
struct timespec res1,res2;

struct mbuf {
long mtype;
char mtext[1024];
};
struct mbuf msg;

struct arguments
{

  char *tty;
  char *queue;                /* arg1 & arg2 */
  int speed;
  char code;
  char stop;
  char par;

};
struct arguments arguments;


/*
inter frame delay (t3.5) and inter char delay (t1.5)

1 Byte = start + 8 bits + parity + stop = 11 bits

Baud rate t 1.5 (16 bits) t 3.5 (38 bits)
1200 13333.3 us 31666.7 us
4800 3333.3 us 7916.7 us
9600 1666.7 us 3958.3 us
19200 833.3 us 1979.2 us
38400 416.7 us 989.6 Us
*/

static void intHandler(int dummy) {

close(Device_fd);
fprintf(stderr, "Poll closed exiting\n");
exit(0);

}



void Init_Device(struct arguments* arguments)
{


	if ((Device_fd = open( arguments->tty, O_RDWR|O_NDELAY)) == -1)
	{perror( "Error openopening serial port:"); exit(1);}

	tcflush(Device_fd, TCIOFLUSH);

	if (tcgetattr(Device_fd, &ttysetting)==-1)
	{perror( "ioctl() erreur:"); exit(1);}

if(arguments->par!='n')
{
	ttysetting.c_iflag = INPCK|IGNBRK;
	ttysetting.c_cflag =CREAD|HUPCL|CLOCAL|PARENB;
	if(arguments->par=='o')
		ttysetting.c_cflag|=PARODD;
}
	else
	{
	ttysetting.c_iflag = IGNBRK;
	ttysetting.c_cflag =CREAD|HUPCL|CLOCAL;
	}
	ttysetting.c_lflag = 0;
	ttysetting.c_oflag = 0;
	if(arguments->code==8)
	ttysetting.c_cflag |= CS8;
	else
	ttysetting.c_cflag |= CS7;
	if(arguments->stop==2)
	ttysetting.c_cflag |= CSTOPB;
	ttysetting.c_cc[VMIN ] = 0;
	ttysetting.c_cc[VTIME] = 0;

	switch(arguments->speed)
	{
	case 200 :
		cfsetspeed(&ttysetting, B200);
		break;
	case 300 :
			cfsetspeed(&ttysetting, B300);
		break;
	case 600 :
				cfsetspeed(&ttysetting, B600);
		break;
	case 1200 :
						cfsetspeed(&ttysetting, B1200);
		break;
	case 2400 :
		cfsetspeed(&ttysetting, B2400);
		break;
	case 4800 :
		cfsetspeed(&ttysetting, B4800);
		break;
	case 9600 :
		cfsetspeed(&ttysetting, B9600);
		break;
	case 19200 :
		cfsetspeed(&ttysetting, B19200);
		break;
	}

//	for(int k=0;k<NCCS;k++)
//	printf("c_ctrl%d %x\n",k, ttysetting.c_cc[k]);

	tcflush(Device_fd, TCIOFLUSH);
	if ((tcsetattr(Device_fd, TCSANOW, &ttysetting))==-1)
	{perror( "ioctl() set erreur:"); exit(1);}

	if (tcgetattr(Device_fd, &ttysetting)==-1)
	{perror( "ioctl() erreur:"); exit(1);}

	clock_getres(CLOCK_REALTIME , &res1);
};

struct timespec tp;


static void t_handler(int sig, siginfo_t *si, void *uc){

	if(si->si_value.sival_int==0)   /* T1.5 fired wait end of frame */
	{
		timer_settime(tidlist[0], 0, &ts_r,&ts_15o);  /* reset T1.5*/
		state=STATE_WAIT_EOF;
	}
	else  /* end of frame received */
	{
		timer_settime(tidlist[1], 0, &ts_r,&ts_35o); /* reset T3.5*/
		//printf("%ld\n",msg.mtype);
		// printf("%s\n",msg.mtext+16);
		if(msgsnd(msqid,&msg, sizeof(long)+j, IPC_NOWAIT)==-1)
		{perror("msg send");
		// exit(1);
				}
		// printf("%ld\n",msg.mtype);
		// printf("%s\n",msg.mtext+16);
		state =STATE_WAIT;

	}

}

static void intdata (int dummy)
{
if (state==STATE_WAIT)
	clock_gettime(CLOCK_REALTIME, (struct timespec*)(msg.mtext));
	hasdata=1;
}




const char *argp_program_version =   "modbus_rtu_probe - 1.0";
const char *argp_program_bug_address =   "<stephane.mocanu@imag.fr>";

/* Program documentation. */
static char doc[] =   "Probe for Modbus RTU network. Timing-based frame detection";

/* A description of the arguments we accept. */
static char args_doc[] = "-c CONF -p TTY -q QUEUE -s SPEED";

/* The options we understand. */
static struct argp_option options[] = {
  {"verbose",  'v', 0,      0,  "Produce verbose output" },
  {"port",	'p', "TTY", 0 ,  "Serial port pseudofile : /dev/ttyS0, /dev/ttyUSB0 and so, default /dev/tty/USB0 "},
  {"speed",    's', "SPEED", 0 ,  "Serial port I/O speed : 2600 4800 9600 119200, default 119200"},
  {"conf",	'c', "CONF", 0 , "Character encoding and parity : encoding bits parity stop bits : like 8n1, default 8e1"},
  {"queue",   'q', "QUEUE", 0 ,  "Message queue for captured Modbus frames. It will be created if it not exists. Default ./msg " },
  { 0 }
};

/* Used by main to communicate with parse_opt. */


/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;

  switch (key)
    {
   case 'q':
		 arguments->tty=realloc(arguments->queue,strlen(arg));
		 memcpy(arguments->queue, arg, strlen(arg));
      break;
    case 'p':
		arguments->tty=realloc(arguments->tty,strlen(arg));
		 memcpy(arguments->tty, arg, strlen(arg));
      break;
    case 's':
      arguments->speed = atoi(arg);
      if((arguments->speed!=200) &&(arguments->speed!=300) &&(arguments->speed!=600) &&(arguments->speed!=1200) && (arguments->speed!=2400)&& (arguments->speed!=4800)&& (arguments->speed!=9600)&& (arguments->speed!=19200)&& (arguments->speed!=38400))
      	{
      	printf("invalid speed coding: only 200 300 600 1200 2400 4800 9600 19200 and 38400 are allowed\n");
      	exit(1);
      	}
      break;
    case 'c':
      arguments->code = atoi(arg);
      	if((arguments->code!=8) && (arguments->code!=7))
      	{
      	printf("invalid character coding: only 7 and 8 are allowed\n");
      	exit(1);
      	}

      arguments->stop = atoi(arg+2);
      if((arguments->stop!=1) && (arguments->stop!=2))
      	{
      	printf("invalid stop bits number: only 1 and 2 are allowed\n");
      	exit(1);
      	}
      arguments->par = (arg[1]);
      if((arguments->par!='e') && (arguments->par!='o')&& (arguments->par!='n'))
      	{
      	printf("invalid stop bits number: only e o and n are allowed\n");
      	exit(1);
      	}
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

int main(int argc, char* argv[])
{


	signal(SIGINT, intHandler);
	signal(SIGIO, intdata);

	printf("Modbus RTU Probe\n");

	
   /* Default values. */
  /* Default values. */
  arguments.tty=malloc(strlen("/dev/ttyUSB0"));
  memcpy(arguments.tty,"/dev/ttyUSB0",strlen("/dev/ttyUSB0"));
  arguments.queue=malloc(strlen("./msg"));
  memcpy(arguments.queue,"./msg",strlen("./msg"));
  arguments.speed=19200;
  arguments.code=8;
  arguments.par='e';
  arguments.stop=1;

   

   /* Parse our arguments; every option seen by parse_opt will
      be reflected in arguments. */
   argp_parse (&argp, argc, argv, 0, 0, &arguments);





	unsigned int byte_size,speed;
  key_t key;
	int flags;
	struct sigaction  sa, sa_old;
	struct sigevent   sev;
  
 // p=fopen(arguments.queue,"w+");
    printf("ts : %ld\n",sizeof(tp));
    printf("msg.type size : %ld\n",sizeof(msg.mtype));
   if(p==NULL)
  perror("fopen'");
  key=ftok(arguments.queue,'x');
 // printf("message queue key %d\n", key);
  msqid = msgget(key, IPC_CREAT|0666);
  if(msqid==-1)
  perror("message queue creation error");
  msg.mtype=101;
struct msqid_ds buf; 
struct msginfo sysinfo;
  msgctl(msqid, IPC_STAT , &buf);
  msgctl(msqid, IPC_INFO , (struct msqid_ds *) &sysinfo);


// int diag=msgsnd(msqid,&msg, sizeof(long)+strlen("hello")  , IPC_NOWAIT);
// printf("diag qsend %d\n",diag);


	ts_r.it_interval.tv_sec=0;
	ts_r.it_interval.tv_nsec=0;
	ts_r.it_value.tv_sec=0;
	ts_r.it_value.tv_nsec=0;
	ts_15.it_interval.tv_sec=0;
	ts_15.it_interval.tv_nsec=0;
	ts_35.it_interval.tv_sec=0;
	ts_35.it_interval.tv_nsec=0;
	ts_15.it_value.tv_sec=0;
	ts_35.it_value.tv_sec=0;




	int interchar,interframe;
	speed=arguments.speed;
	if (arguments.par!='n')
		byte_size=arguments.code+arguments.stop+2;
	else
		byte_size=arguments.code+arguments.stop+1;
	Init_Device(&arguments);

// 19200 833.3 us 1979.2 us




	interchar=byte_size*(1500000000/speed);
	interframe=byte_size*(3500000000/speed);
//	interchar=833333;
//	interframe=1979200;


	ts_15.it_value.tv_nsec=interchar;
	ts_35.it_value.tv_nsec=interframe;
printf("Sniffing on %s at speed %d bps.\n", arguments.tty, arguments.speed);
printf("Character encoding %d%c%d \n", arguments.code,arguments.par,arguments.stop);
printf("Byte timing T1.5 : %d ns\n", interchar);
printf("Frame timing T3.5 : %d ns\n", interframe);
printf("Captured frames send to %s message queue, key %d\n", arguments.queue,key);

	/* Set owner process that is to receive "I/O possible" signal */
	if (fcntl(Device_fd, F_SETOWN, getpid()) == -1)
		perror ("fcntl(F_SETOWN)");
	/* Enable "I/O possible" signaling and make I/O nonblocking       for file descriptor */
	flags = fcntl(Device_fd, F_GETFL);
	if (fcntl(Device_fd, F_SETFL, flags | O_ASYNC | O_NONBLOCK) == -1)
		perror ("fcntl(F_SETFL)");



	   /* Establish handler for notification signal */
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = t_handler;
	sigemptyset(&sa.sa_mask);

	sev.sigev_notify = SIGEV_SIGNAL;    /* Notify via signal */
	sev.sigev_signo = TIMER_SIG;        /* Notify using this signal */
	if (sigaction(TIMER_SIG, &sa, &sa_old)==-1)
		perror("sigaction");
	sev.sigev_value.sival_int=0;
	if(timer_create(CLOCK_REALTIME, &sev, &tidlist[0])==-1)
		perror("timer0");
	  sev.sigev_value.sival_int=1;
	if(timer_create(CLOCK_REALTIME, &sev, &tidlist[1])==-1)
	   perror("timer1");




int size_ts=sizeof(struct timespec);

// initial state machine
	state =STATE_WAIT;
int in;
int unarm=1;
	j=size_ts;

	while(1)
	{
switch(state)
		{
case STATE_WAIT :  /* wait for input to arrive, all clean*/
	j=size_ts;
	timer_settime(tidlist[0], 0, &ts_r,&ts_15o);  /* unarm timers just in case*/
	timer_settime(tidlist[1], 0, &ts_r,&ts_35o);
	if(hasdata)          /* data arrived, go to read */
{	state =STATE_READ ;}
	break;
case STATE_READ :
	hasdata=0;

	 while ((in=read(Device_fd, &(msg.mtext[j]), 1)) > 0)   /* read all available data */
	j++;
	state=STATE_TIMING;  /* en of available data, migth be an end of frame or not*/
	unarm=1;			 /* go to check interchar and interframe timeouts*/
	break;
case STATE_TIMING :
	if(unarm)      /* arm timers T1.5 and T3.5 and wait */
	{
		unarm=0;
	timer_settime(tidlist[0], 0, &ts_15,&ts_15o);
	timer_settime(tidlist[1], 0, &ts_35,&ts_35o);
	}
	if(hasdata)   /* new data arrived before T1.5 fired */
   	{
	timer_settime(tidlist[0], 0, &ts_r,&ts_15o);  /* unarm timers */
	timer_settime(tidlist[1], 0, &ts_r,&ts_35o);
	unarm=1;
	state =STATE_READ ;  /* go back to read*/

	}
	break;
case STATE_WAIT_EOF :  /* T.1.5 fired, wait for end of frame or interchar error */
	timer_settime(tidlist[0], 0, &ts_r,&ts_15o);  /* disarm T1.5 */
	if(hasdata){  /* new data arrived interchar framing violation */
		timer_settime(tidlist[1], 0, &ts_r,&ts_35o);
		state=STATE_ERROR;  /* go to error */
		unarm=1;
	}
	break;
case STATE_ERROR :
	//	printf("ERROR\n");
	break;
}


	}
	return (0);
}
