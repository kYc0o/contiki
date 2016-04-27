#ifndef __Repository_H
#define __Repository_H

#include "KMFContainer.h"
#include "hashmap.h"

typedef struct _Repository Repository;

typedef struct _Repository_VT {
	KMFContainer_VT *super;
	/*
	 * KMFContainer
	 */
	fptrKMFMetaClassName metaClassName;
	fptrKMFInternalGetKey internalGetKey;
	fptrVisit visit;
	fptrFindByPath findByPath;
	fptrDelete delete;
	/*
	 * Repository
	 */
} Repository_VT;

typedef struct _Repository {
	Repository *next;
	Repository_VT *VT;
	/*
	 * KMFContainer
	 */
	char *eContainer;
	char *path;
	/*
	 * Repository
	 */
	char *url;
} Repository;

Repository* new_Repository(void);
void initRepository(Repository * const this);

extern const Repository_VT repository_VT;

#endif
