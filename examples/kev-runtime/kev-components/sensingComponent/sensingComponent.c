#include "contiki.h"
#include "rtkev.h"
#include <stdio.h>

#ifdef IOTLAB_M3
#include "dev/light-sensor.h"
#endif

/* forward declaration */
static void* newBlink(const char*);
static int startBlink(void* instance);
static int stopBlink(void* instance);
static int updateBlink(void* instance);

static
const ComponentInterface SensingComponentInterface = {
	.interfaceType = ComponentInstanceInterface,
	.name = "SensingComponentType", 
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
	process_start(&sensor_collection, NULL);
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
  float light = ((float)light_val) / LIGHT_SENSOR_VALUE_SCALE;
  printf("light: %f lux\n", light);
}


PROCESS_THREAD(sensor_collection, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer;

#ifdef IOTLAB_M3
  config_light();
#endif
  etimer_set(&timer, CLOCK_SECOND);

  while(1) {
    PROCESS_WAIT_EVENT();
    if (ev == PROCESS_EVENT_TIMER) {
#ifdef IOTLAB_M3
      process_light();
#endif

      etimer_restart(&timer);
    }
  }

  PROCESS_END();
}

REGISTER_KEV_TYPES(Sensing_DU, 1, &SensingComponentInterface)
