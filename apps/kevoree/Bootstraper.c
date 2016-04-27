/*
 * Bootstraper.c
 *
 *  Created on: Jan 28, 2015 1:37:29 PM
 *      Author: Francisco Acosta
 *       eMail: fco.ja.ac@gmail.com
 */
#include <stdlib.h>

#include "Bootstraper.h"

Bootstraper *new_Bootstraper()
{
	Bootstraper *pObj = malloc(sizeof(Bootstraper));

	if (pObj == NULL) {
		return NULL;
	}

	/*
	 * Pointing to itself since it's a parent
	 */
	pObj->pDerivedObj = pObj;

	pObj->bootstrapNodeType = Bootstraper_bootsraperNodeType;
	pObj->resolveDeployUnit = Bootstraper_resolveDeployUnit;
	pObj->getDynamicLoader = Bootstraper_getDynamicLoader;
	pObj->setProposedNewModel = Bootstraper_setProposedNewModel;
	pObj->delete = delete_Bootstraper;

	return pObj;
}

void delete_Bootstraper(Bootstraper* const this)
{
	if(this != NULL) {
		free(this);
	}
}
