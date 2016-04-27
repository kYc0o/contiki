#ifndef __ComponentType_H
#define __ComponentType_H

#include "KMFContainer.h"
#include <stdbool.h>
#include "hashmap.h"
#include "NamedElement.h"
#include "TypeDefinition.h"

typedef struct _ComponentType ComponentType;
typedef struct _PortTypeRef PortTypeRef;
typedef struct _DeployUnit DeployUnit;
typedef struct _DictionaryType DictionaryType;

typedef PortTypeRef* (*fptrCompTypeFindRequiredByID)(ComponentType*, char*);
typedef PortTypeRef* (*fptrCompTypeFindProvidedByID)(ComponentType*, char*);
typedef void (*fptrCompTypeAddRequired)(ComponentType*, PortTypeRef*);
typedef void (*fptrCompTypeAddProvided)(ComponentType*, PortTypeRef*);
typedef void (*fptrCompTypeRemoveRequired)(ComponentType*, PortTypeRef*);
typedef void (*fptrCompTypeRemoveProvided)(ComponentType*, PortTypeRef*);

typedef struct _ComponentType_VT {
	TypeDefinition_VT *super;
	/*
	 * KMFContainer
	 * NamedElement
	 */
	fptrKMFMetaClassName metaClassName;
	fptrKMFInternalGetKey internalGetKey;
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
	 * ComponentType
	 */
	fptrCompTypeFindRequiredByID findRequiredByID;
	fptrCompTypeFindProvidedByID findProvidedByID;
	fptrCompTypeAddRequired addRequired;
	fptrCompTypeAddProvided addProvided;
	fptrCompTypeRemoveRequired removeRequired;
	fptrCompTypeRemoveProvided removeProvided;
} ComponentType_VT;

typedef struct _ComponentType {
	ComponentType *next;
	ComponentType_VT *VT;
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
	 * ComponentType
	 */
	map_t required;
	map_t provided;
} ComponentType;

ComponentType* new_ComponentType(void);
void initComponentType(ComponentType * const this);

extern const ComponentType_VT componentType_VT;
 
#endif /* __ComponentType_H */
