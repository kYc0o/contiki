/*
 * shell_group.c
 * This file is part of Kevoree-Contiki
 *
 * Copyright (C) 2015 - Inti Gonzalez-Herrera
 *
 * Kevoree-Contiki is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Kevoree-Contiki is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kevoree-Contiki. If not, see <http://www.gnu.org/licenses/>.
 */



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


#define DEBUG 0
#if DEBUG
#define PRINTF(...)	printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/* forward declaration */
static void* newShellGroup(const char* name);
static int startShellGroup(void* instance);
static int stopShellGroup(void* instance);
static int updateShellGroup(void* instance);
static int sendShellGroup(void* instance, ContainerRoot* model);

/*static const char *NEWMODEL = "{\"eClass\": \"org.kevoree.ContainerRoot\",\"generated_KMF_ID\": \"CtHbJw37\",\"nodes\": [{\"eClass\": \"org.kevoree.ContainerNode\",\"name\": \"node0\",\"started\": \"false\",\"metaData\": \"{\\\"x\\\":296,\\\"y\\\":167}\",\"typeDefinition\": [\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\": [],\"host\": [],\"groups\": [\"groups[group0]\"],\"dictionary\": [],\"fragmentDictionary\": [],\"components\": [{\"eClass\": \"org.kevoree.ComponentInstance\",\"name\": \"comp457\",\"started\": \"true\",\"metaData\": \"{\\\"x\\\":408,\\\"y\\\":239}\",\"typeDefinition\": [\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\": [],\"dictionary\": [{\"eClass\": \"org.kevoree.Dictionary\",\"generated_KMF_ID\": \"0.68263587262481451424775426644\",\"values\": [{\"eClass\": \"org.kevoree.DictionaryValue\",\"name\": \"time\",\"value\": \"6\"}]}],\"fragmentDictionary\": [],\"provided\": [],\"required\": []}],\"networkInformation\": [{\"eClass\": \"org.kevoree.NetworkInfo\",\"name\": \"ip\",\"values\": [{\"eClass\": \"org.kevoree.NetworkProperty\",\"name\": \"local\",\"value\": \"aaaa::0:0:5\"},{\"eClass\": \"org.kevoree.NetworkProperty\",\"name\": \"front\",\"value\": \"m3-XX.lille.iotlab.info\"}]}]}],\"typeDefinitions\": [{\"eClass\": \"org.kevoree.NodeType\",\"abstract\": \"false\",\"bean\": \"\",\"name\": \"ContikiNode\",\"factoryBean\": \"\",\"version\": \"0.0.1\",\"deployUnit\": [\"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]\"],\"superTypes\": [],\"dictionaryType\": [{\"eClass\": \"org.kevoree.DictionaryType\",\"generated_KMF_ID\": \"9o86ZdvQ\",\"attributes\": []}]},{\"eClass\": \"org.kevoree.GroupType\",\"abstract\": \"false\",\"bean\": \"\",\"name\": \"CoAPGroup\",\"factoryBean\": \"\",\"version\": \"0.0.1\",\"deployUnit\": [\"deployUnits[//kevoree-group-coap/0.0.1]\"],\"superTypes\": [],\"dictionaryType\": [{\"eClass\": \"org.kevoree.DictionaryType\",\"generated_KMF_ID\": \"hytCmvXU\",\"attributes\": [{\"eClass\": \"org.kevoree.DictionaryAttribute\",\"fragmentDependant\": \"false\",\"optional\": \"false\",\"name\": \"path\",\"state\": \"false\",\"datatype\": \"string\",\"defaultValue\": \"\",\"genericTypes\": []},{\"eClass\": \"org.kevoree.DictionaryAttribute\",\"fragmentDependant\": \"false\",\"optional\": \"false\",\"name\": \"port\",\"state\": \"false\",\"datatype\": \"number\",\"defaultValue\": \"\",\"genericTypes\": []},{\"eClass\": \"org.kevoree.DictionaryAttribute\",\"fragmentDependant\": \"false\",\"optional\": \"false\",\"name\": \"proxy_port\",\"state\": \"false\",\"datatype\": \"int\",\"defaultValue\": \"20000\",\"genericTypes\": []}]}]},{\"eClass\": \"org.kevoree.ComponentType\",\"abstract\": \"false\",\"bean\": \"\",\"name\": \"hello_world\",\"factoryBean\": \"\",\"version\": \"0.0.1\",\"deployUnit\": [\"deployUnits[kev_contiki//hello_world/0.0.1]\"],\"superTypes\": [],\"dictionaryType\": [{\"eClass\": \"org.kevoree.DictionaryType\",\"generated_KMF_ID\": \"3dddTFpd\",\"attributes\": [{\"eClass\": \"org.kevoree.DictionaryAttribute\",\"fragmentDependant\": \"false\",\"optional\": \"false\",\"name\": \"time\",\"state\": \"false\",\"datatype\": \"int\",\"defaultValue\": \"5\",\"genericTypes\": []}]}],\"required\": [],\"provided\": []}],\"repositories\": [{\"eClass\": \"org.kevoree.Repository\",\"url\": \"coap://[bbbb::1]:5683/libraries\"}],\"dataTypes\": [],\"libraries\": [{\"eClass\": \"org.kevoree.TypeLibrary\",\"name\": \"ContikiLib\",\"subTypes\": [\"typeDefinitions[ContikiNode/0.0.1]\",\"typeDefinitions[CoAPGroup/0.0.1]\"]},{\"eClass\": \"org.kevoree.TypeLibrary\",\"name\": \"Default\",\"subTypes\": [\"typeDefinitions[hello_world/0.0.1]\"]}],\"hubs\": [],\"mBindings\": [],\"deployUnits\": [{\"eClass\": \"org.kevoree.DeployUnit\",\"groupName\": \"\",\"name\": \"kevoree-group-coap\",\"hashcode\": \"\",\"type\": \"ce\",\"url\": \"\",\"version\": \"0.0.1\",\"requiredLibs\": []},{\"eClass\": \"org.kevoree.DeployUnit\",\"groupName\": \"org.kevoree.library.c\",\"name\": \"kevoree-contiki-node\",\"hashcode\": \"\",\"type\": \"ce\",\"url\": \"\",\"version\": \"0.0.1\",\"requiredLibs\": []},{\"eClass\": \"org.kevoree.DeployUnit\",\"groupName\": \"kev_contiki\",\"name\": \"hello_world\",\"hashcode\": \"\",\"type\": \"ce\",\"url\": \"\",\"version\": \"0.0.1\",\"requiredLibs\": []}],\"nodeNetworks\": [],\"groups\": [{\"eClass\": \"org.kevoree.Group\",\"name\": \"group0\",\"started\": \"false\",\"metaData\": \"{\\\"x\\\":504,\\\"y\\\":259}\",\"typeDefinition\": [\"typeDefinitions[CoAPGroup/0.0.1]\"],\"subNodes\": [\"nodes[node0]\"],\"dictionary\": [],\"fragmentDictionary\": [{\"eClass\": \"org.kevoree.FragmentDictionary\",\"name\": \"contiki-node\",\"generated_KMF_ID\": \"QoMNUckL\",\"values\": []}]}]}";
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

	PRINTF("Ok, here I have my instance %p\n", (struct ShellGroup*)data);

	/* define new event */
	NEW_MODEL_IN_JSON = process_alloc_event();
	while (1) {
		/* it runs forever, waiting for some update to the model */
		PROCESS_WAIT_EVENT();
		if (ev == NEW_MODEL_IN_JSON) {
			/* wow I have a new model, do te magic with the traces and so on */
			PRINTF("New model %s received in group with instance %p\n", instance->fileNameWithModel, instance);
			newModel = NULL;

			if((fd_read = cfs_open(instance->fileNameWithModel, CFS_READ)) != -1) {

				cfs_close(fd_read);

				struct jsonparse_state jsonState;

				jsonparse_setup(&jsonState, instance->fileNameWithModel);
				newModel = JSONKevDeserializer(&jsonState, jsonparse_next(&jsonState));
				cfs_close(jsonState.fd);
				PRINTF("INFO: Deserialized finished\n");

				if (newModel == NULL) {
					printf("ERROR: new_model cannot be loaded\n");
					PROCESS_EXIT();
				} else {
					printf("INFO: newModel.json successfully loaded\n");
					int length = hashmap_length(newModel->nodes);
					printf("INFO: newModel with %d nodes is ready!\n", length);
				}
			}
			/* Afterwards, just call notifyNewModel */
			if (newModel != NULL && notifyNewModel(newModel)== PROCESS_ERR_OK)
				PRINTF("INFO: Model was successfully sent\n");
			else
				PRINTF("ERROR: The model cannot be loaded!\n");
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

	PRINTF("INFO: Sending instance %s, at %p\n", inst->fileNameWithModel, inst);

	process_start(&shellGroupP, inst);

	PRINTF("INFO: Sent instance %s, at %p\n", inst->fileNameWithModel, inst);

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
