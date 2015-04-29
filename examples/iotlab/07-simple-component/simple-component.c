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

/*
 * Prints "Hello World !", and echoes whatever arrives on the serial link
 */

PROCESS(serial_echo, "Serial Echo");
//AUTOSTART_PROCESSES(&serial_echo);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(serial_echo, ev, data)
{
  PROCESS_BEGIN();

  printf("Hello World !\n");
  while(1) {
    printf("> ");
    PROCESS_YIELD();
    if (ev == serial_line_event_message) {
      printf("Echo cmd: '%s'\n", (char*)data);
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
