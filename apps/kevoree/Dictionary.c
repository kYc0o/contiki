#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "DictionaryValue.h"
#include "Dictionary.h"
#include "tools.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initDictionary(Dictionary * const this)
{
	/*
	 * Initialize parent
	 */
	initKMFContainer((KMFContainer*)this);
	/*
	 * Initialize itself
	 */
	memset(&this->generated_KMF_ID[0], 0, sizeof(this->generated_KMF_ID));
	rand_str(this->generated_KMF_ID, 8);
	this->values = NULL;
}

static void
delete_Dictionary(Dictionary * const this)
{
	/* delete base object */
	KMF_VT.delete((KMFContainer*)this);

	/* delete data members */
	if (this->values != NULL) {
		deleteContainerContents(this->values);
		hashmap_free(this->values);
	}
}

DictionaryValue
*Dictionary_findValuesByID(Dictionary * const this, char *id)
{
	DictionaryValue* value = NULL;

	if(this->values != NULL)
	{
		if(hashmap_get(this->values, id, (void**)(&value)) == MAP_OK)
			return value;
		else
			return NULL;
	}
	else
	{
		return NULL;
	}
}

void
Dictionary_addValues(Dictionary * const this, DictionaryValue *ptr)
{
	DictionaryValue* container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL)
	{
		PRINTF("The DictionaryValue cannot be added in Dictionary because the key is not defined\n");
	}
	else
	{
		if(this->values == NULL)
		{
			this->values = hashmap_new(get_key_for_hashmap);
		}
		if(hashmap_get(this->values, internalKey, (void**)(&container)) == MAP_MISSING)
		{
			/*container = (DictionaryValue*)ptr;*/
			if(hashmap_put(this->values, internalKey, ptr) == MAP_OK)
			{
				ptr->eContainer = this;
			}
		}
	}
}

void
Dictionary_removeValues(Dictionary * const this, DictionaryValue *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL)
	{
		PRINTF("ERROR: The DictionaryValue cannot be removed in Dictionary because the key is not defined\n");
	}
	else
	{
		if(hashmap_remove(this->values, internalKey) == MAP_OK)
		{
			ptr->eContainer = NULL;
		}
	}
}

static char
*Dictionary_internalGetKey(Dictionary * const this)
{
	return this->generated_KMF_ID;
}

static char
*Dictionary_metaClassName(Dictionary * const this)
{
	return "Dictionary";
}

static void
Dictionary_visit(Dictionary * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
{
	char path[256];
	memset(&path[0], 0, sizeof(path));

	if (visitPaths) {
		sprintf(path, "%s\\generated_KMF_ID", parent);
		action(path, STRING, this->generated_KMF_ID);
	} else {
		/*
		 * Visit parent
		 */
		KMF_VT.visit((KMFContainer*)this, parent, action, secondAction, visitPaths);
		action("generated_KMF_ID", STRING, this->generated_KMF_ID);
		action(NULL, COLON, NULL);
	}


	hashmap_map* m = NULL;

	int length;

	if((m = (hashmap_map*)this->values) != NULL) {
		length = hashmap_length(this->values);
		if (visitPaths) {
			sprintf(path,"%s/values", parent);
			Visitor_visitPaths(m, "values", path, action, secondAction);
		} else {
			action("values", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKET, NULL);
		}
	} else if (!visitPaths) {
		action("values", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKET, NULL);
	}

}

static void
*Dictionary_findByPath(Dictionary * const this, char *attribute)
{
	/* Local attributes */
	if(!strcmp("generated_KMF_ID", attribute))
	{
		return this->generated_KMF_ID;
	}
	/* Local references */
	else
	{
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

		if(strchr(path, '[') != NULL)
		{
			obj = strdup(strtok(path, "["));
			strcpy(path, attribute);
			PRINTF("Object: %s\n", obj);
			strcpy(token, strtok(path, "]"));
			strcpy(path, attribute);
			sprintf(token, "%s]", token);
			PRINTF("Token: %s\n", token);
			sscanf(token, "%*[^[][%[^]]", key);
			PRINTF("Key: %s\n", key);

			if((strchr(path, '\\')) != NULL)
			{
				nextAttribute = strtok(NULL, "\\");
				PRINTF("Attribute: %s\n", nextAttribute);

				if(strchr(nextAttribute, '['))
				{
					sprintf(nextPath, "%s\\%s", ++nextAttribute, strtok(NULL, "\\"));
					PRINTF("Next Path: %s\n", nextPath);
				}
				else
				{
					strcpy(nextPath, nextAttribute);
					PRINTF("Next Path: %s\n", nextPath);
				}
			}
			else
			{
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
		}
		else
		{
			if ((nextAttribute = strtok(path, "\\")) != NULL) {
				if ((nextAttribute = strtok(NULL, "\\")) != NULL) {
					PRINTF("Attribute: %s\n", nextAttribute);
				} else {
					nextAttribute = strtok(path, "\\");
					PRINTF("Attribute: %s\n", nextAttribute);
				}
			}
		}

		if(!strcmp("values", obj))
		{
			free(obj);
			if(nextAttribute == NULL)
			{

				return this->VT->findValuesByID(this, key);
			}
			else
			{
				DictionaryValue* value = this->VT->findValuesByID(this, key);
				if(value != NULL)
					return value->VT->findByPath(value, nextPath);
				else
					return NULL;
			}
		}
		else
		{
			free(obj);
			PRINTF("WARNING: Wrong attribute or reference %s\n", attribute);
			return NULL;
		}
	}
}

static char*
Dictionary_getPath(KMFContainer* kmf)
{
	Dictionary* obj = (Dictionary*)kmf;
	char* tmp = (obj->eContainer)?get_eContainer_path(obj):strdup("");
	char* r = (char*)malloc(strlen(tmp) + strlen("/dictionary[]") + strlen(obj->VT->internalGetKey(obj)) + 1);
	sprintf(r, "%s/dictionary[%s]", tmp, obj->VT->internalGetKey(obj));
	free(tmp);
	return r;
}

const Dictionary_VT dictionary_VT = {
		.super = &KMF_VT,
		/*
		 * KMFContainer
		 */
		.metaClassName = Dictionary_metaClassName,
		.internalGetKey = Dictionary_internalGetKey,
		.getPath = Dictionary_getPath,
		.visit = Dictionary_visit,
		.findByPath = Dictionary_findByPath,
		.delete = delete_Dictionary,
		/*
		 * Dictionary
		 */
		.addValues = Dictionary_addValues,
		.removeValues = Dictionary_removeValues,
		.findValuesByID = Dictionary_findValuesByID
};

Dictionary
*new_Dictionary()
{
	Dictionary* pObj = NULL;

	/* Allocating memory */
	pObj = malloc(sizeof(Dictionary));

	if (pObj == NULL) {
		PRINTF("ERROR: Cannot create Dictionary!\n");
		return NULL;
	}
	/*
	 * Virtual Table
	 */
	pObj->VT = &dictionary_VT;

	/*
	 * Dictionary
	 */
	initDictionary(pObj);

	return pObj;
}
