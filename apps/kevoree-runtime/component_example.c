#include "rtkev.h"

#include <stdlib.h>
#include <stdio.h>

/* forward declaration */
void* newHelloWorld(const char* name);
int startHelloWorld(void* instance);
int stopHelloWorld(void* instance);
int updateHelloWorld(void* instance);

ComponentInterface helloWorld = {
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

REGISTER_COMPONENT("HelloWorldComponent", helloWorld)
