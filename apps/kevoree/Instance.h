#ifndef H_Instance
#define H_Instance

#include "KMFContainer.h"
#include <stdbool.h>

#include "hashmap.h"
#include "NamedElement.h"

typedef struct _Instance Instance;
typedef struct _TypeDefinition TypeDefinition;
typedef struct _Dictionary Dictionary;
typedef struct _FragmentDictionary FragmentDictionary;

typedef FragmentDictionary* (*fptrInstFindFragDictByID)(void*, char*);
typedef void (*fptrInstAddTypeDefinition)(void*, TypeDefinition*);
typedef void (*fptrInstAddDictionary)(void*, Dictionary*);
typedef void (*fptrInstAddFragmentDictionary)(void*, FragmentDictionary*);
typedef void (*fptrInstRemoveTypeDefinition)(void*, TypeDefinition*);
typedef void (*fptrInstRemoveDictionary)(void*, Dictionary*);
typedef void (*fptrInstRemoveFragmentDictionary)(void*, FragmentDictionary*);

typedef struct _Instance_VT {
	/*
	 * KMFContainer_VT
	 * NamedElement_VT
	 */
	NamedElement_VT *super;
	fptrKMFMetaClassName metaClassName;
	fptrKMFInternalGetKey internalGetKey;
	fptrVisit visit;
	fptrFindByPath findByPath;
	fptrDelete delete;
	/*
	 * Instance_VT
	 */
	fptrInstFindFragDictByID findFragmentDictionaryByID;
	fptrInstAddTypeDefinition addTypeDefinition;
	fptrInstAddDictionary addDictionary;
	fptrInstAddFragmentDictionary addFragmentDictionary;
	fptrInstRemoveTypeDefinition removeTypeDefinition;
	fptrInstRemoveDictionary removeDictionary;
	fptrInstRemoveFragmentDictionary removeFragmentDictionary;
} Instance_VT;

typedef struct _Instance {
	Instance *next;
	Instance_VT *VT;
	/*
	 * KMFContainer
	 */
	char *eContainer;
	char *path;
	/*
	 * NamedElement
	 */
	char *name;
	/*
	 * Instance
	 */
	char *metaData;
	bool started;
	TypeDefinition *typeDefinition;
	Dictionary *dictionary;
	map_t fragmentDictionary;
} Instance;

Instance* new_Instance(void);
void initInstance(Instance * const this);

FragmentDictionary *Instance_findFragmentDictionaryByID(Instance * const this, char *id);
void Instance_addTypeDefinition(Instance * this, TypeDefinition *ptr);
void Instance_addDictionary(Instance * const this, Dictionary *ptr);
void Instance_addFragmentDictionary(Instance * const this, FragmentDictionary *ptr);
void Instance_removeTypeDefinition(Instance * const this, TypeDefinition *ptr);
void Instance_removeDictionary(Instance * const this, Dictionary *ptr);
void Instance_removeFragmentDictionary(Instance * const this, FragmentDictionary *ptr);



extern const Instance_VT instance_VT;

#endif /* H_Instance */
