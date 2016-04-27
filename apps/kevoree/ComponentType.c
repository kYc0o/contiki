#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "PortTypeRef.h"
#include "NamedElement.h"
#include "TypeDefinition.h"
#include "Visitor.h"
#include "ComponentType.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initComponentType(ComponentType * const this)
{
	/*
	 * Initialize parent
	 */
	initTypeDefinition((TypeDefinition*)this);

	/*
	 * Initialize itself
	 */
	this->required = NULL;
	this->provided = NULL;
}

static char
*ComponentType_internalGetKey(ComponentType * const this)
{
	return typeDefinition_VT.internalGetKey((TypeDefinition*)this);
}

static char
*ComponentType_metaClassName(ComponentType * const this)
{
	return "ComponentType";
}

static PortTypeRef
*ComponentType_findRequiredByID(ComponentType * const this, char *id)
{
	PortTypeRef* value = NULL;

	if(hashmap_get(this->required, id, (void**)(&value)) == MAP_OK)
		return value;
	else
		return NULL;
}

static PortTypeRef
*ComponentType_findProvidedByID(ComponentType * const this, char *id)
{
	PortTypeRef* value = NULL;

	if(this->provided != NULL) {
		if(hashmap_get(this->provided, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: provided %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no provided in ComponentInstance!\n");
		return NULL;
	}
}

static void
ComponentType_addRequired(ComponentType * const this, PortTypeRef *ptr)
{
	PortTypeRef *container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The PortTypeRef cannot be added in ComponentType because the key is not defined");
	} else {
		if(this->required == NULL) {
			this->required = hashmap_new();
		}

		if(hashmap_get(this->required, internalKey, (void**)(&container)) == MAP_MISSING) {
			if(hashmap_put(this->required, internalKey, ptr) == MAP_OK) {
				ptr->eContainer = strdup(this->path);
				ptr->path = malloc(sizeof(char) * (strlen(this->path) +	strlen("/required[]") +	strlen(internalKey)) + 1);
				sprintf(ptr->path, "%s/required[%s]", this->path, internalKey);
			} else {
				PRINTF("ERROR: required cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in bindings map\n", internalKey);
		}
	}
}

static void
ComponentType_addProvided(ComponentType * const this, PortTypeRef *ptr)
{
	PortTypeRef *container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The PortTypeRef cannot be added in ComponentType because the key is not defined");
	} else {
		if(this->provided == NULL) {
			this->provided = hashmap_new();
		}
		if(hashmap_get(this->provided, internalKey, (void**)(&container)) == MAP_MISSING) {
			if(hashmap_put(this->provided, internalKey, ptr) == MAP_OK) {
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
ComponentType_removeRequired(ComponentType * const this, PortTypeRef *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The PortTypeRef cannot be removed in ComponentType because the key is not defined\n");
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
ComponentType_removeProvided(ComponentType * const this, PortTypeRef *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("The PortTypeRef cannot be removed in ComponentType because the key is not defined\n");
	} else {
		if(hashmap_remove(this->provided, internalKey) == MAP_OK) {
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
delete_ComponentType(ComponentType * const this)
{
	/* destroy base object */
	typeDefinition_VT.delete((TypeDefinition*)this);

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

static void
ComponentType_visit(ComponentType * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	/* TypeDefinition */
	typeDefinition_VT.visit((TypeDefinition*)this, parent, action, secondAction, visitPaths);

	if (!visitPaths) {
		action(NULL, COLON, NULL);
	}

	/* ComponentType */
	Visitor_visitContainer(this->required, "required", parent, action, secondAction, visitPaths);

	hashmap_map *m;
	int length;

	if((m = (hashmap_map*)this->provided) != NULL) {
		length = hashmap_length(this->provided);
		if (visitPaths) {
			sprintf(path,"%s/provided", parent);
			Visitor_visitPaths(m, "provided", path, action, secondAction);
		} else {
			action("provided", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKET, NULL);
		}
	} else if (!visitPaths) {
		action("provided", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKET, NULL);
	}

	/*Visitor_visitContainer(this->provided, "provided", parent, action, secondAction, visitPaths);*/
}

static void
*ComponentType_findByPath(ComponentType * const this, char *attribute)
{
	/* There is no local attributes */
	/* TypeDefinition attributes */
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

	if(!strcmp("required", obj)) {
		free(obj);
		if(nextAttribute == NULL) {
			return this->VT->findRequiredByID(this, key);
		} else {
			PortTypeRef* ptypref = this->VT->findRequiredByID(this, key);
			if(ptypref != NULL) {
				return ptypref->VT->findByPath(ptypref, nextPath);
			} else {
				return NULL;
			}
		}
	} else if(!strcmp("provided", obj)) {
		free(obj);
		if(nextAttribute == NULL) {
			return this->VT->findProvidedByID(this, key);
		} else {
			PortTypeRef* ptypref = this->VT->findProvidedByID(this, key);
			if(ptypref != NULL) {
				return ptypref->VT->findByPath(ptypref, nextPath);
			} else {
				PRINTF("ERROR: Cannot retrieve provided %s\n", key);
				return NULL;
			}
		}
	} else {
		free(obj);
		return typeDefinition_VT.findByPath((TypeDefinition*)this, attribute);
	}
}

const ComponentType_VT componentType_VT = {
		.super = &typeDefinition_VT,
		/*
		 * KMFContainer
		 * NamedElement
		 */
		.metaClassName = ComponentType_metaClassName,
		.internalGetKey = ComponentType_internalGetKey,
		.visit = ComponentType_visit,
		.findByPath = ComponentType_findByPath,
		.delete = delete_ComponentType,
		/*
		 * TypeDefinition
		 */
		.addDeployUnit = TypeDefinition_addDeployUnit,
		.addDictionaryType = TypeDefinition_addDictionaryType,
		.addSuperTypes = TypeDefinition_addSuperTypes,
		.removeDeployUnit = TypeDefinition_removeDeployUnit,
		.removeDictionaryType = TypeDefinition_removeDictionaryType,
		.removeSuperTypes = TypeDefinition_removeSuperTypes,
		/*
		 * ComponentType
		 */
		.findRequiredByID = ComponentType_findRequiredByID,
		.findProvidedByID = ComponentType_findProvidedByID,
		.addRequired = ComponentType_addRequired,
		.addProvided = ComponentType_addProvided,
		.removeRequired = ComponentType_removeRequired,
		.removeProvided = ComponentType_removeProvided
};

ComponentType
*new_ComponentType(void)
{
	ComponentType* pCompTypeObj = NULL;

	/* Allocating memory */
	pCompTypeObj = malloc(sizeof(ComponentType));

	if (pCompTypeObj == NULL) {
		PRINTF("ERROR: Cannot create ComponentType!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pCompTypeObj->VT = &componentType_VT;

	/*
	 * ComponentType
	 */
	initComponentType(pCompTypeObj);

	return pCompTypeObj;
}
