#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "NamedElement.h"
#include "TypeDefinition.h"
#include "Visitor.h"
#include "PortType.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initPortType(PortType * const this)
{
	initTypeDefinition((TypeDefinition*)this);
	this->synchrone = -1;
}

static char
*PortType_metaClassName(PortType * const this)
{
	return "PortType";
}

static char
*PortType_internalGetKey(PortType * const this)
{
	return typeDefinition_VT.internalGetKey((TypeDefinition*)this);
}

static void
delete_PortType(PortType * const this)
{
	/* destroy base object */
	typeDefinition_VT.delete((TypeDefinition*)this);
	/* destroy data memebers */
	/*
	 * There are no data members
	 */
}

static void
PortType_visit(PortType * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	typeDefinition_VT.visit((TypeDefinition*)this, parent, action, secondAction, visitPaths);

	if (visitPaths) {
		sprintf(path, "%s\\synchrone", parent);
		action(path, BOOL, (void*)(this->synchrone));
	} else {
		action("synchrone", BOOL, (void*)this->synchrone);
		action(NULL, RETURN, NULL);
	}
}

void
*PortType_findByPath(PortType * const this, char *attribute)
{
	/* TypeDefinition attributes */
	/* Local attributes */
	if(!strcmp("synchrone", attribute)) {
		return (void*)this->synchrone;
	}
	/* TypeDefinition references */
	else {
		return typeDefinition_VT.findByPath((TypeDefinition*)this, attribute);
	}
}

const PortType_VT portType_VT = {
		/*
		 * KMFContainer
		 * NamedElement
		 */
		.super = &typeDefinition_VT,
		.metaClassName = PortType_metaClassName,
		.internalGetKey = PortType_internalGetKey,
		.getPath = TypeDefinition_getPath,
		.visit = PortType_visit,
		.findByPath = TypeDefinition_findByPath,
		.delete = delete_PortType,
		/*
		 * TypeDefinition
		 */
		.addDeployUnit = TypeDefinition_addDeployUnit,
		.addDictionaryType = TypeDefinition_addDictionaryType,
		.addSuperTypes = TypeDefinition_addSuperTypes,
		.removeDeployUnit = TypeDefinition_removeDeployUnit,
		.removeDictionaryType = TypeDefinition_removeDictionaryType,
		.removeSuperTypes = TypeDefinition_removeSuperTypes
		/*
		 * NodeType
		 */
};

PortType
*new_PortType()
{
	PortType* pPortTypeObj = NULL;

	/* Allocating memory */
	pPortTypeObj = (PortType*)malloc(sizeof(PortType));

	if (pPortTypeObj == NULL) {
		PRINTF("ERROR: Cannot create PortType!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pPortTypeObj->VT = &portType_VT;
	/*
	 * PortType
	 */
	initPortType(pPortTypeObj);

	return pPortTypeObj;
}
