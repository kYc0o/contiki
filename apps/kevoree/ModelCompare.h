#ifndef ModelCompare_H
#define ModelCompare_H

/**
 * Author: fco.ja.ac@gmail.com
 * Date: 21/12/14
 * Time: 12:20
 */

#include <stdbool.h>
#include "KMF4C.h"

/*
#include <microframework/api/trace/TraceSequence.h>
#include <microframework/api/KMFContainer.h>
#include <list>
*/

typedef struct _ModelCompare ModelCompare;
typedef struct _ContainerRoot ContainerRoot;
typedef struct _TraceSequence TraceSequence;
typedef void ** list_t;

typedef TraceSequence *(*fptrCreateSequence)(ModelCompare*);
typedef TraceSequence *(*fptrDiff)(ModelCompare*, ContainerRoot*, ContainerRoot*);
typedef TraceSequence *(*fptrMerge)(ModelCompare*, ContainerRoot*, ContainerRoot*);
typedef TraceSequence *(*fptrInter)(ModelCompare*, ContainerRoot*, ContainerRoot*);
typedef list_t (*fptrInternalDiff)(ModelCompare*, ContainerRoot*, ContainerRoot*, bool, bool);

typedef struct _ModelCompare {
	fptrCreateSequence createSequence;
	fptrDiff diff;
	fptrMerge merge;
	fptrInter inter;
	TraceSequence seq;
	fptrInternalDiff internal_diff;
	fptrDelete delete;
} ModelCompare;

ModelCompare *new_ModelCompare(void);
void delete_ModelCompare(void *const this);
TraceSequence *TraceSequence_createSequence(ModelCompare *const this);
TraceSequence *TraceSequence_diff(ModelCompare *const this, ContainerRoot *origin, ContainerRoot *target);
TraceSequence *TraceSequence_merge(ModelCompare *const this, ContainerRoot *origin, ContainerRoot *target);
TraceSequence *TraceSequence_inter(ModelCompare *const this, ContainerRoot *origin, ContainerRoot *target);
list_t Trace_sequence_internal_diff(ModelCompare *const this, ContainerRoot *origin, ContainerRoot *target, bool inter, bool merge);

/*
class ModelCompare
{

public:
	~ModelCompare(){ }
	TraceSequence* createSequence ();
	TraceSequence* diff (KMFContainer *origin, KMFContainer *target);
	TraceSequence* merge (KMFContainer *origin, KMFContainer *target);
	TraceSequence* inter (KMFContainer *origin, KMFContainer *target);

private:
	TraceSequence seq;
	std::list < ModelTrace * > *internal_diff (KMFContainer *origin,KMFContainer *target,bool inter, bool merge);
};
*/
// END CLASS

#endif
