#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ContainerNode.h"
#include "NodeLink.h"
#include "Visitor.h"
#include "tools.h"
#include "NodeNetwork.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initNodeNetwork(NodeNetwork * const this)
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
	this->link = NULL;
	this->initBy = NULL;
	this->target = NULL;
}

static char
*NodeNetwork_internalGetKey(NodeNetwork * const this)
{
	return this->generated_KMF_ID;
}

static char
*NodeNetwork_metaClassName(NodeNetwork * const this)
{
	return "NodeNetwork";
}

static NodeLink
*NodeNetwork_findLinkByID(NodeNetwork* const this, char* id)
{
	NodeLink* value = NULL;

	if(this->link != NULL)
	{
		if(hashmap_get(this->link, id, (void**)(&value)) == MAP_OK)
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
NodeNetwork_addLink(NodeNetwork * const this, NodeLink *ptr)
{
	NodeLink* container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL)
	{
		PRINTF("ERROR: The NodeLink cannot be added in NodeNetwork because the key is not defined\n");
	}
	else
	{
		if(this->link == NULL)
		{
			this->link = hashmap_new(get_key_for_hashmap);
		}
		if(hashmap_get(this->link, internalKey, (void**)(&container)) == MAP_MISSING)
		{
			/*container = (NodeLink*)ptr;*/
			if(hashmap_put(this->link, internalKey, ptr) == MAP_OK)
			{
				while (1) {
					printf("where is this path coming from %s %s %d\n", "nodeNetwork[]", __FILE__, __LINE__);
				}
				//if (ptr->eContainer) { printf("muy mal in %s\n", __FILE__, __LINE__); free(ptr->eContainer); }
				//ptr->eContainer = malloc(sizeof(char) * (strlen("nodeNetwork[]") + strlen(internalKey)) + 1);
				//sprintf(ptr->eContainer, "nodeNetwork[%s]", internalKey);
			}
		}
	}
}

static void
NodeNetwork_addInitBy(NodeNetwork * const this, ContainerNode *ptr)
{
	this->initBy = ptr;
}

static void
NodeNetwork_addTarget(NodeNetwork * const this, ContainerNode *ptr)
{
	this->target = ptr;
}

static void
NodeNetwork_removeLink(NodeNetwork * const this, NodeLink *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL)
	{
		PRINTF("The NodeLink cannot be removed in NodeNetwork because the key is not defined\n");
	}
	else
	{
		if(hashmap_remove(this->link, internalKey) == MAP_OK)
		{
			ptr->eContainer = NULL;
			free(internalKey);
		}
	}
}

static void
NodeNetwork_removeInitBy(NodeNetwork * const this, ContainerNode *ptr)
{
	this->initBy = NULL;
}

static void
NodeNetwork_removeTarget(NodeNetwork * const this, ContainerNode *ptr)
{
	this->target = NULL;
}

static void
delete_NodeNetwork(NodeNetwork * const this)
{
	/* destroy base object */
	KMF_VT.delete((KMFContainer*)this);

	/* destroy data members */
	if (this->link != NULL) {
		deleteContainerContents(this->link);
		hashmap_free(this->link);
	}
}

static void
NodeNetwork_visit(NodeNetwork * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	hashmap_map* m = NULL;

	int length;

	if (visitPaths) {
		sprintf(path,"%s\\ID", parent);
		action(path, STRING, this->generated_KMF_ID);
	} else {
		/*
		 * Visit parent
		 */
		KMF_VT.visit((KMFContainer*)this, parent, action, secondAction, visitPaths);
		action("ID", STRING, this->VT->metaClassName(this));
		action(NULL, RETURN, NULL);
	}


	if(this->target != NULL) {
		if (visitPaths) {
			char* tmp_path = this->target->VT->getPath(this->target);
			sprintf(path,"%s/%s\\target", parent, tmp_path);
			free(tmp_path);
			action(path, REFERENCE, parent);
		} else {
			action("target", SQBRACKET, NULL);
			sprintf(path, "target[%s]", this->target->VT->internalGetKey(this->target));
			action(path, STRREF, NULL);
			action(NULL, RETURN, NULL);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("target", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}


	if(this->initBy != NULL) {
		if (visitPaths) {
			char* tmp_path = this->initBy->VT->getPath(this->initBy);
			sprintf(path,"%s/%s\\initBy", parent, tmp_path);
			free(tmp_path);
			action(path, REFERENCE, parent);
		} else {
			action("initBy", SQBRACKET, NULL);
			sprintf(path, "initBy[%s]", this->initBy->VT->internalGetKey(this->initBy));
			action(path, STRREF, NULL);
			action(NULL, RETURN, NULL);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("initBy", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}

	if((m = (hashmap_map*)this->link) != NULL) {
		length = hashmap_length(this->link);
		if (visitPaths) {
			sprintf(path,"%s/link", parent);
			Visitor_visitPaths(m, "link", path, action, secondAction);
		} else {
			action("link", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKET, NULL);
		}
	} else if (!visitPaths) {
		action("link", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKET, NULL);
	}


}

static void
*NodeNetwork_findByPath(NodeNetwork * const this, char *attribute)
{
	/* KMFContainer attributes */
	if(!strcmp("generated_KMF_ID", attribute))
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

		if(!strcmp("link", obj))
		{
			free(obj);
			if(nextAttribute == NULL)
			{
				return this->VT->findLinkByID(this, key);
			}
			else
			{
				NodeLink* nodelink = this->VT->findLinkByID(this, key);
				if(nodelink != NULL)
					return nodelink->VT->findByPath(nodelink, nextPath);
				else
					return NULL;
			}
		}
		else if(!strcmp("initBy", obj))
		{
			free(obj);
			if(nextAttribute == NULL)
			{
				return this->initBy;
			}
			else
			{
				return this->initBy->VT->findByPath(this->initBy, nextPath);
			}
		}
		else if(!strcmp("target", obj))
		{
			free(obj);
			if(nextAttribute == NULL)
			{
				return this->target;
			}
			else
			{
				return this->target->VT->findByPath(this->target, nextPath);
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

static char*
NodeNetwork_getPath(KMFContainer* kmf)
{
	NodeNetwork* obj = (NodeNetwork*)kmf;
	char* tmp = (obj->eContainer)?get_eContainer_path(obj):strdup("");
	char* r = (char*)malloc(strlen(tmp) + strlen("nodeNetworks[]") + strlen(obj->VT->internalGetKey(obj)) + 1);
	sprintf(r, "nodeNetworks[%s]", obj->VT->internalGetKey(obj));
	free(tmp);
	return r;
}

const NodeNetwork_VT nodeNetwork_VT = {
		.super = &KMF_VT,
		/*
		 * KMFContainer
		 */
		.metaClassName = NodeNetwork_metaClassName,
		.internalGetKey = NodeNetwork_internalGetKey,
		.getPath = NodeNetwork_getPath,
		.visit = NodeNetwork_visit,
		.findByPath = NodeNetwork_findByPath,
		.delete = delete_NodeNetwork,
		/*
		 * NodeNetwork
		 */
		.findLinkByID = NodeNetwork_findLinkByID,
		.addLink = NodeNetwork_addLink,
		.addInitBy = NodeNetwork_addInitBy,
		.addTarget = NodeNetwork_addTarget,
		.removeLink = NodeNetwork_removeLink,
		.removeInitBy = NodeNetwork_removeInitBy,
		.removeTarget = NodeNetwork_removeTarget
};

NodeNetwork
*new_NodeNetwork()
{
	NodeNetwork* pObj = NULL;

	/* Allocating memory */
	pObj = malloc(sizeof(NodeNetwork));

	if (pObj == NULL) {
		PRINTF("ERROR: Cannot create NodeNetwork!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pObj->VT = &nodeNetwork_VT;

	/*
	 * NodeNetwork
	 */
	initNodeNetwork(pObj);

	return pObj;
}
