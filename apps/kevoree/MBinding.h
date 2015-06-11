#ifndef __MBinding_H
#define __MBinding_H

#include "KMFContainer.h"
#include <stdbool.h>

typedef struct _Channel Channel;
typedef struct _Port Port;
typedef struct _MBinding MBinding;
typedef struct _KMFContainer_VT KMFContainer_VT;

typedef void (*fptrMBindingAddPort)(MBinding*, Port*);
typedef void (*fptrMbindingAddHub)(MBinding*, Channel*);
typedef void (*fptrMBindingRemovePort)(MBinding*, Port*);
typedef void (*fptrMbindingRemoveHub)(MBinding*, Channel*);

typedef struct _MBinding_VT {
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
	 * MBinding
	 */
	fptrMBindingAddPort addPort;
	fptrMbindingAddHub addHub;
	fptrMBindingRemovePort removePort;
	fptrMbindingRemoveHub removeHub;
} MBinding_VT;

typedef struct _MBinding {
	MBinding_VT *VT;
	/*
	 * KMFContainer
	 */
	KMFContainer *eContainer;
	/*
	 * MBinding
	 */
	char generated_KMF_ID[9];
	Port* port;
	Channel* channel;
} MBinding;

MBinding* new_MBinding(void);
void initMBinding(MBinding * const this);

extern const MBinding_VT mBinding_VT;

#endif /* __MBinding_H */
