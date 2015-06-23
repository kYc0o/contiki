#include "contiki.h"
#include "rtkev.h"
#include <stdio.h>

#include "dev/light-sensor.h"

/* forward declaration */
static void* newBlink(const char*);
static int startBlink(void* instance);
static int stopBlink(void* instance);
static int updateBlink(void* instance);

static
const ComponentInterface SensingComponentInterface = {
		.interfaceType = ComponentInstanceInterface,
		.name = "sensing/0.0.1",
		.newInstance = newBlink,
		.start = startBlink,
		.stop = stopBlink,
		.update = updateBlink
};

typedef struct {
	uint32_t interval; 

} SensingComponent;

static
void* newBlink(const char* componentTypeName)
{
	SensingComponent* i = (SensingComponent*)malloc(sizeof(SensingComponent));
	// probably it is good idea to zeroed the memory
	i->interval = 1000; // one second as interval
	return i;
}


PROCESS(sensor_collection, "Sensing");

static
int startBlink(void* instance)
{
	SensingComponent* i = (SensingComponent*)instance;
	KevContext* ctx = getContext(i);
	printf("Hey instance %s\n", getInstanceName(ctx));
	free(ctx);
	process_start(&sensor_collection, i);
	return 0;
}

static
int stopBlink(void* instance)
{
	SensingComponent* i = (SensingComponent*)instance;
	process_exit(&sensor_collection);
	printf("And now the sensors are gone :-)\n");
	return 0;
}

static
int updateBlink(void* instance)
{
	SensingComponent* i = (SensingComponent*)instance;
	KevContext* ctx = getContext(i);
	char *value = getDictionaryAttributeValue(ctx, "interval");
	int iVal = atoi(value);
	i->interval = iVal;
	printf("INFO: Updated value %d\n", i->interval);
	free(ctx);
	process_post(&sensor_collection, PROCESS_EVENT_POLL, i);
	return 0;
}

/* Light sensor */
static void
config_light()
{
	light_sensor.configure(LIGHT_SENSOR_SOURCE, ISL29020_LIGHT__AMBIENT);
	light_sensor.configure(LIGHT_SENSOR_RESOLUTION, ISL29020_RESOLUTION__16bit);
	light_sensor.configure(LIGHT_SENSOR_RANGE, ISL29020_RANGE__1000lux);
	SENSORS_ACTIVATE(light_sensor);
}
static void
process_light()
{
	int light_val = light_sensor.value(0);
	/*
	 * TODO fix cast
	 */
	/*float light = (float)light_val / LIGHT_SENSOR_VALUE_SCALE;*/
	printf("light: %d lux\n", light_val / LIGHT_SENSOR_VALUE_SCALE);
}


PROCESS_THREAD(sensor_collection, ev, data)
{
	PROCESS_BEGIN();
	static struct etimer timer;
	static SensingComponent *i;
	i = (SensingComponent*)data;

	config_light();
	etimer_set(&timer, CLOCK_SECOND * i->interval/1000);

	while(1) {
		PROCESS_WAIT_EVENT();
		if (ev == PROCESS_EVENT_TIMER) {
			process_light();
			etimer_restart(&timer);
		}
		if (ev == PROCESS_EVENT_POLL) {
			i = (SensingComponent*)data;
			etimer_set(&timer, CLOCK_SECOND * i->interval/1000);
		}
	}

	PROCESS_END();
}

REGISTER_KEV_TYPES(Sensing_DU, 1, &SensingComponentInterface)
