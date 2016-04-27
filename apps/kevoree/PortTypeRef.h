#ifndef __PortTypeRef_H
#define __PortTypeRef_H

#include <stdbool.h>

#include "hashmap.h"
#include "NamedElement.h"

typedef struct _KMFContainer_VT KMFContainer_VT;
typedef struct _PortTypeRef PortTypeRef;
typedef struct _PortType PortType;
typedef struct _PortTypeMapping PortTypeMapping;

typedef PortTypeMapping* (*fptrPortTypeRefFindMappingsByID)(PortTypeRef*, char*);
typedef void (*fptrPortTypeRefAddRef)(PortTypeRef*, PortType*);
typedef void (*fptrPortTypeRefAddMappings)(PortTypeRef*, PortTypeMapping*);
typedef void (*fptrPortTypeRefRemoveRef)(PortTypeRef*, PortType*);
typedef void (*fptrPortTypeRefRemoveMappings)(PortTypeRef*, PortTypeMapping*);

typedef struct _PortTypeRef_VT {
	NamedElement_VT *super;
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
	 * PortTypeRef
	 */
	fptrPortTypeRefFindMappingsByID findMappingsByID;
	fptrPortTypeRefAddRef addRef;
	fptrPortTypeRefAddMappings addMappings;
	fptrPortTypeRefRemoveRef removeRef;
	fptrPortTypeRefRemoveMappings removeMappings;
} PortTypeRef_VT;

typedef struct _PortTypeRef {
	PortTypeRef *next;
	PortTypeRef_VT *VT;
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
	 * PortTypeRef
	 */
	bool optional;
	bool noDependency;
	PortType *ref;
	map_t mappings;
} PortTypeRef;

PortTypeRef* new_PortTypeRef(void);
void initPortTypeRef(PortTypeRef * const this);
extern const PortTypeRef_VT portTypeRef_VT;

#endif /* __PortTypeRef_H */
