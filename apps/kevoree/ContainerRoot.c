#include "KMFContainer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "tools.h"
#include "kevoree.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initContainerRoot(ContainerRoot * const this)
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
	this->nodes = NULL;
	this->typeDefinitions = NULL;
	this->repositories = NULL;
	this->dataTypes = NULL;
	this->libraries = NULL;
	this->hubs = NULL;
	this->mBindings = NULL;
	this->deployUnits = NULL;
	this->nodeNetworks = NULL;
	this->groups = NULL;
}

static char
*ContainerRoot_metaClassName(ContainerRoot * const this)
{
	return "ContainerRoot";
}

static char
*ContainerRoot_internalGetKey(ContainerRoot * const this)
{
	return this->generated_KMF_ID;
}

static ContainerNode
*ContainerRoot_findNodesByID(ContainerRoot * const this, char *id)
{
	ContainerNode *value;

	if(this->nodes != NULL) {
		if(hashmap_get(this->nodes, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: Node %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no nodes in ContainerRoot!\n");
		return NULL;
	}
}

static TypeDefinition
*ContainerRoot_findTypeDefsByID(ContainerRoot * const this, char *id)
{
	TypeDefinition *value = NULL;

	if(this->typeDefinitions != NULL) {
		if(hashmap_get(this->typeDefinitions, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: TypeDefinition %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no TypeDefinitions in ContainerRoot!\n");
		return NULL;
	}
}

static Repository
*ContainerRoot_findRepositoriesByID(ContainerRoot * const this, char *id)
{
	Repository *value = NULL;

	if(this->repositories != NULL) {
		if(hashmap_get(this->repositories, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: Repository %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no Repositories in ContainerRoot!\n");
		return NULL;
	}
}

static TypedElement
*ContainerRoot_findDataTypesByID(ContainerRoot * const this, char *id)
{
	TypedElement *value = NULL;

	if(this->dataTypes != NULL) {
		if(hashmap_get(this->dataTypes, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: TypedElement %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no TypedElements in ContainerRoot!\n");
		return NULL;
	}
}

static TypeLibrary
*ContainerRoot_findLibrariesByID(ContainerRoot * const this, char *id)
{
	TypeLibrary *value = NULL;

	if(this->libraries != NULL) {
		if(hashmap_get(this->libraries, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: TypeLibrary %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no TypeLibraries in ContainerRoot!\n");
		return NULL;
	}
}

static Channel
*ContainerRoot_findHubsByID(ContainerRoot * const this, char *id)
{
	Channel *value = NULL;

	if(this->hubs != NULL) {
		if(hashmap_get(this->hubs, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: Channel %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no Channels in ContainerRoot!\n");
		return NULL;
	}
}

static MBinding
*ContainerRoot_findBndingsByID(ContainerRoot * const this, char *id)
{
	MBinding *value = NULL;

	if(this->mBindings != NULL) {
		if(hashmap_get(this->mBindings, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: MBinding %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no MBindings in ContainerRoot!\n");
		return NULL;
	}
}

static DeployUnit
*ContainerRoot_findDeployUnitsByID(ContainerRoot * const this, char *id)
{
	DeployUnit *value = NULL;

	if(this->deployUnits != NULL) {
		if(hashmap_get(this->deployUnits, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: DeployUnit %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no DeployUnits in ContainerRoot!\n");
		return NULL;
	}
}

static NodeNetwork
*ContainerRoot_findNodeNetworksByID(ContainerRoot * const this, char *id)
{
	NodeNetwork *value = NULL;

	if(this->nodeNetworks != NULL) {
		if(hashmap_get(this->nodeNetworks, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: NodeNetwork %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no NodeNetworks in ContainerRoot!\n");
		return NULL;
	}
}

static Group
*ContainerRoot_findGroupsByID(ContainerRoot * const this, char *id)
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
		PRINTF("ERROR: There are no Groups in ContainerRoot!\n");
		return NULL;
	}
}

static void
ContainerRoot_addNodes(ContainerRoot * const this, ContainerNode *ptr)
{
	ContainerNode *container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR :The ContainerNode cannot be added in ContainerRoot because the key is not defined\n");
	} else {
		if(this->nodes == NULL) {
			this->nodes = hashmap_new();
		}
		if(hashmap_get(this->nodes, internalKey, (void**)(&container)) == MAP_MISSING) {
			if(hashmap_put(this->nodes, internalKey, ptr) == MAP_OK) {
				ptr->eContainer = strdup("");
				ptr->path = malloc(sizeof(char) * (strlen("nodes[]") + strlen(internalKey)) + 1);
				sprintf(ptr->path, "nodes[%s]", internalKey);
			} else {
				PRINTF("ERROR: Node cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in nodes map\n", internalKey);
		}
	}
}

static void
ContainerRoot_addTypeDefinitions(ContainerRoot * const this, TypeDefinition *ptr)
{
	TypeDefinition *container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The TypeDefinition cannot be added in ContainerRoot because the key is not defined\n");
	} else {
		if(this->typeDefinitions == NULL) {
			this->typeDefinitions = hashmap_new();
		}
		if(hashmap_get(this->typeDefinitions, internalKey, (void**)(&container)) == MAP_MISSING) {
			/*container = (TypeDefinition*)ptr;*/
			if(hashmap_put(this->typeDefinitions, internalKey, ptr) == MAP_OK) {
				ptr->eContainer = strdup("");
				ptr->path = malloc(sizeof(char) * (strlen("typeDefinitions[]") + strlen(internalKey)) + 1);
				sprintf(ptr->path, "typeDefinitions[%s]", internalKey);
			} else {
				PRINTF("ERROR: TypeDefinition cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in typeDefinitions map\n", internalKey);
		}
	}
}

static void
ContainerRoot_addRepositories(ContainerRoot * const this, Repository *ptr)
{
	Repository* container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR :The Repository cannot be added in ContainerRoot because the key is not defined\n");
	} else {
		if(this->repositories == NULL) {
			this->repositories = hashmap_new();
		}
		if(hashmap_get(this->repositories, internalKey, (void**)(&container)) == MAP_MISSING) {
			if(hashmap_put(this->repositories, internalKey, ptr) == MAP_OK) {
				ptr->eContainer = strdup("");
				ptr->path = malloc(sizeof(char) * (strlen("repositories[]") + strlen(internalKey)) + 1);
				sprintf(ptr->path, "repositories[%s]", internalKey);
			} else {
				PRINTF("ERROR: Repository cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in repositories map\n", internalKey);
		}
	}
}

static void
ContainerRoot_addDataTypes(ContainerRoot * const this, TypedElement *ptr)
{
	TypedElement *container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The TypedElement cannot be added in ContainerRoot because the key is not defined\n");
	} else {
		if(this->dataTypes == NULL) {
			this->dataTypes = hashmap_new();
		}
		if(hashmap_get(this->dataTypes, internalKey, (void**)(&container)) == MAP_MISSING) {
			if(hashmap_put(this->dataTypes, internalKey, ptr) == MAP_OK) {
				ptr->eContainer = strdup("");
				ptr->path = malloc(sizeof(char) * (strlen("dataTypes[]") + strlen(internalKey)) + 1);
				sprintf(ptr->path, "dataTypes[%s]", internalKey);
			} else {
				PRINTF("ERROR: TypedElement cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in dataTypes map\n", internalKey);
		}
	}
}

static void
ContainerRoot_addLibraries(ContainerRoot * const this, TypeLibrary *ptr)
{
	TypeLibrary *container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The TypeLibrary cannot be added in ContainerRoot because the key is not defined\n");
	} else {
		if(this->libraries == NULL) {
			this->libraries = hashmap_new();
		}
		if(hashmap_get(this->libraries, internalKey, (void**)(&container)) == MAP_MISSING) {
			if(hashmap_put(this->libraries, internalKey, ptr) == MAP_OK) {
				ptr->eContainer = strdup("");
				ptr->path = malloc(sizeof(char) * (strlen("dataTypes[]") + strlen(internalKey)) + 1);
				sprintf(ptr->path, "dataTypes[%s]", internalKey);
			} else {
				PRINTF("ERROR: TypeLibrary cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in libraries map\n", internalKey);
		}
	}
}

void
static ContainerRoot_addHubs(ContainerRoot * const this, Channel *ptr)
{
	Channel *container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The Channel cannot be added in ContainerRoot because the key is not defined\n");
	} else {
		if(this->hubs == NULL) {
			this->hubs = hashmap_new();
		}
		if(hashmap_get(this->hubs, internalKey, (void**)(&container)) == MAP_MISSING) {
			if(hashmap_put(this->hubs, internalKey, ptr) == MAP_OK) {
				ptr->eContainer = strdup("");
				ptr->path = malloc(sizeof(char) * (strlen("hubs[]") + strlen(internalKey)) + 1);
				sprintf(ptr->path, "hubs[%s]", internalKey);
			} else {
				PRINTF("ERROR: Channel cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in hubs map\n", internalKey);
		}
	}
}

static void
ContainerRoot_addBindings(ContainerRoot * const this, MBinding *ptr)
{
	MBinding *container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The MBinding cannot be added in ContainerRoot because the key is not defined\n");
	} else {
		if(this->mBindings == NULL) {
			this->mBindings = hashmap_new();
		}
		if(hashmap_get(this->mBindings, internalKey, (void**)(&container)) == MAP_MISSING) {
			if(hashmap_put(this->mBindings, internalKey, ptr) == MAP_OK) {
				ptr->eContainer = strdup("");
				ptr->path = malloc(sizeof(char) * (strlen("mBindings[]") + strlen(internalKey)) + 1);
				sprintf(ptr->path, "mBindings[%s]", internalKey);
			} else {
				PRINTF("ERROR: MBinding cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in mBindings map\n", internalKey);
		}
	}
}

static void
ContainerRoot_addDeployUnits(ContainerRoot * const this, DeployUnit *ptr)
{
	DeployUnit *container = NULL;

	char* internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL)
	{
		PRINTF("ERROR: The DeployUnit cannot be added in ContainerRoot because the key is not defined\n");
	} else {
		if(this->deployUnits == NULL) {
			this->deployUnits = hashmap_new();
		} if(hashmap_get(this->deployUnits, internalKey, (void**)(&container)) == MAP_MISSING) {
			if(hashmap_put(this->deployUnits, internalKey, ptr) == MAP_OK) {
				ptr->eContainer = strdup("");
				ptr->path = malloc(sizeof(char) * (strlen("deployUnits[]") + strlen(internalKey)) + 1);
				sprintf(ptr->path, "deployUnits[%s]", internalKey);
			} else {
				PRINTF("ERROR: DeployUnit cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in DeployUnit map\n", internalKey);
		}
	}
}

static void
ContainerRoot_addNodeNetworks(ContainerRoot * const this, NodeNetwork *ptr)
{
	NodeNetwork *container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The NodeNetwork cannot be added in ContainerRoot because the key is not defined\n");
	} else {
		if(this->nodeNetworks == NULL) {
			this->nodeNetworks = hashmap_new();
		}
		if(hashmap_get(this->nodeNetworks, internalKey, (void**)(&container)) == MAP_MISSING) {
			if(hashmap_put(this->nodeNetworks, internalKey, ptr) == MAP_OK) {
				ptr->eContainer = strdup("");
				ptr->path = malloc(sizeof(char) * (strlen("nodeNetworks[]") + strlen(internalKey)) + 1);
				sprintf(ptr->path, "nodeNetworks[%s]", internalKey);
			} else {
				PRINTF("ERROR: NodeNetwork cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in nodeNetworks map\n", internalKey);
		}
	}
}

static void
ContainerRoot_addGroups(ContainerRoot * const this, Group *ptr)
{
	Group *container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The Group cannot be added in ContainerRoot because the key is not defined\n");
	} else {
		if(this->groups == NULL) {
			this->groups = hashmap_new();
		}
		if(hashmap_get(this->groups, internalKey, (void**)(&container)) == MAP_MISSING) {
			if(hashmap_put(this->groups, internalKey, ptr) == MAP_OK) {
				ptr->eContainer = strdup("");
				ptr->path = malloc(sizeof(char) * (strlen("groups[]") + strlen(internalKey)) + 1);
				sprintf(ptr->path, "groups[%s]", internalKey);
			} else {
				PRINTF("ERROR: Group cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in groups map\n", internalKey);
		}
	}
}

static void
ContainerRoot_removeNodes(ContainerRoot * const this, ContainerNode *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The ContainerNode cannot be removed in ContainerRoot because the key is not defined\n");
	} else {
		if(hashmap_remove(this->nodes, internalKey) == MAP_OK) {
			free(ptr->eContainer);
			ptr->eContainer = NULL;
			free(ptr->path);
			ptr->path = NULL;
		} else {
			PRINTF("ERROR: Cannot remove %s from nodes!\n", internalKey);
		}
	}
}

static void
ContainerRoot_removeTypeDefinitions(ContainerRoot * const this, TypeDefinition *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The TypeDefinition cannot be removed in ContainerRoot because the key is not defined\n");
	} else {
		if(hashmap_remove(this->typeDefinitions, internalKey) == MAP_OK) {
			free(ptr->eContainer);
			ptr->eContainer = NULL;
			free(ptr->path);
			ptr->path = NULL;
		} else {
			PRINTF("ERROR: Cannot remove %s from typeDefinitions!\n", internalKey);
		}
	}
}

static void
ContainerRoot_removeRepositories(ContainerRoot * const this, Repository *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The Repository cannot be removed in ContainerRoot because the key is not defined\n");
	} else {
		if(hashmap_remove(this->repositories, internalKey) == MAP_OK) {
			free(ptr->eContainer);
			ptr->eContainer = NULL;
			free(ptr->path);
			ptr->path = NULL;

		} else {
			PRINTF("ERROR: Cannot remove %s from repositories!\n", internalKey);
		}
	}
}
static void
ContainerRoot_removeDataTypes(ContainerRoot * const this, TypedElement *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The TypedElement cannot be removed in ContainerRoot because the key is not defined\n");
	} else {
		if(hashmap_remove(this->dataTypes, internalKey) == MAP_OK) {
			free(ptr->eContainer);
			ptr->eContainer = NULL;
			free(ptr->path);
			ptr->path = NULL;
		} else {
			PRINTF("ERROR: Cannot remove %s from dataTypes!\n", internalKey);
		}
	}
}

static void
ContainerRoot_removeLibraries(ContainerRoot * const this, TypeLibrary *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The TypeLibrary cannot be removed in ContainerRoot because the key is not defined\n");
	} else {
		if(hashmap_remove(this->libraries, internalKey) == MAP_OK) {
			free(ptr->eContainer);
			ptr->eContainer = NULL;
			free(ptr->path);
			ptr->path = NULL;
		} else {
			PRINTF("ERROR: Cannot remove %s from libraries!\n", internalKey);
		}
	}
}

static void
ContainerRoot_removeHubs(ContainerRoot * const this, Channel *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The Channel cannot be removed in ContainerRoot because the key is not defined\n");
	} else {
		if(hashmap_remove(this->hubs, internalKey) == MAP_OK) {
			free(ptr->eContainer);
			ptr->eContainer = NULL;
			free(ptr->path);
			ptr->path = NULL;
		} else {
			PRINTF("ERROR: Cannot remove %s from hubs!\n", internalKey);
		}
	}
}

static void
ContainerRoot_removeBindings(ContainerRoot * const this, MBinding *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The MBinding cannot be removed in ContainerRoot because the key is not defined\n");
	} else {
		if(hashmap_remove(this->mBindings, internalKey) == MAP_OK) {
			free(ptr->eContainer);
			ptr->eContainer = NULL;
			free(ptr->path);
			ptr->path = NULL;
		} else {
			PRINTF("ERROR: Cannot remove %s from hubs!\n", internalKey);
		}
	}
}

static void
ContainerRoot_removeDeployUnits(ContainerRoot * const this,  DeployUnit *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The DeployUnit cannot be removed in ContainerRoot because the key is not defined\n");
	} else {
		if(hashmap_remove(this->deployUnits, internalKey) == MAP_OK) {
			free(ptr->eContainer);
			ptr->eContainer = NULL;
			free(internalKey);
			ptr->path = NULL;
		} else {
			PRINTF("ERROR: Cannot remove %s from deployUnits!\n", internalKey);
		}
	}
}

static void
ContainerRoot_removeNodeNetworks(ContainerRoot * const this, NodeNetwork *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The NodeNetwork cannot be removed in ContainerRoot because the key is not defined\n");
	} else {
		if(hashmap_remove(this->nodeNetworks, internalKey) == MAP_OK) {
			free(ptr->eContainer);
			ptr->eContainer = NULL;
			free(ptr->path);
			ptr->path = NULL;
		} else {
			PRINTF("ERROR: Cannot remove %s from nodeNetworks!\n", internalKey);
		}
	}
}

static void
ContainerRoot_removeGroups(ContainerRoot * const this, Group *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The Group cannot be removed in ContainerRoot because the key is not defined\n");
	} else {
		if(hashmap_remove(this->groups, internalKey) == MAP_OK) {
			free(ptr->eContainer);
			ptr->eContainer = NULL;
			free(ptr->path);
			ptr->path = NULL;
		} else {
			PRINTF("ERROR: Cannot remove %s from groups!\n", internalKey);
		}
	}
}

static void
delete_ContainerRoot(ContainerRoot * const this)
{
	/*ContainerRoot *pObj = (ContainerRoot*)this;*/
	/* delete base object */
	KMF_VT.delete((KMFContainer*)this);

	if (this->nodes != NULL) {
		deleteContainerContents(this->nodes);
		hashmap_free(this->nodes);
	}

	if (this->typeDefinitions != NULL) {
		deleteContainerContents(this->typeDefinitions);
		hashmap_free(this->typeDefinitions);
	}

	if (this->repositories != NULL) {
		deleteContainerContents(this->repositories);
		hashmap_free(this->repositories);
	}

	if (this->dataTypes != NULL) {
		deleteContainerContents(this->dataTypes);
		hashmap_free(this->dataTypes);
	}

	if (this->libraries != NULL) {
		deleteContainerContents(this->libraries);
		hashmap_free(this->libraries);
	}

	if (this->hubs != NULL) {
		deleteContainerContents(this->hubs);
		hashmap_free(this->hubs);
	}

	if (this->mBindings != NULL) {
		deleteContainerContents(this->mBindings);
		hashmap_free(this->mBindings);
	}

	if (this->deployUnits != NULL) {
		deleteContainerContents(this->deployUnits);
		hashmap_free(this->deployUnits);
	}

	if (this->nodeNetworks != NULL) {
		deleteContainerContents(this->nodeNetworks);
		hashmap_free(this->nodeNetworks);
	}

	if (this->groups != NULL) {
		deleteContainerContents(this->groups);
		hashmap_free(this->groups);
	}
}

static void
ContainerRoot_visit(ContainerRoot * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	char *cClass = NULL;
	memset(&path[0], 0, sizeof(path));

	if (visitPaths) {
		sprintf(path, "\\generated_KMF_ID");
		action(path, STRING, this->generated_KMF_ID);
	} else {

		cClass = malloc(sizeof(char) * (strlen("org.kevoree.") + strlen(this->VT->metaClassName(this)) + 1));
		sprintf(cClass, "org.kevoree.%s", this->VT->metaClassName(this));
		sprintf(path, "eClass");
		action(NULL, BRACKET, NULL);
		action(path, STRING, cClass);
		action(NULL, COLON, NULL);
		free(cClass);

		sprintf(path, "generated_KMF_ID");
		action(path, STRING, this->generated_KMF_ID);
		action(NULL, COLON, NULL);
	}

	hashmap_map* m = NULL;

	int length;

	if((m = (hashmap_map*)(this->nodes)) != NULL) {
		length = hashmap_length(this->nodes);
		if (visitPaths) {
			sprintf(path, "%s", "nodes");
			Visitor_visitPaths(m, "nodes", path, action, secondAction);
		} else {
			action("nodes", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("nodes", SQBRACKET, NULL);
		action(NULL, CLOSEBRACKETCOLON, NULL);
	}


	if((m = (hashmap_map*)(this->typeDefinitions)) != NULL) {
		length = hashmap_length(this->typeDefinitions);
		if (visitPaths) {
			sprintf(path, "%s", "typeDefinitions");
			Visitor_visitPaths(m, "typeDefinitions", path, action, secondAction);
		} else {
			action("typeDefinitions", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("typeDefinitions", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}


	if((m = (hashmap_map*)(this->repositories)) != NULL) {
		length = hashmap_length(this->repositories);
		if (visitPaths) {
			sprintf(path, "%s", "repositories");
			Visitor_visitPaths(m, "repositories", path, action, secondAction);
		} else {
			action("repositories", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("repositories", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}

	if((m = (hashmap_map*)(this->dataTypes)) != NULL) {
		length = hashmap_length(this->dataTypes);
		if (visitPaths) {
			sprintf(path, "%s", "dataTypes");
			Visitor_visitPaths(m, "dataTypes", path, action, secondAction);
		} else {
			action("dataTypes", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("dataTypes", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}


	if((m = (hashmap_map*)(this->libraries)) != NULL) {
		length = hashmap_length(this->libraries);
		if (visitPaths) {
			sprintf(path, "%s", "libraries");
			Visitor_visitPaths(m, "libraries", path, action, secondAction);
		} else {
			action("libraries", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("libraries", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}


	if((m = (hashmap_map*)(this->hubs)) != NULL) {
		length = hashmap_length(this->hubs);
		if (visitPaths) {
			sprintf(path, "%s", "hubs");
			Visitor_visitPaths(m, "hubs", path, action, secondAction);
		} else {
			action("hubs", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("hubs", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}


	if((m = (hashmap_map*) ((ContainerRoot*)(this))->mBindings) != NULL) {
		length = hashmap_length(this->mBindings);
		if (visitPaths) {
			sprintf(path, "%s", "mBindings");
			Visitor_visitPaths(m, "mBindings", path, action, secondAction);
		} else {
			action("mBindings", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("mBindings", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}


	if((m = (hashmap_map*) ((ContainerRoot*)(this))->deployUnits) != NULL) {
		length = hashmap_length(this->deployUnits);
		if (visitPaths) {
			sprintf(path, "%s", "deployUnits");
			Visitor_visitPaths(m, "deployUnits", path, action, secondAction);
		} else {
			action("deployUnits", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("deployUnits", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}


	if((m = (hashmap_map*)(this->nodeNetworks)) != NULL) {
		length = hashmap_length(this->nodeNetworks);
		if (visitPaths) {
			sprintf(path, "%s", "nodeNetworks");
			Visitor_visitPaths(m, "nodeNetworks", path, action, secondAction);
		} else {
			action("nodeNetworks", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("nodeNetworks", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}


	if((m = (hashmap_map*)(this->groups)) != NULL) {
		length = hashmap_length(this->groups);
		if (visitPaths) {
			sprintf(path, "%s", "groups");
			Visitor_visitPaths(m, "groups", path, action, secondAction);
		} else {
			action("groups", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKET, NULL);
			action(NULL, CLOSEBRACKET, NULL);
		}
	} else if (!visitPaths) {
		action("groups", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKET, NULL);
		action(NULL, CLOSEBRACKET, NULL);
	}
}

static void
*ContainerRoot_findByPath(ContainerRoot * const this, char *_path)
{
	/*ContainerRoot *pObj = (ContainerRoot*)this;*/
	/* ContainerRoot attributes */

	if(!strcmp(strdup(""), _path)) {
		return this;
		/*return this;*/
	} else if (!strcmp("\\generated_KMF_ID", _path)) {
		return this->generated_KMF_ID;
	} else {
		char path[250];
		memset(&path[0], 0, sizeof(path));
		char token[100];
		memset(&token[0], 0, sizeof(token));
		char *obj = NULL;
		char key[50];
		memset(&key[0], 0, sizeof(key));
		char nextPath[150];
		memset(&nextPath[0], 0, sizeof(nextPath));
		char *attribute = NULL;

		strcpy(path, _path);

		if(strchr(path, '[') != NULL) {
			obj = strdup(strtok(path, "["));
			strcpy(path, _path);
			PRINTF("Object: %s\n", obj);
			strcpy(token, strtok(path, "]"));
			strcpy(path, _path);
			sprintf(token, "%s]", token);
			PRINTF("Token: %s\n", token);
			sscanf(token, "%*[^[][%[^]]", key);
			PRINTF("Key: %s\n", key);

			if((strchr(path, '\\')) != NULL) {
				attribute = strtok(NULL, "\\");

				if(strchr(attribute, '[')) {
					sprintf(nextPath, "%s\\", ++attribute);
					if ((attribute = strtok(NULL, "\\")) != NULL) {
						PRINTF("Attribute: %s\n", attribute);
						sprintf(nextPath, "%s%s", nextPath, attribute);
					}
					PRINTF("Next Path: %s\n", nextPath);
				} else {
					strcpy(nextPath, attribute);
					PRINTF("Next Path: %s\n", nextPath);
				}
			} else {
				attribute = strtok(path, "]");
				bool isFirst = true;
				char *fragPath = NULL;
				while ((fragPath = strtok(NULL, "]")) != NULL) {
					PRINTF("Attribute: %s]\n", attribute);
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
					attribute = NULL;
				}
			}
		} else {
			attribute = strtok(path, "\\");
			attribute = strtok(NULL, "\\");
			PRINTF("Attribute: %s\n", attribute);
		}

		if(!strcmp("nodes", obj)) {
			if(attribute == NULL) {
				free(obj);
				return this->VT->findNodesByID(this, key);
			} else {
				free(obj);
				ContainerNode* node = this->VT->findNodesByID(this, key);
				if(node != NULL) {
					return node->VT->findByPath(node, nextPath);
				} else {
					PRINTF("ERROR: Cannot retrieve node %s\n", key);
					return NULL;
				}
			}
		} else if(!strcmp("typeDefinitions", obj)) {
			if(attribute == NULL) {
				free(obj);
				return this->VT->findTypeDefsByID(this, key);
			} else {
				free(obj);
				TypeDefinition* typDef = this->VT->findTypeDefsByID(this, key);
				if (typDef != NULL) {
					return typDef->VT->findByPath(typDef, nextPath);
				} else {
					PRINTF("ERROR: Cannot retrieve typeDefinition %s\n", key);
					return NULL;
				}
			}
		} else if(!strcmp("repositories", obj)) {
			if(attribute == NULL) {
				free(obj);
				return this->VT->findRepositoriesByID(this, key);
			} else {
				free(obj);
				Repository* repo = this->VT->findRepositoriesByID(this, key);
				if(repo != NULL) {
					return repo->VT->findByPath(repo, nextPath);
				} else {
					PRINTF("ERROR: Cannot retrieve repository %s\n", key);
					return NULL;
				}
			}
		} else if(!strcmp("dataTypes", obj)) {
			if(attribute == NULL) {
				free(obj);
				return this->VT->findDataTypesByID(this, key);
			} else {
				free(obj);
				TypedElement* typel = this->VT->findDataTypesByID(this, key);
				if(typel != NULL) {
					return typel->VT->findByPath(typel, nextPath);
				} else {
					PRINTF("ERROR: Cannot retrieve dataType %s\n", key);
					return NULL;
				}
			}
		} else if(!strcmp("libraries", obj)) {
			if(attribute == NULL) {
				free(obj);
				return this->VT->findLibrariesByID(this, key);
			} else {
				free(obj);
				TypeLibrary* typlib = this->VT->findLibrariesByID(this, key);
				if(typlib != NULL) {
					return typlib->VT->findByPath(typlib, nextPath);
				} else {
					PRINTF("ERROR: Cannot retrieve library %s\n", key);
					return NULL;
				}
			}
		} else if(!strcmp("deployUnits", obj)) {
			if(attribute == NULL) {
				free(obj);
				return this->VT->findDeployUnitsByID(this, key);
			} else {
				free(obj);
				DeployUnit* depunit = this->VT->findDeployUnitsByID(this, key);
				if(depunit != NULL) {
					return depunit->VT->findByPath(depunit, nextPath);
				} else {
					PRINTF("ERROR: Cannot retrieve deployUnit %s\n", key);
					return NULL;
				}
			}
		} else if(!strcmp("nodeNetworks", obj)) {
			if(attribute == NULL) {
				free(obj);
				return this->VT->findNodeNetworksByID(this, key);
			} else {
				free(obj);
				NodeNetwork* nodenet = this->VT->findNodeNetworksByID(this, key);
				if(nodenet != NULL) {
					return nodenet->VT->findByPath(nodenet, nextPath);
				} else {
					PRINTF("ERROR: Cannot retrieve nodeNetwork %s\n", key);
					return NULL;
				}
			}
		} else if(!strcmp("groups", obj)) {
			if(attribute == NULL) {
				free(obj);
				return this->VT->findGroupsByID(this, key);
			} else {
				free(obj);
				Group* group = this->VT->findGroupsByID(this, key);
				if(group != NULL) {
					return group->VT->findByPath(group, nextPath);
				} else {
					PRINTF("ERROR: Cannot retrieve group %s\n", key);
					return NULL;
				}
			}
		} else {
			free(obj);
			PRINTF("WARNING: Object not found %s\n", attribute);
			return NULL;
		}
	}
}

const
ContainerRoot_VT containerRoot_VT = {
		/*
		 * KMFContainer
		 */
		.super = &KMF_VT,
		.metaClassName = ContainerRoot_metaClassName,
		.internalGetKey = ContainerRoot_internalGetKey,
		.visit = ContainerRoot_visit,
		.findByPath = ContainerRoot_findByPath,
		.delete = delete_ContainerRoot,
		/*
		 * ContainerRoot
		 */
		.findNodesByID = ContainerRoot_findNodesByID,
		.findTypeDefsByID = ContainerRoot_findTypeDefsByID,
		.findRepositoriesByID = ContainerRoot_findRepositoriesByID,
		.findDataTypesByID = ContainerRoot_findDataTypesByID,
		.findLibrariesByID = ContainerRoot_findLibrariesByID,
		.findHubsByID = ContainerRoot_findHubsByID,
		.findBindingsByID = ContainerRoot_findBndingsByID,
		.findDeployUnitsByID = ContainerRoot_findDeployUnitsByID,
		.findNodeNetworksByID = ContainerRoot_findNodeNetworksByID,
		.findGroupsByID = ContainerRoot_findGroupsByID,
		.addNodes = ContainerRoot_addNodes,
		.addTypeDefinitions = ContainerRoot_addTypeDefinitions,
		.addRepositories = ContainerRoot_addRepositories,
		.addDataTypes = ContainerRoot_addDataTypes,
		.addLibraries = ContainerRoot_addLibraries,
		.addHubs = ContainerRoot_addHubs,
		.addBindings = ContainerRoot_addBindings,
		.addDeployUnits = ContainerRoot_addDeployUnits,
		.addNodeNetworks = ContainerRoot_addNodeNetworks,
		.addGroups = ContainerRoot_addGroups,
		.removeNodes = ContainerRoot_removeNodes,
		.removeTypeDefinitions = ContainerRoot_removeTypeDefinitions,
		.removeRepositories = ContainerRoot_removeRepositories,
		.removeDataTypes = ContainerRoot_removeDataTypes,
		.removeLibraries = ContainerRoot_removeLibraries,
		.removeHubs = ContainerRoot_removeHubs,
		.removeBindings = ContainerRoot_removeBindings,
		.removeDeployUnits = ContainerRoot_removeDeployUnits,
		.removeNodeNetworks = ContainerRoot_removeNodeNetworks,
		.removeGroups = ContainerRoot_removeGroups
};

ContainerRoot
*new_ContainerRoot()
{
	ContainerRoot *pObj;

	/* Allocating memory */
	pObj = (ContainerRoot*)malloc(sizeof(ContainerRoot));

	if (pObj == NULL) {
		PRINTF("ERROR: Cannot create ContainerRoot!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pObj->VT = &containerRoot_VT;

	/*
	 * ContainerRoot
	 */
	initContainerRoot(pObj);

	return pObj;
}
