#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Repository.h"
#include "Visitor.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initRepository(Repository * const this)
{
	/*
	 * Initialize parent
	 */
	initKMFContainer((KMFContainer*)this);

	/*
	 * Initialize itself
	 */
	this->url = NULL;
}

static char
*Repository_metaClassName(Repository * const this)
{
	return "Repository";
}

static char
*Repository_internalGetKey(Repository * const this)
{
	return this->url;
}

static void
delete_Repository(Repository * const this)
{
	/* destroy base object */
	KMF_VT.delete((KMFContainer*)this);
	/* destroy data members */
	free(this->url);
}

static void
Repository_visit(Repository * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	if (visitPaths) {
		sprintf(path, "%s\\url", parent);
		action(path, STRING, this->url);
	} else {
		/*
		 * Visit parent
		 */
		KMF_VT.visit((KMFContainer*)this, parent, action, secondAction, visitPaths);
		action("url", STRING, this->url);
		action(NULL, RETURN, NULL);
	}

}

static void
*Repository_findByPath(Repository * const this, char *attribute)
{
	if(!strcmp("url", attribute))
	{
		return this->url;
	}
	else
	{
		PRINTF("WARNING: Object not found %s\n", attribute);
		return NULL;
	}
}

const Repository_VT repository_VT = {
		.super = &KMF_VT,
		/*
		 * KMFContainer
		 */
		.metaClassName = Repository_metaClassName,
		.internalGetKey = Repository_internalGetKey,
		.visit = Repository_visit,
		.findByPath = Repository_findByPath,
		.delete = delete_Repository
		/*
		 * Repository
		 */
};

Repository
*new_Repository()
{
	Repository* pObj = NULL;

	/* Allocating memory */
	pObj = malloc(sizeof(Repository));

	if (pObj == NULL) {
		PRINTF("ERROR: Cannot create Repository!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pObj->VT = &repository_VT;

	/*
	 * Repository
	 */
	initRepository(pObj);

	return pObj;
}
