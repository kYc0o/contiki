/*
#include <microframework/api/compare/ModelCompare.h>
#include <microframework/api/utils/ModelVisitor.h>
#include <microframework/api/utils/Utils.h>
#include <microframework/api/compare/ModelCompareVisitors.h>
*/

#include "ModelCompare.h"
#include "TraceSequence.h"
#include "ContainerRoot.h"
#include "lib/list.h"
#include "hashmap.h"

/**
 * Author: fco.ja.ac@gmail.com
 * Date: 21/12/14
 * Time: 12:40
 */

ModelCompare *new_ModelCompare()
{
	ModelCompare *pModCompObj = malloc(sizeof(ModelCompare));

	if(pModCompObj == NULL)
	{
		return NULL;
	}

	pModCompObj->createSequence = TraceSequence_createSequence;
	pModCompObj->diff = TraceSequence_diff;
	pModCompObj->merge = TraceSequence_merge;
	pModCompObj->inter = TraceSequence_inter;
	pModCompObj->internal_diff = Trace_sequence_internal_diff;

	return pModCompObj;
}

void delete_ModelCompare(void *const this)
{
	if(this != NULL)
	{
		free(this);
	}
}

TraceSequence *TraceSequence_createSequence(ModelCompare *const this)
{
	TraceSequence *ts = new_TraceSequence();
	return ts;
}

TraceSequence *TraceSequence_diff(ModelCompare *const this, ContainerRoot *origin, ContainerRoot *target)
{
	TraceSequence *ts = new_TraceSequence();

	ts->populate(this->internal_diff(origin, target, false, false));

	return ts;
}

TraceSequence *TraceSequence_merge(ModelCompare *const this, ContainerRoot *origin, ContainerRoot *target)
{
	TraceSequence *ts = new_TraceSequence();

	ts->populate(this->internal_diff(origin, target, false, true));

	return ts;
}

TraceSequence *TraceSequence_inter(ModelCompare *const this, ContainerRoot *origin, ContainerRoot *target)
{
	TraceSequence *ts = new_TraceSequence();

	ts->populate(this->internal_diff(origin, target, true, false));

	return ts;
}

list_t Trace_sequence_internal_diff(ModelCompare *const this, ContainerRoot *origin, ContainerRoot *target, bool inter, bool merge)
{
	list_t traces_list;

	ModelTrace *mt;

	return NULL;
}

std::list < ModelTrace * >* ModelCompare::internal_diff (KMFContainer *origin,KMFContainer *target,bool inter, bool merge)
{
	list < ModelTrace * > *traces ;
	list < ModelTrace * > *tracesRef;
	std::map<string,KMFContainer*> values;

	traces = origin->createTraces (target, inter, merge, false, true);
	tracesRef = origin->createTraces (target, inter, merge, true, false);

	// visitors 
	ModelCompareVisitorFiller *filler = new ModelCompareVisitorFiller(&values);
	origin->visit(filler, true, true, false);
	delete filler;


	ModelCompareVisitorCreateTraces *visitorTraces= new ModelCompareVisitorCreateTraces(&values,inter,merge,traces,tracesRef);
	target->visit(visitorTraces, true, true, false);
	delete visitorTraces;

	if(!inter){
		//if diff
		if(!merge)
		{
			for ( std::map<string,KMFContainer*>::const_iterator it = values.begin();  it != values.end(); ++it) {

				KMFContainer *diffChild;
				string src;
				string refNameInParent="";
				diffChild =it->second;

				if( diffChild-> eContainer())
				{
					src =   diffChild-> eContainer()->path();
				}
				refNameInParent = diffChild-> getRefInParent();
				ModelRemoveTrace *modelremovetrace= new ModelRemoveTrace(src,refNameInParent,diffChild->path());
				traces->push_back(modelremovetrace);
			}
		}
	}
	std::copy(tracesRef->begin(), tracesRef->end(), std::back_insert_iterator<std::list<ModelTrace*> >(*traces));
	values.clear();
	delete tracesRef;

	return traces;
}
