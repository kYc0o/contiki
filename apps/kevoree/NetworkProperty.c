#include <stdlib.h>
#include <string.h>
#include "NamedElement.h"
#include "NetworkInfo.h"
#include "NodeLink.h"
#include "Visitor.h"
#include "NetworkProperty.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initNetworkProperty(NetworkProperty * const this)
{
	/*
	 * Initialize parent
	 */
	initNamedElement((NamedElement*)this);

	/*
	 * Initialize itself
	 */
	this->value = NULL;
}

static char
*NetworkProperty_internalGetKey(NetworkProperty * const this)
{
	return namedElement_VT.internalGetKey((NamedElement*)this);
}

static char
*NetworkProperty_metaClassName(NetworkProperty * const this)
{
	return "NetworkProperty";
}

static void
delete_NetworkProperty(NetworkProperty * const this)
{
	/* destroy base object */
	namedElement_VT.delete((NamedElement*)this);

	/* destroy data members */
	free(this->value);
}

static void
NetworkProperty_visit(NetworkProperty * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	/*
	 * Visit parent
	 */
	namedElement_VT.visit((NamedElement*)this, parent, action, secondAction, visitPaths);

	if (visitPaths) {
		sprintf(path, "%s\\value", parent);
		action(path, STRING, this->value);
	} else {
		action("value", STRING, this->value);
		action(NULL, RETURN, NULL);
	}
}

static void
*NetworkProperty_findByPath(NetworkProperty * const this, char *attribute)
{
	/* NamedElement attributes */
	/* Local attributes */
	if(!strcmp("value", attribute)) {
		return this->value;
	} else {
		return namedElement_VT.findByPath((NamedElement*)this, attribute);
	}
}

static char*
NetworkProperty_getPath(KMFContainer* kmf)
{
	NetworkProperty* obj = (NetworkProperty*)kmf;
	char* tmp = (obj->eContainer)?get_eContainer_path(obj):strdup("");
	char* r = (char*)malloc(strlen(tmp) + strlen("/values[]") + strlen(obj->VT->internalGetKey(obj)) + 1);
	sprintf(r, "%s/values[%s]", tmp, obj->VT->internalGetKey(obj));
	free(tmp);
	return r;
}

const NetworkProperty_VT networkProperty = {
		.super = &namedElement_VT,
		/*
		 * KMFContainer
		 * NamedElement
		 */
		.metaClassName = NetworkProperty_metaClassName,
		.internalGetKey = NetworkProperty_internalGetKey,
		.getPath = NetworkProperty_getPath,
		.visit = NetworkProperty_visit,
		.findByPath = NetworkProperty_findByPath,
		.delete = delete_NetworkProperty,
		/*
		 * NetworkProperty
		 */
};

NetworkProperty* new_NetworkProperty()
{
	NetworkProperty* pNetPropObj = NULL;

	/* Allocating memory */
	pNetPropObj = malloc(sizeof(NetworkProperty));

	if (pNetPropObj == NULL) {
		PRINTF("ERROR: Cannot create a NetworkProperty!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pNetPropObj->VT = &networkProperty;

	/*
	 * NetworkProperty
	 */
	initNetworkProperty(pNetPropObj);

	return pNetPropObj;
}
