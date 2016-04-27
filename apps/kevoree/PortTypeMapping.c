#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Port.h"
#include "Visitor.h"
#include "tools.h"
#include "PortTypeMapping.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initPortTypeMapping(PortTypeMapping * const this)
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
	this->beanMethodName = NULL;
	this->serviceMethodName = NULL;
	this->paramTypes = NULL;
}

static char
*PortTypeMapping_metaClassName(PortTypeMapping * const this)
{
	return "PortTypeMapping";
}

static char
*PortTypeMapping_internalGetKey(PortTypeMapping * const this)
{
	PortTypeMapping *pObj = (PortTypeMapping*)this;
	return pObj->generated_KMF_ID;
}

static void
delete_PortTypeMapping(PortTypeMapping * const this)
{
	/* destroy base object */
	KMF_VT.delete((KMFContainer*)this);

	/* destroy data members */
	free(this->beanMethodName);
	free(this->serviceMethodName);
	free(this->paramTypes);
}

static void
PortTypeMapping_visit(PortTypeMapping * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	if (visitPaths) {
		sprintf(path, "%s\\ID", parent);
		action(path, STRING, this->generated_KMF_ID);
		sprintf(path, "%s\\beanMethodName", parent);
		action(path, STRING, this->beanMethodName);
		sprintf(path,"%s\\serviceMethodName",parent);
		action(path, STRING, this->serviceMethodName);
		sprintf(path,"%s\\paramTypes",parent);
		action(path, STRING, this->paramTypes);
	} else {
		/*
		 * Visit parent
		 */
		KMF_VT.visit((KMFContainer*)this, parent, action, secondAction, visitPaths);
		action("ID", STRING, this->generated_KMF_ID);
		action(NULL, COLON, NULL);
		action("beanMethodName", STRING, this->beanMethodName);
		action(NULL, COLON, NULL);
		action("serviceMethodName", STRING, this->serviceMethodName);
		action(NULL, COLON, NULL);
		action("paramTypes", STRING, this->paramTypes);
		action(NULL, RETURN, NULL);
	}
}

static void
*PortTypeMapping_findByPath(PortTypeMapping * const this, char *attribute)
{
	/* Local attributes */
	if(!strcmp("beanMethodName", attribute))
	{
		return this->beanMethodName;
	}
	else if(!strcmp("serviceMethodName", attribute))
	{
		return this->serviceMethodName;
	}
	else if(!strcmp("paramTypes", attribute))
	{
		return this->paramTypes;
	}
	else if(!strcmp("generated_KMF_ID", attribute))
	{
		return this->generated_KMF_ID;
	}
	/* There is no local references */
	else
	{
		PRINTF("WARNING: Object not found %s\n", attribute);
		return NULL;
	}
}

const PortTypeMapping_VT portTypeMapping_VT = {
		.super = &KMF_VT,
		/*
		 * KMFContainer
		 */
		.metaClassName = PortTypeMapping_metaClassName,
		.internalGetKey = PortTypeMapping_internalGetKey,
		.visit = PortTypeMapping_visit,
		.findByPath = PortTypeMapping_findByPath,
		.delete = delete_PortTypeMapping
		/*
		 * PortTypeMapping
		 */
};

PortTypeMapping* new_PortTypeMapping()
{
	PortTypeMapping* pObj = NULL;

	/* Allocating memory */
	pObj = malloc(sizeof(PortTypeMapping));

	if (pObj == NULL) {
		PRINTF("ERROR: Cannot create PortTypeMapping!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pObj->VT = &portTypeMapping_VT;

	/*
	 * Initialize itself
	 */
	initPortTypeMapping(pObj);

	return pObj;
}
