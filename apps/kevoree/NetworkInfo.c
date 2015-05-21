#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "NamedElement.h"
#include "NetworkProperty.h"
#include "ContainerNode.h"
#include "Visitor.h"
#include "NetworkInfo.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initNetworkInfo(NetworkInfo * const this)
{
	/*
	 * Initialize parent
	 */
	initNamedElement((NamedElement*)this);

	/*
	 * Initialize itself
	 */
	this->values = NULL;
}

static char
*NetworkInfo_internalGetKey(NetworkInfo * const this)
{
	return namedElement_VT.internalGetKey((NamedElement*)this);
}

static char
*NetworkInfo_metaClassName(NetworkInfo * const this)
{
	return "NetworkInfo";
}

static void
NetworkInfo_addValues(NetworkInfo * const this, NetworkProperty *ptr)
{
	NetworkProperty* container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The NetworkProperty cannot be added in NetworkInfo because the key is not defined\n");
	} else {
		if(this->values == NULL) {
			this->values = hashmap_new();
		}
		if(hashmap_get(this->values, internalKey, (void**)(&container)) == MAP_MISSING) {
			if(hashmap_put(this->values, internalKey, ptr) == MAP_OK) {
				ptr->eContainer = strdup(this->path);
				ptr->path = malloc(sizeof(char) * (strlen(this->path) + strlen("/values[]") + strlen(internalKey)) + 1);
				sprintf(ptr->path, "%s/values[%s]", this->path, internalKey);
			} else {
				PRINTF("ERROR: value cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in values map\n", internalKey);
		}
	}
}

static void
NetworkInfo_removeValues(NetworkInfo * const this, NetworkProperty *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The NetworkProperty cannot be removed in NetworkInfo because the key is not defined\n");
	} else {
		if(hashmap_remove(this->values, internalKey) == MAP_OK) {
			free(ptr->eContainer);
			ptr->eContainer = NULL;
			free(ptr->path);
			ptr->path = NULL;
		} else {
			PRINTF("ERROR: value %s cannot be removed!\n", internalKey);
		}
	}
}

static NetworkProperty
*NetworkInfo_findValuesByID(NetworkInfo * const this, char *id)
{
	NetworkProperty* value = NULL;

	if(this->values != NULL) {
		if(hashmap_get(this->values, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: NetworkProperty %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no values in ContainerNode!\n");
		return NULL;
	}

}

void delete_NetworkInfo(NetworkInfo * const this)
{
	/* destroy base object */
	namedElement_VT.delete((NamedElement*)this);

	/* destroy data members */
	if (this->values != NULL) {
		deleteContainerContents(this->values);
		hashmap_free(this->values);
	}

}

static void
NetworkInfo_visit(NetworkInfo * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	/*
	 * Visit parent
	 */
	namedElement_VT.visit((NamedElement*)this, parent, action, secondAction, visitPaths);

	hashmap_map *m;
	int length;

	if((m = (hashmap_map*)this->values) != NULL) {
		length = hashmap_length(this->values);
		if (visitPaths) {
			sprintf(path,"%s/values", parent);
			Visitor_visitPaths(m, "values", path, action, secondAction);
		} else {
			action("values", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKET, NULL);
		}
	} else if (!visitPaths) {
		action("values", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKET, NULL);
	}

}

static void
*NetworkInfo_findByPath(NetworkInfo * const this, char *attribute)
{
	/* NamedElement attributes */
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

	if(strchr(path, '[') != NULL)
	{
		obj = strdup(strtok(path, "["));
		strcpy(path, attribute);
		PRINTF("Object: %s\n", obj);
		strcpy(token, strtok(path, "]"));
		strcpy(path, attribute);
		sprintf(token, "%s]", token);
		PRINTF("Token: %s\n", token);
		sscanf(token, "%*[^[][%[^]]", key);
		PRINTF("Key: %s\n", key);

		if((strchr(path, '\\')) != NULL)
		{
			nextAttribute = strtok(NULL, "\\");
			PRINTF("Attribute: %s\n", nextAttribute);

			if(strchr(nextAttribute, '['))
			{
				sprintf(nextPath, "%s\\%s", ++nextAttribute, strtok(NULL, "\\"));
				PRINTF("Next Path: %s\n", nextPath);
			}
			else
			{
				strcpy(nextPath, nextAttribute);
				PRINTF("Next Path: %s\n", nextPath);
			}
		}
		else
		{
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
	}
	else
	{
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

	if(!strcmp("values", obj))
	{
		free(obj);
		if(nextAttribute == NULL)
		{
			return this->VT->findValuesByID(this, key);
		}
		else
		{
			NetworkProperty* netprop = this->VT->findValuesByID(this, key);
			if(netprop != NULL) {
				return netprop->VT->findByPath(netprop, nextPath);
			} else {
				PRINTF("ERROR: Cannot retrieve values %s\n", key);
				return NULL;
			}
		}
	}
	else
	{
		free(obj);
		return namedElement_VT.findByPath((NamedElement*)this, attribute);
	}
}

const NetworkInfo_VT networkInfo_VT = {
		.super = &namedElement_VT,
		/*
		 * KMFContainer
		 * NamedElement
		 */
		.metaClassName = NetworkInfo_metaClassName,
		.internalGetKey = NetworkInfo_internalGetKey,
		.visit = NetworkInfo_visit,
		.findByPath = NetworkInfo_findByPath,
		.delete = delete_NetworkInfo,
		/*
		 * NetworkInfo
		 */
		.addValues = NetworkInfo_addValues,
		.removeValues = NetworkInfo_removeValues,
		.findValuesByID = NetworkInfo_findValuesByID
};

NetworkInfo
*new_NetworkInfo()
{
	NetworkInfo* pNetInfoObj = NULL;

	/* Allocating memory */
	pNetInfoObj = malloc(sizeof(NetworkInfo));

	if (pNetInfoObj == NULL) {
		PRINTF("ERROR: Cannot create NetworkInfo!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pNetInfoObj->VT = &networkInfo_VT;

	/*
	 * NetworkInfo
	 */
	initNetworkInfo(pNetInfoObj);

	return pNetInfoObj;
}
