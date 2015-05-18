#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "NamedElement.h"
#include "DeployUnit.h"
#include "DictionaryType.h"
#include "Visitor.h"
#include "TypeDefinition.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initTypeDefinition(TypeDefinition * const this)
{
	initNamedElement((NamedElement*)this);
	this->version = NULL;
	this->factoryBean = NULL;
	this->bean = NULL;
	this->abstract = -1;
	this->internalKey = NULL;
	this->deployUnits = NULL;
	this->dictionaryType = NULL;
	this->superTypes = NULL;
}

char
*TypeDefinition_internalGetKey(TypeDefinition * const this)
{
	char* internalKey = NULL;

	if (this->internalKey == NULL) {
		internalKey = malloc(sizeof(char) * (strlen(this->name) + strlen("/") + strlen(this->version)) + 1);

		if (internalKey == NULL)
			return NULL;

		sprintf(internalKey, "%s/%s", this->name, this->version);

		this->internalKey = internalKey;
		return internalKey;
	} else {
		return this->internalKey;
	}
}

static char
*TypeDefinition_metaClassName(TypeDefinition * const this)
{
	return "TypeDefinition";
}

void
TypeDefinition_addDeployUnit(TypeDefinition * const this, DeployUnit *ptr)
{
	this->deployUnits = ptr;
}

void
TypeDefinition_addDictionaryType(TypeDefinition * const this, DictionaryType *ptr)
{
	if (this->dictionaryType != NULL) {
		this->VT->removeDictionaryType(this, ptr);
	}
	this->dictionaryType = ptr;
	ptr->eContainer = strdup(this->path);
	ptr->path = malloc(sizeof(char) * (strlen(this->path) + strlen("/dictionaryType[]") + strlen(ptr->VT->internalGetKey(ptr))) + 1);
	sprintf(ptr->path, "%s/dictionaryType[%s]", this->path, ptr->VT->internalGetKey(ptr));
}

void
TypeDefinition_removeDeployUnit(TypeDefinition * const this, DeployUnit *ptr)
{
	this->deployUnits = NULL;
}

void
TypeDefinition_removeDictionaryType(TypeDefinition * const this, DictionaryType *ptr)
{
	this->dictionaryType = NULL;
	free(ptr->eContainer);
	ptr->eContainer = NULL;
	free(ptr->path);
	ptr->path = NULL;
}

void
TypeDefinition_addSuperTypes(TypeDefinition * const this, TypeDefinition *ptr)
{
	TypeDefinition* container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The TypeDefinition cannot be added in TypeDefinition because the key is not defined\n");
	} else {
		if(this->superTypes == NULL) {
			this->superTypes = hashmap_new();
		}
		if(hashmap_get(this->superTypes, internalKey, (void**)(&container)) == MAP_MISSING) {
			if ((hashmap_put(this->superTypes, internalKey, ptr)) == MAP_OK) {

			} else {
				PRINTF("ERROR: superType can't be added!\n");
			}
		}
	}
}

void
TypeDefinition_removeSuperTypes(TypeDefinition * const this, TypeDefinition *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The TypeDefinition cannot be removed in TypeDefinition because the key is not defined\n");
	} else {
		if ((hashmap_remove(this->superTypes, internalKey)) == MAP_OK) {

		} else {
			PRINTF("ERROR: superType %s cannot be removed!\n", internalKey);
		}
	}
}

void delete_TypeDefinition(TypeDefinition * const this)
{
	/* destroy base object */
	namedElement_VT.delete((NamedElement*)this);

	/* destroy data members */
	if (this->version != NULL) {
		free(this->version);
	}

	if (this->factoryBean != NULL) {
		free(this->factoryBean);
	}

	if (this->bean != NULL) {
		free(this->bean);
	}

	if (this->internalKey != NULL) {
		free(this->internalKey);
	}

	if (this->superTypes != NULL) {
		deleteContainerContents(this->superTypes);
		hashmap_free(this->superTypes);
	}

	if (this->dictionaryType != NULL) {
		delete((KMFContainer*)this->dictionaryType);
	}
}

void TypeDefinition_visit(TypeDefinition * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	/*namedElement_VT.visit((NamedElement*)this, parent, action, secondAction, visitPaths);*/
	namedElement_VT.visit((NamedElement*)this, parent, action, secondAction, visitPaths);

	if (visitPaths) {
		sprintf(path, "%s\\version", parent);
		action(path, STRING, this->version);
		sprintf(path,"%s\\factoryBean",parent);
		action(path, STRING, this->factoryBean);
		sprintf(path,"%s\\bean",parent);
		action(path, STRING, this->bean);
		sprintf(path, "%s\\abstract", parent);
		action(path, BOOL, (void*)this->abstract);
	} else {
		sprintf(path, "version");
		action(path, STRING, this->version);
		action(NULL, COLON, NULL);
		sprintf(path, "factoryBean");
		action(path, STRING, this->factoryBean);
		action(NULL, COLON, NULL);
		sprintf(path, "bean");
		action(path, STRING, this->bean);
		action(NULL, COLON, NULL);
		sprintf(path, "abstract");
		action(path, BOOL, (void*)this->abstract);
		action(NULL, COLON, NULL);
	}

	if(this->deployUnits != NULL) {
		if (visitPaths) {
			sprintf(path, "%s/%s\\typeDefinition", parent, this->deployUnits->path);
			action(path, REFERENCE, parent);
		} else {
			action("deployUnit", SQBRACKET, NULL);
			sprintf(path, "deployUnits[%s]", this->deployUnits->VT->internalGetKey(this->deployUnits));
			action(path, STRREF, NULL);
			action(NULL, RETURN, NULL);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("deployUnit", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}

	if(this->dictionaryType != NULL) {
		if (visitPaths) {
			sprintf(path, "%s/dictionaryType[%s]", parent, this->dictionaryType->VT->internalGetKey(this->dictionaryType));
			if (secondAction != NULL) {
				if (secondAction(path, "dictionaryType")) {
					this->dictionaryType->VT->visit(this->dictionaryType, path, action, secondAction, visitPaths);
				} else {
					this->dictionaryType->VT->visit(this->dictionaryType, path, action, secondAction, visitPaths);
				}
			}
		} else {
			action("dictionaryType", SQBRACKET, NULL);
			action(NULL, BRACKET, NULL);
			this->dictionaryType->VT->visit(this->dictionaryType, parent, action, secondAction, visitPaths);
			action(NULL, CLOSEBRACKET, NULL);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("dictionaryType", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}

	hashmap_map* m = NULL;

	int length;

	if((m = (hashmap_map*)this->superTypes) != NULL) {
		length = hashmap_length(this->superTypes);
		if (visitPaths) {
			Visitor_visitPathRefs(m, "superTypes", path, action, secondAction, parent);
		} else {
			action("superTypes", SQBRACKET, NULL);
			Visitor_visitModelRefs(m, length, "superTypes", path, action);
			action(NULL, CLOSESQBRACKET, NULL);
		}
	} else if (!visitPaths) {
		action("superTypes", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKET, NULL);
	}
}

void
*TypeDefinition_findByPath(TypeDefinition * const this, char *attribute)
{
	/* NamedElement attributes */
	/* Local attributes */
	if(!strcmp("version",attribute)) {
		return this->version;
	} else if(!strcmp("factoryBean",attribute)) {
		return this->factoryBean;
	} else if(!strcmp("bean",attribute)) {
		return this->bean;
	} else if(!strcmp("abstract",attribute)) {
		return (void*)this->abstract;
	} else {
		char path[250];
		memset(&path[0], 0, sizeof(path));
		char token[100];
		memset(&token[0], 0, sizeof(token));
		char *obj = NULL;
		char key[50];
		memset(&key[0], 0, sizeof(key));
		char nextPath[150];
		memset(&nextPath[0], 0, sizeof(nextPath));
		char *nextAttribute = NULL;

		strcpy(path, attribute);

		if(strchr(path, '[') != NULL) {
			obj = strdup(strtok(path, "["));
			strcpy(path, attribute);
			PRINTF("Object: %s\n", obj);
			strcpy(token, strtok(path, "]"));
			strcpy(path, attribute);
			sprintf(token, "%s]", token);
			PRINTF("Token: %s\n", token);
			sscanf(token, "%*[^[][%[^]]", key);
			PRINTF("Key: %s\n", key);

			if((strchr(path, '\\')) != NULL) {
				nextAttribute = strtok(NULL, "\\");
				PRINTF("Attribute: %s\n", nextAttribute);

				if(strchr(nextAttribute, '[')) {
					sprintf(nextPath, "%s\\%s", ++nextAttribute, strtok(NULL, "\\"));
					PRINTF("Next Path: %s\n", nextPath);
				} else {
					strcpy(nextPath, nextAttribute);
					PRINTF("Next Path: %s\n", nextPath);
				}
			} else {
				nextAttribute = strtok(path, "]");
				bool isFirst = true;
				char *fragPath = NULL;
				while ((fragPath = strtok(NULL, "]")) != NULL) {
					PRINTF("Attribute: %s]\n", fragPath);
					if (isFirst) {
						sprintf(nextPath, "%s]", ++fragPath);
						isFirst = false;
					} else {
						sprintf(nextPath, "%s/%s]", nextPath, ++fragPath);
					}
					PRINTF("Next Path: %s\n", nextPath);
				}
				if (strlen(nextPath) == 0) {
					PRINTF("Attribute: NULL\n");
					PRINTF("Next Path: NULL\n");
					nextAttribute = NULL;
				}
			}
		} else {
			obj = strdup(attribute);
			if ((nextAttribute = strtok(path, "\\")) != NULL) {
				if ((nextAttribute = strtok(NULL, "\\")) != NULL) {
					PRINTF("Attribute: %s\n", nextAttribute);
				} else {
					nextAttribute = strtok(path, "\\");
					PRINTF("Attribute: %s\n", nextAttribute);
				}
			}
		}

		if(!strcmp("deployUnits", obj)) {
			free(obj);
			if(nextAttribute == NULL) {
				return this->deployUnits;
			} else {
				return this->deployUnits->VT->findByPath(this->deployUnits, nextPath);
			}
		} else if(!strcmp("dictionaryType", obj)) {
			free(obj);
			if(nextAttribute == NULL) {
				return this->dictionaryType;
			} else {
				DictionaryType *dictype = this->dictionaryType;
				return dictype->VT->findByPath(dictype, nextPath);
			}
		} else {
			free(obj);
			return namedElement_VT.findByPath((NamedElement*)this, attribute);
		}
	}
}

const TypeDefinition_VT typeDefinition_VT = {
		.super = &namedElement_VT,
		/*
		 * KMFContainer
		 * NamedElement
		 */
		.metaClassName = TypeDefinition_metaClassName,
		.internalGetKey = TypeDefinition_internalGetKey,
		.visit = TypeDefinition_visit,
		.findByPath = TypeDefinition_findByPath,
		.delete = delete_TypeDefinition,
		/*
		 * TypeDefinition
		 */
		.addDeployUnit = TypeDefinition_addDeployUnit,
		.addDictionaryType = TypeDefinition_addDictionaryType,
		.addSuperTypes = TypeDefinition_addSuperTypes,
		.removeDeployUnit = TypeDefinition_removeDeployUnit,
		.removeDictionaryType = TypeDefinition_removeDictionaryType,
		.removeSuperTypes = TypeDefinition_removeSuperTypes
};
