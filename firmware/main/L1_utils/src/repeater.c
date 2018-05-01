#include "repeater.h"



bool repeater_execute(const repeat_S * const repeater)
{
    bool success = false;

    if (repeater)
    {
        for (uint32_t r = 0; r < repeater->num_retries; r++)
        {
            success = repeater->callback();
            if (success)
            {
                break;
            }
            else
            {
                DELAY_MS(repeater->delay_ms);
            }
        }
    }

    return success;
}
