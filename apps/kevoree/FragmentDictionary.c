#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Dictionary.h"
#include "Instance.h"
#include "Visitor.h"
#include "FragmentDictionary.h"
#include "tools.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initFragmentDictionary(FragmentDictionary * const this)
{
	/*
	 * Initialize parent
	 */
	initDictionary((Dictionary*)this);

	/*
	 * Initialize itself
	 */
	this->name = NULL;
}

static void
delete_FragmentDictionary(FragmentDictionary * const this)
{
	/* destroy base object */
	dictionary_VT.delete((Dictionary*)this);
	/* destroy data members */
	/*
	 * There are no data members
	 */
	free(this->name);
}

static char
*FragmentDictionary_internalGetKey(FragmentDictionary * const this)
{
	return this->name;
}

static char
*FragmentDictionary_metaClassName(FragmentDictionary * const this)
{
	return "FragmentDictionary";
}

static void
FragmentDictionary_visit(FragmentDictionary * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	/*
	 * Visit parent
	 */
	dictionary_VT.visit((FragmentDictionary*)this, parent, action, secondAction, visitPaths);


	if (visitPaths) {
		sprintf(path, "%s\\name", parent);
		action(path, STRING, this->name);
	} else {
		action(NULL, COLON, NULL);
		action("name", STRING, ((FragmentDictionary*)(this))->name);
		action(NULL, RETURN, NULL);
	}
}

static void
*FragmentDictionary_findByPath(FragmentDictionary * const this, char *attribute)
{
	void *try = NULL;

	/* Local attributes */
	/* Dictionary attributes and references */
	if (!strcmp("name", attribute)) {
		return this->name;
	} else {
		return dictionary_VT.findByPath((Dictionary*)this, attribute);
	}
}

static char*
FragmentDictionary_getPath(KMFContainer* kmf)
{
	FragmentDictionary* obj = (FragmentDictionary*)kmf;
	char* tmp = (obj->eContainer)?get_eContainer_path(obj):strdup("");
	char* r = (char*)malloc(strlen(tmp) + strlen("/fragmentDictionary[]") + strlen(obj->VT->internalGetKey(obj)) + 1);
	sprintf(r, "%s/fragmentDictionary[%s]", tmp, obj->VT->internalGetKey(obj));
	free(tmp);
	return r;
}

const FragmentDictionary_VT fragmentDictionary_VT = {
		.super = &dictionary_VT,
		/*
		 * KMFContainer
		 * NamedElement
		 */
		.metaClassName = FragmentDictionary_metaClassName,
		.internalGetKey = FragmentDictionary_internalGetKey,
		.getPath = FragmentDictionary_getPath,
		.visit = FragmentDictionary_visit,
		.findByPath = FragmentDictionary_findByPath,
		.delete = delete_FragmentDictionary,
		/*
		 * Dictionary
		 */
		.addValues = Dictionary_addValues,
		.removeValues = Dictionary_removeValues,
		.findValuesByID = Dictionary_findValuesByID
		/*
		 * FragmentDictionary
		 */
};

FragmentDictionary
*new_FragmentDictionary(void)
{
	FragmentDictionary* pFragDicObj = NULL;

	/* Allocating memory */
	pFragDicObj = malloc(sizeof(FragmentDictionary));

	if (pFragDicObj == NULL) {
		PRINTF("ERROR: Cannot create FragmentDictionary!\n");
		return NULL;
	}

	pFragDicObj->VT = &fragmentDictionary_VT;
	/*
	 * KMFContainer
	 */
	pFragDicObj->eContainer = NULL;
	/*
	 * Dictionary
	 */
	memset(&pFragDicObj->generated_KMF_ID[0], 0, sizeof(pFragDicObj->generated_KMF_ID));
	rand_str(pFragDicObj->generated_KMF_ID, 8);
	pFragDicObj->values = NULL;
	/*
	 * FragmentDictionary
	 */
	initFragmentDictionary(pFragDicObj);

	return pFragDicObj;
}
