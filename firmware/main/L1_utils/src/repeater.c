#include "repeater.h"
// Framework libraries
#include "rom/ets_sys.h"



bool repeater_execute(const repeat_S * const repeater)
{
    bool success = false;

    if (repeater && repeater->callback)
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
                if (taskSCHEDULER_RUNNING == xTaskGetSchedulerState())
                {
                    DELAY_MS(repeater->delay_ms);
                }
                else
                {
                    ets_delay_us(repeater->delay_ms * 1000U);
                }
            }
        }
    }

    return success;
}
