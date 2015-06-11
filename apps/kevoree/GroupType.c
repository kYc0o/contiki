#include "NamedElement.h"
#include "TypeDefinition.h"
#include "Visitor.h"
#include "GroupType.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initGroupType(GroupType * const this)
{
	/*
	 * Initialize parent
	 */
	initTypeDefinition((TypeDefinition*)this);

	/*
	 * Initialize itself
	 */
}

static char
*GroupType_metaClassName(void* const this)
{
	return "GroupType";
}

static char
*GroupType_internalGetKey(GroupType * const this)
{
	return typeDefinition_VT.internalGetKey((TypeDefinition*)this);
}

static void
delete_GroupType(GroupType * const this)
{
	/* destroy base object */
	typeDefinition_VT.delete((GroupType*)this);
	/* destroy data members */
	/*
	 * There are no data members
	 */
}

const GroupType_VT groupType_VT = {
		/*
		 * KMFContainer
		 * NamedElement
		 */
		.super = &typeDefinition_VT,
		.metaClassName = GroupType_metaClassName,
		.internalGetKey = /*TypeDefinition_internalGetKey*/GroupType_internalGetKey,
		.getPath = TypeDefinition_getPath,
		.visit = TypeDefinition_visit,
		.findByPath = TypeDefinition_findByPath,
		.delete = delete_GroupType,
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
		 * GroupType
		 */
};

GroupType
*new_GroupType()
{
	GroupType* pGroupTypeObj = NULL;

	/* Allocating memory */
	pGroupTypeObj = malloc(sizeof(GroupType));

	if (pGroupTypeObj == NULL) {
		PRINTF("ERROR: Cannot create GroupType!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pGroupTypeObj->VT = &groupType_VT;

	/*
	 * GroupType
	 */
	initGroupType(pGroupTypeObj);

	return pGroupTypeObj;
}

