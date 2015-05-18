#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "NamedElement.h"
#include "TypedElement.h"
#include "DictionaryType.h"
#include "Visitor.h"
#include "DictionaryAttribute.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initDictionaryAttribute(DictionaryAttribute * const this)
{
	/*
	 * Initialize parent
	 */
	initTypedElement((TypedElement*)this);

	/*
	 * Initialize itself
	 */
	this->optional = -1;
	this->state = -1;
	this->datatype = NULL;
	this->fragmentDependant = -1;
	this->defaultValue = NULL;
}

static void
delete_DictionaryAttribute(DictionaryAttribute * const this)
{
	/* destroy base object */
	typedElement_VT.delete((TypedElement*)this);

	/* destroy data members */
	free(this->datatype);
	free(this->defaultValue);
}

static char
*DictionaryAttribute_internalGetKey(DictionaryAttribute * const this)
{
	return typedElement_VT.internalGetKey((TypedElement*)this);
}

static char
*DictionaryAttribute_metaClassName(DictionaryAttribute * const this)
{
	return "DictionaryAttribute";
}

static void
DictionaryAttribute_visit(DictionaryAttribute * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	/*
	 * Visit parent
	 */
	typedElement_VT.visit((DictionaryAttribute*)this, parent, action, secondAction, visitPaths);

	/* Local attributes */
	if (visitPaths) {
		sprintf(path, "%s\\optional", parent);
		action(path, BOOL, (void*)this->optional);
		sprintf(path, "%s\\state", parent);
		action(path, BOOL, (void*)this->state);
		sprintf(path, "%s\\datatype", parent);
		action(path, STRING, this->datatype);
		sprintf(path, "%s\\fragmentDependant", parent);
		action(path, BOOL, (void*)this->fragmentDependant);
		sprintf(path, "%s\\defaultValue", parent);
		action(path, STRING, this->defaultValue);
	} else {
		action("optional", BOOL, (void*)this->optional);
		action(NULL, COLON, NULL);
		action("state", BOOL, (void*)this->state);
		action(NULL, COLON, NULL);
		action("datatype", STRING, this->datatype);
		action(NULL, COLON, NULL);
		action("fragmentDependant", BOOL, (void*)this->fragmentDependant);
		action(NULL, COLON, NULL);
		action("defaultValue", STRING, this->defaultValue);
		action(NULL, RETURN, NULL);
	}

}

static void
*DictionaryAttribute_findByPath(DictionaryAttribute * const this, char *attribute)
{
	/* TypedElement attributes and references */
	/* Local attributes */
	if(!strcmp("optional", attribute))
	{
		return (void*)this->optional;
	}
	else if(!strcmp("state", attribute))
	{
		return (void*)this->state;
	}
	else if(!strcmp("datatype", attribute))
	{
		return this->datatype;
	}
	else if(!strcmp("fragmentDependant", attribute))
	{
		return (void*)this->fragmentDependant;
	}
	else if(!strcmp("defaultValue", attribute))
	{
		return this->defaultValue;
	}
	/* There is no local references */
	else
	{
		return typedElement_VT.findByPath((TypedElement*)this, attribute);
	}
}

const DictionaryAttribute_VT dictionaryAttribute_VT = {
		.super = &typedElement_VT,
		/*
		 * KMFContainer
		 * NamedElement
		 */
		.metaClassName = DictionaryAttribute_metaClassName,
		.internalGetKey = DictionaryAttribute_internalGetKey,
		.visit = DictionaryAttribute_visit,
		.findByPath = DictionaryAttribute_findByPath,
		.delete = delete_DictionaryAttribute,
		/*
		 * TypedElement
		 */
		.findGenericTypesByID = TypedElement_findGenericTypesByID,
		.addGenericTypes = TypedElement_addGenericTypes,
		.removeGenericTypes = TypedElement_removeGenericTypes
		/*
		 * DictionaryAttribute
		 */
};

DictionaryAttribute* new_DictionaryAttribute()
{
	DictionaryAttribute* pDicAttrObj = NULL;

	/* Allocating memory */
	pDicAttrObj = malloc(sizeof(DictionaryAttribute));

	if (pDicAttrObj == NULL) {
		PRINTF("ERROR: Cannot create DictionaryAttribute!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pDicAttrObj->VT = &dictionaryAttribute_VT;

	/*
	 * DictionaryAttribute
	 */
	initDictionaryAttribute(pDicAttrObj);

	return pDicAttrObj;
}
