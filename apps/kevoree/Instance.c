#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "NamedElement.h"
#include "TypeDefinition.h"
#include "Dictionary.h"
#include "FragmentDictionary.h"
#include "Visitor.h"
#include "Instance.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initInstance(Instance * const this)
{
	/*
	 * Initialize parent
	 */
	initNamedElement((NamedElement*)this);

	/*
	 * Initialize itself
	 */
	this->metaData = NULL;
	this->started = -1;
	this->typeDefinition = NULL;
	this->dictionary = NULL;
	this->fragmentDictionary = NULL;
}

FragmentDictionary
*Instance_findFragmentDictionaryByID(Instance * const this, char *id)
{
	FragmentDictionary* value = NULL;

	if(this->fragmentDictionary != NULL) {
		if(hashmap_get(this->fragmentDictionary, id, (void**)(&value)) == MAP_OK) {
			return value;
		} else {
			PRINTF("ERROR: FragmentDictionary %s not found!\n", id);
			return NULL;
		}
	} else {
		PRINTF("ERROR: There are no fragmentDictionaries in ContainerNode!\n");
		return NULL;
	}
}

void
Instance_addTypeDefinition(Instance * this, TypeDefinition *ptr)
{
	this->typeDefinition = ptr;
}

void
Instance_addDictionary(Instance * const this, Dictionary *ptr)
{
	if (this->dictionary != NULL) {
		this->VT->removeDictionary(this, ptr);
	}
	this->dictionary = ptr;
	ptr->eContainer = this;
}

void
Instance_addFragmentDictionary(Instance * const this, FragmentDictionary *ptr)
{
	FragmentDictionary* container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The FragmentDictionary cannot be added in Instance because the key is not defined\n");
	} else {
		if(this->fragmentDictionary == NULL) {
			this->fragmentDictionary = hashmap_new(get_key_for_hashmap);
		}
		if(hashmap_get(this->fragmentDictionary, internalKey, (void**)(&container)) == MAP_MISSING) {
			/*container = (FragmentDictionary*)ptr;*/
			if(hashmap_put(this->fragmentDictionary, internalKey, ptr) == MAP_OK) {
				ptr->eContainer = this;
			} else {
				PRINTF("ERROR: fragmentDictionary cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in fragmentDictionary map\n", internalKey);
		}
	}
}

void
Instance_removeTypeDefinition(Instance * const this, TypeDefinition *ptr)
{
	this->typeDefinition = NULL;
}

void
Instance_removeDictionary(Instance * const this, Dictionary *ptr)
{
	ptr->eContainer = NULL;
	this->dictionary = NULL;
}

void
Instance_removeFragmentDictionary(Instance * const this, FragmentDictionary *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The FragmentDictionary cannot be removed in Instance because the key is not defined\n");
	}
	else {
		if(hashmap_remove(this->fragmentDictionary, internalKey) == MAP_OK) {
			ptr->eContainer = NULL;
		} else {
			PRINTF("ERROR: component %s cannot be removed!\n", internalKey);
		}
	}
}

static char
*Instance_internalGetKey(Instance * const this)
{
	return namedElement_VT.internalGetKey((NamedElement*)this);
}

static char
*Instance_metaClassName(void* const this)
{
	return "Instance";
}

void
Instance_visit(Instance * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	/* NamedElement */
	/*namedElement_VT.visit((NamedElement*)this, parent, action, secondAction, visitPaths);*/
	namedElement_VT.visit((NamedElement*)this, parent, action, secondAction, visitPaths);

	/* Local attributes */
	if(visitPaths) {
		sprintf(path, "%s\\metaData", parent);
		action(path, STRING, this->metaData);

		sprintf(path,"%s\\started", parent);
		action(path, BOOL, (void*)this->started);
	} else {
		action("metaData", STRING, this->metaData);
		action(NULL, COLON, NULL);

		action("started", BOOL, (void*)this->started);
		action(NULL, COLON, NULL);
	}

	if(this->typeDefinition != NULL) {
		if (visitPaths) {
			char* tmp_path = this->typeDefinition->VT->getPath(this->typeDefinition);
			sprintf(path, "%s/%s\\typeDefinition", parent, tmp_path);
			free(tmp_path);
			action(path, REFERENCE, parent);
		} else {
			action("typeDefinition", SQBRACKET, NULL);
			sprintf(path, "typeDefinitions[%s]", this->typeDefinition->VT->internalGetKey(this->typeDefinition));
			action(path, STRREF, NULL);
			action(NULL, RETURN, NULL);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("typeDefinition", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}

	if(this->dictionary != NULL) {
		if (visitPaths) {
			sprintf(path, "%s/dictionary[%s]", parent, this->dictionary->VT->internalGetKey(this->dictionary));
			if (secondAction != NULL) {
				if (secondAction(path, "dictionary")) {
					this->dictionary->VT->visit(this->dictionary, path, action, secondAction, visitPaths);
				} else {
					this->dictionary->VT->visit(this->dictionary, path, action, secondAction, visitPaths);
				}
			}
		} else {
			action("dictionary", SQBRACKET, NULL);
			action(NULL, BRACKET, NULL);
			this->dictionary->VT->visit(this->dictionary, parent, action, secondAction, visitPaths);
			action(NULL, CLOSEBRACKET, NULL);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("dictionary", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}

	hashmap_map* m = NULL;

	int length;

	if((m = (hashmap_map*)this->fragmentDictionary) != NULL) {
		length = hashmap_length(this->fragmentDictionary);
		if (visitPaths) {
			sprintf(path,"%s/fragmentDictionary", parent);
			Visitor_visitPaths(m, "fragmentDictionary", path, action, secondAction);
		} else {
			action("fragmentDictionary", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKETCOLON, NULL);
		}
	} else if (!visitPaths) {
		action("fragmentDictionary", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKETCOLON, NULL);
	}
}

static void
*Instance_findByPath(Instance * const this, char *attribute)
{
	/* Local attributes */
	if(!strcmp("metaData",attribute)) {
		return this->metaData;
	} else if(!strcmp("started",attribute)) {
		return (void*)this->started;
	}
	/* Local references */
	else {
		char path[150];
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

		if(!strcmp("typeDefinitions", obj)) {
			free(obj);
			if(nextAttribute == NULL) {
				return this->typeDefinition;
			} else {
				if (this->typeDefinition != NULL) {
					return this->typeDefinition->VT->findByPath(this->typeDefinition, nextPath);
				} else {
					return NULL;
				}
			}
		} else if(!strcmp("dictionary", obj)) {
			free(obj);
			if(nextAttribute == NULL) {
				return this->dictionary;
			} else {
				if (this->dictionary != NULL) {
					return this->dictionary->VT->findByPath(this->dictionary, nextPath);
				} else {
					return NULL;
				}
			}
		} else if(!strcmp("fragmentDictionary", obj)) {
			free(obj);
			if(nextAttribute == NULL) {
				return this->VT->findFragmentDictionaryByID(this, key);
			} else {
				FragmentDictionary* value = this->VT->findFragmentDictionaryByID(this, key);
				if(value != NULL) {
					return value->VT->findByPath(value, nextPath);
				} else {
					PRINTF("ERROR: Cannot find %s!\n", key);
					return NULL;
				}
			}
		} else {
			free(obj);
			/* NamedElement attributes */
			return namedElement_VT.findByPath((NamedElement*)this, attribute);
		}
	}
}

static void
delete_Instance(Instance * const this)
{
	/* destroy base object */
	namedElement_VT.delete((NamedElement*)this);
	/* destroy data members */
	free(this->metaData);

	if (this->fragmentDictionary != NULL) {
		deleteContainerContents(this->fragmentDictionary);
		hashmap_free(this->fragmentDictionary);
	}

	if (this->dictionary != NULL) {
		delete((KMFContainer*)this->dictionary);
	}
}

const Instance_VT instance_VT = {
		/*
		 * KMFContainer_VT
		 * NamedElement_VT
		 */
		.super = &namedElement_VT,
		.metaClassName = Instance_metaClassName,
		.internalGetKey = Instance_internalGetKey,
		.getPath = KMFContainer_get_path,
		.visit = Instance_visit,
		.findByPath = Instance_findByPath,
		.delete = delete_Instance,
		/*
		 * Instance_VT
		 */
		.findFragmentDictionaryByID = Instance_findFragmentDictionaryByID,
		.addTypeDefinition = Instance_addTypeDefinition,
		.addDictionary = Instance_addDictionary,
		.addFragmentDictionary = Instance_addFragmentDictionary,
		.removeTypeDefinition = Instance_removeTypeDefinition,
		.removeDictionary = Instance_removeDictionary,
		.removeFragmentDictionary = Instance_removeFragmentDictionary
};
