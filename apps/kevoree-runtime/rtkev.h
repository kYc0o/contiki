#ifndef __RT_KEV__
#define __RT_KEV__

#include "contiki.h"

typedef struct _ContainerRoot ContainerRoot;

enum InterfaceType {
	ComponentInstanceInterface,
	GroupInstanceInterface
};

/* protos to handle kevoree instances */
typedef void* (*NewInstanceCallBack)(const char* kevType);
typedef int (*StartInstanceCallBack)(void*);
typedef int (*StopInstanceCallBack)(void*);
typedef int (*UpdateInstanceCallBack)(void*);

#define KEV_INSTANCE_FIELDS \
	enum InterfaceType interfaceType; \
	const char* name; \
    NewInstanceCallBack newInstance; \
    StartInstanceCallBack start; \
    StopInstanceCallBack stop; \
    UpdateInstanceCallBack update;

/* generic definition if kevoree types (components, channels, groups ans so on) */
typedef struct {
	KEV_INSTANCE_FIELDS
} KevInterface;

/* 
 * Each component type must define a variable of this type.
 * It describes how the Kevoree Runtime interacts with this component type.
 * */
typedef struct {
	KEV_INSTANCE_FIELDS
} ComponentInterface;

/* protos to handle Group instances */
typedef int (*SendModelCallBack)(void*, ContainerRoot*);

/*
 * Each group type must define a variable of this type
 */
typedef struct {
	KEV_INSTANCE_FIELDS
	SendModelCallBack send;
} GroupInterface;

typedef struct {
	int (*getDeployUnit)(const char *);
} DeployUnitRetriver;

/* represent the context of a Kevoree Instance, observe that it is an opaque type */
typedef struct _KevContext KevContext; 

/** Get the context of an instance */
KevContext* getContext(void* instance);

/* functionn to deal with the context */
const char* getInstanceName(KevContext* context);
const char* getDictionaryAttributeValue(KevContext* context, const char* att);

/*
 * The runime offers many functions to components, channels, groups and nodes
 * */

/* Error codes used by the runtime */
#define ERR_KEV_REGISTRATION_FAIL ((int)-1)
#define ERR_KEV_UNKNOWN_TYPE ((int)-2)
#define ERR_KEV_INIT_FAILURE ((int) -3)
#define ERR_KEV_INSTANCE_CREATION_FAIL ((int) -4)

/* init runtime */
int initKevRuntime(const DeployUnitRetriver* retriever);

/* register component type */
int registerComponent(int count, ... );
int unregisterComponent(const char* name);

/* Notify about a new model, normally this will be mostly used Groups.
 * However, it is also available for "smart"components.
 * it receive a ContainerRoot as parameter
 * */
int notifyNewModel(ContainerRoot *model);

/* create an instance of some type */
int createInstance(char* typeName, char* instanceName, void** instance);
int removeInstance(const char* instanceName);
int startInstance(const char* instanceName);
int stopInstance(const char* instanceName);

/* dealing with deploy units */
void notifyDeployUnitDownloaded(const char*);

/* loads an elf file into the system and executes its autostart processes */
void loadElfFile(const char* filename);

/* macros to register a component */
#define REGISTER_KEV_TYPES(DEPLOY_UNIT_ID, COMP_COUNT, ...) \
PROCESS(PReg##DEPLOY_UNIT_ID,"ProcessToRegisterComponent"); \
AUTOSTART_PROCESSES(&PReg##DEPLOY_UNIT_ID); \
PROCESS_THREAD(PReg##DEPLOY_UNIT_ID, ev, data) \
{ \
    PROCESS_BEGIN(); \
    registerComponent(COMP_COUNT, __VA_ARGS__); \
    PROCESS_END(); \
}

#define DECLARE_KEV_TYPES(COMP_COUNT, ...) \
PROCESS(PReg,"ProcessToRegisterComponent"); \
PROCESS_THREAD(PReg, ev, data) \
{ \
    PROCESS_BEGIN(); \
    registerComponent(COMP_COUNT, __VA_ARGS__); \
    PROCESS_END(); \
}

#define REGISTER_KEV_TYPES_NOW() process_start(&PReg, NULL)

#endif
