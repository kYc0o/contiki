#ifndef H_NetworkInfo
#define H_NetworkInfo

#include "KMFContainer.h"
#include <stdbool.h>
#include "hashmap.h"
#include "NamedElement.h"

typedef struct _NetworkInfo NetworkInfo;
typedef struct _NetworkProperty NetworkProperty;

typedef void (*fptrNetInfoAddValues)(NetworkInfo*, NetworkProperty*);
typedef void (*fptrNetInfoRemoveValues)(NetworkInfo*, NetworkProperty*);
typedef NetworkProperty* (*fptrNetInfoFindValuesByID)(NetworkInfo*, char*);

typedef struct _NetworkInfo_VT {
	NamedElement_VT *super;
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
	 * NetworkInfo
	 */
	fptrNetInfoAddValues addValues;
	fptrNetInfoRemoveValues removeValues;
	fptrNetInfoFindValuesByID findValuesByID;
} NetworkInfo_VT;

typedef struct _NetworkInfo {
	NetworkInfo_VT *VT;
	/*
	 * KMFContainer
	 */
	KMFContainer *eContainer;
	/*
	 * NamedElement
	 */
	char *name;
	/*
	 * NetworkInfo
	 */
	map_t values;
} NetworkInfo;

NetworkInfo* new_NetworkInfo(void);
void initNetworkInfo(NetworkInfo * const this);

extern const NetworkInfo_VT networkInfo_VT;

#endif /* H_NetworkInfo */
