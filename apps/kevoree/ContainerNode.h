#ifndef H_ContainerNode
#define H_ContainerNode

#include <stdbool.h>
#include "hashmap.h"
#include "KMFContainer.h"
#include "NamedElement.h"
#include "Instance.h"

typedef struct _ContainerNode ContainerNode;
typedef struct _ComponentInstance ComponentInstance;
typedef struct _Group Group;
typedef struct _NetworkInfo NetworkInfo;

typedef ComponentInstance* (*fptrContNodeFindComponentsByID)(ContainerNode*, char*);
typedef ContainerNode* (*fptrContNodeFindHostsByID)(ContainerNode*, char*);
typedef Group* (*fptrContNodeFindGroupsByID)(ContainerNode*, char*);
typedef NetworkInfo* (*fptrContNodeFindNetworkInformationByID)(ContainerNode*, char*);
typedef void (*fptrContNodeAddComponents)(ContainerNode*, ComponentInstance*);
typedef void (*fptrContNodeAddHosts)(ContainerNode*, ContainerNode*);
typedef void (*fptrContNodeAddHost)(ContainerNode*, ContainerNode*);
typedef void (*fptrContNodeAddGroups)(ContainerNode*, Group*);
typedef void (*fptrContNodeAddNetworkInformation)(ContainerNode*, NetworkInfo*);
typedef void (*fptrContNodeRemoveComponents)(ContainerNode*, ComponentInstance*);
typedef void (*fptrContNodeRemoveHosts)(ContainerNode*, ContainerNode*);
typedef void (*fptrContNodeRemoveHost)(ContainerNode*, ContainerNode*);
typedef void (*fptrContNodeRemoveGroups)(ContainerNode*, Group*);
typedef void (*fptrContNodeRemoveNetworkInformation)(ContainerNode*, NetworkInfo*);

typedef struct _ContainerNode_VT {
	/*
	 * KMFContainer_VT
	 * NamedElement_VT
	 */
	Instance_VT *super;
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
	 * ContainerNode_VT
	 */
	fptrContNodeFindComponentsByID findComponentsByID;
	fptrContNodeFindHostsByID findHostsByID;
	fptrContNodeFindGroupsByID findGroupsByID;
	fptrContNodeFindNetworkInformationByID findNetworkInformationByID;
	fptrContNodeAddComponents addComponents;
	fptrContNodeAddHosts addHosts;
	fptrContNodeAddHost addHost;
	fptrContNodeAddGroups addGroups;
	fptrContNodeAddNetworkInformation addNetworkInformation;
	fptrContNodeRemoveComponents removeComponents;
	fptrContNodeRemoveHost removeHost;
	fptrContNodeRemoveHosts removeHosts;
	fptrContNodeRemoveGroups removeGroups;
	fptrContNodeRemoveNetworkInformation removeNetworkInformation;
} ContainerNode_VT;

typedef struct _ContainerNode {
	ContainerNode_VT *VT;
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
	 * ContainerNode
	 */
	map_t components;
	map_t hosts;
	ContainerNode *host;
	map_t networkInformation;
	map_t groups;
} ContainerNode;

ContainerNode* new_ContainerNode(void);
void initContainerNode(ContainerNode * const this);

extern const ContainerNode_VT containerNode_VT;

#endif /* H_ContainerNode */
