/*
 * note: our client and server must agree upon this structure!
 */

#define KILL_SERVER	0
#define	GET_DATE	1
#define GET_TIME	2
#define GET_RND_NUM	3
#define ADD_VALUES	4
#define TERM_SERVER	5
#define TERM_CLIENT	6

#define CLIENT_2_MSG	1

typedef struct tagSAMPLEMESSAGE
{
	int message_command;
	int data1;
	int data2;
	int result;
	char resultstring[256];
} SAMPLEMESSAGE;


typedef struct tagMSGENVELOPE 
{
	long type;
	SAMPLEMESSAGE data;
} MSGENVELOPE;

#define TRUE  1
#define FALSE 0
