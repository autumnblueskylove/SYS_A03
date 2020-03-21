#define DEBUG 1
#if defined(DEBUG) && DEBUG > 0
#define dp(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
    __FILE__, __LINE__, __func__, ##args)
#else
 #define dp(fmt, args...) /* Don't do anything in release builds */
#endif