#ifndef __DictionaryAttribute_H
#define __DictionaryAttribute_H

#include <stdbool.h>

#include "hashmap.h"
#include "KMFContainer.h"
#include "NamedElement.h"
#include "TypedElement.h"

typedef struct _DictionaryAttribute DictionaryAttribute;

typedef struct _DictionaryAttribute_VT {
	TypedElement_VT *super;
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
	 * TypedElement
	 */
	fptrTypElemFindGenericTypesByID findGenericTypesByID;
	fptrTypElemAddGenericTypes addGenericTypes;
	fptrTypElemRemoveGenericTypes removeGenericTypes;
	/*
	 * DictionaryAttribute
	 */
} DictionaryAttribute_VT;

typedef struct _DictionaryAttribute {
	DictionaryAttribute *next;
	DictionaryAttribute_VT *VT;
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
	 * TypedElement
	 */
	map_t genericTypes;
	/*
	 * DictionaryAttribute
	 */
	bool optional;
	bool state;
	char *datatype;
	bool fragmentDependant;
	char *defaultValue;
} DictionaryAttribute;

DictionaryAttribute* new_DictionaryAttribute(void);
void initDictionaryAttribute(DictionaryAttribute * const this);

extern const DictionaryAttribute_VT dictionaryAttribute_VT;

#endif /* __DictionaryAttribute_H */
