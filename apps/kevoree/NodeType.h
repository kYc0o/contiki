#ifndef H_NodeType
#define H_NodeType

#include "TypeDefinition.h"

typedef struct _NodeType NodeType;

typedef struct _NodeType_VT {
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
	 * NodeType
	 */
} NodeType_VT;

typedef struct _NodeType {
	TypeDefinition *next;
	NodeType_VT *VT;
	/*
	 * KMFContainer
	 */
	char *eContainer;
	char *path;
	/*
	 * TypeDefinition
	 */
	char *name;
	char *version;
	char *factoryBean;
	char *bean;
	bool abstract;
	char *internalKey;
	DeployUnit *deployUnits;
	DictionaryType *dictionaryType;
	map_t superTypes;
	/*
	 * NodeType
	 */
} NodeType;

NodeType* new_NodeType(void);
void initNodeType(NodeType * const this);

extern const NodeType_VT nodeType_VT;

#endif /* H_NodeType */
