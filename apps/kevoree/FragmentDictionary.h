#ifndef __FragmentDictionary_H
#define __FragmentDictionary_H

#include "hashmap.h"
#include "KMFContainer.h"
#include "Dictionary.h"

typedef struct _FragmentDictionary FragmentDictionary;

typedef struct _FragmentDictionary_VT {
	Dictionary_VT *super;
	/*
	 * KMFContainer
	 */
	fptrKMFMetaClassName metaClassName;
	fptrKMFInternalGetKey internalGetKey;
	fptrVisit visit;
	fptrFindByPath findByPath;
	fptrDelete delete;
	/*
	 * Dictionary
	 */
	fptrDicoFindValuesByID findValuesByID;
	fptrDicoAddValues addValues;
	fptrDicoRemoveValues removeValues;
	/*
	 * FragmentDictionary
	 */
} FragmentDictionary_VT;

typedef struct _FragmentDictionary {
	FragmentDictionary *next;
	FragmentDictionary_VT *VT;
	/*
	 * KMFContainer
	 */
	char *eContainer;
	char *path;
	/*
	 * Dictionary
	 * TODO fix size
	 */
	char generated_KMF_ID[33];
	map_t values;
	/*
	 * FragmentDictionary
	 */
	char *name;
} FragmentDictionary;

FragmentDictionary* new_FragmentDictionary(void);
void initFragmentDictionary(FragmentDictionary * const this);

extern const FragmentDictionary_VT fragmentDictionary_VT;

#endif /* __FragmentDictionary_H */
