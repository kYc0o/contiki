#ifndef H_Group
#define H_Group

#include <stdbool.h>
#include "hashmap.h"
#include "Instance.h"

typedef struct _ContainerNode ContainerNode;
typedef struct _Group Group;

typedef ContainerNode* (*fptrGroupFindSubNodesByID)(Group*, char*);
typedef void (*fptrGroupAddSubNodes)(Group*, ContainerNode*);
typedef void (*fptrGroupRemoveSubNodes)(Group*, ContainerNode*);
typedef void* (*fptrFindByPathGroup)(char*, Group*);

typedef struct _Group_VT {
	Instance_VT *super;
	/*
	 * KMFContainer_VT
	 * NamedElement_VT
	 */
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
	/*
	 * Group_VT
	 */
	fptrGroupAddSubNodes addSubNodes;
	fptrGroupRemoveSubNodes removeSubNodes;
	fptrGroupFindSubNodesByID findSubNodesByID;
} Group_VT;

typedef struct _Group {
	Group *next;
	Group_VT *VT;
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
	/*
	 * Group
	 */
	map_t subNodes;
} Group;

Group* new_Group(void);
void initGroup(Group * const this);

extern const Group_VT group_VT;

#endif /* H_Group */
