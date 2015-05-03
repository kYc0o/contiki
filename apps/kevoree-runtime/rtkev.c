#include "cfs/cfs.h"
#include "loader/elfloader.h"

#include "rtkev.h"
#include "SimpleTraces.h"
#include "lib/list.h"

#include "ContainerRoot.h"
#include "JSONModelLoader.h"
#include "ModelCompare.h"
#include "TraceSequence.h"

#include <stdarg.h>

#define DEBUG
#ifdef DEBUG
#define PRINTF printf
#else
#define PRINTF(S, ...)
#endif

/**
 * The Kevoree runtime for contiki includes a set of processes
 * that perform different tasks.
 *
 * Of course, it also includes several datastructure to represent the internal runtime
 * state.
 *
 */

struct TypeEntry {
	struct TypeEntry* next;
	ComponentInterface* interface;
};

struct InstanceEntry {
	struct InstanceEntry* next;
	ComponentInterface* interface;
	void* instance;
	char* name;
};

static struct Runtime {
    /* the current model */
	ContainerRoot *currentModel;
    /* hash_map from type name to type definition */
	LIST_STRUCT(types);
	/* instanaces */
	LIST_STRUCT(instances);
	/* deployUnit retriever */
	DeployUnitRetriver* deployUnitRetriever;
} runtime;

static const char *DEFAULTMODEL = "{\"eClass\" : \"org.kevoree.ContainerRoot\",\"generated_KMF_ID\" : \"BXX5q3eV\",\"nodes\" : [{\"eClass\" : \"org.kevoree.ContainerNode\",\"name\" : \"node0\",\"metaData\" : \"\",\"started\" : \"1\",\"components\" : [],\"hosts\" : [],\"host\" : [],\"groups\" : [\"groups[group0]\"],\"networkInformation\" : [{\"eClass\" : \"org.kevoree.NetworkInfo\",\"name\" : \"ip\",\"values\" : [{\"eClass\" : \"org.kevoree.NetworkProperty\",\"name\" : \"front\",\"value\" : \"m3-XX.lille.iotlab.info\"},{\"eClass\" : \"org.kevoree.NetworkProperty\",\"name\" : \"local\",\"value\" : \"fe80:0000:0000:0000:0323:4501:4471:0343\"}]}],\"typeDefinition\" : [\"typeDefinitions[ContikiNode/0.0.1]\"],\"dictionary\" : [],\"fragmentDictionary\" : []}],\"typeDefinitions\" : [{\"eClass\" : \"org.kevoree.NodeType\",\"name\" : \"ContikiNode\",\"version\" : \"0.0.1\",\"factoryBean\" : \"\",\"bean\" : \"\",\"abstract\" : \"0\",\"deployUnit\" : [\"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]\"],\"dictionaryType\" : [{\"eClass\" : \"org.kevoree.DictionaryType\",\"generated_KMF_ID\" : \"o8AVQY3e\",\"attributes\" : []}],\"superTypes\" : []},{\"eClass\" : \"org.kevoree.GroupType\",\"name\" : \"CoAPGroup\",\"version\" : \"0.0.1\",\"factoryBean\" : \"\",\"bean\" : \"\",\"abstract\" : \"0\",\"deployUnit\" : [\"deployUnits[//kevoree-group-coap/0.0.1]\"],\"dictionaryType\" : [{\"eClass\" : \"org.kevoree.DictionaryType\",\"generated_KMF_ID\" : \"3dddTFpd\",\"attributes\" : [{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"proxy_port\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"int\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"20000\"},{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"port\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"number\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"\"},{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"path\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"string\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"\"}]}],\"superTypes\" : []}],\"repositories\" : [],\"dataTypes\" : [],\"libraries\" : [{\"eClass\" : \"org.kevoree.TypeLibrary\",\"name\" : \"ContikiLib\",\"subTypes\" : [\"typeDefinitions[ContikiNode/0.0.1]\",\"typeDefinitions[CoAPGroup/0.0.1]\"]},{\"eClass\" : \"org.kevoree.TypeLibrary\",\"name\" : \"Default\",\"subTypes\" : []}],\"hubs\" : [],\"mBindings\" : [],\"deployUnits\" : [{\"eClass\" : \"org.kevoree.DeployUnit\",\"name\" : \"kevoree-group-coap\",\"groupName\" : \"\",\"version\" : \"0.0.1\",\"url\" : \"\",\"hashcode\" : \"\",\"type\" : \"ce\"},{\"eClass\" : \"org.kevoree.DeployUnit\",\"name\" : \"kevoree-contiki-node\",\"groupName\" : \"org.kevoree.library.c\",\"version\" : \"0.0.1\",\"url\" : \"\",\"hashcode\" : \"\",\"type\" : \"ce\"}],\"nodeNetworks\" : [],\"groups\" : [{\"eClass\" : \"org.kevoree.Group\",\"name\" : \"group0\",\"metaData\" : \"\",\"started\" : \"1\",\"subNodes\" : [\"nodes[node0]\"],\"typeDefinition\" : [\"typeDefinitions[CoAPGroup/0.0.1]\"],\"dictionary\" : [],\"fragmentDictionary\" : [{\"eClass\" : \"org.kevoree.FragmentDictionary\",\"generated_KMF_ID\" : \"VEj2RlNr\",\"name\" : \"contiki-node\",\"values\" : []}]}]}";

/* kevoree event types */
static process_event_t NEW_KEV_TYPE;
static process_event_t NEW_MODEL;

static process_event_t NEW_TRACE_MODEL;
static process_event_t DEPLOY_UNIT_DOWNLOADED;
static process_event_t TRACE_EXECUTED;

LIST(simpleTraces);

/* forward declaration */
static void
processTrace(struct SimpleTrace* t);

/* this process downloads, installs and removes the necessesary deploy units */
PROCESS(kev_model_installer, "kev_model_installer");
PROCESS_THREAD(kev_model_installer, ev, data)
{
	static char* filename;
	int fd, r;
	struct SimpleTrace* trace;
	PROCESS_BEGIN();
	
	/* register new event types */
	NEW_TRACE_MODEL = process_alloc_event();
	DEPLOY_UNIT_DOWNLOADED = process_alloc_event();
	TRACE_EXECUTED = process_alloc_event();

	/* initialize list of traces */
	list_init(simpleTraces);

	while (1) {
		/* it runs forever, waiting for a new trace model */
		PROCESS_WAIT_EVENT();
		if (ev == NEW_TRACE_MODEL) {
			/* data should point to a trace model */
			
			/* I will fake some traces */
			fd = cfs_open("traces.traces", CFS_READ);
			do {
				trace = (struct SimpleTrace*)malloc(sizeof(struct SimpleTrace));
				r = nextTrace(fd, trace);
				if (r < 0)	free(trace);
				else list_add(simpleTraces, trace);
			} while (r==0);
			cfs_close(fd);
			PRINTF("The number of traces is %d\n", list_length(simpleTraces));			
#ifdef DEBUG
			/* iterate through list of traces */
			for(trace = list_head(simpleTraces);
			  trace != NULL;
			  trace = list_item_next(trace)) {
				PRINTF("trace: %c %s %s\n", trace->type, trace->nodeName, trace->param0.deployUnit);
			}
#endif
			/* execute next trace */
			if (list_length(simpleTraces) > 0) {
				trace = list_pop(simpleTraces);
				processTrace(trace);
				free(trace);			
			}
		}
		else if (ev == DEPLOY_UNIT_DOWNLOADED) {
			filename = (char*)data;
			/* data contains the deploy unit ID */
			PRINTF("The deploy unit %s was downloaded and now I can install the types inside\n", filename);
			/* load elf file with the deploy unit */
			loadElfFile(filename);
			/* here I must free the memory */			
			free(filename);
			/* execute next trace */
			if (list_length(simpleTraces) > 0) {
				trace = list_pop(simpleTraces);
				processTrace(trace);
				free(trace);			
			}
		}
		else if (ev == TRACE_EXECUTED) {
			/* execute next trace */
			if (list_length(simpleTraces) > 0) {
				trace = list_pop(simpleTraces);
				processTrace(trace);
				free(trace);			
			}
		}
	}

	PROCESS_END();
}

static void
processTrace(struct SimpleTrace* t) {
	void* inst;
	switch(t->type) {
		case TRACE_INST_DEPLOY_UNIT:
			runtime.deployUnitRetriever->getDeployUnit(t->param0.deployUnit);
		break;
		case TRACE_NEW_INSTANCE:
			createInstance(t->param0.kevType, t->param1.instanceName, &inst);
			process_post(&kev_model_installer, TRACE_EXECUTED, NULL);
		break;
		case TRACE_START_INSTANCE:
			startInstance(t->param0.instanceName);
			process_post(&kev_model_installer, TRACE_EXECUTED, NULL);
		break;
	}
}

/* this process wait for new models in order to analysis them 
 * It builds a trace model
*/
PROCESS(kev_model_listener, "kev_model_listener");
PROCESS_THREAD(kev_model_listener, ev, data)
{
    PROCESS_BEGIN();
	PRINTF("Ejecutando proceso kev_model_listener\n");

	/* register new event type */
	NEW_MODEL = process_alloc_event();
	printf("INFO: NEW_MODEL event was allocated %d\n", NEW_MODEL);

    while (1) {
        /* it runs forever, waiting for some update to the model */
		PROCESS_WAIT_EVENT_UNTIL(ev == NEW_MODEL);
		/* wow I have a new model, do te magic with the traces and so on */
		PRINTF("Here a new model is coming\n");
		/*if (data != NULL && runtime.currentModel != NULL) {
			//TraceSequence *ts = ModelCompare((ContainerRoot*)data, runtime.currentModel);
		} else {
			if (data == NULL) {
				PRINTF("ERROR: New model is NULL!\n");
			} else if (runtime.currentModel == NULL) {
				PRINTF("ERROR: Current model is NULL!\n");
			}
		}*/

		// TODO : this is temporarary, only to check mechanism to deal with the download of deploy units
		if (data == NULL) {
			return process_post(&kev_model_installer, NEW_TRACE_MODEL, NULL);
		}
    }

    PROCESS_END();
}

/* init runtime */
int initKevRuntime(DeployUnitRetriver* retriever)
{
	LIST_STRUCT_INIT(&runtime, types);
	LIST_STRUCT_INIT(&runtime, instances);

	runtime.deployUnitRetriever = retriever;

	PRINTF("%p %p JEJEJJEJ\n", retriever, retriever->getDeployUnit);

	/* let's assign the empty model as the current model */
	struct jsonparse_state jsonState;

	jsonparse_setup(&jsonState, DEFAULTMODEL, strlen(DEFAULTMODEL) + 1);
	runtime.currentModel = JSONKevDeserializer(&jsonState, jsonparse_next(&jsonState));

	if (runtime.currentModel == NULL)
		return ERR_KEV_INIT_FAILURE;

	/* start support protothreads */
	process_start(&kev_model_listener, NULL);
	process_start(&kev_model_installer, NULL);

	return 0;
}

/* register component type */
int registerComponent(int count, ... )
{
	va_list ap;
	ComponentInterface* interface;
	
	/* this is here for debug, when you deploy an example which is a component 
	 * you must start the runtime somehow	
	*/	
	/*initKevRuntime();*/

	/* get the arguments */
	va_start(ap, count);

	/* iterate to register arguments */
	while (count) {
		interface = va_arg(ap, ComponentInterface*);
		count--;

		PRINTF("Registering Kevoree Type %s located at %p\n", interface->name, interface);
		
		/* it add a new entry to the list :-) */
		struct TypeEntry* entry = (struct TypeEntry*)malloc(sizeof(struct TypeEntry));
		entry->interface = interface;

		list_add(runtime.types, entry);
	}

	/* done with the variadic arguments*/
	va_end(ap);

	return 0; 
}

/* Notify about a new model, normally this will be mostly used Groups.
 * However, it is also available for "smart"components.
 * it receive a ContainerRoot as parameter
 * */
int notifyNewModel(ContainerRoot *model)
{
	PRINTF("INFO: Sending model %p\n", model);
	// it essentially sends a message to the process kev_model_listener
	// well, I am guessing everything is Ok, :-)
	process_post(&kev_model_listener, NEW_MODEL, model);
	//TraceSequence *ts = ModelCompare(model, runtime.currentModel);
	return 0;
}

/* create an instance of some type */
int createInstance(char* typeName, char* instanceName, void** instance)
{
	struct TypeEntry* entry;
	/* iterate through list of ComponentInterface */
	for(entry = list_head(runtime.types);
      entry != NULL;
      entry = list_item_next(entry)) {
		if (!strcmp(typeName, entry->interface->name)) {
			PRINTF("\tType Found\n");
			
			/* create instance using supplied component interface */
			*instance = entry->interface->newInstance(entry->interface->name);

			if (!*instance)
				return ERR_KEV_INSTANCE_CREATION_FAIL;
			
			struct InstanceEntry* entry2 = (struct InstanceEntry*)malloc(sizeof(struct InstanceEntry));
			entry2->instance = *instance;
			entry2->interface = entry->interface;
			entry2->name = (char*)strdup(instanceName);

			list_add(runtime.instances, entry2);
		}
	}
	return 0;
}

/* start an instance */
int startInstance(char* instanceName)
{
	struct InstanceEntry* entry;
	/* iterate through list of ComponentInterface */
	for(entry = list_head(runtime.instances);
      entry != NULL;
      entry = list_item_next(entry)) {
		if (!strcmp(instanceName, entry->name)) {
			PRINTF("Instance Found\n");
			/* start instance using supplied component interface */
			if (!entry->interface->start(entry->instance)) {
				PRINTF("INFO: starting instance OK\n");
			} else {
				PRINTF("ERROR: instance cannot be started!\n");
			}
		}
	}
	return 0;
}

/* dealing with deploy units */
void notifyDeployUnitDownloaded(const char* fileName)
{
	process_post(&kev_model_installer, DEPLOY_UNIT_DOWNLOADED, fileName);
}

/* loads an elf file into the system and executes its autostart processes */
void loadElfFile(const char* filename)
{
	uint32_t fdFile, received;
	// Cleanup previous loads
	if (elfloader_autostart_processes != NULL)
		autostart_exit(elfloader_autostart_processes);
	elfloader_autostart_processes = NULL;

	// Load elf file
	fdFile = cfs_open(filename, CFS_READ | CFS_WRITE);
	received = elfloader_load(fdFile);
	cfs_close(fdFile);
	PRINTF("INFO: Result of loading %lu\n", received);

	// As the file has been modified and can't be reloaded, remove it
	PRINTF("WARNING: Remove dirty firmware '%s'\n", filename);
	cfs_remove(filename);

	// execute the program
	if (ELFLOADER_OK == received) {
		if (elfloader_autostart_processes) {
			//PRINT_PROCESSES(elfloader_autostart_processes);
			autostart_start(elfloader_autostart_processes);
		}
	}
	else if (ELFLOADER_SYMBOL_NOT_FOUND == received) {
	  printf("Symbol not found: '%s'\n", elfloader_unknown);
	}
}
