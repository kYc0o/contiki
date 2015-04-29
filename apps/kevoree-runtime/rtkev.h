#ifndef __RT_KEV__
#define __RT_KEV__

#include "contiki.h"

/* proto to handle component instances */
typedef void* (*NewComponentInstance)(const char* componentType);
typedef int (*StartComponent)(void*);
typedef int (*StopComponent)(void*);
typedef int (*UpdateComponent)(void*);

/* Each component type must define a variable of this type.
 * It describes how the Kevoree Runtime interacts with this component type.
 * */
typedef struct {
    NewComponentInstance newInstance;
    StartComponent start;
    StopComponent stop;
    UpdateComponent update;
} ComponentInterface;

/*
 * The runime offers many functions to components, channels, groups and nodes
 * */

/* Error codes used by the runtime */
#define ERR_KEV_REGISTRATION_FAIL ((int)-1)
#define ERR_KEV_UNKNOWN_TYPE ((int)-2)

/* init runtime */
int initKevRuntime();

/* register component type */
int registerComponent(int count, ... );
int unregisterComponent(const char* name);

/* Notify about a new model, normally this will be mostly used Groups.
 * However, it is also available for "smart"components.
 * it receive a ContainerRoot as parameter
 * */
int notifyNewModel(/*ContainerRoot* model*/);

/* macro to register a component */
#define REGISTER_COMPONENT(COMP_COUNT, ...) \
PROCESS(PRegister,"ProcessToRegisterComponent"); \
AUTOSTART_PROCESSES(&PRegister); \
PROCESS_THREAD(PRegister, ev, data) \
{ \
    PROCESS_BEGIN(); \
    registerComponent(COMP_COUNT, __VA_ARGS__); \
    PROCESS_END(); \
}

#endif
