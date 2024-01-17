#include "logger.h"
#include <linux/kernel.h>

void logger(LOG_LEVEL level, const char* module, const char* msg)
{
    switch (level)
    {
    case INFO:
    case ERROR:
    case CRITICAL:
        printk("%s: %s\n", module, msg);
        break;
    default:
        break;
    }
}

void logger_info(const char* module, const char* msg)
{
    logger(INFO, module, msg);
}
void logger_error(const char* module, const char* msg)
{
    logger(ERROR, module, msg);

}
void logger_critical(const char* module, const char* msg)
{
    logger(CRITICAL, module, msg);
}



