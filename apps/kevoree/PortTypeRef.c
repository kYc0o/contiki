#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "NamedElement.h"
#include "PortType.h"
#include "PortTypeMapping.h"
#include "ComponentType.h"
#include "Visitor.h"
#include "PortTypeRef.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initPortTypeRef(PortTypeRef * const this)
{
	initNamedElement((NamedElement*)this);
	this->mappings = NULL;
	this->ref = NULL;
	this->optional = -1;
	this->noDependency = -1;
}

static char
*PortTypeRef_metaClassName(PortTypeRef * const this)
{
	return "PortTypeRef";
}

static char
*PortTypeRef_internalGetKey(PortTypeRef * const this)
{
	return namedElement_VT.internalGetKey((NamedElement*)this);
}

static PortTypeMapping
*PortTypeRef_findMappingsByID(PortTypeRef * const this, char *id)
{
	PortTypeMapping* value = NULL;

	if(this->mappings != NULL)
	{
		if(hashmap_get(this->mappings, id, (void**)(&value)) == MAP_OK)
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
PortTypeRef_addRef(PortTypeRef * const this, PortType *ptr)
{
	this->ref = ptr;
}

static void
PortTypeRef_addMappings(PortTypeRef * const this, PortTypeMapping *ptr)
{
	PortTypeMapping* container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL)
	{
		PRINTF("The PortTypeMapping cannot be added in PortTypeRef because the key is not defined\n");
	}
	else
	{
		if(this->mappings == NULL)
		{
			this->mappings = hashmap_new();
		}
		if(hashmap_get(this->mappings, internalKey, (void**)(&container)) == MAP_MISSING)
		{
			if(hashmap_put(this->mappings, internalKey, ptr) == MAP_OK)
			{
				ptr->eContainer = strdup(this->path);
				ptr->path = malloc(sizeof(char) * (strlen(this->path) + strlen("/mappings[]") + strlen(internalKey)) + 1);
				sprintf(ptr->path, "%s/mappings[%s]", this->path, internalKey);
			}
		}
	}
}

static void
PortTypeRef_removeRef(PortTypeRef * const this, PortType *ptr)
{
	this->ref = NULL;
}

static void
PortTypeRef_removeMappings(PortTypeRef * const this, PortTypeMapping *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL)
	{
		PRINTF("The PortTypeMapping cannot be removed in PortTypeRef because the key is not defined\n");
	}
	else
	{
		if(hashmap_remove(this->mappings, internalKey) == MAP_OK)
		{
			free(ptr->eContainer);
			ptr->eContainer = NULL;
			free(ptr->path);
			ptr->path = NULL;
		}
	}
}

static void
delete_PortTypeRef(PortTypeRef * const this)
{
	/* destroy base object */
	namedElement_VT.delete((NamedElement*)this);
	/* destroy data members */
	if (this->mappings != NULL) {
		deleteContainerContents(this->mappings);
		hashmap_free(this->mappings);
	}
}

static void
PortTypeRef_visit(PortTypeRef * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	/*
	 * Visit parent
	 */
	namedElement_VT.visit((NamedElement*)this, parent, action, secondAction, visitPaths);

	hashmap_map* m = NULL;

	int length;

	if (visitPaths) {
		sprintf(path,"%s\\optional",parent);
		action(path, BOOL, (void*)this->optional);
		sprintf(path,"%s\\noDependency",parent);
		action(path, BOOL, (void*)this->noDependency);
	} else {
		action("optional", BOOL, (void*)this->optional);
		action(NULL, COLON, NULL);
		action("noDependency", BOOL, (void*)this->noDependency);
		action(NULL, RETURN, NULL);
	}

	if(this->ref != NULL) {
		if (visitPaths) {
			sprintf(path,"%s/%s\\ref", parent, this->ref->path);
			action(path, REFERENCE, parent);
		} else {
			action("ref", SQBRACKET, NULL);
			sprintf(path, "ref[%s]", this->ref->VT->internalGetKey(this->ref));
			action(path, STRREF, NULL);
			action(NULL, RETURN, NULL);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("ref", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}

	if((m = (hashmap_map*)this->mappings) != NULL) {
		length = hashmap_length(this->mappings);
		if (visitPaths) {
			sprintf(path,"%s/mappings", parent);
			Visitor_visitPaths(m, "mappings", path, action, secondAction);
		} else {
			action("mappings", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKET, NULL);
		}
	} else if (!visitPaths) {
		action("mappings", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKET, NULL);
	}
}

static void
*PortTypeRef_findByPath(PortTypeRef * const this, char *attribute)
{
	/* NamedElement attributes */
	/* Local attributes */
	if(!strcmp("optional", attribute))
	{
		return (void*)this->optional;
	}
	else if(!strcmp("noDependency", attribute))
	{
		return (void*)this->noDependency;
	}
	/* Local references */
	else
	{
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
			if ((nextAttribute = strtok(path, "\\")) != NULL) {
				if ((nextAttribute = strtok(NULL, "\\")) != NULL) {
					PRINTF("Attribute: %s\n", nextAttribute);
				} else {
					nextAttribute = strtok(path, "\\");
					PRINTF("Attribute: %s\n", nextAttribute);
				}
			}
		}

		if(!strcmp("mappings", obj))
		{
			free(obj);
			if(nextAttribute == NULL)
			{
				return this->VT->findMappingsByID(this, key);
			}
			else
			{
				PortTypeMapping* ptmapping = this->VT->findMappingsByID(this, key);
				if(ptmapping != NULL)
					return ptmapping->VT->findByPath(ptmapping, nextPath);
				else
					return NULL;
			}
		}
		else if(!strcmp("ref", obj))
		{
			free(obj);
			if(nextAttribute == NULL)
			{
				return this->ref;
			}
			else
			{
				return this->ref->VT->findByPath(this->ref, attribute);
			}
		}
		else
		{
			free(obj);
			return namedElement_VT.findByPath((NamedElement*)this, attribute);
		}
	}
}

const PortTypeRef_VT portTypeRef_VT = {
		.super = &namedElement_VT,
		/*
		 * KMFContainer
		 * NamedElement
		 */
		.metaClassName = PortTypeRef_metaClassName,
		.internalGetKey = PortTypeRef_internalGetKey,
		.visit = PortTypeRef_visit,
		.findByPath = PortTypeRef_findByPath,
		.delete = delete_PortTypeRef,
		/*
		 * PortTypeRef
		 */
		.findMappingsByID = PortTypeRef_findMappingsByID,
		.addRef = PortTypeRef_addRef,
		.addMappings = PortTypeRef_addMappings,
		.removeRef = PortTypeRef_removeRef,
		.removeMappings = PortTypeRef_removeMappings
};

PortTypeRef* new_PortTypeRef(void)
{
	PortTypeRef* pPortTypeRefObj = NULL;

	/* Allocating memory */
	pPortTypeRefObj = malloc(sizeof(PortTypeRef));

	if (pPortTypeRefObj == NULL) {
		PRINTF("ERROR: Cannot create PortTypeRef!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pPortTypeRefObj->VT = &portTypeRef_VT;

	/*
	 * PortTypeRef
	 */
	initPortTypeRef(pPortTypeRefObj);

	return pPortTypeRefObj;
}
