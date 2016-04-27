#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "MBinding.h"
#include "Channel.h"
#include "Port.h"
#include "ContainerRoot.h"
#include "Visitor.h"
#include "MBinding.h"
#include "tools.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initMBinding(MBinding * const this)
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
	this->port = NULL;
	this->channel = NULL;
}

static void
delete_MBinding(MBinding * const this)
{
	/* delete base object */
	KMF_VT.delete((KMFContainer*)this);
}

static char
*MBinding_metaClassName(MBinding * const this)
{
	return "MBinding";
}

static char
*MBinding_internalGetKey(MBinding * const this)
{
	return this->generated_KMF_ID;
}

static void
MBinding_addPort(MBinding * const this, Port *ptr)
{
	this->port = ptr;
}

static void
MBinding_addHub(MBinding * const this, Channel *ptr)
{
	this->channel = ptr;
}

static void
MBinding_removePort(MBinding * const this, Port *ptr)
{
	this->port = NULL;
}

static void
MBinding_removeHub(MBinding * const this, Channel *ptr)
{
	this->channel = NULL;
}

static void
MBinding_visit(MBinding * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	/*
	 * Visit parent
	 */
	KMF_VT.visit((MBinding*)this, parent, action, secondAction, visitPaths);

	if (visitPaths) {
		sprintf(path, "%s\\generated_KMF_ID", parent);
		action(path, STRING, this->generated_KMF_ID);
	} else {
		action("generated_KMF_ID", STRING, this->generated_KMF_ID);
		action(NULL, COLON, NULL);
	}

	if(this->port != NULL) {
		if (visitPaths) {
			sprintf(path,"%s/%s\\port", parent, this->port->path);
			action(path, REFERENCE, parent);
		} else {
			action("port", SQBRACKET, NULL);
			sprintf(path, "port[%s]", this->port->VT->internalGetKey(this->port));
			action(path, STRREF, NULL);
			action(NULL, RETURN, NULL);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("port", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}

	if(this->channel != NULL) {
		if (visitPaths) {
			sprintf(path,"%s/%s\\hub", parent, this->channel->path);
			action(path, REFERENCE, parent);
		} else {
			action("hub", SQBRACKET, NULL);
			sprintf(path, "hub[%s]", this->channel->VT->internalGetKey(this->channel));
			action(path, STRREF, NULL);
			action(NULL, RETURN, NULL);
			action(NULL, CLOSESQBRACKET, NULL);
		}
	} else if (!visitPaths) {
		action("hub", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKET, NULL);
	}
}

static void
*MBinding_findByPath(MBinding * const this, char *attribute)
{
	/* Local attributes */
	if(!strcmp("generated_KMF_ID", attribute))
	{
		return this->generated_KMF_ID;
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

		if(!strcmp("port", obj))
		{
			free(obj);
			if(nextAttribute == NULL)
			{
				return this->port;
			}
			else
			{
				return this->port->VT->findByPath(this->port, nextPath);
			}
		}
		else if(!strcmp("channel", obj))
		{
			free(obj);
			if(nextAttribute == NULL)
			{
				return this->channel;
			}
			else
			{
				return this->channel->VT->findByPath(this->channel, nextPath);
			}
		}
		else
		{
			free(obj);
			PRINTF("WARNING: Wrong attribute or reference %s\n", attribute);
			return NULL;
		}
	}
}

const MBinding_VT mBinding_VT = {
		.super = &KMF_VT,
		/*
		 * KMFContainer
		 */
		.metaClassName = MBinding_metaClassName,
		.internalGetKey = MBinding_internalGetKey,
		.visit = MBinding_visit,
		.findByPath = MBinding_findByPath,
		.delete = delete_MBinding,
		/*
		 * MBinding
		 */
		.addPort = MBinding_addPort,
		.addHub = MBinding_addHub,
		.removeHub = MBinding_removeHub,
		.removePort = MBinding_removePort
};

MBinding* new_MBinding(void)
{
	MBinding* pObj = NULL;
	/* Allocating memory */
	pObj = malloc(sizeof(MBinding));

	if (pObj == NULL)
	{
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pObj->VT = &mBinding_VT;

	/*
	 * MBinding
	 */
	initMBinding(pObj);

	return pObj;
}
