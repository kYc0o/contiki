#ifndef __TypedElement_H
#define __TypedElement_H

#include "hashmap.h"
#include "KMFContainer.h"
#include "NamedElement.h"

typedef struct _TypedElement TypedElement;

typedef TypedElement* (*fptrTypElemFindGenericTypesByID)(TypedElement*, char*);
typedef void (*fptrTypElemAddGenericTypes)(TypedElement*, TypedElement*);
typedef void (*fptrTypElemRemoveGenericTypes)(TypedElement*, TypedElement*);

typedef struct _TypedElement_VT {
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
	fptrTypElemFindGenericTypesByID findGenericTypesByID;
	fptrTypElemAddGenericTypes addGenericTypes;
	fptrTypElemRemoveGenericTypes removeGenericTypes;
} TypedElement_VT;

typedef struct _TypedElement {
	TypedElement_VT *VT;
	/*
	 * KMFContainer
	 */
	KMFContainer *eContainer;
	/*
	 * NamedElement
	 */
	char *name;
	/*
	 * TypedElement
	 */
	map_t genericTypes;
} TypedElement;

TypedElement* new_TypedElement(void);
void initTypedElement(TypedElement * const this);

TypedElement *TypedElement_findGenericTypesByID(TypedElement * const this, char *id);
void TypedElement_addGenericTypes(TypedElement * const this, TypedElement *ptr);
void TypedElement_removeGenericTypes(TypedElement * const this, TypedElement *ptr);

char* TypedElement_getPath(KMFContainer* kmf);

extern const TypedElement_VT typedElement_VT;

#endif /* __TypedElement_H */
