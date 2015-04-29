#include "contiki.h"
#include "dev/serial-line.h"
#include "rtkev.h"
#include <stdio.h>

/* forward declaration */
static void* newHelloWorld(const char* name);
static int startHelloWorld(void* instance);
static int stopHelloWorld(void* instance);
static int updateHelloWorld(void* instance);

const ComponentInterface helloWorld = {
	.name = "HelloWordType_0", 
    .newInstance = newHelloWorld,
    .start = startHelloWorld,
    .stop = stopHelloWorld,
    .update = updateHelloWorld
};

const ComponentInterface helloWorld_Second = {
	.name = "HelloWordType_1",    
	.newInstance = newHelloWorld,
    .start = startHelloWorld,
    .stop = stopHelloWorld,
    .update = updateHelloWorld
};

typedef struct {
    char name[30];
} HelloWorld;

static
void* newHelloWorld(const char* name)
{
    HelloWorld* i = (HelloWorld*)malloc(sizeof(HelloWorld));
    // probably it is good idea to zeroed the memory
    return i;
}

static
int startHelloWorld(void* instance)
{
    HelloWorld* inst = (HelloWorld*) instance;
    return 0;
}

static
int stopHelloWorld(void* instance)
{
    HelloWorld* inst = (HelloWorld*) instance;
    return 0;
}

static
int updateHelloWorld(void* instance)
{
    HelloWorld* inst = (HelloWorld*) instance;
    return 0;
}
