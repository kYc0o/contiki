#include "contiki.h"
#include "dev/serial-line.h"
#include "rtkev.h"
#include <stdio.h>

/* forward declaration */
void* newHelloWorld(const char* name);
int startHelloWorld(void* instance);
int stopHelloWorld(void* instance);
int updateHelloWorld(void* instance);

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

void* newHelloWorld(const char* name)
{
    HelloWorld* i = (HelloWorld*)malloc(sizeof(HelloWorld));
    // probably it is good idea to zeroed the memory
    return i;
}

int startHelloWorld(void* instance)
{
    HelloWorld* inst = (HelloWorld*) instance;
    return 0;
}

int stopHelloWorld(void* instance)
{
    HelloWorld* inst = (HelloWorld*) instance;
    return 0;
}

int updateHelloWorld(void* instance)
{
    HelloWorld* inst = (HelloWorld*) instance;
    return 0;
}

REGISTER_KEV_TYPES(2, &helloWorld, &helloWorld_Second)
