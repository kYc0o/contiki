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
		.interfaceType = ComponentInstanceInterface,
		.name = "blink/0.0.1",
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
	process_start(&blinking_led_kev, i);
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
	KevContext* ctx = getContext(i);
	char *value = getDictionaryAttributeValue(ctx, "interval");
	int iVal = atoi(value);
	i->interval = iVal;
	printf("INFO: Updated value %d\n", i->interval);
	free(ctx);
	process_post(&blinking_led_kev, PROCESS_EVENT_POLL, i);
	return 0;
}

PROCESS_THREAD(blinking_led_kev, ev, data)
{
	PROCESS_BEGIN();
	static struct etimer timer;
	static BlinkComponent *i;

	i = (BlinkComponent*)data;

	etimer_set(&timer, CLOCK_SECOND * i->interval/1000);

	while(1) {
		PROCESS_WAIT_EVENT();
		if (ev == PROCESS_EVENT_TIMER) {
			printf("Current time: %d\n", RTIMER_NOW());
			etimer_restart(&timer);
			leds_toggle(LEDS_RED);
		}
		if (ev == PROCESS_EVENT_POLL) {
			i = (BlinkComponent*)data;
			etimer_set(&timer, CLOCK_SECOND * i->interval/1000);
		}
	}
	PROCESS_END();
}

REGISTER_KEV_TYPES(Blink_DU, 1, &BlinkInterface)
