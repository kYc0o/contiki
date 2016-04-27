/*
 * DynamicLoader.c
 *
 *  Created on: Jan 28, 2015 1:57:09 PM
 *      Author: Francisco Acosta
 *       eMail: fco.ja.ac@gmail.com
 */
#include "AbstractTypeDefinition.h"
#include "KevoreeModelHandlerService.h"

#include "DynamicLoader.h"
#include "IDynamicLoader.h"
#include "Bootstraper.h"
#include "TypeDefinition.h"
#include "DeployUnit.h"
#include "Instance.h"
#include "Dictionary.h"
#include "FragmentDictionary.h"
#include "DictionaryAttribute.h"
#include "DictionaryType.h"
#include "DictionaryValue.h"
#include "TypedElement.h"
#include "NamedElement.h"

#include "loader/elfloader.h"
#include "cfs/cfs.h"
#include "sys/process.h"

static uint32_t instHandler;

DynamicLoader *new_DynamicLoader(Bootstraper *bootstrap)
{
	IDynamicLoader *pObj = new_IDynamicLoader();
	DynamicLoader *pDynLoadObj = NULL;

	if(pObj != NULL) {
		pDynLoadObj	= malloc(sizeof(DynamicLoader));
		if(pDynLoadObj == NULL) {
			return NULL;
		}
	} else {
		return NULL;
	}

	pObj->pDerivedObj = pDynLoadObj;
	pDynLoadObj->super = pObj;

	pDynLoadObj->references = NULL;
	pDynLoadObj->deployUnits = NULL;
	pDynLoadObj->instances = NULL;
	pDynLoadObj->mservice = NULL;

	pDynLoadObj->bootstrap = bootstrap;
	pDynLoadObj->select_du_architecture = DynamicLoader_select_du_architecture;
	pDynLoadObj->setModelService = DynamicLoader_setModelService;
	pDynLoadObj->update_param = DynamicLoader_update_param;
	pDynLoadObj->elfloader_load = DynamicLoader_elfloader_load;
	pDynLoadObj->newInstance = DynamicLoader_newInstance;
	pDynLoadObj->destroyInstance = DynamicLoader_destroyInstance;
	pDynLoadObj->delete = delete_DynamicLoader;

	return pDynLoadObj;
}

void delete_DynamicLoader(DynamicLoader * const this)
{
	if(this != NULL) {
		IDynamicLoader *super;
		super = this->super;

		super->delete(super);

		if(this->references != NULL) {
			hashmap_free(this->references);
		}
		if(this->deployUnits != NULL) {
			hashmap_free(this->deployUnits);
		}
		if(this->instances != NULL) {
			hashmap_free(this->instances);
		}

		free(this);
	}
}

bool IDynamicLoader_register_instance(IDynamicLoader * const this, Instance *i)
{
	DynamicLoader *dynLoad = (DynamicLoader*)this->pDerivedObj;
	TypeDefinition *type = i->typeDefinition;
	DeployUnit *du = NULL;
	char *internalGetKey;

	if(type == NULL) {
		printf("ERROR: There is no type definition defined\n");
		return false;
	}

	if(type->deployUnits == NULL) {
		printf("ERROR: There is no DeployUnit defined\n");
		return false;
	}

	if((du = dynLoad->select_du_architecture(dynLoad, type)) == NULL) {
		printf("ERROR: architecture cannot be retrieved!\n");
		return false;
	} else {
		if((internalGetKey = du->internalGetKey(du)) == NULL) {
			printf("ERROR: not enough memory for internalGetKey\n");
			return false;
		}
	}

	if(dynLoad->deployUnits != NULL) {
		void *dummy; /* dummy pointer */
		if(hashmap_get(dynLoad->deployUnits, internalGetKey, (void**)(&dummy)) == MAP_OK) {
			printf("INFO: The DeployUnit is already loaded!\n");
			int *ref;
			if(hashmap_get(dynLoad->references, internalGetKey, (void**)(&ref)) == MAP_OK) {
				*ref += 1;
				free(internalGetKey);
				return true;
			} else {
				printf("ERROR: The DeployUnit reference does not exist!\n");
				free(internalGetKey);
				return false;
			}
		}
	} else {
		int *pRef = malloc(sizeof(int));
		if(pRef != NULL) {
			*pRef = 1;
		} else {
			printf("ERROR: not enough memory for a reference\n");
			free(internalGetKey);
			return false;
		}
		if(dynLoad->references != NULL) {
			int *pCurRef;
			if(hashmap_get(dynLoad->references, internalGetKey, (void**)(&pCurRef)) == MAP_OK) {
				printf("WARNING: reference already exists, overwriting to 1\n");
				*pCurRef = 1;
			} else {
				if(hashmap_put(dynLoad->references, du->internalGetKey(du), (void**)(&pRef)) == MAP_OK) {
					printf("INFO: reference has been added!\n");
				}
			}
		} else {
			dynLoad->references = hashmap_new();
			if(dynLoad->references != NULL) {
				if(hashmap_put(dynLoad->references, du->internalGetKey(du), (void**)(&pRef)) == MAP_OK) {
					printf("INFO: reference has been added!\n");
				}
			}
		}
	}

	Bootstraper *bootstrap = dynLoad->bootstrap;

	char *libpath = bootstrap->resolveDeployUnit(bootstrap, du);

	if(libpath != NULL) {
		printf("INFO: install_deploy_unit %s\n", libpath);
		int fd;
		if((fd = cfs_open(libpath, CFS_WRITE | CFS_READ)) != -1) {
			int elfErr;
			if((elfErr = elfloader_load(fd)) == ELFLOADER_OK) {
				int err;
				printf("INFO: done %s!\n", libpath);
				int j;
				for(j = 0; elfloader_autostart_processes[j] != NULL; ++j) {
					if(!strcmp(elfloader_autostart_processes[j]->name, internalGetKey)) {
						process_start(elfloader_autostart_processes[j], NULL);
						if(dynLoad->deployUnits != NULL) {
							if((err = hashmap_put(dynLoad->deployUnits, du->internalGetKey(du), (void**)(&elfloader_autostart_processes[j]))) == MAP_OK) {
								printf("INFO: DeployUnit %s is installed!\n", internalGetKey);
								free(internalGetKey);
								return true;
							} else {
								printf("ERROR: Deploy unit cannot be installed, error %d\n", err);
								free(internalGetKey);
								return false;
							}
						} else {
							dynLoad->deployUnits = hashmap_new();
							if(dynLoad->deployUnits != NULL) {
								if((err = hashmap_put(dynLoad->deployUnits, du->internalGetKey(du), (void**)(&elfloader_autostart_processes[j]))) == MAP_OK) {
									printf("INFO: DeployUnit %s is installed!\n", internalGetKey);
									free(internalGetKey);
									return true;
								} else {
									printf("ERROR: Deploy unit cannot be installed, error %d\n", err);
									free(internalGetKey);
									return false;
								}
							} else {
								printf("ERROR: not enough memory for deployUnits\n");
								free(internalGetKey);
								return false;
							}
						}
					} else {
						printf("ERROR: Process not found!\n");
						free(internalGetKey);
						return false;
					}
				}
			} else {
				printf("ERROR: ELF loader failed! error %d\n", elfErr);
				if(elfErr == ELFLOADER_SYMBOL_NOT_FOUND) {
					printf("Symbols not found: %s\n", elfloader_unknown);
				}
				free(internalGetKey);
				return false;
			}
		} else {
			printf("ERROR: file cannot be opened!\n");
			free(internalGetKey);
			return false;
		}
	} else {
		printf("ERROR: path to lib not found!\n");
		free(internalGetKey);
		return false;
	}

	return false;
}

AbstractTypeDefinition *IDynamicLoader_create_instance(IDynamicLoader * const this, Instance *i)
{
	DynamicLoader *dynLoad = (DynamicLoader*)this->pDerivedObj;
	TypeDefinition *type = i->typeDefinition;
	char *internalGetKey;

	if(type == NULL) {
		printf("ERROR: There is no TypeDefinition defined\n");
		return NULL;
	}

	if(type->deployUnits == NULL) {
		printf("ERROR: There is no DeployUnit defined\n");
		return false;
	}

	DeployUnit *du = NULL;

	if((du = dynLoad->select_du_architecture(dynLoad, type)) == NULL) {
		printf("ERROR: architecture cannot be retrieved!\n");
		return false;
	} else {
		if((internalGetKey = du->internalGetKey(du)) == NULL) {
			printf("ERROR: not enough memory for internalGetKey\n");
			return NULL;
		}
	}

	if(dynLoad->deployUnits != NULL) {
		void *loadedDU;
		if(hashmap_get(dynLoad->deployUnits, internalGetKey, (void**)(&loadedDU)) != MAP_OK) {
			printf("ERROR: The DeployUnit is not registered!\n");
			free(internalGetKey);
			return NULL;
		} else {
			printf("INFO: newInstance of %s %s\n", du->super->name, i->path);
			void *instance = dynLoad->newInstance(dynLoad, loadedDU);
			if(instance == NULL) {
				printf("ERROR: Cannot create Instance of %s!\n", du->super->name);
				free(internalGetKey);
				return NULL;
			}
			int err;
			if(dynLoad->instances == NULL) {
				if((dynLoad->instances = hashmap_new()) == NULL) {
					printf("ERROR: cannot create instances!\n");
					free(internalGetKey);
					return NULL;
				}
			}
			if((err = hashmap_put(dynLoad->instances, i->path, (void**)(&instance))) == MAP_OK) {
				return (AbstractTypeDefinition*)instance;
			} else {
				printf("ERROR: The instance %s cannot be registered! error: %d", i->path, err);
				free(internalGetKey);
				return NULL;
			}
		}
	} else {
		printf("ERROR: there is no deployUnits registered!\n");
		free(internalGetKey);
		return NULL;
	}

}

AbstractTypeDefinition *IDynamicLoader_get_instance(IDynamicLoader * const this, Instance *i)
{
	DynamicLoader *dynLoad = (DynamicLoader*)this->pDerivedObj;
	AbstractTypeDefinition *inst = NULL;

	if(dynLoad->instances == NULL) {
		printf("ERROR: there is no instances!\n");
		return NULL;
	}

	int err;
	if((err = hashmap_get(dynLoad->instances, i->path, (void**)(&inst))) == MAP_OK) {
		return inst;
	}

	printf("ERROR: instance cannot be found! error: %d", err);
	return NULL;
}

bool IDynamicLoader_destroy_instance(IDynamicLoader * const this, Instance *i)
{
	DynamicLoader *dynLoad = (DynamicLoader*)this->pDerivedObj;
	TypeDefinition *type = i->typeDefinition;
	char *internalGetKey;

	if(type == NULL) {
		printf("ERROR: there is no TypeDefinition defined!\n");
		return false;
	}

	if(dynLoad->instances != NULL) {
		int err;
		AbstractTypeDefinition *inst;
		if((err = hashmap_get(dynLoad->instances, i->path, (void**)(&inst))) == MAP_OK) {
			DeployUnit *du = dynLoad->select_du_architecture(dynLoad, type);

			if(du == NULL) {
				printf("ERROR: architecture cannot be retrieved\n");
				return false;
			} else {
				if((internalGetKey = du->internalGetKey(du)) == NULL) {
					printf("ERROR: not enough memory for internalGetKey\n");
					return false;
				}
			}

			/*
			 * Destroying instance
			 */
			if(dynLoad->deployUnits != NULL) {
				void *loadedDU;
				if((err = hashmap_get(dynLoad->deployUnits, internalGetKey, (void**)(&loadedDU))) == MAP_OK) {
					printf("INFO: the instance %s was removed from memory\n", i->path);
					dynLoad->destroyInstance(dynLoad, loadedDU, inst);
				} else {
					printf("ERROR: cannot find DeployUnit! error: %d\n", err);
					free(internalGetKey);
					return false;
				}
			} else {
				printf("ERROR: deployUnits is empty!\n");
				free(internalGetKey);
				return false;
			}

			/*
			 * Removing instance from cache
			 */
			if(dynLoad->instances != NULL) {
				if((err = hashmap_remove(dynLoad->instances, i->path)) == MAP_OK) {
					printf("INFO: instance %s was unloaded from cache\n", i->path);
				} else {
					printf("ERROR: cannot remove instance! error: %d\n", err);
					free(internalGetKey);
					return false;
				}
			} else {
				printf("ERROR: instances is empty!\n");
				free(internalGetKey);
				return false;
			}

			/*
			 * Decreasing references to this instance
			 */
			if(dynLoad->references != NULL) {
				int *pRef;
				if((err = hashmap_get(dynLoad->references, internalGetKey, (void**)(&pRef))) == MAP_OK) {
					(*pRef) -= 1;
					/*
					 * If there is no more reference, kill the process and remove everything
					 */
					if(*pRef == 0) {
						printf("INFO: unloading dynamic library %s\n", du->super->name);
						void *loadedDU;
						if((err = hashmap_get(dynLoad->deployUnits, internalGetKey, (void**)(&loadedDU))) == MAP_OK) {
							printf("INFO: the instance %s is being removed from memory\n", i->path);
							dynLoad->destroyInstance(dynLoad, loadedDU, inst);
							struct process *duProc = loadedDU;
							process_exit(duProc);
							printf("INFO: the DeployUnit %s has been killed\n", du->super->name);
							if((err = hashmap_remove(dynLoad->deployUnits, internalGetKey)) == MAP_OK) {
								printf("INFO: deployUnit %s removed\n", internalGetKey);
							} else {
								printf("ERROR: cannot remove deployUnit %s! error: %d", internalGetKey, err);
								free(internalGetKey);
								return false;
							}
							if((err = hashmap_remove(dynLoad->references, internalGetKey)) == MAP_OK) {
								printf("INFO: deployUnit removed since references to it are 0\n");
							} else {
								printf("ERROR: cannot remove reference %s! error: %d\n", internalGetKey, err);
								free(internalGetKey);
								return false;
							}
						} else {
							printf("ERROR: cannot find DeployUnit! error: %d\n", err);
							free(internalGetKey);
							return false;
						}
					}
				} else {
					printf("ERROR: cannot decrease references!\n");
					free(internalGetKey);
					return false;
				}
			} else {
				printf("ERROR: there is no registered references!\n");
				free(internalGetKey);
				return false;
			}
		} else {
			printf("ERROR: cannot retrieve instance!\n");
			free(internalGetKey);
			return false;
		}
	} else {
		printf("ERROR: there is no registered instances!\n");
		free(internalGetKey);
		return false;
	}

	free(internalGetKey);
	return true;
}

bool IDynamicLoader_start_instance(IDynamicLoader * const this, Instance *i)
{
	DynamicLoader *dynLoad = (DynamicLoader*)this->pDerivedObj;
	AbstractTypeDefinition *inst = NULL;
	int err;

	if((err = hashmap_get(dynLoad->instances, i->path, (void**)(&inst))) == MAP_OK) {
		if(inst != NULL) {
			printf("INFO: invoking start of %s\n", i->super->name);
			dynLoad->update_param(dynLoad, i, inst);
			inst->start(inst);
			return true;
		} else {
			printf("ERROR: instance is NULL\n");
			return false;
		}
	} else {
		printf("ERROR: instance cannot be retrieved! error %d\n", err);
		return false;
	}
}

bool IDynamicLoader_stop_instance(IDynamicLoader * const this, Instance *i)
{
	DynamicLoader *dynLoad = (DynamicLoader*)this->pDerivedObj;
	AbstractTypeDefinition *inst = NULL;
	int err;

	if((err = hashmap_get(dynLoad->instances, i->path, (void**)(&inst))) == MAP_OK) {
		if(inst != NULL) {
			printf("INFO: invoking stop of %s\n", i->super->name);
			inst->stop(inst);
			return true;
		} else {
			printf("ERROR: instance is NULL\n");
			return false;
		}
	} else {
		printf("ERROR: instance cannot be retrieved! error %d\n", err);
		return false;
	}
}

bool IDynamicLoader_update_instance(IDynamicLoader * const this, Instance *i)
{
	DynamicLoader *dynLoad = (DynamicLoader*)this->pDerivedObj;
	AbstractTypeDefinition *inst = NULL;
	int err;

	if((err = hashmap_get(dynLoad->instances, i->path, (void**)(&inst))) == MAP_OK) {
		if(inst != NULL) {
			printf("INFO: invoking update of %s\n", i->super->name);
			dynLoad->update_param(dynLoad, i, inst);
			inst->update(inst, 0);
			return true;
		} else {
			printf("ERROR: instance is NULL\n");
			return false;
		}
	} else {
		printf("ERROR: instance cannot be retrieved! error %d\n", err);
		return false;
	}
}

DeployUnit *DynamicLoader_select_du_architecture(DynamicLoader * const this, TypeDefinition *type)
{
	DeployUnit *du = NULL;

	if((du = type->deployUnits) == NULL) {
		printf("ERROR: there is no deployUnit defined!\n");
		return NULL;
	} else {
		return du;
	}
}

void DynamicLoader_setModelService(DynamicLoader * const this, KevoreeModelHandlerService *mservice)
{
	this->mservice = mservice;
}

bool DynamicLoader_update_param(DynamicLoader * const this, Instance *i, AbstractTypeDefinition *inst)
{
	TypeDefinition *type = i->typeDefinition;

	if(type != NULL) {
		DictionaryType *dType = type->dictionaryType;

		if(dType != NULL) {
			Dictionary *dico = i->dictionary;
			hashmap_map *m = (hashmap_map*)dType->attributes;
			int j;
			for(j = 0; j < m->table_size; j++) {
				if(m->data[j].in_use != 0) {
					any_t data = (any_t)(m->data[j].data);
					DictionaryAttribute *da = (DictionaryAttribute*)data;

					if(dico != NULL) {
						DictionaryValue *dv = NULL;
						KevoreeModelHandlerService *mservice = this->mservice;
						FragmentDictionary *fd = NULL;
						int err;
						if((err = hashmap_get(dico->values, da->super->super->name, (void**)&dv)) == MAP_OK) {
							printf("INFO: adding parameter through Dictionary\n");
							if(inst->params != NULL) {
								int err2;
								if((err2 = hashmap_put(inst->params, da->super->super->name, (void**)&dv->value)) == MAP_OK) {
									printf("INFO: parameter %s added!\n", da->super->super->name);
									return true;
								} else {
									printf("ERROR: cannot add parameter! error %d\n", err2);
									return false;
								}
							} else {
								int err2;
								if((inst->params = hashmap_new()) != NULL) {
									if((err2 = hashmap_put(inst->params, da->super->super->name, (void**)&dv->value)) == MAP_OK) {
										printf("INFO: parameter %s added!\n", da->super->super->name);
									} else {
										printf("ERROR: cannot add parameter! error %d\n", err2);
										return false;
									}
								} else {
									printf("ERROR: cannot create parameters map!\n");
									return false;
								}
							}
						} else if ((err = hashmap_get(i->fragmentDictionary, mservice->getNodeName(mservice), (void**)&fd)) == MAP_OK) {
							printf("INFO: adding parameter through FragmentDictionary\n");
							if(inst->params != NULL) {
								int err2;
								if((err2 = hashmap_get(fd->super->values, da->super->super->name, (void**)&dv)) == MAP_OK) {
									int err3;
									if((err3 = hashmap_put(inst->params, da->super->super->name, (void**)&dv->value)) == MAP_OK) {
										printf("INFO: parameter %s added!\n", da->super->super->name);
										return true;
									} else {
										printf("ERROR: cannot add parameter! error %d\n", err3);
										return false;
									}
								} else {
									printf("ERROR: cannot get DictionaryAttribute %s! error %d\n", da->super->super->name, err2);
									return false;
								}
							} else {
								if((inst->params = hashmap_new()) != NULL) {
									int err2;
									if((err2 = hashmap_get(fd->super->values, da->super->super->name, (void**)&dv)) == MAP_OK) {
										int err3;
										if((err3 = hashmap_put(inst->params, da->super->super->name, (void**)&dv->value)) == MAP_OK) {
											printf("INFO: parameter %s added!\n", da->super->super->name);
											return true;
										} else {
											printf("ERROR: cannot add parameter! error %d\n", err3);
											return false;
										}
									} else {
										printf("ERROR: cannot get DictionaryAttribute %s! error %d\n", da->super->super->name, err2);
										return false;
									}
								} else {
									printf("ERROR: cannot create parameters map!\n");
									return false;
								}
							}
						} else {
							printf("INFO: no dictionaries found, setting default value %s\n", da->defaultValue);
							if(inst->params != NULL) {
								int err2;
								if((err2 = hashmap_put(inst->params, da->super->super->name, (void**)&da->defaultValue)) == MAP_OK) {
									printf("INFO: parameter %s added!\n", da->super->super->name);
									return true;
								} else {
									printf("ERROR: cannot add parameter! error %d\n", err2);
									return false;
								}
							} else {
								if((inst->params = hashmap_new()) != NULL) {
									int err2;
									if((err2 = hashmap_put(inst->params, da->super->super->name, (void**)&da->defaultValue)) == MAP_OK) {
										printf("INFO: parameter %s added!\n", da->super->super->name);
										return true;
									} else {
										printf("ERROR: cannot add parameter! error %d\n", err2);
										return false;
									}
								} else {
									printf("ERROR: cannot create parameters map!\n");
									return false;
								}
							}
						}
					} else {
						printf("ERROR: there is no dictionary!\n");
						return false;
					}
				}
			}
		} else {
			printf("ERROR: there is no dictionaryType!\n");
			return false;
		}
	} else {
		printf("ERROR: there is no typeDefinition!\n");
		return false;
	}

	return false;
}

/*
 * implemented in register_instance
 */
void *DynamicLoader_elfloader_load(DynamicLoader * const this, int fd)
{
	return NULL;
}

AbstractTypeDefinition *DynamicLoader_newInstance(DynamicLoader * const this, void *handle)
{
	AbstractTypeDefinition *create;
	printf("INFO: trying to create new instance!\n");

	struct process *du = handle;
	process_post_synch(du, PROCESS_EVENT_POLL, &instHandler);
	create = (AbstractTypeDefinition*)instHandler;

	if(create != NULL) {
		printf("INFO: instance created with sucess!\n");
		return create;
	} else {
		printf("ERROR: instance cannot be created!\n");
		return NULL;
	}
}
void DynamicLoader_destroyInstance(DynamicLoader * const this, void *handler, AbstractTypeDefinition *instance)
{
	printf("INFO: trying to destroy instance!\n");

	instance->delete(instance);

}
