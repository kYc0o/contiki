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
    /* the current model */
    /* hash_map from type name to type definition */
};

/* this process is in charge of registering types */
PROCESS(kev_reg, "kev_reg");
PROCESS_THREAD(kev_reg, ev, data)
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

/* register component type */
int registerComponent(const char* name, ComponentInterface* interface)
{
	// it essentially sends a message to the process kev_reg
	return 0; // well, I am guessing everything is Ok, :-)
}

/* Notify about a new model, normally this will be mostly used Groups.
 * However, it is also available for "smart"components.
 * it receive a ContainerRoot as parameter
 * */
int notifyNewModel(/*ContainerRoot* model*/)
{
	// it essentially sends a message to the process kev_model_listener
	return 0; // well, I am guessing everything is Ok, :-)
}
