#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "NamedElement.h"
#include "Instance.h"
#include "Port.h"
#include "TypeDefinition.h"
#include "ContainerNode.h"
#include "Visitor.h"
#include "ComponentInstance.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initComponentInstance(ComponentInstance * const this)
{
	/*
	 * Initialize parent
	 */
	initInstance((Instance*)this);

	/*
	 * Initialize itself
	 */
	this->provided = NULL;
	this->required = NULL;
}

static char
*ComponentInstance_internalGetKey(ComponentInstance * const this)
{
	return instance_VT.internalGetKey((Instance*)this);
}

static char
*ComponentInstance_metaClassName(ComponentInstance * const this)
{
	return "ComponentInstance";
}

static void
delete_ComponentInstance(ComponentInstance * const this)
{
	/* destroy base object */
	instance_VT.delete((Instance*)this);
	/* destroy data members */
	if (this->required != NULL) {
		deleteContainerContents(this->required);
		hashmap_free(this->required);
	}

	if (this->provided != NULL) {
		deleteContainerContents(this->provided);
		hashmap_free(this->provided);
	}
}

static Port
*ComponentInstance_findProvidedByID(ComponentInstance * const this, char *id)
{
	Port *value = NULL;

	if(this->provided != NULL) {
		if(hashmap_get(this->provided, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: Port %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no provided in ComponentInstance!\n");
		return NULL;
	}
}

static Port
*ComponentInstance_findRequiredByID(ComponentInstance * const this, char *id)
{
	Port *value = NULL;

	if(this->required != NULL) {
		if(hashmap_get(this->required, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: Port %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no required in ComponentInstance!\n");
		return NULL;
	}
}

static void
ComponentInstance_addProvided(ComponentInstance * const this, Port *ptr)
{
	Port *container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The Port cannot be added in ComponentInstance because the key is not defined\n");
	} else {
		if(this->provided == NULL) {
			/*
			 * TODO add if == NULL
			 */
			this->provided = hashmap_new();
		}
		if(hashmap_get(this->provided, internalKey, (void**)(&container)) == MAP_MISSING) {
			/*container = (MBinding*)ptr;*/
			if(hashmap_put(this->provided, internalKey, ptr) == MAP_OK) {
				/*
				 * TODO add if == NULL
				 */
				if (ptr->eContainer) {
					free(ptr->eContainer);
				}
				ptr->eContainer = strdup(this->path);
				ptr->path = malloc(sizeof(char) * (strlen(this->path) +	strlen("/provided[]") +	strlen(internalKey)) + 1);
				sprintf(ptr->path, "%s/provided[%s]", this->path, internalKey);
			} else {
				PRINTF("ERROR: provided cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in provided map\n", internalKey);
		}
	}
}

static void
ComponentInstance_addRequired(ComponentInstance * const this, Port *ptr)
{
	Port *container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The Port cannot be added in ComponentInstance because the key is not defined\n");
	} else {
		if(this->required == NULL) {
			this->required = hashmap_new();
		}
		if(hashmap_get(this->required, internalKey, (void**)(&container)) == MAP_MISSING) {
			if(hashmap_put(this->required, internalKey, ptr) == MAP_OK)
			{
				ptr->eContainer = strdup(this->path);
				ptr->path = malloc(sizeof(char) * (strlen(this->path) +	strlen("/required[]") +	strlen(internalKey)) + 1);
				sprintf(ptr->path, "%s/required[%s]", this->path, internalKey);
			} else {
				PRINTF("ERROR: required cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in required map\n", internalKey);
		}
	}
}

static void
ComponentInstance_removeProvided(ComponentInstance * const this, Port *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The Port cannot be removed in ComponentInstance because the key is not defined\n");
	} else {
		if(hashmap_remove(this->provided, internalKey) == MAP_OK) {
			free(ptr->eContainer);
			ptr->eContainer = NULL;
			free(ptr->path);
			ptr->path = NULL;
		} else {
			PRINTF("ERROR: provided %s cannot be removed!\n", internalKey);
		}
	}
}

static void
ComponentInstance_removeRequired(ComponentInstance * const this, Port *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The Port cannot be removed in ComponentInstance because the key is not defined\n");
	} else {
		if(hashmap_remove(this->required, internalKey) == MAP_OK) {
			free(ptr->eContainer);
			ptr->eContainer = NULL;
			free(ptr->path);
			ptr->path = NULL;
		} else {
			PRINTF("ERROR: required %s cannot be removed!\n", internalKey);
		}
	}
}

static void
ComponentInstance_visit(ComponentInstance * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	/*
	 * Visit parent
	 */
	instance_VT.visit((Instance*)this, parent, action, secondAction, visitPaths);

	/*
	 * Visit ComponentInstance
	 */
	hashmap_map* m = NULL;

	int length;

	if((m = (hashmap_map*)this->provided) != NULL) {
		length = hashmap_length(this->provided);
		if (visitPaths) {
			sprintf(path,"%s/provided", parent);
			Visitor_visitPaths(m, "provided", path, action, secondAction);
		} else {
			action("provided", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("provided", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}

	if((m = (hashmap_map*)this->required) != NULL) {
		length = hashmap_length(this->required);
		if (visitPaths) {
			sprintf(path,"%s/required", parent);
			Visitor_visitPaths(m, "required", path, action, secondAction);
		} else {
			action("required", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKET, NULL);
		}
	} else if (!visitPaths) {
		action("required", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKET, NULL);
	}
}

static void
*ComponentInstance_findByPath(ComponentInstance * const this, char *attribute)
{
	/* There is no local attributes */

	/* Instance */
	/* Local references */
	char path[250];
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
		obj = strdup(attribute);
		if ((nextAttribute = strtok(path, "\\")) != NULL) {
			if ((nextAttribute = strtok(NULL, "\\")) != NULL) {
				PRINTF("Attribute: %s\n", nextAttribute);
			} else {
				nextAttribute = strtok(path, "\\");
				PRINTF("Attribute: %s\n", nextAttribute);
			}
		}
	}

	if(!strcmp("provided", obj)) {
		free(obj);
		if(nextAttribute == NULL) {
			return this->VT->findProvidedByID(this, key);
		} else {
			Port* port = this->VT->findProvidedByID(this, key);
			if(port != NULL) {
				return port->VT->findByPath(port, nextPath);
			} else {
				PRINTF("ERROR: Cannot retrieve provided %s\n", key);
				return NULL;
			}
		}
	} else if(!strcmp("required", obj)) {
		free(obj);
		if(nextAttribute == NULL) {
			return this->VT->findRequiredByID(this, key);
		} else {
			Port* port = this->VT->findRequiredByID(this, key);
			if(port != NULL)
				return port->VT->findByPath(port, nextPath);
			else
				return NULL;
		}
	} else {
		free(obj);
		return instance_VT.findByPath((Instance*)this, attribute);
	}
}

const ComponentInstance_VT componentInstance_VT = {
		/*
		 * KMFContainer_VT
		 * NamedElement_VT
		 */
		.super = &instance_VT,
		.metaClassName = ComponentInstance_metaClassName,
		.internalGetKey = ComponentInstance_internalGetKey,
		.visit = ComponentInstance_visit,
		.findByPath = ComponentInstance_findByPath,
		.delete = delete_ComponentInstance,
		/*
		 * Instance_VT
		 */
		.findFragmentDictionaryByID = Instance_findFragmentDictionaryByID,
		.addTypeDefinition = Instance_addTypeDefinition,
		.addDictionary = Instance_addDictionary,
		.addFragmentDictionary = Instance_addFragmentDictionary,
		.removeTypeDefinition = Instance_removeTypeDefinition,
		.removeDictionary = Instance_removeDictionary,
		.removeFragmentDictionary = Instance_removeFragmentDictionary,
		/*
		 * ComponentInstance_VT
		 */
		.findProvidedByID = ComponentInstance_findProvidedByID,
		.findRequiredByID = ComponentInstance_findRequiredByID,
		.addProvided = ComponentInstance_addProvided,
		.addRequired = ComponentInstance_addRequired,
		.removeProvided = ComponentInstance_removeProvided,
		.removeRequired = ComponentInstance_removeRequired
};

ComponentInstance
*new_ComponentInstance()
{
	ComponentInstance* pCompInstanceObj = NULL;

	/* Allocating memory */
	pCompInstanceObj = malloc(sizeof(ComponentInstance));

	if (pCompInstanceObj == NULL) {
		PRINTF("ERROR: Cannot create ComponentInstance!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pCompInstanceObj->VT = &componentInstance_VT;

	/*
	 * ComponentInstance
	 */
	initComponentInstance(pCompInstanceObj);

	return pCompInstanceObj;
}
