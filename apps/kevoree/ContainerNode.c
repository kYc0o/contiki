#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "NamedElement.h"
#include "Instance.h"
#include "ComponentInstance.h"
#include "Group.h"
#include "Visitor.h"
#include "NetworkInfo.h"
#include "tools.h"
#include "ContainerNode.h"
#include "ActionType.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initContainerNode(ContainerNode * const this)
{
	/*
	 * Initialize parent
	 */
	initInstance((Instance*)this);

	/*
	 * Initialize itself
	 */
	this->components = NULL;
	this->hosts = NULL;
	this->host = NULL;
	this->networkInformation = NULL;
	this->groups = NULL;
}

static char
*ContainerNode_internalGetKey(ContainerNode * const this)
{
	/*ContainerNode *pObj = (ContainerNode*)this;*/
	return this->name;
}

static char
*ContainerNode_metaClassName(void* const this)
{
	return "ContainerNode";
}

static ComponentInstance
*ContainerNode_findComponentsByID(ContainerNode * const this, char *id)
{
	ComponentInstance *value = NULL;

	if(this->components != NULL) {
		if(hashmap_get(this->components, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: ComponentInstance %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no components in ContainerNode!\n");
		return NULL;
	}

}

static ContainerNode
*ContainerNode_findHostsByID(ContainerNode * const this, char *id)
{
	ContainerNode *value = NULL;

	if(this->hosts != NULL) {
		if(hashmap_get(this->hosts, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: ContainerNode %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no hosts in ContainerNode!\n");
		return NULL;
	}

}

static Group
*ContainerNode_findGroupsByID(ContainerNode * const this, char *id)
{
	Group *value = NULL;

	if(this->groups != NULL) {
		if(hashmap_get(this->groups, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: Group %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no groups in ContainerNode!\n");
		return NULL;
	}

}

static NetworkInfo
*ContainerNode_findNetworkInformationByID(ContainerNode * const this, char *id)
{
	NetworkInfo *value = NULL;

	if(this->networkInformation != NULL) {
		if(hashmap_get(this->networkInformation, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: NetworkInfo %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no networkInformation in ContainerNode!\n");
		return NULL;
	}

}

static void
ContainerNode_addComponents(ContainerNode * const this, ComponentInstance *ptr)
{
	ComponentInstance *container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The ComponentInstance cannot be added in ContainerNode because the key is not defined\n");
	} else {
		if(this->components == NULL) {
			this->components = hashmap_new();
		}

		if(hashmap_get(this->components, internalKey, (void**)(&container)) == MAP_MISSING) {
			/*container = (ComponentInstance*)ptr;*/
			if(hashmap_put(this->components, internalKey, ptr) == MAP_OK) {
				ptr->eContainer = strdup(this->path);
				ptr->path = malloc(sizeof(char) * (strlen(this->path) + strlen("/components[]") + strlen(internalKey)) + 1);
				sprintf(ptr->path, "%s/components[%s]", this->path, internalKey);
			} else {
				PRINTF("ERROR: component cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in components map\n", internalKey);
		}
	}
}

static void
ContainerNode_addHosts(ContainerNode * const this, ContainerNode *ptr)
{
	/*
	 * Is reference
	 */
	ContainerNode *container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The ContainerNode cannot be added in ContainerNode because the key is not defined\n");
	} else {
		if(this->hosts == NULL) {
			/*
			 * TODO if result == NULL
			 */
			this->hosts = hashmap_new();
		}
		if(hashmap_get(this->hosts, internalKey, (void**)(&container)) == MAP_MISSING) {
			if ((hashmap_put(this->hosts, internalKey, ptr)) == MAP_OK) {

			} else {
				PRINTF("ERROR: hosts cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in hosts map\n", internalKey);
		}
	}
}

static void
ContainerNode_addHost(ContainerNode * const this, ContainerNode *ptr)
{
	/*
	 * Is reference
	 */
	this->host = ptr;
}

static void
ContainerNode_addGroups(ContainerNode * const this, Group *ptr)
{
	/*
	 * Is reference
	 */
	Group *container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("The Group cannot be added in ContainerNode because the key is not defined\n");
	} else {
		if(this->groups == NULL) {
			this->groups = hashmap_new();
		}
		if(hashmap_get(this->groups, internalKey, (void**)(&container)) == MAP_MISSING) {
			if ((hashmap_put(this->groups, internalKey, ptr)) == MAP_OK) {
				/*PRINTF("INFO: Successfully added Group %s to ContainerNode %s\n", internalKey, this->internalGetKey(this));*/
			} else {
				PRINTF("ERROR: group cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in groups map\n", internalKey);
		}
	}
}

static void
ContainerNode_addNetworkInformation(ContainerNode * const this, NetworkInfo *ptr)
{
	NetworkInfo *container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The NetworkInfo cannot be added in ContainerNode because the key is not defined\n");
	} else {
		if(this->networkInformation == NULL) {
			this->networkInformation = hashmap_new();
		}
		if(hashmap_get(this->networkInformation, internalKey, (void**)(&container)) == MAP_MISSING) {
			/*container = (NetworkInfo*)ptr;*/
			if(hashmap_put(this->networkInformation, internalKey, ptr) == 0) {
				ptr->eContainer = strdup(this->path);
				ptr->path = malloc(sizeof(char) * (strlen(this->path) +	strlen("/networkInformation[]") + strlen(internalKey)) + 1);
				sprintf(ptr->path, "%s/networkInformation[%s]", this->path, internalKey);
			} else {
				PRINTF("ERROR: networkInformation cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in networkInformation map\n", internalKey);
		}
	}
}

static void
ContainerNode_removeComponents(ContainerNode* const this, ComponentInstance* ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: ComponentInstance cannot be removed in ContainerNode because the key is not defined\n");
	} else {
		if(hashmap_remove(this->components, internalKey) == MAP_OK) {
			free(ptr->eContainer);
			ptr->eContainer = NULL;
			free(ptr->path);
			ptr->path = NULL;
		} else {
			PRINTF("ERROR: component %s cannot be removed!\n", internalKey);
		}
	}
}

static void
ContainerNode_removeHosts(ContainerNode * const this, ContainerNode *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: ContainerNode cannot be removed in ContainerNode because the key is not defined\n");
	}
	else
	{
		if ((hashmap_remove(this->hosts, internalKey)) == MAP_OK) {
		} else {
			PRINTF("ERROR: hosts %s cannot be removed!\n", internalKey);
		}
	}
}

static void
ContainerNode_removeHost(ContainerNode * const this, ContainerNode *ptr)
{
	this->host = NULL;
}

static void
ContainerNode_removeGroups(ContainerNode * const this, Group *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: Group cannot be removed in ContainerNode because the key is not defined\n");
	} else {
		if ((hashmap_remove(this->groups, internalKey)) == MAP_OK) {
		} else {
			PRINTF("ERROR: group %s cannot be removed!\n", internalKey);
		}
	}
}

static void
ContainerNode_removeNetworkInformation(ContainerNode * const this, NetworkInfo *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("The NetworkInfo cannot be removed in ContainerNode because the key is not defined\n");
	} else {
		if(hashmap_remove(this->networkInformation, internalKey) == MAP_OK) {
			free(ptr->eContainer);
			ptr->eContainer = NULL;
			free(ptr->path);
			ptr->path = NULL;
		} else {
			PRINTF("ERROR: networkInformation %s cannot be removed!\n", internalKey);
		}
	}
}

static void
delete_ContainerNode(ContainerNode * const this)
{
	/* destroy base object */
	instance_VT.delete((Instance*)this);

	/* destroy data members */
	if (this->components != NULL) {
		deleteContainerContents(this->components);
		hashmap_free(this->components);
	}

	if (this->hosts != NULL) {
		/*deleteContainerContents(this->hosts);*/
		hashmap_free(this->hosts);
	}

	if (this->groups != NULL) {
		/*deleteContainerContents(this->groups);*/
		hashmap_free(this->groups);
	}

	if (this->networkInformation != NULL) {
		deleteContainerContents(this->networkInformation);
		hashmap_free(this->networkInformation);
	}
}

static void
ContainerNode_visit(ContainerNode * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	/*
	 * Visit parent
	 */
	/*instance_VT.visit((Instance*)this, parent, action, secondAction, visitPaths);*/
	instance_VT.visit((Instance*)this, parent, action, secondAction, visitPaths);

	hashmap_map* m = NULL;

	int length;

	if((m = (hashmap_map*)this->components) != NULL) {
		length = hashmap_length(this->components);
		if (visitPaths) {
			sprintf(path,"%s/components", parent);
			Visitor_visitPaths(m, "components", path, action, secondAction);
		} else {
			action("components", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("components", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}

	if((m = (hashmap_map*)this->hosts) != NULL) {
		length = hashmap_length(this->hosts);
		if (visitPaths) {
			Visitor_visitPathRefs(m, "hosts", path, action, secondAction, parent);
		} else {
			action("hosts", SQBRACKET, NULL);
			Visitor_visitModelRefs(m, length, "hosts", path, action);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("hosts", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}

	if(this->host != NULL) {
		if (visitPaths) {
			sprintf(path,"%s/%s\\host", parent, this->host->path);
			action(path, REFERENCE, parent);
		} else {
			action("host", SQBRACKET, NULL);
			sprintf(path, "host[%s]", this->host->VT->internalGetKey(this->host));
			action(path, STRREF, NULL);
			action(NULL, RETURN, NULL);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("host", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}

	if((m = (hashmap_map*)this->groups) != NULL) {
		length = hashmap_length(this->groups);
		if (visitPaths) {
			Visitor_visitPathRefs(m, "groups", path, action, secondAction, parent);
		} else {
			action("groups", SQBRACKET, NULL);
			Visitor_visitModelRefs(m, length, "groups", path, action);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("groups", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}

	if((m = (hashmap_map*)this->networkInformation) != NULL) {
		length = hashmap_length(this->networkInformation);
		if (visitPaths) {
			sprintf(path,"%s/networkInformation", parent);
			Visitor_visitPaths(m, "networkInformation", path, action, secondAction);
		} else {
			action("networkInformation", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKET, NULL);
		}
	} else if (!visitPaths) {
		action("networkInformation", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKET, NULL);
	}
}

void
*ContainerNode_findByPath(ContainerNode * const this, char *attribute)
{
	/* There are no local attributes */

	/* Instance attributes and references */
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

	if(!strcmp("components", obj)) {
		if(nextAttribute == NULL) {
			free(obj);
			return this->VT->findComponentsByID(this, key);
		} else {
			free(obj);
			ComponentInstance* compins = this->VT->findComponentsByID(this, key);
			if(compins != NULL) {
				return compins->VT->findByPath(compins, nextPath);
			} else {
				PRINTF("ERROR: Cannot retrieve component %s\n", key);
				return NULL;
			}
		}
	} else if(!strcmp("hosts", obj)) {
		if(nextAttribute == NULL) {
			free(obj);
			return this->VT->findHostsByID(this, key);
		} else {
			free(obj);
			ContainerNode* contnode = this->VT->findHostsByID(this, key);
			if(contnode != NULL) {
				return contnode->VT->findByPath(contnode, nextPath);
			} else {
				PRINTF("ERROR: Cannot retrieve hosts %s\n", key);
				return NULL;
			}
		}
	} else if(!strcmp("host", obj)) {
		free(obj);
		if(nextAttribute == NULL) {
			return this->host;
		} else {
			if (this->host != NULL) {
				return this->host->VT->findByPath(this->host, nextPath);
			} else {
				return NULL;
			}
		}
	} else if(!strcmp("networkInformation", obj)) {
		free(obj);
		if(nextAttribute == NULL) {
			return this->VT->findNetworkInformationByID(this, key);
		} else {
			NetworkInfo* netinfo = this->VT->findNetworkInformationByID(this, key);
			if(netinfo != NULL) {
				return netinfo->VT->findByPath(netinfo, nextPath);
			} else {
				PRINTF("ERROR: Cannot retrieve host %s\n", key);
				return NULL;
			}
		}
	} else if(!strcmp("groups", obj)) {
		free(obj);
		if(nextAttribute == NULL) {
			return this->VT->findGroupsByID(this, key);
		} else {
			Group* group = this->VT->findGroupsByID(this, key);
			if(group != NULL) {
				return group->VT->findByPath(group, nextPath);
			} else {
				PRINTF("ERROR: Cannot retrieve groups %s\n", key);
				return NULL;
			}
		}
	} else {
		free(obj);
		return instance_VT.findByPath((Instance*)this, attribute);
	}
}

const ContainerNode_VT containerNode_VT = {
		.super = &instance_VT,
		/*
		 * KMFContainer_VT
		 * NamedElement_VT
		 */
		.metaClassName = ContainerNode_metaClassName,
		.internalGetKey = ContainerNode_internalGetKey,
		.visit = ContainerNode_visit,
		.findByPath = ContainerNode_findByPath,
		.delete = delete_ContainerNode,
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
		 * ContainerNode
		 */
		.findComponentsByID = ContainerNode_findComponentsByID,
		.findHostsByID = ContainerNode_findHostsByID,
		.findGroupsByID = ContainerNode_findGroupsByID,
		.findNetworkInformationByID = ContainerNode_findNetworkInformationByID,
		.addComponents = ContainerNode_addComponents,
		.addHosts = ContainerNode_addHosts,
		.addHost = ContainerNode_addHost,
		.addGroups = ContainerNode_addGroups,
		.addNetworkInformation = ContainerNode_addNetworkInformation,
		.removeComponents = ContainerNode_removeComponents,
		.removeHost = ContainerNode_removeHost,
		.removeHosts = ContainerNode_removeHosts,
		.removeGroups = ContainerNode_removeGroups,
		.removeNetworkInformation = ContainerNode_removeNetworkInformation
};

ContainerNode
*new_ContainerNode()
{
	ContainerNode* pContNodeObj = NULL;

	/* Allocating memory */
	pContNodeObj = (ContainerNode*)malloc(sizeof(ContainerNode));

	if (pContNodeObj == NULL) {
		PRINTF("ERROR: Cannot create ContainerNode!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pContNodeObj->VT = &containerNode_VT;

	/*
	 * ContainerNode
	 */
	initContainerNode(pContNodeObj);

	return pContNodeObj;
}
