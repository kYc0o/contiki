/*
 * DynamicLoader.h
 *
 *  Created on: Jan 28, 2015 11:43:31 AM
 *      Author: facosta
 */

#ifndef APPS_KEVOREE_DYNAMICLOADER_H_
#define APPS_KEVOREE_DYNAMICLOADER_H_

#include <stdbool.h>
#include "hashmap.h"

/*
 * Kevoree API
 */
typedef struct _DynamicLoader DynamicLoader;
typedef struct _KevoreeModelHandlerService KevoreeModelHandlerService;
typedef struct _AbstractTypeDefinition AbstractTypeDefinition;
typedef struct _AbstractNodeType AbstractNodeType;
typedef struct _AbstractComponent AbstractComponent;
typedef struct _Bootstraper Bootstraper;
typedef struct _IDynamicLoader IDynamicLoader;

/*
 * Kevoree meta-model
 */
typedef struct _TypeDefinition TypeDefinition;
/*typedef struct _Dictionary Dictionary;
typedef struct _DictionaryValue DictionaryValue;*/
typedef struct _DeployUnit DeployUnit;
/*typedef struct _FragmentDictionary FragmentDictionary;
typedef struct _DictionaryType DictionaryType;
typedef struct _DictionaryAttribute DictionaryAttribute;*/
typedef struct _Instance Instance;

typedef DeployUnit *(*fptrSelectDUArch)(DynamicLoader*, TypeDefinition*);
typedef void (*fptrSetModelSrv)(DynamicLoader*, KevoreeModelHandlerService*);
typedef bool (*fptrDLUpdateParam)(DynamicLoader*, Instance*, AbstractTypeDefinition*);
typedef void *(*fptrELFLoaderLoad)(DynamicLoader*, int);
typedef AbstractTypeDefinition *(*fptrNewInstance)(DynamicLoader*, void*);
typedef void (*fptrDestroyInst)(DynamicLoader*, void*, AbstractTypeDefinition*);
typedef void (*fptrDLDelete)(DynamicLoader*);

typedef struct _DynamicLoader {
	IDynamicLoader *super;
	map_t references; /* int */
	map_t deployUnits; /* Contiki processes* */
	map_t instances; /* void* */
	Bootstraper *bootstrap;
	KevoreeModelHandlerService *mservice;
	fptrSelectDUArch select_du_architecture;
	fptrSetModelSrv setModelService;
	fptrDLUpdateParam update_param;
	fptrELFLoaderLoad elfloader_load;
	fptrNewInstance newInstance;
	fptrDestroyInst destroyInstance;
	fptrDLDelete delete;
} DynamicLoader;

DynamicLoader *new_DynamicLoader(Bootstraper *bootstrap);
void delete_DynamicLoader(DynamicLoader * const this);
DeployUnit *DynamicLoader_select_du_architecture(DynamicLoader * const this, TypeDefinition *type);
void DynamicLoader_setModelService(DynamicLoader * const this, KevoreeModelHandlerService *mservice);
bool DynamicLoader_update_param(DynamicLoader * const this, Instance *i, AbstractTypeDefinition *inst);
void *DynamicLoader_elfloader_load(DynamicLoader * const this, int fd);
AbstractTypeDefinition *DynamicLoader_newInstance(DynamicLoader * const this, void *handle);
void DynamicLoader_destroyInstance(DynamicLoader * const this, void *handler, AbstractTypeDefinition *instance);

#endif /* APPS_KEVOREE_DYNAMICLOADER_H_ */
