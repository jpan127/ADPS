#include "leaky_bucket.h"



void lbucket_leak(leaky_bucket_S * const lbucket)
{
    if (lbucket && lbucket->current_level > 0)
    {
        // If almost empty, then set to 0 to watch out for underflow
        if (lbucket->output_rate >= lbucket->current_level)
        {
            lbucket->current_level = 0;
        }
        // Otherwise, fill bucket
        else
        {
            lbucket->current_level -= lbucket->output_rate;
        }
    }
}

void lbucket_fill(leaky_bucket_S * const lbucket)
{
    if (lbucket && lbucket->current_level < lbucket->max_level)
    {
        // Only fill if not already over max level
        if (lbucket->current_level <= lbucket->max_level)
        {
            lbucket->current_level += lbucket->input_rate;
        }
    }
}

bool lbucket_is_empty(leaky_bucket_S * const lbucket)
{
    return (0 == lbucket->current_level);
}

bool lbucket_is_full(leaky_bucket_S * const lbucket)
{
    return (lbucket->current_level >= lbucket->max_level);
}
