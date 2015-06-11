#ifndef H_TypeDefinition
#define H_TypeDefinition

#include <stdbool.h>

#include "hashmap.h"
#include "NamedElement.h"

typedef struct _KMFContainer_VT KMFContainer_VT;
typedef struct _TypeDefinition TypeDefinition;
typedef struct _DeployUnit DeployUnit;
typedef struct _DictionaryType DictionaryType;

typedef void (*fptrTypeDefAddDeployUnit)(void*, DeployUnit*);
typedef void (*fptrTypeDefAddDictionaryType)(void*, DictionaryType*);
typedef void (*fptrTypeDefAddSuperTypes)(void*, TypeDefinition*);
typedef void (*fptrTypeDefRemoveDeployUnit)(void*, DeployUnit*);
typedef void (*fptrTypeDefRemoveDictionaryType)(void*, DictionaryType*);
typedef void (*fptrTypeDefRemoveSuperTypes)(void*, TypeDefinition*);

typedef struct _TypeDefinition_VT {
	/*
	 * KMFContainer
	 * NamedElement
	 */
	NamedElement_VT *super;
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
} TypeDefinition_VT;

typedef struct _TypeDefinition {
	TypeDefinition_VT *VT;
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
} TypeDefinition;

TypeDefinition *new_TypeDefinition(void);
void initTypeDefinition(TypeDefinition * const this);

char *TypeDefinition_internalGetKey(TypeDefinition * const this);
void *TypeDefinition_findByPath(TypeDefinition * const this, char *attribute);
void TypeDefinition_addDeployUnit(TypeDefinition * const this, DeployUnit *ptr);
void TypeDefinition_addDictionaryType(TypeDefinition * const this, DictionaryType *ptr);
void TypeDefinition_addSuperTypes(TypeDefinition * const this, TypeDefinition *ptr);
void TypeDefinition_removeDeployUnit(TypeDefinition * const this, DeployUnit *ptr);
void TypeDefinition_removeDictionaryType(TypeDefinition * const this, DictionaryType *ptr);
void TypeDefinition_removeSuperTypes(TypeDefinition * const this, TypeDefinition *ptr);
void TypeDefinition_visit(TypeDefinition * const this, char *parent, fptrVisitAction action, fptrVisitActionRef secondAction, bool visitPaths);

char* TypeDefinition_getPath(KMFContainer*);

extern const TypeDefinition_VT typeDefinition_VT;

#endif /* H_TypeDefinition */
