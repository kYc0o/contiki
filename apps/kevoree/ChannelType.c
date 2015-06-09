#include "NamedElement.h"
#include "TypeDefinition.h"
#include "Visitor.h"
#include "ChannelType.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initChannelType(ChannelType * const this)
{
	/*
	 * Initialize parent
	 */
	initTypeDefinition((TypeDefinition*)this);

	/*
	 * Initialize itself
	 */
	this->lowerBindings = -1;
	this->upperBindings = -1;
	this->lowerFragments = -1;
	this->upperFragments = -1;
}

void
ChannelType_addDeployUnit(ChannelType * const this, DeployUnit *ptr)
{
	this->deployUnits = ptr;
}

void
ChannelType_addDictionaryType(ChannelType * const this, DictionaryType *ptr)
{
	typeDefinition_VT.addDictionaryType((TypeDefinition*)this, ptr);
}

void
ChannelType_removeDeployUnit(ChannelType * const this, DeployUnit *ptr)
{
	this->deployUnits = NULL;
}

void
ChannelType_removeDictionaryType(ChannelType * const this, DictionaryType *ptr)
{
	typeDefinition_VT.removeDictionaryType((TypeDefinition*)this, ptr);
}

void
ChannelType_addSuperTypes(ChannelType * const this, TypeDefinition *ptr)
{
	typeDefinition_VT.addSuperTypes((TypeDefinition*)this, ptr);
}

void
ChannelType_removeSuperTypes(ChannelType * const this, TypeDefinition *ptr)
{
	typeDefinition_VT.removeSuperTypes((TypeDefinition*)this, ptr);
}


void
delete_ChannelType(ChannelType * const this)
{
	/* destroy base object */
	typeDefinition_VT.delete((TypeDefinition*)this);
	/* destroy data members */
	/*
	 * No data members
	 */
}

static char
*ChannelType_internalGetKey(ChannelType* const this)
{
	return typeDefinition_VT.internalGetKey((TypeDefinition*)this);
}

static char
*ChannelType_metaClassName(ChannelType * const this)
{
	return "ChannelType";
}

static void
ChannelType_visit(ChannelType * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	typeDefinition_VT.visit((TypeDefinition*)this, parent, action, secondAction, visitPaths);

	if (visitPaths) {
		sprintf(path, "%s\\lowerBindings", parent);
		action(path, BOOL, (void*)this->lowerBindings);
		sprintf(path, "%s\\upperBindings", parent);
		action(path, BOOL, (void*)this->upperBindings);
		sprintf(path, "%s\\lowerFragments", parent);
		action(path, BOOL, (void*)this->lowerFragments);
		sprintf(path, "%s\\upperFragments", parent);
		action(path, BOOL, (void*)this->upperFragments);
	} else {
		action("lowerBindings", INTEGER, (void*)this->lowerBindings);
		action(NULL, COLON, NULL);
		action("upperBindings", INTEGER, (void*)this->upperBindings);
		action(NULL, COLON, NULL);
		action("lowerFragments", INTEGER, (void*)this->lowerFragments);
		action(NULL, COLON, NULL);
		action("upperFragments", INTEGER, (void*)this->upperFragments);
		action(NULL, COLON, NULL);
	}
}

static void
*ChannelType_findByPath(ChannelType * const this, char *attribute)
{
	/* Local attributes */
	if(!strcmp("lowerBindings", attribute)) {
		return (void*)this->lowerBindings;
	} else if(!strcmp("upperBindings", attribute)) {
		return (void*)this->upperBindings;
	} else if(!strcmp("lowerFragments", attribute)) {
		return (void*)this->lowerFragments;
	} else if(!strcmp("upperFragments", attribute)) {
		return (void*)this->upperFragments;
	}
	/* TypeDefinition references */
	else {
		return typeDefinition_VT.findByPath((TypeDefinition*)this, attribute);
	}
}

const ChannelType_VT channelType_VT = {
		/*
		 * KMFContainer
		 * NamedElement
		 */
		.super = &typeDefinition_VT,
		.metaClassName = ChannelType_metaClassName,
		.internalGetKey = ChannelType_internalGetKey,
		.getPath = TypeDefinition_getPath,
		.visit = ChannelType_visit,
		.findByPath = ChannelType_findByPath,
		.delete = delete_ChannelType,
		/*
		 * TypeDefinition
		 */
		.addDeployUnit = ChannelType_addDeployUnit,
		.addDictionaryType = ChannelType_addDictionaryType,
		.addSuperTypes = ChannelType_addSuperTypes,
		.removeDeployUnit = ChannelType_removeDeployUnit,
		.removeDictionaryType = ChannelType_removeDictionaryType,
		.removeSuperTypes = ChannelType_removeSuperTypes
		/*
		 * ChannelType
		 */
};

ChannelType
*new_ChannelType()
{
	ChannelType *pChanTypeObj = NULL;

	/* Allocating memory */
	pChanTypeObj = malloc(sizeof(ChannelType));

	if (pChanTypeObj == NULL) {
		PRINTF("ERROR: Cannot create ChannelType!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pChanTypeObj->VT = &channelType_VT;

	/*
	 * ChannelType
	 */
	initChannelType(pChanTypeObj);

	return pChanTypeObj;
}
