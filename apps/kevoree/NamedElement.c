#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "NamedElement.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initNamedElement(NamedElement * const this)
{
	initKMFContainer((KMFContainer*)this);
	this->name = NULL;
}

static char
*NamedElement_internalGetKey(NamedElement * const this)
{
	return this->name;
}

static char
*NamedElement_metaClassName(void * const this)
{
	return "NamedElement";
}

static void
delete_NamedElement(NamedElement * const this)
{
	KMF_VT.delete((KMFContainer*)this);
	if(this->name != NULL) {
		free(this->name);
	}
}

static void
NamedElement_visit(NamedElement * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	if(visitPaths) {
		sprintf(path, "%s\\name", parent);
		action(path, STRING, this->name);
	} else {
		KMF_VT.visit((KMFContainer*)this, parent, action, secondAction, visitPaths);
		action("name", STRING, this->name);
		action(NULL, COLON, NULL);
	}
}

static void
*NamedElement_findByPath(NamedElement * const this, char *attribute)
{
	if(!strcmp("name", attribute)) {
		return this->name;
	} else {
		PRINTF("ERROR: Wrong attribute %s\n", attribute);
		return NULL;
	}
}

const NamedElement_VT namedElement_VT = {
		.super = &KMF_VT,
		/*
		 * KMFContainer
		 * NamedElement
		 */
		.metaClassName = NamedElement_metaClassName,
		.internalGetKey = NamedElement_internalGetKey,
		.getPath = KMFContainer_get_path,
		.visit = NamedElement_visit,
		.findByPath = NamedElement_findByPath,
		.delete = delete_NamedElement
};
