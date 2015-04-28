#ifndef __RT_KEV__
#define __RT_KEV__

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
#define ERR_KEV_REGISTRATION_FAIL -1


/* register component type */
int registerComponent(const char* name, ComponentInterface* interface);

#endif
