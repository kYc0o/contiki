#ifndef __KMF4C_H
#define __KMF4C_H

#include <stdbool.h>

#include "Visitor.h"
#include "hashmap.h"

typedef struct _KMFContainer_VT KMFContainer_VT;
typedef struct _KMFContainer KMFContainer;

typedef char* (*fptrKMFMetaClassName)(void*);
typedef char* (*fptrKMFInternalGetKey)(void*);
typedef void (*fptrVisit)(void*, char*, fptrVisitAction, fptrVisitActionRef, bool);
typedef void* (*fptrFindByPath)(void*, char*);
typedef void (*fptrDelete)(void*);

typedef struct _KMFContainer_VT {
	void *super;
	/*
	 * KMFContainer_VT
	 */
	fptrKMFMetaClassName metaClassName;
	fptrKMFInternalGetKey internalGetKey;
	fptrVisit visit;
	fptrFindByPath findByPath;
	fptrDelete delete;
} KMFContainer_VT;

typedef struct _KMFContainer {
	KMFContainer *next;
	KMFContainer_VT *VT;
	/*
	 * KMFContainer
	 */
	char *eContainer;
	char *path;
} KMFContainer;

/*static void delete_KMFContainer(KMFContainer * const this);*/
void delete(KMFContainer *object);
void deleteContainerContents(map_t container);
void initKMFContainer(KMFContainer * const this);

extern const KMFContainer_VT KMF_VT;

#endif
