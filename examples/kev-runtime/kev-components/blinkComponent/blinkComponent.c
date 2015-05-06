#include "contiki.h"
#include "rtkev.h"
#include <stdio.h>

#include "dev/leds.h"

/* forward declaration */
static void* newBlink(const char*);
static int startBlink(void* instance);
static int stopBlink(void* instance);
static int updateBlink(void* instance);

static
const ComponentInterface BlinkInterface = {
	.name = "BlinkComponentType", 
    .newInstance = newBlink,
    .start = startBlink,
    .stop = stopBlink,
    .update = updateBlink
};

typedef struct {
	uint32_t interval; 
	
} BlinkComponent;

static
void* newBlink(const char* componentTypeName)
{
    BlinkComponent* i = (BlinkComponent*)malloc(sizeof(BlinkComponent));
    // probably it is good idea to zeroed the memory
	i->interval = 1000; // one second as interval
	return i;
}


PROCESS(blinking_led_kev, "Led blinking");

static
int startBlink(void* instance)
{
    BlinkComponent* i = (BlinkComponent*)instance;
	KevContext* ctx = getContext(i);
	printf("Hey instance %s\n", getInstanceName(ctx));
	free(ctx);
	process_start(&blinking_led_kev, NULL);
    return 0;
}

static
int stopBlink(void* instance)
{
    BlinkComponent* i = (BlinkComponent*)instance;
	process_exit(&blinking_led_kev);
	printf("And now the blink is gone :-)\n");
    return 0;
}

static
int updateBlink(void* instance)
{
    BlinkComponent* i = (BlinkComponent*)instance;
    return 0;
}

PROCESS_THREAD(blinking_led_kev, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer;

  etimer_set(&timer, CLOCK_SECOND);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
    printf("Current time: %d\n", RTIMER_NOW());
    etimer_restart(&timer);
    leds_toggle(LEDS_RED);
  }
  PROCESS_END();
}

REGISTER_KEV_TYPES(Blink_DU, 1, &BlinkInterface)
