#define LENGTH_FUNCTION 25
#if defined(DEBUG) && DEBUG > 0
#define dp(fmt, args...) fprintf(stderr, "DEBUG: %d: %-25s(): " fmt, \
    __LINE__, __FUNCTION__, ##args)
#else
 #define dp(fmt, args...) /* Don't do anything in release builds */
#endif