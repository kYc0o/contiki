#include <string.h>
#include <stdlib.h>

#include "NodeType.h"
#include "TypeDefinition.h"
#include "NamedElement.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initNodeType(NodeType * const this)
{
	/*
	 * Initialize parent
	 */
	initTypeDefinition((TypeDefinition*)this);
}

static char
*NodeType_internalGetKey(NodeType * const this)
{
	return typeDefinition_VT.internalGetKey((TypeDefinition*)this);
}

static char
*NodeType_metaClassName(void * const this)
{
	return "NodeType";
}

static void
delete_NodeType(NodeType * const this)
{
	/* destroy base object */
	typeDefinition_VT.delete((TypeDefinition*)this);
	/* destroy data members */
	/*
	 * There are no data members
	 */

}

static void NodeType_visit(NodeType * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	typeDefinition_VT.visit((TypeDefinition*)this, parent, action, secondAction, visitPaths);

}

static void *NodeType_findByPath(NodeType * const this, char *attribute)
{
	return typeDefinition_VT.findByPath((TypeDefinition*)this, attribute);
}

const NodeType_VT nodeType_VT = {
		/*
		 * KMFContainer
		 * NamedElement
		 */
		.super = &typeDefinition_VT,
		.metaClassName = NodeType_metaClassName,
		.internalGetKey = NodeType_internalGetKey,
		.visit = NodeType_visit,
		.findByPath = NodeType_findByPath,
		.delete = delete_NodeType,
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

NodeType* new_NodeType()
{
	NodeType* pNodeTypeObj = NULL;

	/* Allocating memory */
	pNodeTypeObj = malloc(sizeof(NodeType));

	if (pNodeTypeObj == NULL) {
		PRINTF("ERROR: Cannot create NodeType!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pNodeTypeObj->VT = &nodeType_VT;

	/*
	 * NodeType
	 */
	initNodeType(pNodeTypeObj);
	/*
	 * There are no members
	 */

	return pNodeTypeObj;
}
