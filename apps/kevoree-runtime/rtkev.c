#include "rtkev.h"
#include "lib/list.h"

#include <stdarg.h>

/**
 * The Kevoree runtime for contiki includes a set of processes
 * that perform different tasks.
 *
 * Of course, it also includes several datastructure to represent the internal runtime
 * state.
 *
 */

struct TypeEntry {
	struct TypeEntry* next;
	ComponentInterface* interface;
};

static struct Runtime {
    /* the current model */
    /* hash_map from type name to type definition */
	LIST_STRUCT(types);
} runtime;

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
PROCESS_THREAD(kev_reg, ev, data)
{
	ComponentInterface* p;
    PROCESS_BEGIN();

	printf("Ejecutando proceso kev_reg\n");

	/* register new event type */
	NEW_KEV_TYPE = process_alloc_event(); 

    while (1) {
        /* it runs forever, waiting for a request of new type */
		PROCESS_WAIT_EVENT();
		if (ev == NEW_KEV_TYPE) {
			p = (ComponentInterface*) data;
			
			struct TypeEntry* entry = (struct TypeEntry*)malloc(sizeof(struct TypeEntry));
			entry->interface = p;

			list_add(runtime.types, entry);			

			printf("Hey, a new type is being reported. Its name is %s\n", (char*)p->name);
		}
    }

    PROCESS_END();
}

PROCESS(kev_model_listener, "kev_model_listener");
PROCESS_THREAD(kev_model_listener, ev, data)
{
    PROCESS_BEGIN();

	printf("Ejecutando proceso kev_model_listener\n");

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

int initKevRuntime()
{

	LIST_STRUCT_INIT(&runtime, types);

	process_start(&kev_reg, NULL);
	process_start(&kev_model_listener, NULL);
	return 0;
}

/* register component type */
int registerComponent(int count, ... )
{
	va_list ap;
	ComponentInterface* interface;
	
	/* this is here for debug, when you deploy an example which is a component 
	 * you must start the runtime somehow	
	*/	
	initKevRuntime();

	/* get the arguments */
	va_start(ap, count);

	/* iterate to register arguments */
	while (count) {
		interface = va_arg(ap, ComponentInterface*);
		count--;

		printf("En registrar componente %s %p\n", interface->name, interface);
		
		/* it essentially sends a message to the process kev_reg
	 	well, I am guessing everything is Ok if I can send the message, :-) */
		
		process_post(&kev_reg, NEW_KEV_TYPE, interface);
	}

	/* done with the variadic arguments*/
	va_end(ap);

	return 0; 
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

/* create an instance of some type */
int createInstance(char* typeName, char* instanceName, void** instance)
{
	struct TypeEntry* entry;
	/* iterate through list of ComponentInterface */
	for(entry = list_head(runtime.types);
      entry != NULL;
      entry = list_item_next(entry)) {
		if (!strcmp(typeName, entry->interface->name)) {
			printf("\tType Found\n");
			*instance = entry->interface->newInstance(entry->interface->name);
		}
  }
}
