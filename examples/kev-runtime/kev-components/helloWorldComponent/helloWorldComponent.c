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
	.name = "HelloWorldType", 
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
	strcpy(i->name, "Inti");
    return i;
}

static
int startHelloWorld(void* instance)
{
    HelloWorld* inst = (HelloWorld*) instance;
	printf("Hey %s, this is the often abused Hello World application\n", inst->name);
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
    return 0;
}

REGISTER_KEV_TYPES(HW_DU, 1, &helloWorldInterface)
