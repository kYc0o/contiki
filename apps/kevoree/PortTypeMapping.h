#ifndef __PortTypeMapping_H
#define __PortTypeMapping_H

#include "hashmap.h"
#include "KMFContainer.h"

typedef struct _PortTypeMapping PortTypeMapping;

typedef struct _PortTypeMapping_VT {
	KMFContainer_VT *super;
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
	 * PortTypeMapping
	 */
} PortTypeMapping_VT;

typedef struct _PortTypeMapping {
	PortTypeMapping *next;
	PortTypeMapping_VT *VT;
	/*
	 * KMFContainer
	 */
	char *eContainer;
	char *path;
	/*
	 * PortTypeMapping
	 */
	char generated_KMF_ID[9];
	char *beanMethodName;
	char *serviceMethodName;
	char *paramTypes;
} PortTypeMapping;

PortTypeMapping* new_PortTypeMapping(void);
void initPortTypeMapping(PortTypeMapping * const this);

extern const PortTypeMapping_VT portTypeMapping_VT;

#endif /*__PortTypeMapping_H */
