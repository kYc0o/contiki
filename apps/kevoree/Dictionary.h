#ifndef __Dictionary_H
#define __Dictionary_H

#include "hashmap.h"
#include "KMFContainer.h"

typedef struct _DictionaryValue DictionaryValue;
typedef struct _Dictionary Dictionary;

typedef void (*fptrDicoAddValues)(Dictionary*, DictionaryValue*);
typedef void (*fptrDicoRemoveValues)(Dictionary*, DictionaryValue*);
typedef DictionaryValue* (*fptrDicoFindValuesByID)(Dictionary*, char*);

typedef struct _Dictionary_VT {
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
	 * Dictionary
	 */
	fptrDicoFindValuesByID findValuesByID;
	fptrDicoAddValues addValues;
	fptrDicoRemoveValues removeValues;
} Dictionary_VT;

typedef struct _Dictionary {
	Dictionary_VT *VT;
	/*
	 * KMFContainer
	 */
	KMFContainer *eContainer;
	/*
	 * Dictionary
	 * TODO fix id size
	 */
	char generated_KMF_ID[33];
	map_t values;
} Dictionary;

Dictionary *new_Dictionary(void);
void initDictionary(Dictionary * const this);

DictionaryValue *Dictionary_findValuesByID(Dictionary * const this, char *id);
void Dictionary_addValues(Dictionary * const this, DictionaryValue *ptr);
void Dictionary_removeValues(Dictionary * const this, DictionaryValue *ptr);

extern const Dictionary_VT dictionary_VT;

#endif /* __Dictionary_H */
