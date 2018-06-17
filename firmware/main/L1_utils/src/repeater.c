#include "repeater.h"



bool repeater_execute(const repeat_S * const repeater)
{
    bool success = false;

    if (repeater)
    {
        for (uint32_t r = 0; r < repeater->num_retries; r++)
        {
            // Call the callback
            success = repeater->callback();

            // Any success immediately exits
            if (success)
            {
                break;
            }
            // If failed, wait a bit and try again
            else
            {
                DELAY_MS(repeater->delay_ms);
            }
        }
    }

    return success;
}
