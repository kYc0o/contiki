#ifndef H_DeployUnit
#define H_DeployUnit

#include "hashmap.h"
#include "KMFContainer.h"
#include "NamedElement.h"

typedef struct _DeployUnit DeployUnit;

typedef void (*fptrDepUnitAddRequiredLibs)(DeployUnit*, DeployUnit*);
typedef void (*fptrDepUnitRemoveRequiredLibs)(DeployUnit*, DeployUnit*);
typedef DeployUnit* (*fptrDepUnitFindRequiredLibsByID)(DeployUnit*, char*);

typedef struct _DeployUnit_VT {
	NamedElement_VT *super;
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
	 * DeployUnit
	 */
	fptrDepUnitAddRequiredLibs addRequiredLibs;
	fptrDepUnitRemoveRequiredLibs removeRequiredLibs;
	fptrDepUnitFindRequiredLibsByID findRequiredLibsByID;
} DeployUnit_VT;

typedef struct _DeployUnit {
	DeployUnit *next;
	DeployUnit_VT *VT;
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
	 * DeployUnit
	 */
	char *groupName;
	char *version;
	char *url;
	char *hashcode;
	char *type;
	char *internalKey;
	map_t requiredLibs;
} DeployUnit;

DeployUnit* new_DeployUnit(void);
void initDeployUnit(DeployUnit * const this);

extern const DeployUnit_VT deployUnit_VT;

#endif /* H_DeployUnit */
