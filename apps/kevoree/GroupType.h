#ifndef __GroupType_H
#define __GroupType_H

#include "KMFContainer.h"
#include <stdbool.h>
#include "hashmap.h"
#include "NamedElement.h"
#include "TypeDefinition.h"

typedef struct _GroupType GroupType;

typedef struct _GroupType_VT {
	TypeDefinition_VT *super;
	/*
	 * KMFContainer
	 * NamedElement
	 */
	fptrKMFMetaClassName metaClassName;
	fptrKMFInternalGetKey internalGetKey;
	fptrKMFGetPath getPath;
	fptrVisit visit;
	fptrFindByPath findByPath;
	fptrDelete delete;
	/*
	 * TypeDefinition
	 */
	fptrTypeDefAddDeployUnit addDeployUnit;
	fptrTypeDefAddDictionaryType addDictionaryType;
	fptrTypeDefAddSuperTypes addSuperTypes;
	fptrTypeDefRemoveDeployUnit removeDeployUnit;
	fptrTypeDefRemoveDictionaryType removeDictionaryType;
	fptrTypeDefRemoveSuperTypes removeSuperTypes;
	/*
	 * GroupType
	 */
} GroupType_VT;

typedef struct _GroupType {
	GroupType_VT *VT;
	/*
	 * KMFContainer
	 */
	KMFContainer *eContainer;
	/*
	 * NamedElement
	 */
	char *name;
	/*
	 * TypeDefinition
	 */
	char *version;
	char *factoryBean;
	char *bean;
	bool abstract;
	char *internalKey;
	DeployUnit *deployUnits;
	DictionaryType *dictionaryType;
	map_t superTypes;
	/*
	 * GroupType
	 */
} GroupType;

GroupType* new_GroupType(void);
void initGroupType(GroupType * const this);

extern const GroupType_VT groupType_VT;

#endif /* __GroupType_H */
