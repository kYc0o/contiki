#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Dictionary.h"
#include "DictionaryValue.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initDictionaryValue(DictionaryValue * const this)
{
	/*
	 * Initialize parent
	 */
	initKMFContainer((KMFContainer*)this);

	/*
	 * Initialize itself
	 */
	this->name = NULL;
	this->value = NULL;
}

static void
delete_DictionaryValue(DictionaryValue * const this)
{
	/* delete base object */
	KMF_VT.delete((KMFContainer*)this);
	/* delete data members */
	free(this->name);
	free(this->value);
}

static char
*DictionaryValue_internalGetKey(DictionaryValue * const this)
{
	return this->name;
}

static char
*DictionaryValue_metaClassName(DictionaryValue * const this)
{
	return "DictionaryValue";
}

static void
DictionaryValue_visit(DictionaryValue * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	if (visitPaths) {
		sprintf(path, "%s\\name", parent);
		action(path, STRING, this->name);

		sprintf(path, "%s\\value", parent);
		action(path, STRING, this->value);
	} else {
		/*
		 * Visit parent
		 */
		KMF_VT.visit((KMFContainer*)this, parent, action, secondAction, visitPaths);
		action("name", STRING, this->name);
		action(NULL, COLON, NULL);
		action("value", STRING, this->value);
		action(NULL, RETURN, NULL);
	}
}

void
*DictionaryValue_findByPath(DictionaryValue * const this, char *attribute)
{
	/* Local attributes */

	if(!strcmp("name", attribute)) {
		return this->name;
	} else if(!strcmp("value", attribute)) {
		return this->value;
	}
	/* There is no local references */
	else {
		PRINTF("WARNING: Wrong attribute or reference %s\n", attribute);
		return NULL;
	}
}

const DictionaryValue_VT dictionaryValue_VT = {
		.super = &KMF_VT,
		/*
		 * KMFContainer
		 */
		.metaClassName = DictionaryValue_metaClassName,
		.internalGetKey = DictionaryValue_internalGetKey,
		.visit = DictionaryValue_visit,
		.findByPath = DictionaryValue_findByPath,
		.delete = delete_DictionaryValue
};

DictionaryValue
*new_DictionaryValue(void)
{
	DictionaryValue *pObj = NULL;

	/* Allocating memory */
	pObj = malloc(sizeof(DictionaryValue));

	if (pObj == NULL) {
		PRINTF("ERROR: Cannot create DictionaryValue!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pObj->VT = &dictionaryValue_VT;

	/*
	 * DictionaryValue
	 */
	initDictionaryValue(pObj);

	return pObj;
}
