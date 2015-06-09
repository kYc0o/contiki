#ifndef __NodeLink_H
#define __NodeLink_H

#include "hashmap.h"
#include "KMFContainer.h"

typedef struct _NodeLink NodeLink;
typedef struct _NetworkProperty NetworkProperty;

typedef char* (*fptrNodeLinkMetaClassName)(NodeLink*);
typedef char* (*fptrNodeLinkInternalGetKey)(NodeLink*);
typedef void (*fptrNodeLinkAddNetworkProperties)(NodeLink*, NetworkProperty*);
typedef void (*fptrNodeLinkRemoveNetworkProperties)(NodeLink*, NetworkProperty*);
typedef NetworkProperty* (*fptrNodeLinkFindNetworkPropertiesByID)(NodeLink*, char*);

typedef struct _NodeLink_VT {
	KMFContainer_VT *super;
	/*
	 * KMFContainer_VT
	 */
	fptrKMFMetaClassName metaClassName;
	fptrKMFInternalGetKey internalGetKey;
	fptrKMFGetPath getPath;
	fptrVisit visit;
	fptrFindByPath findByPath;
	fptrDelete delete;
	/*
	 * NodeLink
	 */
	fptrNodeLinkFindNetworkPropertiesByID findNetworkPropertiesByID;
	fptrNodeLinkAddNetworkProperties addNetworkProperties;
	fptrNodeLinkRemoveNetworkProperties removeNetworkProperties;
} NodeLink_VT;

typedef struct _NodeLink {
	NodeLink_VT *VT;
	/*
	 * KMFContainer
	 */
	KMFContainer *eContainer;
	/*
	 * NodeLink
	 */
	char generated_KMF_ID[9];
	char *networkType;
	int estimatedRate;
	char *lastCheck;
	char *zoneID;
	map_t networkProperties;
} NodeLink;

NodeLink* new_NodeLink(void);
void initNodeLink(NodeLink * const this);

extern const NodeLink_VT nodeLink_VT;

#endif /* __NodeLink_H */
