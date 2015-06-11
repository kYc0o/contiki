#include <string.h>
#include <stdio.h>

#include "Instance.h"
#include "ContainerRoot.h"
#include "MBinding.h"
#include "tools.h"
#include "Channel.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initChannel(Channel * const this)
{
	/*
	 * Initialize parent
	 */
	initInstance((Instance*)this);

	/*
	 * Initialize itself
	 */
	this->bindings = NULL;
}

void
delete_Channel(Channel * const this)
{
	/* destroy base object */
	instance_VT.delete((NamedElement*)this);
	/* destroy data memebers */
	if (this->bindings != NULL) {
		/*deleteContainerContents(this->bindings);*/
		hashmap_free(this->bindings);
	}
}

static char
*Channel_metaClassName(Channel * const this)
{
	return "Channel";
}

static char
*Channel_internalGetKey(Channel * const this)
{
	return instance_VT.internalGetKey((NamedElement*)this);
}

static void
Channel_addBindings(Channel * const this, MBinding *ptr)
{
	/*
	 * TODO change map by reference 1..1
	 */
	MBinding* container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The MBinding cannot be added in Channel because the key is not defined\n");
	} else {
		if(this->bindings == NULL) {
			/*
			 * TODO add if == NULL
			 */
			this->bindings = hashmap_new(get_key_for_hashmap);
		}
		if(hashmap_get(this->bindings, internalKey, (void**)(&container)) == MAP_MISSING) {
			if ((hashmap_put(this->bindings, internalKey, ptr)) == MAP_OK) {

			} else {
				PRINTF("ERROR: binding cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in bindings map\n", internalKey);
		}
	}
}

static void
Channel_removeBindings(Channel * const this, MBinding *ptr)
{
	/*
	 * TODO change map by reference 1..1
	 * TODO remove reference too
	 */
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The MBinding cannot be removed in Channel because the key is not defined\n");
	} else {
		if(hashmap_remove(this->bindings, internalKey) == MAP_OK) {

		} else {
			PRINTF("ERROR: binding %s cannot be removed!\n", internalKey);
		}
	}
}

static MBinding
*Channel_findBindingsByID(Channel * const this, char *id)
{
	MBinding* value = NULL;

	if(this->bindings != NULL) {
		if(hashmap_get(this->bindings, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: binding %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no bindings in Channel!\n");
		return NULL;
	}
}

static void
Channel_visit(Channel * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	/* Instance */
	instance_VT.visit((Instance*)this, parent, action, secondAction, visitPaths);

	/* Channel */
	hashmap_map* m = NULL;
	int length;

	if((m = (hashmap_map*)this->bindings) != NULL) {
		length = hashmap_length(this->bindings);
		if (visitPaths) {
			Visitor_visitPathRefs(m, "bindings", path, action, secondAction, parent);
		} else {
			action("bindings", SQBRACKET, NULL);
			Visitor_visitModelRefs(m, length, "mBindings", path, action);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("bindings", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}
}

static void
*Channel_findByPath(Channel * const this, char *attribute)
{
	/* There is no local attributes */

	/* Local references */
	char path[150];
	memset(&path[0], 0, sizeof(path));
	char token[100];
	memset(&token[0], 0, sizeof(token));
	char *obj = NULL;
	char key[50];
	memset(&key[0], 0, sizeof(key));
	char nextPath[150];
	memset(&nextPath[0], 0, sizeof(nextPath));
	char *nextAttribute = NULL;

	strcpy(path, attribute);

	if(strchr(path, '[') != NULL) {
		obj = strdup(strtok(path, "["));
		strcpy(path, attribute);
		PRINTF("Object: %s\n", obj);
		strcpy(token, strtok(path, "]"));
		strcpy(path, attribute);
		sprintf(token, "%s]", token);
		PRINTF("Token: %s\n", token);
		sscanf(token, "%*[^[][%[^]]", key);
		PRINTF("Key: %s\n", key);

		if((strchr(path, '\\')) != NULL) {
			nextAttribute = strtok(NULL, "\\");
			PRINTF("Attribute: %s\n", nextAttribute);

			if(strchr(nextAttribute, '[')) {
				sprintf(nextPath, "%s\\%s", ++nextAttribute, strtok(NULL, "\\"));
				PRINTF("Next Path: %s\n", nextPath);
			} else {
				strcpy(nextPath, nextAttribute);
				PRINTF("Next Path: %s\n", nextPath);
			}
		} else {
			nextAttribute = strtok(path, "]");
			bool isFirst = true;
			char *fragPath = NULL;
			while ((fragPath = strtok(NULL, "]")) != NULL) {
				PRINTF("Attribute: %s]\n", fragPath);
				if (isFirst) {
					sprintf(nextPath, "%s]", ++fragPath);
					isFirst = false;
				} else {
					sprintf(nextPath, "%s/%s]", nextPath, ++fragPath);
				}
				PRINTF("Next Path: %s\n", nextPath);
			}
			if (strlen(nextPath) == 0) {
				PRINTF("Attribute: NULL\n");
				PRINTF("Next Path: NULL\n");
				nextAttribute = NULL;
			}
		}
	} else {
		if ((nextAttribute = strtok(path, "\\")) != NULL) {
			if ((nextAttribute = strtok(NULL, "\\")) != NULL) {
				PRINTF("Attribute: %s\n", nextAttribute);
			} else {
				nextAttribute = strtok(path, "\\");
				PRINTF("Attribute: %s\n", nextAttribute);
			}
		}
	}

	if(!strcmp("bindings", obj))
	{
		free(obj);
		if(nextAttribute == NULL) {
			return this->VT->findBindingsByID(this, key);
		} else {
			MBinding* binding = this->VT->findBindingsByID(this, key);
			if(binding != NULL) {
				return binding->VT->findByPath(binding, nextPath);
			} else {
				PRINTF("ERROR: Cannot retrieve binding %s\n", key);
				return NULL;
			}
		}
	} else {
		free(obj);
		/* Instance attributes and references */
		return instance_VT.findByPath((Instance*)this, attribute);
	}
}

static char*
Channel_getPath(KMFContainer* kmf)
{
	Channel* channel = (Channel*)kmf;
	char* tmp = (channel->eContainer)?get_eContainer_path(channel):strdup("");
	char* r = (char*)malloc(strlen(tmp) + strlen("hubs[]") + strlen(channel->VT->internalGetKey(channel)) + 1);
	sprintf(r, "hubs[%s]", channel->VT->internalGetKey(channel));
	free(tmp);
	return r;
}

const Channel_VT channel_VT = {
		/*
		 * KMFContainer
		 * NamedElement
		 */
		.super = &instance_VT,
		.metaClassName = Channel_metaClassName,
		.internalGetKey = Channel_internalGetKey,
		.getPath = Channel_getPath,
		.visit = Channel_visit,
		.findByPath = Channel_findByPath,
		.delete = delete_Channel,
		/*
		 * Instance
		 */
		.findFragmentDictionaryByID = Instance_findFragmentDictionaryByID,
		.addTypeDefinition = Instance_addTypeDefinition,
		.addDictionary = Instance_addDictionary,
		.addFragmentDictionary = Instance_addFragmentDictionary,
		.removeTypeDefinition = Instance_removeTypeDefinition,
		.removeDictionary = Instance_removeDictionary,
		.removeFragmentDictionary = Instance_removeFragmentDictionary,
		/*
		 * Channel
		 */
		.findBindingsByID = Channel_findBindingsByID,
		.addBindings = Channel_addBindings,
		.removeBindings = Channel_removeBindings
};

Channel* new_Channel()
{
	Channel* pChannelObj = NULL;

	/* Allocating memory */
	pChannelObj = malloc(sizeof(Channel));

	if (pChannelObj == NULL) {
		PRINTF("ERROR: Cannot create Channel!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pChannelObj->VT = &channel_VT;

	/*
	 * Channel
	 */
	initChannel(pChannelObj);

	return pChannelObj;
}
