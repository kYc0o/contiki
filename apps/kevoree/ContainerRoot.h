#ifndef H_ContainerRoot
#define H_ContainerRoot

#include "KMFContainer.h"
#include "hashmap.h"

typedef struct _Group Group;
typedef struct _Visitor Visitor;
typedef struct _ContainerRoot ContainerRoot;
typedef struct _ContainerNode ContainerNode;
typedef struct _TypeDefinition TypeDefinition;
typedef struct _DeployUnit DeployUnit;
typedef struct _Group Group;
typedef struct _Channel Channel;
typedef struct _TypeLibrary TypeLibrary;
typedef struct _TypedElement TypedElement;
typedef struct _NodeNetwork NodeNetwork;
typedef struct _MBinding MBinding;
typedef struct _Repository Repository;

typedef ContainerNode* (*fptrContRootFindNodesByID)(ContainerRoot*, char*);
typedef TypeDefinition* (*fptrContRootFindTypeDefsByID)(ContainerRoot*, char*);
typedef Repository* (*fptrContRootFindRepositoriesByID)(ContainerRoot*, char*);
typedef TypedElement* (*fptrContRootFindDataTypesByID)(ContainerRoot*, char*);
typedef TypeLibrary* (*fptrContRootFindLibrariesByID)(ContainerRoot*, char*);
typedef Channel* (*fptrContRootFindHubsByID)(ContainerRoot*, char*);
typedef MBinding* (*fptrContRootFindBndingsByID)(ContainerRoot*, char*);
typedef DeployUnit* (*fptrContRootFindDeployUnitsByID)(ContainerRoot*, char*);
typedef NodeNetwork* (*fptrContRootFindNodeNetworksByID)(ContainerRoot*, char*);
typedef Group* (*fptrContRootFindGroupsByID)(ContainerRoot*, char*);
typedef void (*fptrContRootAddNodes)(ContainerRoot*, ContainerNode*);
typedef void (*fptrContRootAddTypeDefinitions)(ContainerRoot*, TypeDefinition*);
typedef void (*fptrContRootAddRepositories)(ContainerRoot*, Repository*);
typedef void (*fptrContRootAddDataTypes)(ContainerRoot*, TypedElement*);
typedef void (*fptrContRootAddLibraries)(ContainerRoot*, TypeLibrary*);
typedef void (*fptrContRootAddHubs)(ContainerRoot*, Channel*);
typedef void (*fptrContRootAddBindings)(ContainerRoot*, MBinding*);
typedef void (*fptrContRootAddDeployUnits)(ContainerRoot*, DeployUnit*);
typedef void (*fptrContRootAddNodeNetworks)(ContainerRoot*, NodeNetwork*);
typedef void (*fptrContRootAddGroups)(ContainerRoot*, Group*);
typedef void (*fptrContRootRemoveNodes)(ContainerRoot*, ContainerNode*);
typedef void (*fptrContRootRemoveTypeDefinitions)(ContainerRoot*, TypeDefinition*);
typedef void (*fptrContRootRemoveRepositories)(ContainerRoot*, Repository*);
typedef void (*fptrContRootRemoveDataTypes)(ContainerRoot*, TypedElement*);
typedef void (*fptrContRootRemoveLibraries)(ContainerRoot*, TypeLibrary*);
typedef void (*fptrContRootRemoveHubs)(ContainerRoot*, Channel*);
typedef void (*fptrContRootRemoveBindings)(ContainerRoot*, MBinding*);
typedef void (*fptrContRootRemoveDeployUnits)(ContainerRoot*, DeployUnit*);
typedef void (*fptrContRootRemoveNodeNetworks)(ContainerRoot*, NodeNetwork*);
typedef void (*fptrContRootRemoveGroups)(ContainerRoot*, Group*);

typedef struct _ContainerRoot_VT {
	/*
	 * KMFContainer
	 */
	KMFContainer_VT *super;
	fptrKMFMetaClassName metaClassName;
	fptrKMFInternalGetKey internalGetKey;
	fptrVisit visit;
	fptrFindByPath findByPath;
	fptrDelete delete;
	/*
	 * ContainerRoot
	 */
	fptrContRootFindNodesByID findNodesByID;
	fptrContRootFindTypeDefsByID findTypeDefsByID;
	fptrContRootFindRepositoriesByID findRepositoriesByID;
	fptrContRootFindDataTypesByID findDataTypesByID;
	fptrContRootFindLibrariesByID findLibrariesByID;
	fptrContRootFindHubsByID findHubsByID;
	fptrContRootFindBndingsByID findBindingsByID;
	fptrContRootFindDeployUnitsByID findDeployUnitsByID;
	fptrContRootFindNodeNetworksByID findNodeNetworksByID;
	fptrContRootFindGroupsByID findGroupsByID;
	fptrContRootAddNodes addNodes;
	fptrContRootAddTypeDefinitions addTypeDefinitions;
	fptrContRootAddRepositories addRepositories;
	fptrContRootAddDataTypes addDataTypes;
	fptrContRootAddLibraries addLibraries;
	fptrContRootAddHubs addHubs;
	fptrContRootAddBindings addBindings;
	fptrContRootAddDeployUnits addDeployUnits;
	fptrContRootAddNodeNetworks addNodeNetworks;
	fptrContRootAddGroups addGroups;
	fptrContRootRemoveNodes removeNodes;
	fptrContRootRemoveTypeDefinitions removeTypeDefinitions;
	fptrContRootRemoveRepositories removeRepositories;
	fptrContRootRemoveDataTypes removeDataTypes;
	fptrContRootRemoveLibraries removeLibraries;
	fptrContRootRemoveHubs removeHubs;
	fptrContRootRemoveBindings removeBindings;
	fptrContRootRemoveDeployUnits removeDeployUnits;
	fptrContRootRemoveNodeNetworks removeNodeNetworks;
	fptrContRootRemoveGroups removeGroups;
} ContainerRoot_VT;

typedef struct _ContainerRoot {
	ContainerRoot *next;
	ContainerRoot_VT *VT;
	/*
	 * KMFContainer
	 */
	char *eContainer;
	char *path;
	/*
	 * ContainerRoot
	 */
	char generated_KMF_ID[9];
	map_t nodes;
	map_t typeDefinitions;
	map_t repositories;
	map_t dataTypes;
	map_t libraries;
	map_t hubs;
	map_t mBindings;
	map_t deployUnits;
	map_t nodeNetworks;
	map_t groups;
} ContainerRoot;

ContainerRoot* new_ContainerRoot(void);
void initContainerRoot(ContainerRoot * const this);

extern const ContainerRoot_VT containerRoot_VT;

#endif
