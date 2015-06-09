#ifndef __PortType_H
#define __PortType_H

#include <stdbool.h>

#include "hashmap.h"
#include "KMFContainer.h"
#include "NamedElement.h"
#include "TypeDefinition.h"

typedef struct _PortType PortType;

typedef struct _PortType_VT {
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
} PortType_VT;

typedef struct _PortType {
	PortType_VT *VT;
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
	TypeDefinition_VT *typDefVT;
	/*
	 * PortType
	 */
	bool synchrone;
} PortType;

PortType* new_PortType(void);
void initPortType(PortType * const this);

extern const PortType_VT portType_VT;

#endif /*__PortType_H */
