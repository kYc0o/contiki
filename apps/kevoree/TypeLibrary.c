#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "NamedElement.h"
#include "TypeDefinition.h"
#include "Visitor.h"
#include "TypeLibrary.h"
#include "TypedElement.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initTypeLibrary(TypeLibrary * const this)
{
	/*
	 * Initialize parent
	 */
	initNamedElement((NamedElement*)this);

	/*
	 * Initialize itself
	 */
	this->subTypes = NULL;
}

static char
*TypeLibrary_metaClassName(TypeLibrary * const this)
{
	return "TypeLibrary";
}

static char
*TypeLibrary_internalGetKey(TypeLibrary * const this)
{
	return namedElement_VT.internalGetKey((NamedElement*)this);
}

static TypeDefinition
*TypeLibrary_findSubTypesByID(TypeLibrary * const this, char *id)
{
	TypeDefinition *value = NULL;

	if(this->subTypes != NULL)
	{
		if(hashmap_get(this->subTypes, id, (void**)(&value)) == MAP_OK)
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
TypeLibrary_addSubTypes(TypeLibrary * const this, TypeDefinition *ptr)
{
	TypeDefinition* container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL)
	{
		PRINTF("The TypeDefinition cannot be added in TypeLibrary because the key is not defined\n");
	}
	else
	{
		if(this->subTypes == NULL)
		{
			this->subTypes = hashmap_new(get_key_for_hashmap);
		}
		if(hashmap_get(this->subTypes, internalKey, (void**)(&container)) == MAP_MISSING)
		{
			hashmap_put(this->subTypes, internalKey, ptr);
		}
	}
}

static void
TypeLibrary_removeSubTypes(TypeLibrary * const this, TypeDefinition *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL)
	{
		PRINTF("The TypeDefinition cannot be removed in TypeLibrary because the key is not defined\n");
	}
	else
	{
		hashmap_remove(this->subTypes, internalKey);
		free(internalKey);
	}
}

void delete_TypeLibrary(TypeLibrary * const this)
{
	/* destroy base object */
	namedElement_VT.delete((NamedElement*)this);

	/* destroy data members */
	if (this->subTypes != NULL) {
		/*deleteContainerContents(this->subTypes);*/
		hashmap_free(this->subTypes);
	}
}

static void
TypeLibrary_visit(TypeLibrary * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	namedElement_VT.visit((NamedElement*)this, parent, action, secondAction, visitPaths);

	hashmap_map *m;
	int length;

	if((m = (hashmap_map*)this->subTypes) != NULL) {
		length = hashmap_length(this->subTypes);
		if (visitPaths) {
			Visitor_visitPathRefs(m, "subTypes", path, action, secondAction, parent);
		} else {
			action("subTypes", SQBRACKET, NULL);
			Visitor_visitModelRefs(m, length, "typeDefinitions", path, action);
			action(NULL, CLOSESQBRACKET, NULL);
		}
	} else if (!visitPaths) {
		action("subTypes", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKET, NULL);
	}
}

static void
*TypeLibrary_findByPath(TypeLibrary * const this, char *attribute)
{
	/* There are no local attributes */

	/* NamedElement */
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

	if(!strcmp("typeDefinitions", obj))
	{
		free(obj);
		if(nextAttribute == NULL)
		{
			return this->VT->findSubTypesByID(this, key);
		}
		else
		{
			TypeDefinition* typdef = this->VT->findSubTypesByID(this, key);
			if(typdef != NULL)
				return typdef->VT->findByPath(typdef, nextPath);
			else
				return NULL;
		}
	}
	else
	{
		free(obj);
		return namedElement_VT.findByPath((NamedElement*)this, attribute);
	}
}

const TypeLibrary_VT typeLibrary_VT = {
		.super = &namedElement_VT,
		/*
		 * KMFContainer
		 * NamedElement
		 */
		.metaClassName = TypeLibrary_metaClassName,
		.internalGetKey = TypeLibrary_internalGetKey,
		.getPath = TypedElement_getPath,
		.visit = TypeLibrary_visit,
		.findByPath = TypeLibrary_findByPath,
		.delete = delete_TypeLibrary,
		/*
		 * TypeLibrary
		 */
		.findSubTypesByID = TypeLibrary_findSubTypesByID,
		.addSubTypes = TypeLibrary_addSubTypes,
		.removeSubTypes = TypeLibrary_removeSubTypes
};

TypeLibrary
*new_TypeLibrary()
{
	TypeLibrary* pTypeLibObj = NULL;

	/* Allocating memory */
	pTypeLibObj = malloc(sizeof(TypeLibrary));

	if (pTypeLibObj == NULL) {
		PRINTF("ERROR: Cannot create TypeLibrary!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pTypeLibObj->VT = &typeLibrary_VT;

	/*
	 * TypeLibrary
	 */
	initTypeLibrary(pTypeLibObj);

	return pTypeLibObj;
}
