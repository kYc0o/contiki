#ifndef H_NetworkProperty
#define H_NetworkProperty

#include <stdbool.h>
#include "KMFContainer.h"
#include "NamedElement.h"

typedef struct _NetworkProperty NetworkProperty;

typedef struct _NetworkProperty_VT {
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
	 * NetworkProperty
	 */

} NetworkProperty_VT;

typedef struct _NetworkProperty {
	NetworkProperty_VT *VT;
	/*
	 * KMFContainer
	 */
	KMFContainer *eContainer;
	/*
	 * NamedElement
	 */
	char *name;
	/*
	 * NetworkProperty
	 */
	char *value;
} NetworkProperty;

NetworkProperty *new_NetworkProperty(void);
void initNetworkProperty(NetworkProperty * const this);

extern const NetworkProperty_VT networkProperty_VT;

#endif /* H_NetworkProperty */
