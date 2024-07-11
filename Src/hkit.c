#include "hkit.h"
#include "hstr.h"

void (*error_handler)(HError err) = HKIT_DefaultErrorHandler;

void HKIT_Init(void (*err_handler)(HError err))
{
    error_handler = err_handler;
}

void HKIT_TriggerError(HError err)
{
    if (error_handler != null) error_handler(err);
}

void HKIT_DefaultErrorHandler(HError err)
{
#ifdef ENABLE_HDEBUG
    if (HDEBUG_IsAvailable()) {
        HDEBUG_PrintCRLF();
        HDEBUG_Println("[HKit] Error Occurs:");
        HDEBUG_PrintError(err);
        HDEBUG_PrintCRLF();
    }
#endif
}