#include "contiki.h"
#include "cfs/cfs.h"
#include "json.h"
#include "jsonparse.h"

#include "ContainerRoot.h"
#include "JSONModelLoader.h"
#include "Visitor.h"
#include "ModelCompare.h"

#include "rtkev.h"
#include "shell_group.h"

/* forward declaration */
static void* newShellGroup(const char* name);
static int startShellGroup(void* instance);
static int stopShellGroup(void* instance);
static int updateShellGroup(void* instance);
static int sendShellGroup(void* instance, ContainerRoot* model);

/*static const char *NEWMODEL = "{\"eClass\" : \"org.kevoree.ContainerRoot\",\"generated_KMF_ID\" : \"CtHbJw37\",\"nodes\" : [{\"eClass\" : \"org.kevoree.ContainerNode\",\"name\" : \"node0\",\"metaData\" : \"{\\\"x\\\":296,\\\"y\\\":167}\",\"started\" : \"0\",\"typeDefinition\" : [\"typeDefinitions[ContikiNode/0.0.1]\"],\"dictionary\" : [],\"fragmentDictionary\" : [],\"components\" : [{\"eClass\" : \"org.kevoree.ComponentInstance\",\"name\" : \"comp457\",\"metaData\" : \"{\\\"x\\\":408,\\\"y\\\":239}\",\"started\" : \"1\",\"typeDefinition\" : [\"typeDefinitions[hello_world/0.0.1]\"],\"dictionary\" : [{\"eClass\" : \"org.kevoree.Dictionary\",\"generated_KMF_ID\" : \"0.68263587262481451424775426644\",\"values\" : [{\"eClass\" : \"org.kevoree.DictionaryValue\",\"name\" : \"time\",\"value\" : \"6\"}]}],\"fragmentDictionary\" : [],\"provided\" : [],\"required\" : []}],\"hosts\" : [],\"host\" : [],\"groups\" : [\"groups[group0]\"],\"networkInformation\" : [{\"eClass\" : \"org.kevoree.NetworkInfo\",\"name\" : \"ip\",\"values\" : [{\"eClass\" : \"org.kevoree.NetworkProperty\",\"name\" : \"local\",\"value\" : \"aaaa::0:0:5\"},{\"eClass\" : \"org.kevoree.NetworkProperty\",\"name\" : \"front\",\"value\" : \"m3-XX.lille.iotlab.info\"}]}]}],\"typeDefinitions\" : [{\"eClass\" : \"org.kevoree.ComponentType\",\"name\" : \"hello_world\",\"version\" : \"0.0.1\",\"factoryBean\" : \"\",\"bean\" : \"\",\"abstract\" : \"0\",\"deployUnit\" : [\"deployUnits[kev_contiki//hello_world/0.0.1]\"],\"dictionaryType\" : [{\"eClass\" : \"org.kevoree.DictionaryType\",\"generated_KMF_ID\" : \"3dddTFpd\",\"attributes\" : [{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"time\",\"genericTypes\" : [],\"optional\" : \"0\",\"state\" : \"0\",\"datatype\" : \"int\",\"fragmentDependant\" : \"0\",\"defaultValue\" : \"5\"}]}],\"superTypes\" : [],\"required\" : [],\"provided\" : []},{\"eClass\" : \"org.kevoree.NodeType\",\"name\" : \"ContikiNode\",\"version\" : \"0.0.1\",\"factoryBean\" : \"\",\"bean\" : \"\",\"abstract\" : \"0\",\"deployUnit\" : [\"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]\"],\"dictionaryType\" : [{\"eClass\" : \"org.kevoree.DictionaryType\",\"generated_KMF_ID\" : \"9o86ZdvQ\",\"attributes\" : []}],\"superTypes\" : []},{\"eClass\" : \"org.kevoree.GroupType\",\"name\" : \"CoAPGroup\",\"version\" : \"0.0.1\",\"factoryBean\" : \"\",\"bean\" : \"\",\"abstract\" : \"0\",\"deployUnit\" : [\"deployUnits[//kevoree-group-coap/0.0.1]\"],\"dictionaryType\" : [{\"eClass\" : \"org.kevoree.DictionaryType\",\"generated_KMF_ID\" : \"hytCmvXU\",\"attributes\" : [{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"path\",\"genericTypes\" : [],\"optional\" : \"0\",\"state\" : \"0\",\"datatype\" : \"string\",\"fragmentDependant\" : \"0\",\"defaultValue\" : \"\"},{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"port\",\"genericTypes\" : [],\"optional\" : \"0\",\"state\" : \"0\",\"datatype\" : \"number\",\"fragmentDependant\" : \"0\",\"defaultValue\" : \"\"},{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"proxy_port\",\"genericTypes\" : [],\"optional\" : \"0\",\"state\" : \"0\",\"datatype\" : \"int\",\"fragmentDependant\" : \"0\",\"defaultValue\" : \"20000\"}]}],\"superTypes\" : []}],\"repositories\" : [],\"dataTypes\" : [],\"libraries\" : [{\"eClass\" : \"org.kevoree.TypeLibrary\",\"name\" : \"ContikiLib\",\"subTypes\" : [\"typeDefinitions[ContikiNode/0.0.1]\",\"typeDefinitions[CoAPGroup/0.0.1]\"]},{\"eClass\" : \"org.kevoree.TypeLibrary\",\"name\" : \"Default\",\"subTypes\" : [\"typeDefinitions[hello_world/0.0.1]\"]}],\"hubs\" : [],\"mBindings\" : [],\"deployUnits\" : [{\"eClass\" : \"org.kevoree.DeployUnit\",\"name\" : \"hello_world\",\"groupName\" : \"kev_contiki\",\"version\" : \"0.0.1\",\"url\" : \"\",\"hashcode\" : \"\",\"type\" : \"ce\",\"requiredLibs\" : []},{\"eClass\" : \"org.kevoree.DeployUnit\",\"name\" : \"kevoree-group-coap\",\"groupName\" : \"\",\"version\" : \"0.0.1\",\"url\" : \"\",\"hashcode\" : \"\",\"type\" : \"ce\",\"requiredLibs\" : []},{\"eClass\" : \"org.kevoree.DeployUnit\",\"name\" : \"kevoree-contiki-node\",\"groupName\" : \"org.kevoree.library.c\",\"version\" : \"0.0.1\",\"url\" : \"\",\"hashcode\" : \"\",\"type\" : \"ce\",\"requiredLibs\" : []}],\"nodeNetworks\" : [],\"groups\" : [{\"eClass\" : \"org.kevoree.Group\",\"name\" : \"group0\",\"metaData\" : \"{\\\"x\\\":504,\\\"y\\\":259}\",\"started\" : \"0\",\"typeDefinition\" : [\"typeDefinitions[CoAPGroup/0.0.1]\"],\"dictionary\" : [],\"fragmentDictionary\" : [{\"eClass\" : \"org.kevoree.FragmentDictionary\",\"generated_KMF_ID\" : \"QoMNUckL\",\"values\" : [],\"name\" : \"contiki-node\"}],\"subNodes\" : [\"nodes[node0]\"]}]}";
*/
int fd_read;
uint32_t length;
char *jsonModel;

const GroupInterface ShellGroupInterface = {
		.interfaceType = GroupInstanceInterface,
		.name = "ShellGroupType",
		.newInstance = newShellGroup,
		.start = startShellGroup,
		.stop = stopShellGroup,
		.update = updateShellGroup,
		.send = sendShellGroup
};

typedef struct  {
	char* fileNameWithModel;
} ShellGroup;

/* event used to notify about the new model */
process_event_t NEW_MODEL_IN_JSON;

/* this process handle the reception of messages */
PROCESS(shellGroupP, "shellGroupProcess");
PROCESS_THREAD(shellGroupP, ev, data)
{
	PROCESS_BEGIN();
	static ShellGroup *instance;
	ContainerRoot * newModel;

	instance = (ShellGroup*)data;

	printf("Ok, here I have my instance %p\n", (struct ShellGroup*)data);

	/* define new event */
	NEW_MODEL_IN_JSON = process_alloc_event();
	while (1) {
		/* it runs forever, waiting for some update to the model */
		PROCESS_WAIT_EVENT();
		if (ev == NEW_MODEL_IN_JSON) {
			/* wow I have a new model, do te magic with the traces and so on */
			printf("New model %s received in group with instance %p\n", instance->fileNameWithModel, instance);

			newModel = NULL;
			
			if((fd_read = cfs_open(instance->fileNameWithModel, CFS_READ)) != -1) {

				cfs_close(fd_read);

				struct jsonparse_state jsonState;

				jsonparse_setup(&jsonState, instance->fileNameWithModel);
				newModel = JSONKevDeserializer(&jsonState, jsonparse_next(&jsonState));
				cfs_close(jsonState.fd);
				printf("INFO: Deserialized finished\n");
			}
			/* Afterwards, just call notifyNewModel */
			if (newModel != NULL && notifyNewModel(newModel)== PROCESS_ERR_OK)
				printf("INFO: Model was successfully sent\n");
			else
				printf("ERROR: The model cannot be loaded!\n");
		}
		else if (ev == PROCESS_EVENT_EXIT) {
			PROCESS_EXIT();
		}
	}

	PROCESS_END();
}


static
void* newShellGroup(const char* name)
{
	ShellGroup* i = (ShellGroup*)malloc(sizeof(ShellGroup));
	// probably it is good idea to zeroed the memory
	return i;
}

static
int startShellGroup(void* instance)
{
	ShellGroup* inst = (ShellGroup*) instance;

	inst->fileNameWithModel = "new_model-compact.json";

	printf("INFO: Sending instance %s, at %p\n", inst->fileNameWithModel, inst);

	process_start(&shellGroupP, inst);

	printf("INFO: Sent instance %s, at %p\n", inst->fileNameWithModel, inst);

	return 0;
}

static
int stopShellGroup(void* instance)
{
	ShellGroup* inst = (ShellGroup*) instance;

	process_exit(&shellGroupP);

	return 0;
}

static
int updateShellGroup(void* instance)
{
	ShellGroup* inst = (ShellGroup*) instance;
	return 0;
}

static int
sendShellGroup(void* instance, ContainerRoot* model)
{
	// empty, so far it makes no sense to send a model back to the local computer.
	// anyway, if someday we want to use this group as a real one, this function is trivial to implement
	return 0;
}
