#ifndef __TypeLibrary_H
#define __TypeLibrary_H

#include "hashmap.h"

typedef struct _TypeLibrary TypeLibrary;
typedef struct _TypeDefinition TypeDefinition;
typedef struct _KMFContainer_VT KMFContainer_VT;

typedef TypeDefinition* (*fptrTypeLibFindSubTypesByID)(TypeLibrary*, char*);
typedef void (*fptrTypeLibAddSubTypes)(TypeLibrary*, TypeDefinition*);
typedef void (*fptrTypeLibRemoveSubTypes)(TypeLibrary*, TypeDefinition*);

typedef struct _TypeLibrary_VT {
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
	 * TypedElement
	 */
	fptrTypeLibFindSubTypesByID findSubTypesByID;
	fptrTypeLibAddSubTypes addSubTypes;
	fptrTypeLibRemoveSubTypes removeSubTypes;
} TypeLibrary_VT;

typedef struct _TypeLibrary {
	TypeLibrary_VT *VT;
	/*
	 * KMFContainer
	 */
	KMFContainer *eContainer;
	/*
	 * NamedElement
	 */
	char *name;
	/*
	 * TypeLibrary
	 */
	map_t subTypes;
} TypeLibrary;

TypeLibrary* new_TypeLibrary(void);
void initTypeLibrary(TypeLibrary * const this);

extern const TypeLibrary_VT typeLibrary_VT;

#endif /* __TypeLibrary_H */
