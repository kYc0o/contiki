#ifndef H_NodeNetwork
#define H_NodeNetwork

#include "KMFContainer.h"
#include "hashmap.h"

typedef struct _NodeNetwork NodeNetwork;
typedef struct _ContainerNode ContainerNode;
typedef struct _NodeLink NodeLink;

typedef NodeLink* (*fptrNodeNetFindLinkByID)(NodeNetwork*, char*);
typedef void (*fptrNodeNetAddLink)(NodeNetwork*, NodeLink*);
typedef void (*fptrNodeNetAddInitBy)(NodeNetwork*, ContainerNode*);
typedef void (*fptrNodeNetAddTarget)(NodeNetwork*, ContainerNode*);
typedef void (*fptrNodeNetRemoveLink)(NodeNetwork*, NodeLink*);
typedef void (*fptrNodeNetRemoveInitBy)(NodeNetwork*, ContainerNode*);
typedef void (*fptrNodeNetRemoveTarget)(NodeNetwork*, ContainerNode*);

typedef struct _NodeNetwork_VT {
	KMFContainer_VT *super;
	/*
	 * KMFContainer_VT
	 */
	fptrKMFMetaClassName metaClassName;
	fptrKMFInternalGetKey internalGetKey;
	fptrVisit visit;
	fptrFindByPath findByPath;
	fptrDelete delete;
	/*
	 * NodeNetwork
	 */
	fptrNodeNetFindLinkByID findLinkByID;
	fptrNodeNetAddLink addLink;
	fptrNodeNetAddInitBy addInitBy;
	fptrNodeNetAddTarget addTarget;
	fptrNodeNetRemoveLink removeLink;
	fptrNodeNetRemoveInitBy removeInitBy;
	fptrNodeNetRemoveTarget removeTarget;
} NodeNetwork_VT;

typedef struct _NodeNetwork {
	NodeNetwork *next;
	NodeNetwork_VT *VT;
	/*
	 * KMFContainer
	 */
	char *eContainer;
	char *path;
	/*
	 * NodeNetwork
	 */
	char generated_KMF_ID[9];
	map_t link;
	ContainerNode *initBy;
	ContainerNode *target;
} NodeNetwork;

NodeNetwork* new_NodeNetwork(void);
void initNodeNetwork(NodeNetwork * const this);

extern const NodeNetwork_VT nodeNetwork_VT;

#endif /* H_NodeNetwork */
