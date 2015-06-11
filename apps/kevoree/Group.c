#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tools.h"
#include "ContainerNode.h"
#include "Instance.h"
#include "ContainerRoot.h"
#include "Group.h"
#include "NamedElement.h"
#include "Visitor.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initGroup(Group * const this)
{
	/*
	 * Initialize parent
	 */
	initInstance((Instance*)this);

	/*
	 * Initialize itself
	 */
	this->subNodes = NULL;
}

static char
*Group_internalGetKey(Group * const this)
{
	return instance_VT.internalGetKey((Instance*)this);
}

static char
*Group_metaClassName(Group * const this)
{
	return "Group";
}

static ContainerNode
*Group_findSubNodesByID(Group * const this, char *id)
{
	ContainerNode* value = NULL;

	if(this->subNodes != NULL) {
		if(hashmap_get(this->subNodes, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: ContainerNode %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no subNodes in Group!\n");
		return NULL;
	}
}

static void
Group_addSubNodes(Group * const this, ContainerNode *ptr)
{
	ContainerNode* container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The ContainerNode cannot be added in Group because the key is not defined\n");
	} else {
		if(this->subNodes == NULL) {
			this->subNodes = hashmap_new(get_key_for_hashmap);
		}
		if(hashmap_get(this->subNodes, internalKey, (void**)(&container)) == MAP_MISSING) {
			if ((hashmap_put(this->subNodes, internalKey, ptr)) == MAP_OK) {

			} else {
				PRINTF("ERROR: subNodes cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in subNodes map\n", internalKey);
		}
	}
}

static void
Group_removeSubNodes(Group * const this, ContainerNode *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The ContainerNode cannot be removed in Group because the key is not defined\n");
	} else {
		if ((hashmap_remove(this->subNodes, internalKey)) == MAP_OK) {

		} else {
			PRINTF("ERROR: subNodes %s cannot be removed!\n", internalKey);
		}
	}
}

static void
delete_Group(Group * const this)
{
	instance_VT.delete((Instance*)this);

	if (this->subNodes != NULL) {
		/*deleteContainerContents(this->subNodes);*/
		hashmap_free(this->subNodes);
	}
}

static void
Group_visit(Group * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	/* Instance */
	instance_VT.visit((Instance*)this, parent, action, secondAction, visitPaths);

	/* Group */
	hashmap_map *m;
	int length;

	if((m = (hashmap_map*)this->subNodes) != NULL) {
		length = hashmap_length(this->subNodes);
		if (visitPaths) {
			Visitor_visitPathRefs(m, "subNodes", path, action, secondAction, parent);
		} else {
			action("subNodes", SQBRACKET, NULL);
			Visitor_visitModelRefs(m, length, "nodes", path, action);
			action(NULL, CLOSESQBRACKET, NULL);
		}
	} else if (!visitPaths) {
		action("subNodes", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKET, NULL);
	}
}

void
*Group_findByPath(Group * const this, char *attribute)
{
	/* There is no local attributes */

	/* Instance attributes and references */
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

	if(!strcmp("nodes", obj))
	{
		free(obj);
		if(nextAttribute == NULL) {
			return this->VT->findSubNodesByID(this, key);
		} else {
			ContainerNode* contnode = this->VT->findSubNodesByID(this, key);
			if(contnode != NULL) {
				return contnode->VT->findByPath(contnode, nextPath);
			} else {
				PRINTF("ERROR: Cannot retrieve node %s\n", key);
				return NULL;
			}
		}
	} else {
		free(obj);
		return instance_VT.findByPath((Instance*)this, attribute);
	}
}

static char*
Group_getPath(KMFContainer* kmf)
{
	Group* obj = (Group*)kmf;
	char* tmp = (obj->eContainer)?get_eContainer_path(obj):strdup("");
	char* r = (char*)malloc(strlen(tmp) + strlen("groups[]") + strlen(obj->VT->internalGetKey(obj)) + 1);
	sprintf(r, "groups[%s]", obj->VT->internalGetKey(obj));
	free(tmp);
	return r;
}

const Group_VT group_VT = {
		/*
		 * KMFContainer_VT
		 * NamedElement_VT
		 */
		.super = &instance_VT,
		.metaClassName = Group_metaClassName,
		.internalGetKey = Group_internalGetKey,
		.getPath = Group_getPath,
		.visit = Group_visit,
		.findByPath = Group_findByPath,
		.delete = delete_Group,
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
		 * Group_VT
		 */
		.addSubNodes = Group_addSubNodes,
		.removeSubNodes = Group_removeSubNodes,
		.findSubNodesByID = Group_findSubNodesByID
};

Group
*new_Group()
{
	Group *pGroupObj = NULL;

	/* Allocating memory */
	pGroupObj = malloc(sizeof(Group));

	if (pGroupObj == NULL) {
		PRINTF("ERROR: Group cannot be created!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pGroupObj->VT = &group_VT;
	/*
	 * Group
	 */
	initGroup(pGroupObj);

	return pGroupObj;
}

