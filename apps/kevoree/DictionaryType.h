#ifndef __DictionaryType_H
#define __DictionaryType_H

#include "hashmap.h"

typedef struct _KMFContainer_VT KMFContainer_VT;
typedef struct _DictionaryAttribute DictionaryAttribute;
typedef struct _DictionaryType DictionaryType;

typedef DictionaryAttribute* (*fptrDicTypeFindAttrByID)(DictionaryType*, char*);
typedef void (*fptrDicTypeAddAttr)(DictionaryType*, DictionaryAttribute*);
typedef void (*fptrDicTypeRemAttr)(DictionaryType*, DictionaryAttribute*);

typedef struct _DictionaryType_VT {
	KMFContainer_VT *super;
	/*
	 * KMFContainer
	 */
	fptrKMFMetaClassName metaClassName;
	fptrKMFInternalGetKey internalGetKey;
	fptrKMFGetPath getPath;
	fptrVisit visit;
	fptrFindByPath findByPath;
	fptrDelete delete;
	/*
	 * DictionaryType
	 */
	fptrDicTypeFindAttrByID findAttributesByID;
	fptrDicTypeAddAttr addAttributes;
	fptrDicTypeRemAttr removeAttributes;
} DictionaryType_VT;

typedef struct _DictionaryType {
	DictionaryType_VT *VT;
	/*
	 * KMFContainer
	 */
	KMFContainer *eContainer;
	/*
	 * DictionaryType
	 */
	char generated_KMF_ID[9];
	map_t attributes;
} DictionaryType;

DictionaryType *new_DictionaryType(void);
void initDictionaryType(DictionaryType * const this);

extern const DictionaryType_VT dictionaryType_VT;

#endif /* __DictionaryType_H */
