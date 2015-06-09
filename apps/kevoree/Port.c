#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "Visitor.h"
#include "MBinding.h"
#include "PortTypeRef.h"
#include "ComponentInstance.h"
#include "TypeDefinition.h"
#include "NamedElement.h"
#include "Port.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initPort(Port * const this)
{
	/*
	 * Initialize parent
	 */
	initNamedElement((NamedElement*)this);
	/*
	 * Initialize itself
	 */
	this->bindings = NULL;
	this->portTypeRef = NULL;
}

static void
delete_Port(Port * const this)
{
	/* delete base object */
	namedElement_VT.delete((NamedElement*)this);
	/* delete data members */
	if (this->bindings != NULL) {
		/*deleteContainerContents(this->bindings);*/
		hashmap_free(this->bindings);
	}
}

static char
*Port_internalGetKey(Port * const this)
{
	return namedElement_VT.internalGetKey((Port*)this);
}

static char
*Port_metaClassName(Port * const this)
{
	return "Port";
}

static MBinding
*Port_findBindingsByID(Port * const this, char *id)
{
	MBinding* value = NULL;

	if(this->bindings != NULL)
	{
		if(hashmap_get(this->bindings, id, (void**)(&value)) == MAP_OK)
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
Port_addBindings(Port * const this, MBinding *ptr)
{
	MBinding* container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL)
	{
		PRINTF("The MBinding cannot be added in Port because the key is not defined\n");
	}
	else
	{
		if(this->bindings == NULL)
		{
			this->bindings = hashmap_new(get_key_for_hashmap);
		}

		if(hashmap_get(this->bindings, internalKey, (void**)(&container)) == MAP_MISSING)
		{
			hashmap_put(this->bindings, internalKey, ptr);
		}
	}
}

static void
Port_addPortTypeRef(Port * const this, PortTypeRef *ptr)
{
	this->portTypeRef = ptr;
}

static void
Port_removeBindings(Port * const this, MBinding *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL)
	{
		PRINTF("The MBinding cannot be removed in Port because the key is not defined\n");
	}
	else
	{
		hashmap_remove(this->bindings, internalKey);
		free(internalKey);
	}
}

static void
Port_removePortTypeRef(Port * const this, PortTypeRef *ptr)
{
	this->portTypeRef = NULL;
}

static void
Port_visit(Port * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	/*
	 * Visit parent
	 */
	namedElement_VT.visit((NamedElement*)this, parent, action, secondAction, visitPaths);

	hashmap_map* m = NULL;

	int length;

	if((m = (hashmap_map*)this->bindings) != NULL) {
		length = hashmap_length(this->bindings);
		if (visitPaths) {
			Visitor_visitPathRefs(m, "bindings", path, action, secondAction, parent);
		} else {
			action("bindings", SQBRACKET, NULL);
			Visitor_visitModelRefs(m, length, "mBindings", path, action);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("bindings", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}

	if(this->portTypeRef != NULL) {
		if (visitPaths) {
			char* ref_path = this->portTypeRef->VT->getPath(this->portTypeRef);
			sprintf(path,"%s/%s\\portTypeRef", parent, ref_path);
			free(ref_path);
			action(path, REFERENCE, parent);
		} else {
			action("portTypeRef", SQBRACKET, NULL);
			sprintf(path, "portTypeRef[%s]", this->portTypeRef->VT->internalGetKey(this->portTypeRef));
			action(path, STRREF, NULL);
			action(NULL, RETURN, NULL);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("portTypeRef", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}
}

static void
*Port_findByPath(Port * const this, char *attribute)
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
		if ((nextAttribute = strtok(path, "\\")) != NULL) {
			if ((nextAttribute = strtok(NULL, "\\")) != NULL) {
				PRINTF("Attribute: %s\n", nextAttribute);
			} else {
				nextAttribute = strtok(path, "\\");
				PRINTF("Attribute: %s\n", nextAttribute);
			}
		}
	}

	if(!strcmp("bindings", obj))
	{
		free(obj);
		if(nextAttribute == NULL)
		{

			return this->VT->findBindingsByID(this, key);
		}
		else
		{
			MBinding* binding = this->VT->findBindingsByID(this, key);
			if(binding != NULL)
				return binding->VT->findByPath(binding, nextPath);
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

static char*
Port_getPath(KMFContainer* kmf)
{
	any_t any;
	Port* obj = (Port*)kmf;
	char* internalKey = obj->VT->internalGetKey(obj);
	
	char* tmp = (obj->eContainer)?get_eContainer_path(obj):strdup("");
	
	ComponentInstance* container = (ComponentInstance*)obj->eContainer;
	if (hashmap_get(container->provided, internalKey, (void**)(&any)) == MAP_OK) {
		char* r = (char*)malloc(strlen(tmp) + strlen("/provided[]") + strlen(internalKey) + 1);
		sprintf(r, "%s/provided[%s]", tmp, internalKey);
		free(tmp);
		return r;
	}
	else if (hashmap_get(container->required, internalKey, (void**)(&any)) == MAP_OK) {
		char* r = (char*)malloc(strlen(tmp) + strlen("/required[]") + strlen(internalKey) + 1);
		sprintf(r, "%s/required[%s]", tmp, internalKey);
		free(tmp);
		return r;
	}
	else {
		free(tmp);
		return strdup("");
	}
}

const Port_VT port_VT = {
		.super = &namedElement_VT,
		/*
		 * KMFContainer
		 */
		.metaClassName = Port_metaClassName,
		.internalGetKey = Port_internalGetKey,
		.getPath = Port_getPath,
		.visit = Port_visit,
		.findByPath = Port_findByPath,
		.delete = delete_Port,
		/*
		 * Port
		 */
		.findBindingsByID = Port_findBindingsByID,
		.addBindings = Port_addBindings,
		.addPortTypeRef = Port_addPortTypeRef,
		.removeBindings = Port_removeBindings,
		.removePortTypeRef = Port_removePortTypeRef
};

Port* new_Port()
{
	Port* pObj = NULL;

	/* Allocating memory */
	pObj = malloc(sizeof(Port));

	if (pObj == NULL) {
		PRINTF("ERROR: Cannot create Port!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pObj->VT = &port_VT;
	/*
	 * Initialize itself
	 */
	initPort(pObj);

	return pObj;
}
