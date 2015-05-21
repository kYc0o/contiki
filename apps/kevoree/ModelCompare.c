/*
 * ModelCompare.c
 *
 *  Created on: Apr 29, 2015 5:58:41 PM
 *      Author: Francisco Acosta
 *       eMail: fco.ja.ac@gmail.com
 */

#include "ModelCompare.h"
#include "ContainerRoot.h"
#include "Visitor.h"
#include "ActionType.h"
#include "TraceSequence.h"
#include "ModelTrace.h"

#include "lib/list.h"

#include <stdio.h>
#include <stdbool.h>

#define MAX_NUMBER 8

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

static ContainerRoot *current_model = NULL;
static ContainerRoot *new_model = NULL;
static ContainerNode *myself = NULL;

LIST(model_traces);

static void actionprintpath(char *path, Type type, void *value)
{
	switch(type)
	{
	case STRING:
	case REFERENCE:
		printf("path = %s  value = %s\n",path,(char*)value);
		break;

	case BOOL:
	case INTEGER:
		printf("path = %s  value = %d\n", path, (int)value);
		break;

	case STRREF:
	case BRACKET:
	case SQBRACKET:
	case CLOSEBRACKET:
	case CLOSESQBRACKET:
	case CLOSEBRACKETCOLON:
	case CLOSESQBRACKETCOLON:
	case COLON:
	case RETURN:
		printf("Type non valid!\n");
		break;
	}
}

TraceSequence *ModelCompare(ContainerRoot *_newModel, ContainerRoot *_currentModel, char *nodeName)
{
	if (_newModel == NULL) {
		PRINTF("ERROR: new model is NULL!\n");
		return NULL;
	}
	if (_currentModel == NULL) {
		PRINTF("ERROR: current model is NULL!\n");
		return NULL;
	}
	if ((myself = _newModel->VT->findNodesByID(_newModel, nodeName)) == NULL) {
		PRINTF("INFO: the model does not correspond with this node!\n");
		TraceSequence *ts = new_TraceSequence();
		/*
		 * TODO delete this node from the model and exit
		 */
		return ts;
	}

	current_model = _currentModel;
	new_model = _newModel;

	TraceSequence *ts = new_TraceSequence();
	char *trace_sequence;

	if(ts == NULL) {
		return NULL;
	}

	PRINTF("INFO: new_model detected, comparing with curent_model\n");
	current_model->VT->visit(current_model, "", actionUpdate, actionRemove, true);
	PRINTF("INFO: current_model compared, comparing new_model\n");
	new_model->VT->visit(new_model, "", actionAddSet, actionAdd, true);
	PRINTF("INFO: diff finished!\n");

	if(model_traces == NULL) {
		printf("ERROR: Cannot create traces\n");
		return NULL;
	}
	PRINTF("INFO: Traces created successfully\n");

	ts->populate(ts, model_traces);

	return ts;
}

void actionprintf(char *path, Type type, void* value)
{
	switch(type)
	{
	case STRING:
		printf("\"%s\" : \"%s\"", path, (char*)value);
		break;

	case STRREF:
		printf("\"%s\"", path);
		break;

	case BOOL:
		if ((int)value == 1) {
			printf("\"%s\" : \"%s\"", path, "true");
		} else {
			printf("\"%s\" : \"%s\"", path, "false");
		}
		break;

	case INTEGER:
		printf("\"%s\" : \"%d\"", path, (int)value);
		break;

	case BRACKET:
		printf("{\n");
		break;

	case SQBRACKET:
		printf("\"%s\" : [\n", path);
		break;

	case CLOSEBRACKET:
		printf("}\n");
		break;

	case CLOSESQBRACKET:
		printf("]\n");
		break;

	case CLOSEBRACKETCOLON:
		printf("},\n");
		break;

	case CLOSESQBRACKETCOLON:
		printf("],\n");
		break;

	case COLON:
		printf(",\n");
		break;

	case RETURN:
		printf("\n");
		break;

	case REFERENCE:
		printf("ERROR: non valid type\n");
		break;
	}
}

void *actionRemove(char *_path, void *value)
{
	char *refname = NULL;
	char *src = NULL;
	KMFContainer *container;

	if ((container = new_model->VT->findByPath(new_model, _path)) != NULL) {
		return (void*)true;
	} else if ((container = (KMFContainer*)current_model->VT->findByPath(current_model, _path)) != NULL) {
		if ((src = strdup(container->eContainer)) != NULL) {
			ModelTrace *mt = newPoly_ModelRemoveTrace(src, value, _path);
			if (mt != NULL)	{
				list_add(model_traces, mt);
				free(src);
				return (void*)false;
			} else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", _path, value);
				return (void*)true;
			}
		} else {
			PRINTF("ERROR: not enough memory for src!\n");
			return (void*)true;
		}
	}

	return (void*)true;
}

void *actionAdd(char* _path, void *value)
{
	char *refname = NULL;
	char *src = NULL;
	KMFContainer *container;

	if ((container = current_model->VT->findByPath(current_model, _path)) != NULL) {
		return (void*)true;
	} else if ((container = (KMFContainer*)new_model->VT->findByPath(new_model, _path)) != NULL) {
		if ((src = strdup(container->eContainer)) != NULL) {
			ModelTrace *mt = newPoly_ModelAddTrace(src, value, _path, container->VT->metaClassName(container));
			if (mt != NULL)	{
				list_add(model_traces, mt);
				free(src);
				return (void*)true;
			} else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", _path, (char*)value);
				return (void*)true;
			}
		} else {
			PRINTF("ERROR: not enough memory for src!\n");
			return (void*)true;
		}
	}

	return (void*)true;
}

void actionUpdate(char* _path, Type type, void* value)
{
	char *__path = strdup(_path);
	char path[250];
	char *path2 = strdup(_path);
	char *path3 = NULL;
	char *refname = NULL;
	char *src = NULL;
	KMFContainer *container;

	if ((refname = strtok(path2, "\\")) != NULL) {
		if ((refname = strtok(NULL, "\\")) == NULL) {
			refname = strtok(path2, "\\");
		}
	}

	strcpy(path, strtok(__path, "\\"));

	if (!strcmp("generated_KMF_ID", path)) {
		strcpy(path, "");
	}

	if ((container = new_model->VT->findByPath(new_model,path)) != NULL) {
		if ((src = strdup(path)) != NULL) {
		} else {
			PRINTF("ERROR: not enough memory for src!\n");
		}
	}

	switch(type)
	{
	case REFERENCE:
		if (container == NULL) {
			if ((container = (KMFContainer*)current_model->VT->findByPath(current_model, path)) != NULL) {
				if ((src = strdup(path)) != NULL) {
				} else {
					PRINTF("ERROR: not enough memory for src!\n");
				}
			} else {
				PRINTF("ERROR: Cannot retrieve source!\n");
				src = malloc(1);
				strcpy(src, "");
			}

			ModelTrace *mt = newPoly_ModelRemoveTrace(src, refname, path);

			if (mt != NULL)	{
				list_add(model_traces, mt);
			} else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", path, (char*)value);
			}
		}
		break;

	case STRING:
		if (container == NULL) {
			/*
			 * TODO check NULL
			 */
			if ((container = (KMFContainer*)current_model->VT->findByPath(current_model, path)) != NULL) {
				if ((src = strdup(container->eContainer)) != NULL) {
				} else {
					PRINTF("ERROR: not enough memory for src!\n");
				}
			} else {
				PRINTF("ERROR: Cannot retrieve source!\n");
				src = malloc(1);
				strcpy(src, "");
			}

			ModelTrace *mt = newPoly_ModelRemoveTrace(src, refname, path);

			if (mt != NULL)	{
				list_add(model_traces, mt);
			} else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", path, (char*)value);
			}
		} else {
			/*printf("path = %s  value = %s\n", _path, (char*)value);*/
			char* string = current_model->VT->findByPath(current_model, _path);
			char* string2 = new_model->VT->findByPath(new_model, _path);
			/*printf("Current attribute value: %s\n", string);
			printf("New attribute value: %s\n", string2);*/
			if(string != NULL && string2 != NULL)
			{
				if(!strcmp(string2, string))
				{
					/*printf("Identical attributes, nothing to change...\n\n");*/
				}
				else
				{
					/*printf("Changing attribute to %s in current_model\n\n", string2);*/
					ModelTrace *mt = newPoly_ModelSetTrace(src, refname, string2);
					/*char *strTrace = mt->ToString(mt->pDerivedObj);
						PRINTF(strTrace);
						free(strTrace);*/
					if(mt != NULL)
					{
						list_add(model_traces, mt);
					}
					else {
						printf("ERROR: ModelTrace cannot be added!\n");
						printf("path = %s  value = %s\n", path, (char*)value);
					}
				}
			}
			else if(string == NULL && string2 != NULL)
			{
				/*printf("Current attribute is NULL, changing to new attribute '%s'\n\n", string2);*/
				ModelTrace *mt = newPoly_ModelSetTrace(src, refname, string2);
				/*
					char *strTrace = mt->ToString(mt->pDerivedObj);
					PRINTF(strTrace);
					free(strTrace);
				 */
				if(mt != NULL)
				{
					list_add(model_traces, mt);
				}
				else {
					printf("ERROR: ModelTrace cannot be added!\n");
					printf("path = %s  value = %s\n", path, (char*)value);
				}
			}
			else if(string != NULL && string2 == NULL)
			{
				/*printf("Changing attribute to NULL\n\n");*/
				ModelTrace *mt = newPoly_ModelSetTrace(src, refname, "");
				/*char *strTrace = mt->ToString(mt->pDerivedObj);
					PRINTF(strTrace);
					free(strTrace);*/
				if(mt != NULL)
				{
					list_add(model_traces, mt);
				}
				else {
					printf("ERROR: ModelTrace cannot be added!\n");
					printf("path = %s  value = %s\n", path, (char*)value);
				}
			}
		}
		break;

	case BOOL:
		if(container == NULL) {
			/*
			 * TODO check NULL
			 */
			container = (KMFContainer*)current_model->VT->findByPath(current_model, path);
			src = strdup(container->eContainer);
			PRINTF("src: %s\n", src);
			ModelTrace *mt = newPoly_ModelRemoveTrace(src, refname, path);
			/*char *strTrace = mt->ToString(mt->pDerivedObj);
						PRINTF(strTrace);
						free(strTrace);*/
			if(mt != NULL) {
				list_add(model_traces, mt);
			} else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", path, (char*)value);
			}
		} else {
			bool v = (bool)(current_model->VT->findByPath(current_model, _path));
			bool v2 = (bool)(new_model->VT->findByPath(new_model, _path));
			char v2str[MAX_NUMBER] = {0};
			if(v == v2) {
				/*printf("Identical attributes, nothing to change...\n\n");*/
			} else {
				if (v2 == 1) {
					sprintf(v2str, "%s", "true");
				} else {
					sprintf(v2str, "%s", "false");
				}
				ModelTrace *mt = newPoly_ModelSetTrace(src, refname, v2str);
				if(mt != NULL) {
					list_add(model_traces, mt);
				} else {
					printf("ERROR: ModelTrace cannot be added!\n");
					printf("path = %s  value = %s\n", path, (char*)value);
				}
			}
		}
		break;
	case INTEGER:
		if(container == NULL) {
			/*
			 * TODO check NULL
			 */
			container = (KMFContainer*)current_model->VT->findByPath(current_model, path);
			src = strdup(container->eContainer);
			PRINTF("src: %s\n", src);
			ModelTrace *mt = newPoly_ModelRemoveTrace(src, refname, path);
			/*char *strTrace = mt->ToString(mt->pDerivedObj);
				PRINTF(strTrace);
				free(strTrace);*/
			if(mt != NULL) {
				list_add(model_traces, mt);
			} else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", path, (char*)value);
			}
		} else {
			int v = (int)(current_model->VT->findByPath(current_model, _path));
			int v2 = (int)(new_model->VT->findByPath(new_model, _path));
			char v2str[MAX_NUMBER] = {0};
			if(v == v2) {
				/*printf("Identical attributes, nothing to change...\n\n");*/
			} else {
				sprintf(v2str, "%d", v2);
				ModelTrace *mt = newPoly_ModelSetTrace(src, refname, v2str);
				if(mt != NULL) {
					list_add(model_traces, mt);
				} else {
					printf("ERROR: ModelTrace cannot be added!\n");
					printf("path = %s  value = %s\n", path, (char*)value);
				}
			}
		}
		break;

	case STRREF:
	case BRACKET:
	case SQBRACKET:
	case CLOSEBRACKET:
	case CLOSESQBRACKET:
	case CLOSEBRACKETCOLON:
	case CLOSESQBRACKETCOLON:
	case COLON:
	case RETURN:
		printf("Type non valid!\n");
		break;
	}

	free(__path);
	free(path2);
	if (src != NULL) {
		free(src);
	}
}

void actionAddSet(char *_path, Type type, void *value)
{
	char *__path = strdup(_path);
	char path[250];
	char *path2 = strdup(_path);
	char *refname = NULL;
	char *src = NULL;
	char *typename = NULL;
	KMFContainer *container;
	/*
	 * TODO return if memory is full
	 */
	if ((refname = strtok(path2, "\\")) != NULL) {
		if ((refname = strtok(NULL, "\\")) == NULL) {
			refname = strtok(path2, "\\");
		}
	}

	strcpy(path, strtok(__path, "\\"));

	if (!strcmp("generated_KMF_ID", path)) {
		strcpy(path, "");
	}

	switch(type)
	{
	case REFERENCE:
		container = current_model->VT->findByPath(current_model, path);
		if(container == NULL)
		{
			if ((container = (KMFContainer*)new_model->VT->findByPath(new_model, path)) != NULL) {
				if ((src = strdup(container->path)) != NULL) {
					typename = strdup(container->VT->metaClassName(container));
				} else {
					PRINTF("ERROR: not enough memory for src!\n");
				}
			} else {
				PRINTF("ERROR: Cannot retrieve source!\n");
			}
			/*printf("Path %s does not exist in curent_model, adding...\n\n", path);*/
			ModelTrace *mt = newPoly_ModelAddTrace((char*)value, refname, container->path, NULL);
			/*char *strTrace = mt->ToString(mt->pDerivedObj);
				PRINTF(strTrace);
				free(strTrace);*/
			if(mt != NULL)
			{
				list_add(model_traces, mt);
			}
			else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", path, (char*)value);
			}
		}
		else
		{
			/*printf("Path %s already exists...\n", path);*/
		}
		break;

	case STRING:
		/*printf("path = %s  value = %s\n", path, (char*)value);*/
		/*path = strtok(path, "\\");*/
		container = current_model->VT->findByPath(current_model, path);
		if(container == NULL)
		{
			if ((container = (KMFContainer*)new_model->VT->findByPath(new_model, path)) != NULL) {
				if ((src = strdup(container->path)) != NULL) {
					typename = strdup(container->VT->metaClassName(container));
				} else {
					PRINTF("ERROR: not enough memory for src!\n");
				}
			} else {
				PRINTF("ERROR: Cannot retrieve source!\n");
			}

			/*printf("Path %s does not exist in curent_model, adding...\n\n", path);*/
			ModelTrace *mt = newPoly_ModelSetTrace(src, refname, (char*)value);
			/*char *strTrace = mt->ToString(mt->pDerivedObj);
				PRINTF(strTrace);
				free(strTrace);*/
			if(mt != NULL)
			{
				list_add(model_traces, mt);
			}
			else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", path, (char*)value);
			}
		}
		else
		{
			/*printf("Path %s already exists...\n", path);*/
		}
		break;

	case BOOL:
		container = current_model->VT->findByPath(current_model, path);
		if (container == NULL) {
			if ((container = (KMFContainer*)new_model->VT->findByPath(new_model, path)) != NULL) {
				if ((src = strdup(container->path)) != NULL) {
					typename = strdup(container->VT->metaClassName(container));
				} else {
					PRINTF("ERROR: not enough memory for src!\n");
				}
			} else {
				PRINTF("ERROR: Cannot retrieve source!\n");
			}

			char v2str[MAX_NUMBER] = {0};
			if ((bool)value == 1) {
				sprintf(v2str, "%s", "true");
			} else {
				sprintf(v2str, "%s", "false");
			}

			ModelTrace *mt = newPoly_ModelSetTrace(src, refname, v2str);

			if(mt != NULL) {
				list_add(model_traces, mt);
			} else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", path, (char*)value);
			}
		} else {
			/*printf("Path %s already exists...\n", path);*/
		}
		break;

	case INTEGER:
		container = current_model->VT->findByPath(current_model, path);
		if (container == NULL) {
			if ((container = (KMFContainer*)new_model->VT->findByPath(new_model, path)) != NULL) {
				if ((src = strdup(container->path)) != NULL) {
					typename = strdup(container->VT->metaClassName(container));
				} else {
					PRINTF("ERROR: not enough memory for src!\n");
				}
			} else {
				PRINTF("ERROR: Cannot retrieve source!\n");
			}

			char v2str[MAX_NUMBER] = {0};
			/*int v2 = (int)(new_model->VT->findByPath(_path, new_model));*/
			sprintf(v2str, "%d", (int)value);

			ModelTrace *mt = newPoly_ModelSetTrace(src, refname, v2str);

			if(mt != NULL) {
				list_add(model_traces, mt);
			} else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", path, (char*)value);
			}
		} else {
			/*printf("Path %s already exists...\n", path);*/
		}
		break;

	case STRREF:
	case BRACKET:
	case SQBRACKET:
	case CLOSEBRACKET:
	case CLOSESQBRACKET:
	case CLOSEBRACKETCOLON:
	case CLOSESQBRACKETCOLON:
	case COLON:
	case RETURN:
		printf("Type non valid!\n");
		break;
	}

	free(__path);
	free(path2);
	if (src != NULL) {
		free(src);
	}
	if (typename != NULL) {
		free(typename);
	}
}
