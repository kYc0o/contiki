#include <string.h>
#include <stdio.h>

#include "NetworkProperty.h"
#include "NodeNetwork.h"
#include "Visitor.h"
#include "tools.h"
#include "NodeLink.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initNodeLink(NodeLink * const this)
{
	/*
	 * Initialize parent
	 */
	initKMFContainer((KMFContainer*)this);

	/*
	 * Initialize itself
	 */
	memset(&this->generated_KMF_ID[0], 0, sizeof(this->generated_KMF_ID));
	rand_str(this->generated_KMF_ID, 8);
	this->networkType = NULL;
	this->estimatedRate = -1;
	this->lastCheck = NULL;
	this->zoneID = NULL;
	this->networkProperties = NULL;
}

static char
*NodeLink_internalGetKey(NodeLink * const this)
{
	return this->generated_KMF_ID;
}

static char
*NodeLink_metaClassName(NodeLink * const this)
{
	return "NodeLink";
}

static NetworkProperty
*NodeLink_findNetworkPropertiesByID(NodeLink * const this, char *id)
{
	NetworkProperty* value = NULL;

	if(this->networkProperties != NULL)
	{
		if(hashmap_get(this->networkProperties, id, (void**)(&value)) == MAP_OK)
			return value;
		else
			return NULL;
	}
	else
	{
		return NULL;
	}

}

static void
NodeLink_addNetworkProperties(NodeLink * const this, NetworkProperty *ptr)
{
	NetworkProperty* container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL)
	{
		PRINTF("The NetworkProperty cannot be added in NodeLink because the key is not defined\n");
	}
	else
	{
		if(this->networkProperties == NULL)
		{
			this->networkProperties = hashmap_new(get_key_for_hashmap);
		}
		if(hashmap_get(this->networkProperties, internalKey, (void**)(&container)) == MAP_MISSING)
		{
			/*container = (NetworkProperty*)ptr;*/
			if(hashmap_put(this->networkProperties, internalKey, ptr) == MAP_OK)
			{
				ptr->eContainer = this;
			}
		}
	}
}

void
NodeLink_removeNetworkProperties(NodeLink * const this, NetworkProperty *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL)
	{
		PRINTF("The NetworkProperty cannot be removed in NodeLink because the key is not defined\n");
	}
	else
	{
		if(hashmap_remove(this->networkProperties, internalKey) == MAP_OK)
		{
			ptr->eContainer = NULL;
		}
	}
}

static void
delete_NodeLink(NodeLink * const this)
{
	/* destroy base object */
	KMF_VT.delete((KMFContainer*)this);

	/* destroy data members */
	if (this->networkProperties != NULL) {
		deleteContainerContents(this->networkProperties);
		hashmap_free(this->networkProperties);
	}
	free(this->networkType);
	free(this->lastCheck);
	free(this->zoneID);
}

static void
NodeLink_visit(NodeLink * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	if (visitPaths) {
		sprintf(path, "%s\\ID", parent);
		action(path, STRING, this->generated_KMF_ID);
		sprintf(path, "%s\\networkType", parent);
		action(path, STRING, this->networkType);
		sprintf(path,"%s\\lastCheck",parent);
		action(path, STRING, this->lastCheck);
		sprintf(path,"%s\\zoneID",parent);
		action(path, STRING, this->zoneID);
		sprintf(path, "%s\\estimatedRate", parent);
		action(path, BOOL, (void*)this->estimatedRate);
	} else {
		/*
		 * Visit parent
		 */
		KMF_VT.visit((KMFContainer*)this, parent, action, secondAction, visitPaths);
		action("ID", STRING, this->generated_KMF_ID);
		action(NULL, COLON, NULL);
		action("networkType", STRING, this->networkType);
		action(NULL, COLON, NULL);
		action("lastCheck", STRING, this->lastCheck);
		action(NULL, COLON, NULL);
		action("zoneID", STRING, this->zoneID);
		action(NULL, COLON, NULL);
		action("estimatedRate", INTEGER, (void*)this->estimatedRate);
		action(NULL, RETURN, NULL);
	}

	hashmap_map *m;
	int length;

	if((m = (hashmap_map*)this->networkProperties) != NULL) {
		length = hashmap_length(this->networkProperties);
		if (visitPaths) {
			sprintf(path,"%s/networkProperties", parent);
			Visitor_visitPaths(m, "networkProperties", path, action, secondAction);
		} else {
			action("networkProperties", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKET, NULL);
		}
	} else if (!visitPaths) {
		action("networkProperties", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKET, NULL);
	}

}

static void
*NodeLink_findByPath(NodeLink * const this, char *attribute)
{
	/* Local attributes */
	if(!strcmp("networkType", attribute))
	{
		return this->networkType;
	}
	else if(!strcmp("estimatedRate", attribute))
	{
		return (void*)this->estimatedRate;
	}
	else if(!strcmp("lastCheck", attribute))
	{
		return this->lastCheck;
	}
	else if(!strcmp("zoneID", attribute))
	{
		return this->zoneID;
	}
	else if(!strcmp("ID", attribute))
	{
		return this->generated_KMF_ID;
	}
	/* Local references */
	else
	{
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
			if ((nextAttribute = strtok(path, "\\")) != NULL) {
				if ((nextAttribute = strtok(NULL, "\\")) != NULL) {
					PRINTF("Attribute: %s\n", nextAttribute);
				} else {
					nextAttribute = strtok(path, "\\");
					PRINTF("Attribute: %s\n", nextAttribute);
				}
			}
		}

		if(!strcmp("networkProperties", obj))
		{
			free(obj);
			if(nextAttribute == NULL)
			{
				return this->VT->findNetworkPropertiesByID(this, key);
			}
			else
			{
				NetworkProperty* netprop = this->VT->findNetworkPropertiesByID(this, key);
				if(netprop != NULL)
					return netprop->VT->findByPath(netprop, nextPath);
				else
					return NULL;
			}
		}
		else
		{
			free(obj);
			PRINTF("WARNING: Object not found %s\n", attribute);
			return NULL;
		}
	}
}

const NodeLink_VT nodeLink_VT = {
		.super = &KMF_VT,
		/*
		 * KMFContainer
		 */
		.metaClassName = NodeLink_metaClassName,
		.internalGetKey = NodeLink_internalGetKey,
		.getPath = KMFContainer_get_path,
		.visit = NodeLink_visit,
		.findByPath = NodeLink_findByPath,
		.delete = delete_NodeLink,
		/*
		 * NodeLink
		 */
		.findNetworkPropertiesByID = NodeLink_findNetworkPropertiesByID,
		.addNetworkProperties = NodeLink_addNetworkProperties,
		.removeNetworkProperties = NodeLink_removeNetworkProperties
};

NodeLink
*new_NodeLink()
{
	NodeLink* pObj = NULL;

	/* Allocating memory */
	pObj = malloc(sizeof(NodeLink));

	if (pObj == NULL) {
		PRINTF("ERROR: Cannot create NodeLink!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pObj->VT = &nodeLink_VT;

	/*
	 * NodeLink
	 */
	initNodeLink(pObj);

	return pObj;
}
