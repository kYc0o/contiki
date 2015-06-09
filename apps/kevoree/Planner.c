/*
 * Planner.c
 *
 *  Created on: May 5, 2015 9:57:23 PM
 *      Author: Francisco Acosta
 *       eMail: fco.ja.ac@gmail.com
 */

#include "KMFContainer.h"
#include "Planner.h"
#include "ContainerRoot.h"
#include "TraceSequence.h"
#include "AdaptationPrimitive.h"
#include "Primitives.h"
#include "ContainerNode.h"
#include "Instance.h"
#include "DictionaryValue.h"
#include "ActionType.h"
#include "TypeDefinition.h"
#include "DeployUnit.h"
#include "ComponentInstance.h"
#include "ModelTrace.h"

#include "lib/list.h"

#include <stdbool.h>

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
LIST(adaptations);

static void AdaptationModel_add(AdaptationPrimitive *ptr)
{
	/*PRINTF("INFO: Adding trace %s\n", ptr->ref->path);*/
	list_add(adaptations, ptr);
}

void Planner_compareModels(ContainerRoot *currModel, ContainerRoot *targetModel, char *nodeName, TraceSequence *traces)
{
	ContainerNode *currentNode = currModel->VT->findNodesByID(currModel, nodeName);
	if (currentNode == NULL) {
		PRINTF("ERROR: node %s cannot be found in current model!\n", nodeName);
		return;
	}
	ContainerNode *targetNode = targetModel->VT->findNodesByID(currModel, nodeName);
	if (targetNode == NULL) {
		PRINTF("ERROR: node %s cannot be found in target model!\n", nodeName);
	}
	int tracesLength, i;
	bool isFirst = true;

	tracesLength = list_length(traces->traces_list);
	PRINTF("INFO: Received traces of length %d\n", tracesLength);
	ModelTrace *trace;

	list_init(adaptations);
	
	/* save the path for future usage */
	char* targetNode_path = targetNode->VT->getPath(targetNode);
	for (trace = list_head(traces->traces_list); trace != NULL; trace = list_item_next(trace)) {
		/*PRINTF("INFO: Passing trace %s\n", trace->refName);*/

		KMFContainer *modelElement = targetModel->VT->findByPath(targetModel, trace->srcPath);

		if(!strcmp(trace->refName, "components"))
		{			
			if(!strcmp(trace->srcPath, targetNode_path))
			{
				if (trace->vt->getType() == ADD) {
					KMFContainer *elemToAdd = targetModel->VT->findByPath(targetModel, ((ModelAddTrace*)trace)->previousPath);
					char* container_path = get_eContainer_path(elemToAdd);
					if (!strcmp(container_path, targetNode_path)) {
						AdaptationModel_add(Planner_adapt(AddInstance, elemToAdd));
					}
					free(container_path);
				} else if (trace->vt->getType() == REMOVE) {
					KMFContainer *elemToAdd = currModel->VT->findByPath(currModel, ((ModelRemoveTrace*)trace)->objPath);
					char* container_path = get_eContainer_path(elemToAdd);
					if (!strcmp(container_path, targetNode_path)) {
						AdaptationModel_add(Planner_adapt(StopInstance, elemToAdd));
						AdaptationModel_add(Planner_adapt(RemoveInstance, elemToAdd));
					}
					free(container_path);
				} else {
					PRINTF("ERROR: Cannot cast ModelTrace!\n");
				}
			}
		} else if(!strcmp(trace->refName, "started")) {
			if (
					(
							(!strcmp(modelElement->VT->metaClassName(modelElement), "ComponentInstance")) /*||
							(!strcmp(modelElement->metaClassName(modelElement), "ContainerNode")) ||
							(!strcmp(modelElement->metaClassName(modelElement), "Group"))*/
					) &&
					(trace->vt->getType() == SET)
			) {
				ModelSetTrace *modelsettrace = (ModelSetTrace*)trace;

				if (!strcmp(modelsettrace->srcPath, targetNode_path)) {
					PRINTF("HARAKIRI: %s\n", modelsettrace->vt->ToString(modelsettrace));
				} else {
					KMFContainer *comp = targetModel->VT->findByPath(targetModel, modelsettrace->srcPath);
					char* container_path = get_eContainer_path(comp);
					if (comp != NULL && !strcmp(container_path, targetNode_path)) {
						if (!strcmp(modelsettrace->content, "true")) {
							AdaptationModel_add(Planner_adapt(StartInstance, modelElement));
						} else {
							AdaptationModel_add(Planner_adapt(StopInstance, modelElement));
						}
					}
					free(container_path);
				}
			}
		} else if(!strcmp(trace->refName, "value")) {
			if (!strcmp(modelElement->VT->metaClassName(modelElement), "DictionaryValue")) {
				char* container_path = get_eContainer_path(modelElement);
				KMFContainer *container = targetModel->VT->findByPath(targetModel, container_path);
				free(container_path);
				container_path = get_eContainer_path(container);
				KMFContainer *container2 = targetModel->VT->findByPath(targetModel, container_path);
				free(container_path);
				container_path = get_eContainer_path(container2);
				if (!strcmp(container_path, targetNode_path)) {
					AdaptationModel_add(Planner_adapt(UpdateDictionaryInstance, container2));
				}
				free(container_path);
				/*
				 * Check why modelElement->eContainer->eContainer
				 */
			}
		} else if (!strcmp(trace->refName, "typeDefinition")) {
			if (!strcmp(modelElement->VT->metaClassName(modelElement), "ComponentInstance")) {
				ComponentInstance *ci = (ComponentInstance*)modelElement;
				char* container_path = get_eContainer_path(ci);
				if (!strcmp(container_path, targetNode_path)) {
					TypeDefinition *t = ci->typeDefinition;
					DeployUnit *du = t->deployUnits;
					AdaptationModel_add(Planner_adapt(AddDeployUnit, (KMFContainer*)du));
				}
				free(container_path);
			}
		}

	}
	/* free some memory */
	free(targetNode_path);
}

list_t Planner_schedule()
{
	int i, j, adaptLength;
	bool isFirst = true;
	LIST(sortedAdapt);
	list_init(sortedAdapt);

	AdaptationPrimitive *ap;

	for (j = 0; j <= PRIORITY_MAX; ++j) {
		adaptLength = list_length(adaptations);
		for (i = 0; i < adaptLength; ++i) {
			ap = list_pop(adaptations);
			if (ap->priority == j) {
				list_add(sortedAdapt, ap);
			} else {
				list_add(adaptations, ap);
			}
		}
	}

	return sortedAdapt;
}

AdaptationPrimitive *Planner_adapt(Primitives p, KMFContainer *elem)
{
	AdaptationPrimitive *ccmd = new_AdaptationPrimitive();

	if (ccmd == NULL) {
		PRINTF("ERROR: AdaptationPrimitive cannot be created!\n");
		return NULL;
	}

	ccmd->primitiveType = p;
	ccmd->priority = Priority_Primitives(p);
	ccmd->ref = elem;
	return ccmd;
}
