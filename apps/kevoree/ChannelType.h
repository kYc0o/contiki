#ifndef __ChannelType_H
#define __ChannelType_H

#include "KMFContainer.h"
#include <stdbool.h>

#include "hashmap.h"

typedef struct _ChannelType ChannelType;
typedef struct _TypeDefinition TypeDefinition;
typedef struct _DeployUnit DeployUnit;
typedef struct _DictionaryType DictionaryType;

typedef struct _ChannelType_VT {
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
} ChannelType_VT;

typedef struct _ChannelType {
	ChannelType *next;
	ChannelType_VT *VT;
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
	 * ChannelType
	 */
	int lowerBindings;
	int upperBindings;
	int lowerFragments;
	int upperFragments;
} ChannelType;

ChannelType* new_ChannelType(void);
void initChannelType(ChannelType * const this);

extern const ChannelType_VT channelType_VT;

#endif /* __ChannelType_H */
