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

/* kevoree event types */
static process_event_t NEW_KEV_TYPE;
static process_event_t NEW_MODEL;

/* internal structures used to send data along events */
typedef struct {
	void* first;
	void* second;
} Pair;

/* this process is in charge of registering types */
PROCESS(kev_reg, "kev_reg");
AUTOSTART_PROCESSES(&kev_reg);

PROCESS_THREAD(kev_reg, ev, data)
{
	Pair* p;
    PROCESS_BEGIN();

	/* register new event type */
	NEW_KEV_TYPE = process_alloc_event(); 

    while (1) {
        /* it runs forever, waiting for a request of new type */
		PROCESS_WAIT_EVENT();
		if (ev == NEW_KEV_TYPE) {
			p = (Pair*) data;
			
			printf("Hey, a new type is being reported. Its name is %s\n", (char*)p->first);

			free (p);	
		}
    }

    PROCESS_END();
}

PROCESS(kev_model_listener, "kev_model_listener");
AUTOSTART_PROCESSES(&kev_model_listener);
PROCESS_THREAD(kev_model_listener, ev, data)
{
    PROCESS_BEGIN();

	/* register new event type */
	NEW_MODEL = process_alloc_event();

    while (1) {
        /* it runs forever, waiting for some update to the model */
		PROCESS_WAIT_EVENT();
		if (ev == NEW_MODEL) {

			/* wow I ave a new model, do te magic with the traces and so on */
			printf("Here a new model is coming\n");
		
		}
    }

    PROCESS_END();
}

/* register component type */
int registerComponent(const char* name, ComponentInterface* interface)
{
	/* it essentially sends a message to the process kev_reg
	 well, I am guessing everything is Ok if I can send the message, :-) */
	Pair* pair = (Pair*)malloc(sizeof(Pair));
	pair->first = (void*)name;
	pair->second = interface;

	return process_post(&kev_reg, NEW_KEV_TYPE, pair); 
}

/* Notify about a new model, normally this will be mostly used Groups.
 * However, it is also available for "smart"components.
 * it receive a ContainerRoot as parameter
 * */
int notifyNewModel(/*ContainerRoot* model*/)
{
	// it essentially sends a message to the process kev_model_listener
	// well, I am guessing everything is Ok, :-)
	return process_post(&kev_model_listener, NEW_MODEL, NULL);
}
