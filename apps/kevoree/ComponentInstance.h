#ifndef H_ComponentInstance
#define H_ComponentInstance

#include "hashmap.h"
#include "Instance.h"

typedef struct _ComponentInstance ComponentInstance;
typedef struct _Port Port;

typedef Port* (*fptrCompInstFindProvidedByID)(ComponentInstance*, char*);
typedef Port* (*fptrCompInstFindRequiredByID)(ComponentInstance*, char*);
typedef void (*fptrCompInstAddProvided)(ComponentInstance*, Port*);
typedef void (*fptrCompInstAddRequired)(ComponentInstance*, Port*);
typedef void (*fptrCompInstRemoveProvided)(ComponentInstance*, Port*);
typedef void (*fptrCompInstRemoveRequired)(ComponentInstance*, Port*);

typedef struct _ComponentInstance_VT {
	Instance_VT *super;
	/*
	 * KMFContainer_VT
	 * NamedElement_VT
	 */
	fptrKMFMetaClassName metaClassName;
	fptrKMFInternalGetKey internalGetKey;
	fptrKMFGetPath getPath;
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
	/*
	 * ComponentInstance_VT
	 */
	fptrCompInstFindProvidedByID findProvidedByID;
	fptrCompInstFindRequiredByID findRequiredByID;
	fptrCompInstAddProvided addProvided;
	fptrCompInstAddRequired addRequired;
	fptrCompInstRemoveProvided removeProvided;
	fptrCompInstRemoveRequired removeRequired;
} ComponentInstance_VT;

typedef struct _ComponentInstance {
	ComponentInstance_VT *VT;
	/*
	 * KMFContainer
	 */
	KMFContainer *eContainer;
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
	/*
	 * ComponentInstace
	 */
	map_t provided;
	map_t required;
} ComponentInstance;

ComponentInstance* new_ComponentInstance(void);
void initComponentInstance(ComponentInstance * const this);

extern const ComponentInstance_VT componentInstance_VT;

#endif /* H_ComponentInstance */
