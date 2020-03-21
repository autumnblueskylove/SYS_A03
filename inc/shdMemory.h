#define MAX_DC_ROLES                    10          // the maximum number of clients

typedef struct
{
    pid_t   dcProcessID;
    time_t  lastTimeHeardFrom;
} DCInfo;

typedef struct                          // related to a shared memory structure
{
    int     msgQueueID;
    int     numberOfDCs;
    DCInfo  dc[MAX_DC_ROLES];
} MasterList;