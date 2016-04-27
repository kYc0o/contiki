#ifndef __DictionaryValue_H
#define __DictionaryValue_H

#include "KMFContainer.h"

typedef struct _DictionaryValue DictionaryValue;

typedef struct _DictionaryValue_VT {
	KMFContainer_VT *super;
	/*
	 * KMFContainer_VT
	 */
	fptrKMFMetaClassName metaClassName;
	fptrKMFInternalGetKey internalGetKey;
	fptrVisit visit;
	fptrFindByPath findByPath;
	fptrDelete delete;

} DictionaryValue_VT;

typedef struct _DictionaryValue {
	DictionaryValue *next;
	DictionaryValue_VT *VT;
	/*
	 * KMFContainer
	 */
	char *eContainer;
	char *path;
	/*
	 * DictionaryValue
	 */
	char *name;
	char *value;
} DictionaryValue;

DictionaryValue* new_DictionaryValue(void);
void initDictionaryValue(DictionaryValue * const this);

extern const DictionaryValue_VT dictionaryValue_VT;

#endif /* __DictionaryValue_H */
