#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "NamedElement.h"
#include "ContainerRoot.h"
#include "Visitor.h"
#include "TypedElement.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initTypedElement(TypedElement * const this)
{
	/*
	 * Initialize parent
	 */
	initNamedElement((NamedElement*)this);

	/*
	 * Initialize itself
	 */
	this->genericTypes = NULL;
}

static char
*TypedElement_metaClassName(TypedElement * const this)
{
	return "TypedElement";
}

static char
*TypedElement_internalGetKey(TypedElement * const this)
{
	return namedElement_VT.internalGetKey((NamedElement*)this);
}

TypedElement
*TypedElement_findGenericTypesByID(TypedElement * const this, char *id)
{
	TypedElement* value = NULL;

	if(this->genericTypes)
	{
		if(hashmap_get(this->genericTypes, id, (void**)(&value)) == MAP_OK)
			return value;
		else
			return NULL;
	}
	else
	{
		return NULL;
	}
}

void
TypedElement_addGenericTypes(TypedElement * const this, TypedElement *ptr)
{
	TypedElement* container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The TypedElement cannot be added in TypedElement because the key is not defined");
	} else {
		if(this->genericTypes == NULL) {
			this->genericTypes = hashmap_new(get_key_for_hashmap);
		}
		if(hashmap_get(this->genericTypes, internalKey, (void**)(&container)) == MAP_MISSING) {
			if(hashmap_put(this->genericTypes, internalKey, ptr) == MAP_OK) {

			} else {
				PRINTF("ERROR: genericType cannot be added!\n");
			}

		} else {
			PRINTF("WARNING: id %s already exists in genericType map\n", internalKey);
		}
	}
}

void
TypedElement_removeGenericTypes(TypedElement * const this, TypedElement *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The TypedElement cannot be removed in TypedElement because the key is not defined\n");
	} else {
		if(hashmap_remove(this->genericTypes, internalKey) == MAP_OK) {

		} else {
			PRINTF("ERROR: genericType %s cannot be removed!\n", internalKey);
		}
	}
}

static void
delete_TypedElement(TypedElement * const this)
{
	/* destroy base object */
	namedElement_VT.delete((NamedElement*)this);

	/* destroy data members */
	if (this->genericTypes != NULL) {
		deleteContainerContents(this->genericTypes);
		hashmap_free(this->genericTypes);
	}
}

static void
TypedElement_visit(TypedElement * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	/*
	 * Visit parent
	 */
	namedElement_VT.visit((NamedElement*)this, parent, action, secondAction, visitPaths);

	hashmap_map* m = NULL;
	int length;

	if((m = (hashmap_map*)this->genericTypes) != NULL) {
		length = hashmap_length(this->genericTypes);
		if (visitPaths) {
			Visitor_visitPathRefs(m, "genericTypes", path, action, secondAction, parent);
		} else {
			action("genericTypes", SQBRACKET, NULL);
			Visitor_visitModelRefs(m, length, "genericTypes", path, action);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("genericTypes", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}

}

static void
*TypedElement_findByPath(TypedElement * const this, char *attribute)
{
	/* NamedElement attributes */
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

	if(!strcmp("genericTypes", obj))
	{
		free(obj);
		if(nextAttribute == NULL)
		{
			return this->VT->findGenericTypesByID(this, key);
		}
		else
		{
			TypedElement* typelem = this->VT->findGenericTypesByID(this, key);
			if(typelem != NULL) {
				return typelem->VT->findByPath(typelem, nextPath);
			} else {
				PRINTF("ERROR: Cannot retrieve genericTypes %s\n", key);
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

char*
TypedElement_getPath(KMFContainer* kmf)
{
	TypedElement* te = (TypedElement*)kmf;
	char* tmp = (te->eContainer)?get_eContainer_path(te):strdup("");
	char* r = (char*)malloc(strlen(tmp) + strlen("dataTypes[]") + strlen(te->VT->internalGetKey(te)) + 1);
	sprintf(r, "dataTypes[%s]", te->VT->internalGetKey(te));
	free(tmp);
	return r;
}

const TypedElement_VT typedElement_VT = {
		.super = &namedElement_VT,
		/*
		 * KMFContainer
		 * NamedElement
		 */
		.metaClassName = TypedElement_metaClassName,
		.internalGetKey = TypedElement_internalGetKey,
		.getPath = TypedElement_getPath,
		.visit = TypedElement_visit,
		.findByPath = TypedElement_findByPath,
		.delete = delete_TypedElement,
		/*
		 * TypedElement
		 */
		.findGenericTypesByID = TypedElement_findGenericTypesByID,
		.addGenericTypes = TypedElement_addGenericTypes,
		.removeGenericTypes = TypedElement_removeGenericTypes
};

TypedElement* new_TypedElement(void)
{
	TypedElement* pTypeElemObj = NULL;

	/* Allocating memory */
	pTypeElemObj = malloc(sizeof(TypedElement));

	if (pTypeElemObj == NULL) {
		PRINTF("ERROR: Cannot create TypedElement!\n");
		return NULL;
	}

	pTypeElemObj->VT = &typedElement_VT;

	/*
	 * TypedElement
	 */
	initTypedElement(pTypeElemObj);

	return pTypeElemObj;
}
