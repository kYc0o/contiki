#include "cfs/cfs.h"
#include "loader/elfloader.h"

#include "rtkev.h"
#include "lib/list.h"

#include "ContainerRoot.h"
#include "JSONModelLoader.h"
#include "ModelCompare.h"
#include "TraceSequence.h"
#include "AdaptationPrimitive.h"
#include "Planner.h"
#include "ComponentInstance.h"
#include "TypeDefinition.h"
#include "NamedElement.h"
#include "Instance.h"
#include "hashmap.h"
#include "Dictionary.h"
#include "DictionaryValue.h"

#include <stdarg.h>

#define DEBUG
#ifdef DEBUG
#define PRINTF printf
#else
#define PRINTF(S, ...)
#endif

/**as
 * The Kevoree runtime for contiki includes a set of processes
 * that perform different tasks.
 *
 * Of course, it also includes several datastructure to represent the internal runtime
 * state.
 *
 */

struct TypeEntry {
	struct TypeEntry* next;
	KevInterface* interface;
};

struct InstanceEntry {
	// to put them on lists
	struct InstanceEntry* next;
	// pointer to structure with the methods to start, stop, update
	KevInterface* interface;
	// the real instance which hold the data
	void* instance;
	// name of the instance
	char* name;
	// the dictionary of this instance
	LIST_STRUCT(dictionary);
};

struct DeployUnitEntry {
	// to put them on lists
	struct DeployUnitEntry* next;
	// deploy unit's ID	
	char* id;
};

/* this is used to hold the values of each dictionary attribute */
struct DictionaryPair {
	struct DictionaryPair* next;
	char* key;
	char* value;
};

static struct Runtime {
	/* the current model */
	ContainerRoot *currentModel;
	/* hash_map from type name to type definition */
	LIST_STRUCT(types);
	/* instanaces */
	LIST_STRUCT(instances);
	/* deploy units already installed */
	LIST_STRUCT(deployUnits);
	/* deployUnit retriever */
	DeployUnitRetriver* deployUnitRetriever;
} runtime;

static const char *DEFAULTMODEL = "{\"eClass\" : \"org.kevoree.ContainerRoot\",\"generated_KMF_ID\" : \"BXX5q3eV\",\"nodes\" : [{\"eClass\" : \"org.kevoree.ContainerNode\",\"name\" : \"node0\",\"metaData\" : \"\",\"started\" : \"1\",\"components\" : [],\"hosts\" : [],\"host\" : [],\"groups\" : [\"groups[group0]\"],\"networkInformation\" : [{\"eClass\" : \"org.kevoree.NetworkInfo\",\"name\" : \"ip\",\"values\" : [{\"eClass\" : \"org.kevoree.NetworkProperty\",\"name\" : \"front\",\"value\" : \"m3-XX.lille.iotlab.info\"},{\"eClass\" : \"org.kevoree.NetworkProperty\",\"name\" : \"local\",\"value\" : \"fe80:0000:0000:0000:0323:4501:4471:0343\"}]}],\"typeDefinition\" : [\"typeDefinitions[ContikiNode/0.0.1]\"],\"dictionary\" : [],\"fragmentDictionary\" : []}],\"typeDefinitions\" : [{\"eClass\" : \"org.kevoree.NodeType\",\"name\" : \"ContikiNode\",\"version\" : \"0.0.1\",\"factoryBean\" : \"\",\"bean\" : \"\",\"abstract\" : \"0\",\"deployUnit\" : [\"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]\"],\"dictionaryType\" : [{\"eClass\" : \"org.kevoree.DictionaryType\",\"generated_KMF_ID\" : \"o8AVQY3e\",\"attributes\" : []}],\"superTypes\" : []},{\"eClass\" : \"org.kevoree.GroupType\",\"name\" : \"CoAPGroup\",\"version\" : \"0.0.1\",\"factoryBean\" : \"\",\"bean\" : \"\",\"abstract\" : \"0\",\"deployUnit\" : [\"deployUnits[//kevoree-group-coap/0.0.1]\"],\"dictionaryType\" : [{\"eClass\" : \"org.kevoree.DictionaryType\",\"generated_KMF_ID\" : \"3dddTFpd\",\"attributes\" : [{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"proxy_port\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"int\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"20000\"},{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"port\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"number\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"\"},{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"path\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"string\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"\"}]}],\"superTypes\" : []}],\"repositories\" : [],\"dataTypes\" : [],\"libraries\" : [{\"eClass\" : \"org.kevoree.TypeLibrary\",\"name\" : \"ContikiLib\",\"subTypes\" : [\"typeDefinitions[ContikiNode/0.0.1]\",\"typeDefinitions[CoAPGroup/0.0.1]\"]},{\"eClass\" : \"org.kevoree.TypeLibrary\",\"name\" : \"Default\",\"subTypes\" : []}],\"hubs\" : [],\"mBindings\" : [],\"deployUnits\" : [{\"eClass\" : \"org.kevoree.DeployUnit\",\"name\" : \"kevoree-group-coap\",\"groupName\" : \"\",\"version\" : \"0.0.1\",\"url\" : \"\",\"hashcode\" : \"\",\"type\" : \"ce\"},{\"eClass\" : \"org.kevoree.DeployUnit\",\"name\" : \"kevoree-contiki-node\",\"groupName\" : \"org.kevoree.library.c\",\"version\" : \"0.0.1\",\"url\" : \"\",\"hashcode\" : \"\",\"type\" : \"ce\"}],\"nodeNetworks\" : [],\"groups\" : [{\"eClass\" : \"org.kevoree.Group\",\"name\" : \"group0\",\"metaData\" : \"\",\"started\" : \"1\",\"subNodes\" : [\"nodes[node0]\"],\"typeDefinition\" : [\"typeDefinitions[CoAPGroup/0.0.1]\"],\"dictionary\" : [],\"fragmentDictionary\" : [{\"eClass\" : \"org.kevoree.FragmentDictionary\",\"generated_KMF_ID\" : \"VEj2RlNr\",\"name\" : \"contiki-node\",\"values\" : []}]}]}";

/* kevoree event types */
static process_event_t NEW_KEV_TYPE;
static process_event_t NEW_MODEL;

static process_event_t NEW_ADAPTATION_MODEL;
static process_event_t DEPLOY_UNIT_DOWNLOADED;
static process_event_t ADAPTATION_EXECUTED;

/*LIST(simpleTraces);*/
LIST(plannedAdaptations);

/* forward declaration */
static void
processTrace(AdaptationPrimitive *ap);

static void
disseminateTheModel(ContainerRoot* model);

static int
isAlreadyInstalled(const char* deployUnitId);

/* this process downloads, installs and removes the necessesary deploy units */
PROCESS(kev_model_installer, "kev_model_installer");
PROCESS_THREAD(kev_model_installer, ev, data)
{
	static char* filename;	
	static AdaptationPrimitive *ap;
	PROCESS_BEGIN();

	/* register new event types */
	NEW_ADAPTATION_MODEL = process_alloc_event();
	DEPLOY_UNIT_DOWNLOADED = process_alloc_event();
	ADAPTATION_EXECUTED = process_alloc_event();

	/* initialize list of traces
	list_init(simpleTraces);*/

	while (1) {
		/* it runs forever, waiting for a new trace model */
		PROCESS_WAIT_EVENT();
		if (ev == NEW_ADAPTATION_MODEL) {
			PRINTF("INFO: Starting adaptations\n");
			
			if (list_length(plannedAdaptations) > 0) {
				ap = list_pop(plannedAdaptations);
				processTrace(ap);
				ap->delete(ap);
			}
			else {
				// remove old model
				// notify we have a new model
				disseminateTheModel(NULL);
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
			/* execute next adaptation */
			if (list_length(plannedAdaptations) > 0) {
				ap = list_pop(plannedAdaptations);
				processTrace(ap);
				ap->delete(ap);
			}
			else {
				// remove old model
				// notify we have a new model
				disseminateTheModel(NULL);
			}
		}
		else if (ev == ADAPTATION_EXECUTED) {
			/* execute next adaptation */
			if (list_length(plannedAdaptations) > 0) {
				ap = list_pop(plannedAdaptations);
				processTrace(ap);
				ap->delete(ap);
			}
			else {
				// remove old model
				// notify we have a new model
				disseminateTheModel(NULL);
			}
		}
	}

	PROCESS_END();
}

static struct InstanceEntry*
findInstanceByname(const char* instanceName);

static void
processTrace(AdaptationPrimitive *ap) {
	void* inst;
	ComponentInstance *ci;
	TypeDefinition *td;
	struct InstanceEntry* e;
	struct DictionaryPair* pair;
	switch(ap->primitiveType) {
	case AddDeployUnit:
		PRINTF("INFO: Processing %s\n", ap->ref->internalGetKey(ap->ref));
		if (!isAlreadyInstalled(ap->ref->internalGetKey(ap->ref))) {
			runtime.deployUnitRetriever->getDeployUnit(ap->ref->internalGetKey(ap->ref));
			// FIXME : this is wrong, we should only add the entry to the list once we download it, but I am lazy and I know it won't fail (at least for the ShellRetriever)
			struct DeployUnitEntry* entry = (struct DeployUnitEntry*)malloc(sizeof(struct DeployUnitEntry));
			entry->id = strdup(ap->ref->internalGetKey(ap->ref));
			list_add(runtime.deployUnits, entry);
		}
		else
			process_post(&kev_model_installer, ADAPTATION_EXECUTED, NULL);
		break;
	case AddInstance:
		ci = (ComponentInstance*)ap->ref;
		td = ci->super->typeDefinition;
		PRINTF("INFO: Processing %s\n", td->internalGetKey(td));
		createInstance(td->internalGetKey(td), ci->super->super->name, &inst);
		process_post(&kev_model_installer, ADAPTATION_EXECUTED, NULL);
		break;
	case UpdateDictionaryInstance:
		ci = (ComponentInstance*)ap->ref;
		Dictionary *d = ci->super->dictionary;
		e = findInstanceByname(ci->super->super->name);
		/* components */
		hashmap_map* m = d->values;

		/* compare components */
		int i;
		for(i = 0; i< m->table_size; i++)
		{
			if(m->data[i].in_use != 0)
			{
				any_t data = (any_t) (m->data[i].data);
				DictionaryValue* n = data;
				PRINTF("INFO: DictionaryValue %s -> %s\n", n->name, n->value);
				pair = (struct DictionaryPair*)malloc(sizeof(struct DictionaryPair));
				pair->key = strdup(n->name);
				pair->value = strdup(n->value);
				list_add(e->dictionary, pair);
			}
		}
		process_post(&kev_model_installer, ADAPTATION_EXECUTED, NULL);
		break;
	case StartInstance:
		ci = (ComponentInstance*)ap->ref;
		PRINTF("INFO: Processing StartInstance %s\n", ci->super->super->name);
		startInstance(ci->super->super->name);
		process_post(&kev_model_installer, ADAPTATION_EXECUTED, NULL);
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
		if (data != NULL && runtime.currentModel != NULL) {
			// char *traces;
			TraceSequence *ts = ModelCompare((ContainerRoot*)data, runtime.currentModel);

			list_init(plannedAdaptations);

			Planner_compareModels(runtime.currentModel, (ContainerRoot*)data, "node0", ts);
			plannedAdaptations = Planner_schedule();

			if (plannedAdaptations != NULL) {
				int adaptListLength = list_length(plannedAdaptations);
				PRINTF("INFO: Number of adaptations: %d\n", adaptListLength);
				AdaptationPrimitive *c;
				for (c = list_head(plannedAdaptations); c != NULL; c = list_item_next(c)) {
					PRINTF("%s: Priority: %d Type: %d\n", c->ref->path, Priority_Primitives(c->primitiveType), c->primitiveType);
				}
			} else {
				PRINTF("ERROR: cannot create Adaptation primitives\n");
			}

			ModelTrace *mt;
			while (list_length(ts->traces_list)) {
				mt = list_pop(ts->traces_list);
#ifdef DEBUG
				char *trace;
				trace = mt->vt->ToString(mt);
				PRINTF("%s", trace);
				free(trace);
#endif
				mt->vt->Delete(mt);
			}
			process_post(&kev_model_installer, NEW_ADAPTATION_MODEL, NULL);

		} else {
			if (data == NULL) {
				PRINTF("ERROR: New model is NULL!\n");
			} else if (runtime.currentModel == NULL) {
				PRINTF("ERROR: Current model is NULL!\n");
			}
		}
	}

	PROCESS_END();
}

/* init runtime */
int initKevRuntime(const DeployUnitRetriver* retriever)
{
	LIST_STRUCT_INIT(&runtime, types);
	LIST_STRUCT_INIT(&runtime, instances);
	LIST_STRUCT_INIT(&runtime, deployUnits);	

	runtime.deployUnitRetriever = (DeployUnitRetriver*)retriever;

	/* let's assign the empty model as the current model */
	struct jsonparse_state jsonState;

	int fd = cfs_open("file.txt", CFS_WRITE);
	cfs_write(fd, DEFAULTMODEL, strlen(DEFAULTMODEL));
	cfs_close(fd);

	jsonparse_setup(&jsonState, "file.txt");
	runtime.currentModel = JSONKevDeserializer(&jsonState, jsonparse_next(&jsonState));
	cfs_close(jsonState.fd);
	cfs_remove("file.txt");

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

			PRINTF("\tInstance create at %p\n", *instance);

			if (!*instance)
				return ERR_KEV_INSTANCE_CREATION_FAIL;

			// create metadata for the instance
			struct InstanceEntry* instEntry = (struct InstanceEntry*)malloc(sizeof(struct InstanceEntry));
			instEntry->instance = *instance;
			instEntry->interface = entry->interface;
			instEntry->name = (char*)strdup(instanceName);
			LIST_STRUCT_INIT(instEntry, dictionary);
			// fill the dictionary with objects of structure DictionaryPair

			// save the instance
			list_add(runtime.instances, instEntry);
		}
	}
	return 0;
}


static struct InstanceEntry*
findInstanceByname(const char* instanceName)
{
	struct InstanceEntry* entry;
	/* iterate through list of ComponentInterface */
	for(entry = list_head(runtime.instances);
			entry != NULL;
			entry = list_item_next(entry)) {
		if (!strcmp(instanceName, entry->name)) return entry;
	}
	return NULL;
}

/* free an instance */
int
removeInstance(const char* instanceName)
{
	struct InstanceEntry* found = findInstanceByname(instanceName);
	if (found) {
		list_remove(runtime.instances, found);
		free(found->name);
		free(found->instance);
		// free the dictionary
		struct DictionaryPair* dp;
		for (dp = list_head(found->dictionary); dp != NULL; 
				dp = list_item_next(dp)) {
			free(dp->key);
			free(dp->value);
			free(dp);
		}
		return 0;
	}
	return -1;
}

/* start an instance */
int startInstance(const char* instanceName)
{
	struct InstanceEntry* entry = findInstanceByname(instanceName);
	if (entry) {
		/* start instance using supplied component interface */
		if (!entry->interface->start(entry->instance))
			PRINTF("INFO: starting instance OK\n");
		else {
			PRINTF("ERROR: instance cannot be started!\n");
			return -1;
		}
		return 0;
	}
	return -1;
}

/* stop an instance */
int
stopInstance(const char* instanceName)
{
	struct InstanceEntry* entry = findInstanceByname(instanceName);
	if (entry) {
		/* start instance using supplied component interface */
		if (!entry->interface->stop(entry->instance))
			PRINTF("INFO: starting instance OK\n");
		else {
			PRINTF("ERROR: instance cannot be started!\n");
			return -1;
		}
		return 0;
	}
	return -1;
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
	PRINTF("WARNING: Removing dirty firmware '%s'\n", filename);
	cfs_remove(filename);

	// execute the program
	if (ELFLOADER_OK == received) {
		PRINTF("tRYING TO EXECUTE AUTOSTART PROCESSES\n");
		if (elfloader_autostart_processes) {
			PRINTF("EXECUTING AUTOSTART PROCESSES\n");
			//PRINT_PROCESSES(elfloader_autostart_processes);
			autostart_start(elfloader_autostart_processes);
		}
	}
	else if (ELFLOADER_SYMBOL_NOT_FOUND == received) {
		printf("Symbol not found: '%s'\n", elfloader_unknown);
	}
}

static void
disseminateTheModel(ContainerRoot* model)
{
	struct InstanceEntry* entry;
	/* iterate through list of Instances */
	for(entry = list_head(runtime.instances);
			entry != NULL;
			entry = list_item_next(entry)) {
		if (entry->interface->interfaceType == GroupInstanceInterface) {
			GroupInterface* gInt = (GroupInterface*)entry->interface;
			gInt->send(entry->instance, model);
		}
	}
	return NULL;
}

static int
isAlreadyInstalled(const char* deployUnitId)
{
	struct DeployUnitEntry* entry;
	/* iterate through list of deployUnits */
	for(entry = list_head(runtime.deployUnits);
			entry != NULL;
			entry = list_item_next(entry)) {
		if (!strcmp(entry->id, deployUnitId)) return 1;
	}
	return 0;
}

/* these functions deal with the context of each instance */
struct _KevContext {
	struct InstanceEntry* entry;
};

/** Get the context of an instance */
KevContext* getContext(void* instance)
{
	struct InstanceEntry* entry;
	/* iterate through list of ComponentInterface */
	for(entry = list_head(runtime.instances);
			entry != NULL;
			entry = list_item_next(entry)) {
		if (instance == entry->instance) {
			KevContext* ctx = (KevContext*)malloc(sizeof(KevContext));
			ctx->entry = entry;
			return ctx; 
		}
	}
	return NULL;
}

/* functions to deal with the context */
const char*
getInstanceName(KevContext* context)
{
	return context->entry->name;
}

const char*
getDictionaryAttributeValue(KevContext* context, const char* att)
{
	struct InstanceEntry* entry = context->entry;
	struct DictionaryPair* pair;
	/* iterate through list of ComponentInterface */
	for(pair = list_head(entry->dictionary);
			pair != NULL;
			pair = list_item_next(pair)) {
		if (!strcmp(att, pair->key))
			return pair->value; 
	}
	return NULL;
}
