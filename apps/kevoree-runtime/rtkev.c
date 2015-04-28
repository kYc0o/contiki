#include "rtkev.h"

/**
 * The Kevoree runtime for contiki includes a set of processes
 * that perform different tasks.
 *
 * Of course, it also includes several datastructure to represent the internal runtime
 * state.
 *
 */

struct Runtime {
    /* hash_map from type name to type definition */
};

/* this process is in charge of registering types */
PROCESS(kev_reg, "kev_reg");
PROCESS_THREAD(kev_reg_component, ev, data)
{
    PROCESS_START();
    while (1) {
        // it runs forever, waiting for a request of new type
    }
    PROCESS_END();
}

PROCESS(kev_model_listener, "kev_model_listener");
PROCESS_THREAD(kev_model_listener, "kev_model_listener")
{
    PROCESS_START();
    while (1) {
        // it runs forever, waiting for some update to the model
    }
    PROCESS_END();
}
