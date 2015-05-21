/*
 * KMF4C.c
 *
 *  Created on: May 11, 2015 12:45:19 AM
 *      Author: Francisco Acosta
 *       eMail: fco.ja.ac@gmail.com
 */

#include "KMFContainer.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

char *my_strdup(const char *string)
{
	char *r = malloc (strlen(string) + 1);
	strcpy(r, string);
	if (!strcmp(string, "Group")) {
		printf("Te agarramos cabron! : %s\n", string);
	}
	return r;
}

void
initKMFContainer(KMFContainer * const this)
{
	/*
	 * KMFContainer
	 */
	this->eContainer = NULL;
	this->path = NULL;
}

static void
delete_KMFContainer(KMFContainer * const this)
{
	if (this->path != NULL) {
		free(this->path);
	}
	if (this->eContainer != NULL) {
		free(this->eContainer);
	}
}

void
delete(KMFContainer *container)
{
	container->VT->delete(container);
	free(container);
}


void
deleteContainerContents(map_t container)
{
	hashmap_map *m = (hashmap_map*)container;
	int i;
	for(i = 0; i< m->table_size; i++) {
		if(m->data[i].in_use != 0) {
			any_t data = (any_t) (m->data[i].data);
			KMFContainer *n = data;
			delete(n);
		}
	}
}

static void
KMFContainer_visit(KMFContainer * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char class[100];
	sprintf(class, "org.kevoree.%s", this->VT->metaClassName(this));
	action("eClass", STRING, class);
	action(NULL, COLON, NULL);
}

const KMFContainer_VT KMF_VT = {
		.super = NULL,
		.metaClassName = NULL,
		.internalGetKey = NULL,
		.visit = KMFContainer_visit,
		.findByPath = NULL,
		.delete = delete_KMFContainer
};
