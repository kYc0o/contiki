#include "contiki.h"
#include "rtkev.h"
#include <stdio.h>


/* forward declaration */
static void* newSieveofEratosthenes(const char*);
static int startSieveofEratosthenes(void* instance);
static int stopSieveofEratosthenes(void* instance);
static int updateSieveofEratosthenes(void* instance);

static
const ComponentInterface SieveofEratosthenesInterface = {
	.name = "SieveofEratosthenesComponentType", 
    .newInstance = newSieveofEratosthenes,
    .start = startSieveofEratosthenes,
    .stop = stopSieveofEratosthenes,
    .update = updateSieveofEratosthenes
};

typedef struct {
	uint32_t interval; 
	
} SieveofEratosthenesComponent;

static
void* newSieveofEratosthenes(const char* componentTypeName)
{
    SieveofEratosthenesComponent* i = (SieveofEratosthenesComponent*)malloc(sizeof(SieveofEratosthenesComponent));
    // probably it is good idea to zeroed the memory
	i->interval = 5000; // one second as interval
	return i;
}


PROCESS(sieve_kev, "sieve");

static
int startSieveofEratosthenes(void* instance)
{
    SieveofEratosthenesComponent* i = (SieveofEratosthenesComponent*)instance;
	KevContext* ctx = getContext(i);
	printf("Hey instance %s\n", getInstanceName(ctx));
	free(ctx);
	process_start(&sieve_kev, (const char*)i);
    return 0;
}

static
int stopSieveofEratosthenes(void* instance)
{
    SieveofEratosthenesComponent* i = (SieveofEratosthenesComponent*)instance;
	process_exit(&sieve_kev);
	printf("And now the sieve is gone :-)\n");
    return 0;
}

static
int updateSieveofEratosthenes(void* instance)
{
    SieveofEratosthenesComponent* i = (SieveofEratosthenesComponent*)instance;
    return 0;
}

void sieve(int n){
    uint32_t i,j;
    uint32_t *primes;

    primes = (uint32_t *)malloc(sizeof(int)*n);

    for (i=2;i<n;i++)
        primes[i]=1;

    for (i=2;i<n;i++)
        if (primes[i])
            for (j=i;i*j<n;j++)
                primes[i*j]=0;

	j = 1;
    for (i=2;i<n;i++)
        if (primes[i])
            printf("%ldth prime = %ldn",j++,i);

	free(primes);
}

PROCESS_THREAD(sieve_kev, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer timer;
  SieveofEratosthenesComponent* i = (SieveofEratosthenesComponent*)data;

  etimer_set(&timer, CLOCK_SECOND * (i->interval/1000));

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
	sieve(500);
    etimer_restart(&timer);
  }
  PROCESS_END();
}

REGISTER_KEV_TYPES(SIEVE_DU, 1, &SieveofEratosthenesInterface)
