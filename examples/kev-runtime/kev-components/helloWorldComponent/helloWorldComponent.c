#include "contiki.h"
#include "rtkev.h"
#include <stdio.h>

/* forward declaration */
static void* newHelloWorld(const char*);
static int startHelloWorld(void* instance);
static int stopHelloWorld(void* instance);
static int updateHelloWorld(void* instance);

static
const ComponentInterface helloWorldInterface = {
		.interfaceType = ComponentInstanceInterface,
		.name = "hello_world/0.0.1",
		.newInstance = newHelloWorld,
		.start = startHelloWorld,
		.stop = stopHelloWorld,
		.update = updateHelloWorld
};

typedef struct {
	char name[30];
} HelloWorld;

static
void* newHelloWorld(const char* componentTypeName)
{
	HelloWorld* i = (HelloWorld*)malloc(sizeof(HelloWorld));
	// probably it is good idea to zeroed the memory
	strcpy(i->name, "nobody");
	return i;
}

static
int startHelloWorld(void* instance)
{
	HelloWorld* inst = (HelloWorld*) instance;
	KevContext* ctx = getContext(inst);
	printf("Hey %s, this is the often abused Hello World application running in component %s\n", inst->name, getInstanceName(ctx));
	/*printf("the value for name is %s\n", getDictionaryAttributeValue(ctx, "name"));*/
	free(ctx);
	return 0;
}

static
int stopHelloWorld(void* instance)
{
	HelloWorld* inst = (HelloWorld*) instance;
	printf("And now the world is gone :-)\n");
	return 0;
}

static
int updateHelloWorld(void* instance)
{
	HelloWorld* inst = (HelloWorld*) instance;
	KevContext* ctx = getContext(inst);
	char *value = getDictionaryAttributeValue(ctx, "name");
	strcpy(inst->name, value);
	printf("INFO: Updated value %s\n", inst->name);
	free(ctx);
	startHelloWorld(inst);
	return 0;
}

REGISTER_KEV_TYPES(HW_DU, 1, &helloWorldInterface)
