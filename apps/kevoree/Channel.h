#ifndef __Channel_H
#define __Channel_H

#include "KMFContainer.h"
#include <stdbool.h>
#include "hashmap.h"
#include "Instance.h"

typedef struct _MBinding MBinding;
typedef struct _Channel Channel;

typedef void (*fptrChannelAddBindings)(Channel*, MBinding*);
typedef void (*fptrChannelRemoveBindings)(Channel*, MBinding*);
typedef MBinding* (*fptrChannelFindBindingsByID)(Channel*, char*);

typedef struct _Channel_VT {
	Instance_VT *super;
	/*
	 * KMFContainer_VT
	 * NamedElement_VT
	 */
	fptrKMFMetaClassName metaClassName;
	fptrKMFInternalGetKey internalGetKey;
	fptrVisit visit;
	fptrFindByPath findByPath;
	fptrDelete delete;
	/*
	 * Instance_VT
	 */
	fptrInstFindFragDictByID findFragmentDictionaryByID;
	fptrInstAddTypeDefinition addTypeDefinition;
	fptrInstAddDictionary addDictionary;
	fptrInstAddFragmentDictionary addFragmentDictionary;
	fptrInstRemoveTypeDefinition removeTypeDefinition;
	fptrInstRemoveDictionary removeDictionary;
	fptrInstRemoveFragmentDictionary removeFragmentDictionary;
	/*
	 * Channel_VT
	 */
	fptrChannelFindBindingsByID findBindingsByID;
	fptrChannelAddBindings addBindings;
	fptrChannelRemoveBindings removeBindings;
} Channel_VT;

typedef struct _Channel {
	Channel *next;
	Channel_VT *VT;
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
	 * Instance
	 */
	char *metaData;
	bool started;
	TypeDefinition *typeDefinition;
	Dictionary *dictionary;
	map_t fragmentDictionary;
	/*
	 * Channel
	 */
	map_t bindings;
} Channel;

Channel* new_Channel(void);
void initChannel(Channel * const this);

extern const Channel_VT channel_VT;

#endif /*__Channel_H */
