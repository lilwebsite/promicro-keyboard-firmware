#define __USER // comment this out if you don't want to use any user functions
#ifdef __USER
void functions(void);

#ifdef ENABLE_STDBY
static uint8_t standby = 0;
#endif

