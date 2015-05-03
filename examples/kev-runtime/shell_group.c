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

ContainerRoot *intiModel;
int fd_read;
uint32_t length;
char *jsonModel;

const GroupInterface ShellGroupInterface = {
		.name = "ShellGroupType",
		.newInstance = newShellGroup,
		.start = startShellGroup,
		.stop = stopShellGroup,
		.update = updateShellGroup
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
	static ShellGroup *instance;

	PROCESS_BEGIN();

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
			/* TODO Paco, you should load the model into a ContainerRoot from the json file with name in variable instance->fileNameWithModel*/


			if((fd_read = cfs_open(instance->fileNameWithModel, CFS_READ)) != -1) {
				length = cfs_seek(fd_read, 0 , CFS_SEEK_END);
				cfs_seek(fd_read, 0, CFS_SEEK_SET);

				jsonModel = malloc(length + 1);

				if((cfs_read(fd_read, jsonModel, length + 1)) != -1) {
					printf("INFO: new_model JSON loaded in RAM\n");
				} else {
					printf("ERROR: Empty model!\n");
				}

				cfs_close(fd_read);

				struct jsonparse_state jsonState;

				jsonparse_setup(&jsonState, jsonModel, strlen(jsonModel) + 1);
				intiModel = JSONKevDeserializer(&jsonState, jsonparse_next(&jsonState));
				printf("INFO: Deserialized finished\n");
				free(jsonModel);
				/*Visitor *visitor = malloc(sizeof(Visitor));
				visitor->action = actionprintf;
				visitor->secondAction = NULL;

				intiModel->Visit(intiModel, visitor);*/
			}
			/* TODO Afterwards, just call notifyNewModel */
			if (intiModel != NULL) {
				int err;
				if ((err = notifyNewModel(intiModel)) == PROCESS_ERR_OK) {
					printf("INFO: Model was successfully sent\n");
				} else {
					printf("ERROR: Something is wrong sending the model, err: %d\n", err);
				}
			} else {
				printf("ERROR: The model cannot be loaded!\n");
			}
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
