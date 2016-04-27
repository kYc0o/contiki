#include <string.h>
#include <stdio.h>
#include "tools.h"
#include "NamedElement.h"
#include "DeployUnit.h"
#include "Visitor.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initDeployUnit(DeployUnit * const this)
{
	initNamedElement((NamedElement*)this);
	this->groupName = NULL;
	this->version = NULL;
	this->url = NULL;
	this->hashcode = NULL;
	this->type = NULL;
	this->internalKey = NULL;
	this->requiredLibs = NULL;
}

static char
*DeployUnit_internalGetKey(DeployUnit * const this)
{
	if (this->internalKey == NULL) {
		char* internalKey;

		internalKey = malloc(sizeof(char) * (strlen(this->groupName) + strlen("/") +
				strlen(this->hashcode) + strlen("/") +
				strlen(this->name) + strlen("/") +
				strlen(this->version)) + 1);

		if (internalKey == NULL) {
			PRINTF("ERROR: not enough memory for internalKey\n");
			return NULL;
		}

		sprintf(internalKey, "%s/%s/%s/%s", this->groupName, this->hashcode, this->name, this->version);
		this->internalKey = internalKey;

		return internalKey;
	} else {
		return this->internalKey;
	}
}

static char
*DeployUnit_metaClassName(void* const this)
{
	return "DeployUnit";
}

static void
DeployUnit_addRequiredLibs(DeployUnit * const this, DeployUnit *ptr)
{
	DeployUnit* container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("The DeployUnit cannot be added in DeployUnit because the key is not defined\n");
	} else {
		if(this->requiredLibs == NULL) {
			this->requiredLibs = hashmap_new();
		}
		if(hashmap_get(this->requiredLibs, internalKey, (void**)(&container)) == MAP_MISSING) {
			hashmap_put(this->requiredLibs, internalKey, ptr);
		} else {
			PRINTF("ERROR: requiredLibs can't be added!\n");
		}
	}
}

static void
DeployUnit_removeRequiredLibs(DeployUnit * const this, DeployUnit *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The DeployUnit cannot be removed in DeployUnit because the key is not defined\n");
	} else {
		if (hashmap_remove(this->requiredLibs, internalKey) == MAP_OK) {

		} else {
			PRINTF("ERROR: superType %s cannot be removed!\n", internalKey);
		}
	}
}

DeployUnit
*DeployUnit_findRequiredLibsByID(DeployUnit * const this, char *id)
{
	DeployUnit* value = NULL;

	if(this->requiredLibs != NULL) {
		if(hashmap_get(this->requiredLibs, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: requiredLibs %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no requiredLibs in DeployUnit!\n");
		return NULL;
	}
}

static void
delete_DeployUnit(DeployUnit * const this)
{
	/* destroy base object */
	namedElement_VT.delete((NamedElement*)this);
	/* destroy data memebers */
	if (this->requiredLibs != NULL) {
		/*deleteContainerContents(this->requiredLibs);*/
		hashmap_free(this->requiredLibs);
	}

	free(this->groupName);
	free(this->version);
	free(this->url);
	free(this->hashcode);
	free(this->type);
	if (this->internalKey != NULL)
		free(this->internalKey);
}

static void
DeployUnit_visit(DeployUnit * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	namedElement_VT.visit((NamedElement*)this, parent, action, secondAction, visitPaths);

	if (visitPaths) {
		sprintf(path,"%s\\groupName", parent);
		action(path, STRING, this->groupName);
		sprintf(path, "%s\\version", parent);
		action(path, STRING, this->version);
		sprintf(path,"%s\\url",parent);
		action(path, STRING, this->url);
		sprintf(path, "%s\\hashcode", parent);
		action(path, STRING, this->hashcode);
		sprintf(path, "%s\\type", parent);
		action(path, STRING, this->type);
	} else {
		action("groupName", STRING, this->groupName);
		action(NULL, COLON, NULL);
		action("version", STRING, this->version);
		action(NULL, COLON, NULL);
		action("url", STRING, this->url);
		action(NULL, COLON, NULL);
		action("hashcode", STRING, this->hashcode);
		action(NULL, COLON, NULL);
		action("type", STRING, this->type);
		action(NULL, COLON, NULL);
	}

	hashmap_map *m;
	int length;

	if((m = (hashmap_map*)this->requiredLibs) != NULL) {
		length = hashmap_length(this->requiredLibs);
		if (visitPaths) {
			Visitor_visitPathRefs(m, "requiredLibs", path, action, secondAction, parent);
		} else {
			action("requiredLibs", SQBRACKET, NULL);
			Visitor_visitModelRefs(m, length, "requiredLibs", path, action);
			action(NULL, CLOSESQBRACKET, NULL);
		}
	} else if (!visitPaths) {
		action("requiredLibs", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKET, NULL);
	}
}

static void
*DeployUnit_findByPath(DeployUnit * const this, char *attribute)
{
	/* There are no local attributes */

	/* NamedElement */
	/* Local attributes */
	if(!strcmp("groupName", attribute))
	{
		return this->groupName;
	}
	else if(!strcmp("version", attribute))
	{
		return this->version;
	}
	else if(!strcmp("url", attribute))
	{
		return this->url;
	}
	else if(!strcmp("hashcode", attribute))
	{
		return this->hashcode;
	}
	else if(!strcmp("type", attribute))
	{
		return this->type;
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

		if(!strcmp("requiredLibs", obj))
		{
			free(obj);
			if(nextAttribute == NULL)
			{
				return this->VT->findRequiredLibsByID(this, key);
			}
			else
			{
				DeployUnit* reqlibs = this->VT->findRequiredLibsByID(this, key);
				if(reqlibs != NULL) {
					return reqlibs->VT->findByPath(reqlibs, nextPath);
				} else {
					PRINTF("ERROR: Cannot retrieve requiredLibs %s\n", key);
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
}

const DeployUnit_VT deployUnit_VT = {
		.super = &namedElement_VT,
		/*
		 * KMFContainer
		 * NamedElement
		 */
		.metaClassName = DeployUnit_metaClassName,
		.internalGetKey = DeployUnit_internalGetKey,
		.visit = DeployUnit_visit,
		.findByPath = DeployUnit_findByPath,
		.delete = delete_DeployUnit,
		/*
		 * DeployUnit
		 */
		.addRequiredLibs = DeployUnit_addRequiredLibs,
		.removeRequiredLibs = DeployUnit_removeRequiredLibs,
		.findRequiredLibsByID = DeployUnit_findRequiredLibsByID
};

DeployUnit
*new_DeployUnit()
{
	DeployUnit* pDepUnitObj = NULL;

	/* Allocating memory */
	pDepUnitObj = malloc(sizeof(DeployUnit));

	if (pDepUnitObj == NULL) {
		PRINTF("ERROR: Cannot create DeployUnit!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pDepUnitObj->VT = &deployUnit_VT;
	/*
	 * DeployUnit
	 */
	initDeployUnit(pDepUnitObj);

	return pDepUnitObj;
}
