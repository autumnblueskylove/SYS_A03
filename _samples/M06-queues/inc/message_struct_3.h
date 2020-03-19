/*
 * note: our client and server must agree upon this structure!
 */

typedef struct tagSAMPLEMESSAGE
{
	pid_t requestingClient;
	int jobNumber;
	int result;
} SAMPLEMESSAGE;


typedef struct tagMSGENVELOPE 
{
	long type;
	SAMPLEMESSAGE data;
} MSGENVELOPE;

#define TRUE  1
#define FALSE 0

#define LOW_PRIORITY	20
#define MEDIUM_PRIORITY	40
#define HIGH_PRIORITY	60

#define LOW_CHOICE	1
#define MED_CHOICE	2
#define HIGH_CHOICE	3
#define RESP_CHOICE	4
#define KILL_CLIENT	5
#define KILL_SERVER	5

#define SEND	1
#define RECEIVE	2


