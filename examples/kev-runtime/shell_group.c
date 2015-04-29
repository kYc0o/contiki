#include "contiki.h"
#include "cfs/cfs.h"
#include "json.h"
#include "jsonparse.h"

#include "rtkev.h"
#include "shell_group.h"

/* forward declaration */
static void* newShellGroup(const char* name);
static int startShellGroup(void* instance);
static int stopShellGroup(void* instance);
static int updateShellGroup(void* instance);
static ContainerRoot *newModel;

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
	int fd_read;
	char *jsonModel;

	PROCESS_BEGIN();
	static ShellGroup *instance = (ShellGroup*)data;
	printf("Ok, here I have my instance %p\n", (struct ShellGroup*)data);

	/* define new event */
	NEW_MODEL_IN_JSON = process_alloc_event();
	while (1) {
		/* it runs forever, waiting for some update to the model */
		PROCESS_WAIT_EVENT();
		if (ev == NEW_MODEL_IN_JSON) {
			/* wow I have a new model, do te magic with the traces and so on */
			printf("New model received in group\n");
			/* TODO Paco, you should load the model into a ContainerRoot from the json file with name in variable instance->fileNameWithModel*/


			if((fd_read = cfs_open(instance->fileNameWithModel, CFS_READ)) != -1) {
				length = cfs_seek(fd_read, 0 , CFS_SEEK_END);
				cfs_seek(fd_read, 0, CFS_SEEK_SET);

				jsonModel = malloc(length + 1);

				if((cfs_read(fd_read, jsonModel, length + 1)) != -1) {
					printf("INFO: new_model JSON loaded in RAM\n");
					/*printf("%s\n", jsonModel);*/
				} else {
					printf("ERROR: Empty model!\n");
				}

				struct jsonparse_state jsonState;

				jsonparse_setup(&jsonState, jsonModel, strlen(jsonModel) + 1);
				newModel = JSONKevDeserializer(&jsonState, jsonparse_next(&jsonState));

			}
			/* TODO Afterwards, just call notifyNewModel */
			notifyNewModel(newModel);
		}
		else if (ev == PROCESS_EVENT_EXITED || ev == PROCESS_EVENT_EXIT) {
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

	process_start(&shellGroupP, instance);

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
