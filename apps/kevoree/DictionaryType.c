#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "DictionaryAttribute.h"
#include "TypeDefinition.h"
#include "Visitor.h"
#include "DictionaryType.h"
#include "tools.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void initDictionaryType(DictionaryType * const this)
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
	this->attributes = NULL;
}

static void
delete_DictionaryType(DictionaryType * const this)
{
	/* destroy base object */
	KMF_VT.delete((KMFContainer*)this);

	/* destroy data members */
	if (this->attributes != NULL) {
		deleteContainerContents(this->attributes);
		hashmap_free(this->attributes);
	}
}

static char
*DictionaryType_internalGetKey(DictionaryType * const this)
{
	return this->generated_KMF_ID;
}

static char
*DictionaryType_metaClassName(DictionaryType * const this)
{
	return "DictionaryType";
}

static DictionaryAttribute
*DictionaryType_findAttributesByID(DictionaryType * const this, char *id)
{
	DictionaryAttribute* value = NULL;

	if(this->attributes != NULL)
	{
		if(hashmap_get(this->attributes, id, (void**)(&value)) == MAP_OK)
			return value;
		else
			return NULL;
	}
	else
	{
		return NULL;
	}
}

static void
DictionaryType_addAttributes(DictionaryType * const this, DictionaryAttribute *ptr)
{
	DictionaryAttribute *container = NULL;

	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The DictionaryAttribute cannot be added in DictionaryType because the key is not defined\n");
	} else {
		if(this->attributes == NULL) {
			this->attributes = hashmap_new(get_key_for_hashmap);
		}
		if(hashmap_get(this->attributes, internalKey, (void**)(&container)) == MAP_MISSING) {
			if(hashmap_put(this->attributes, internalKey, ptr) == MAP_OK) {
				ptr->eContainer = this;
			} else {
				PRINTF("ERROR: attribute cannot be added!\n");
			}
		} else {
			PRINTF("WARNING: id %s already exists in attributes map\n", internalKey);
		}
	}
}

static void
DictionaryType_removeAttributes(DictionaryType * const this, DictionaryAttribute *ptr)
{
	char *internalKey = ptr->VT->internalGetKey(ptr);

	if(internalKey == NULL) {
		PRINTF("ERROR: The DictionaryValue cannot be removed in Dictionary because the key is not defined\n");
	} else {
		if(hashmap_remove(this->attributes, internalKey) == MAP_OK) {
			ptr->eContainer = NULL;
		} else {
			PRINTF("ERROR: attribute %s cannot be removed!\n", internalKey);
		}
	}
}

static void
DictionaryType_visit(DictionaryType * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths)
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

	if((m = (hashmap_map*)this->attributes) != NULL) {
		length = hashmap_length(this->attributes);
		if (visitPaths) {
			sprintf(path,"%s/attributes", parent);
			Visitor_visitPaths(m, "attributes", path, action, secondAction);
		} else {
			action("attributes", SQBRACKET, NULL);
			Visitor_visitModelContainer(m, length, action);
			action(NULL, CLOSESQBRACKET, NULL);
		}
	} else if (!visitPaths) {
		action("attributes", SQBRACKET, NULL);
		action(NULL, CLOSESQBRACKET, NULL);
	}

}

static void
*DictionaryType_findByPath(DictionaryType * const this, char *attribute)
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

		if(!strcmp("attributes", obj))
		{
			free(obj);
			if(nextAttribute == NULL)
			{
				DictionaryAttribute *dicattr =  this->VT->findAttributesByID(this, key);
				PRINTF("DEBUG: returning %s object\n", dicattr->metaClassName(dicattr));
				return dicattr;
			}
			else
			{
				DictionaryAttribute* value = this->VT->findAttributesByID(this, key);
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
DictionaryType_getPath(KMFContainer* kmf)
{
	DictionaryType* obj = (DictionaryType*)kmf;
	char* tmp = (obj->eContainer)?get_eContainer_path(obj):strdup("");
	char* r = (char*)malloc(strlen(tmp) + strlen("/dictionaryType[]") + strlen(obj->VT->internalGetKey(obj)) + 1);
	sprintf(r, "%s/dictionaryType[%s]", tmp, obj->VT->internalGetKey(obj));
	free(tmp);
	return r;
}

const DictionaryType_VT dictionaryType_VT = {
		.super = &KMF_VT,
		/*
		 * KMFContainer
		 * NamedElement
		 */
		.metaClassName = DictionaryType_metaClassName,
		.internalGetKey = DictionaryType_internalGetKey,
		.getPath = DictionaryType_getPath,
		.visit = DictionaryType_visit,
		.findByPath = DictionaryType_findByPath,
		.delete = delete_DictionaryType,
		/*
		 * DictionaryType
		 */
		.addAttributes = DictionaryType_addAttributes,
		.removeAttributes = DictionaryType_removeAttributes,
		.findAttributesByID = DictionaryType_findAttributesByID
};

DictionaryType
*new_DictionaryType()
{
	DictionaryType* pObj = NULL;

	/* Allocating memory */
	pObj = malloc(sizeof(DictionaryType));

	if (pObj == NULL) {
		PRINTF("ERROR: Cannot create DictionaryType!\n");
		return NULL;
	}

	/*
	 * Virtual Table
	 */
	pObj->VT = &dictionaryType_VT;

	/*
	 * DictionaryType
	 */
	initDictionaryType(pObj);

	return pObj;
}
