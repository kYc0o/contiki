#ifndef __Port_H
#define __Port_H

#include <stdbool.h>

#include "hashmap.h"
#include "NamedElement.h"

typedef struct _KMFContainer_VT KMFContainer_VT;
typedef struct _MBinding MBinding;
typedef struct _PortTypeRef PortTypeRef;
typedef struct _Port Port;
typedef struct _MBinding MBinding;

typedef MBinding* (*fptrPortFindBindingsByID)(Port*, char*);
typedef void (*fptrPortAddBindings)(Port*, MBinding*);
typedef void (*fptrPortAddPortTypeRef)(Port*, PortTypeRef*);
typedef void (*fptrPortRemoveBindings)(Port*, MBinding*);
typedef void (*fptrPortRemovePortTypeRef)(Port*, PortTypeRef*);

typedef struct _Port_VT {
	NamedElement_VT *super;
	/*
	 * KMFContainer_VT
	 * NamedElement_VT
	 */
	fptrKMFMetaClassName metaClassName;
	fptrKMFInternalGetKey internalGetKey;
	fptrKMFGetPath getPath;
	fptrVisit visit;
	fptrFindByPath findByPath;
	fptrDelete delete;
	/*
	 * Port
	 */
	fptrPortFindBindingsByID findBindingsByID;
	fptrPortAddBindings addBindings;
	fptrPortAddPortTypeRef addPortTypeRef;
	fptrPortRemoveBindings removeBindings;
	fptrPortRemovePortTypeRef removePortTypeRef;
} Port_VT;

typedef struct _Port {
	Port_VT *VT;
	/*
	 * KMFContainer
	 */
	KMFContainer *eContainer;
	/*
	 * NamedElement
	 */
	char *name;
	/*
	 * Port
	 */
	map_t bindings;
	PortTypeRef *portTypeRef;
} Port;

Port* new_Port(void);
void initPort(Port * const this);

extern const Port_VT port_VT;

#endif /* __Port_H */
