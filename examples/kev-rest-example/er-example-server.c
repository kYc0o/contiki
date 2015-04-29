/*
 * Copyright (c) 2013, Matthias Kovatsch
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      Erbium (Er) REST Engine example (with CoAP-specific code)
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "contiki.h"
#include "contiki-net.h"
#include "cfs/cfs.h"
#include "loader/elfloader.h"
#include "lib/list.h"
#include "json.h"
#include "jsonparse.h"

#include "kevoree.h"
#include "KMF4C.h"
#include "AbstractComponent.h"
#include "AbstractTypeDefinition.h"
#include "JSONModelLoader.h"
#include "ModelTrace.h"
#include "TraceSequence.h"
#include "hello-world-component.h"


/* Define which resources to include to meet memory constraints. */
#define REST_RES_MODELS 1
#define REST_RES_PUT 1
#define REST_RES_HELLO 1
#define REST_RES_CHUNKS 0
#define REST_RES_SEPARATE 0
#define REST_RES_PUSHING 0
#define REST_RES_EVENT 0
#define REST_RES_SUB 0
#define REST_RES_LEDS 0
#define REST_RES_TOGGLE 1
#define REST_RES_LIGHT 0
#define REST_RES_BATTERY 0
#define REST_RES_RADIO 0
#define REST_RES_MIRROR 0 /* causes largest code size */

#define SERVER_NODE(ipaddr)   uip_ip6addr(ipaddr, 0xbbbb, 0, 0, 0, 0x0323, 0x4501, 0x2760, 0x0243) /* bbbb::323:4501:2760:243 */
#define LOCAL_PORT      UIP_HTONS(COAP_DEFAULT_PORT+1)
#define REMOTE_PORT     UIP_HTONS(COAP_DEFAULT_PORT)

#include "erbium.h"
#include "er-coap-13-engine.h"

#if defined (PLATFORM_HAS_BUTTON)
#include "dev/button-sensor.h"
#endif
#if defined (PLATFORM_HAS_LEDS)
#include "dev/leds.h"
#endif
#if defined (PLATFORM_HAS_LIGHT)
#include "dev/light-sensor.h"
#endif
#if defined (PLATFORM_HAS_BATTERY)
#include "dev/battery-sensor.h"
#endif
#if defined (PLATFORM_HAS_SHT11)
#include "dev/sht11-sensor.h"
#endif
#if defined (PLATFORM_HAS_RADIO)
#include "dev/radio-sensor.h"
#endif


/* For CoAP-specific example: not required for normal RESTful Web service. */
#if WITH_COAP == 3
#include "er-coap-03.h"
#elif WITH_COAP == 7
#include "er-coap-07.h"
#elif WITH_COAP == 12
#include "er-coap-12.h"
#elif WITH_COAP == 13
#include "er-coap-13.h"
#else
#warning "Erbium example without CoAP-specifc functionality"
#endif /* CoAP-specific example */

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define SPRINT6ADDR(buffer, addr) sprintf(buffer, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define SPRINT6ADDR(buffer, addr) sprintf(buffer, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(addr)
#endif

#define MAX_KEVMOD_BODY 10240
#define MAX_KEVMODEL_SIZE 5*1024
#define CHUNKS_TOTAL 2097152
#define IP6_ADDR_SIZE 40
#define MAX_NUMBER 6

/* Global variables */
char buffer[100];
const char *modelname = "current_model";
const char *new_modelname = "new_model";
char jsonTraces[6000];
int modelLength = 0;
int mem_count = 0;
ContainerRoot *current_model = NULL;
ContainerRoot *new_model = NULL;
LIST(model_traces);
Visitor *visitor_print;
static int32_t large_update_size = 0;
static uint8_t large_update_store[MAX_KEVMOD_BODY] = {0};
static unsigned int large_update_ct = -1;
int32_t strAcc = 0;
int32_t length = 0;
int32_t length2 = 0;
uint16_t pref_size = 0;
static uint32_t compPointer;
static struct etimer kev_timer;
static struct etimer inst_timer;
static struct etimer process_timer;
PROCESS(kevoree_adaptations, "Kevoree Adaptations engine");
PROCESS(instantiator, "uKev instantiator");
PROCESS(rest_server_example, "Erbium Example Server");

/* For printing ipv6 address in DEBUG=1*/
static void print_local_addresses(void)
{
	int i;
	uint8_t state;

	PRINTF("Server IPv6 addresses: \n");

	for(i = 0; i < UIP_DS6_ADDR_NB; i++)
	{
		state = uip_ds6_if.addr_list[i].state;

		if(uip_ds6_if.addr_list[i].isused && (state == ADDR_TENTATIVE || state == ADDR_PREFERRED))
		{
			PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
			PRINTF("\n");
		}
	}
}

/* For printing ipv6 address in DEBUG=1*/
static char *get_local_addresses(char *_buffer)
{
	char *buf = _buffer;
	int i;
	uint8_t state;

	for(i = 0; i < UIP_DS6_ADDR_NB; i++)
	{
		state = uip_ds6_if.addr_list[i].state;

		if(uip_ds6_if.addr_list[i].isused && (state == ADDR_TENTATIVE || state == ADDR_PREFERRED))
		{
			SPRINT6ADDR(buf, &uip_ds6_if.addr_list[i].ipaddr);
		}
	}

	return buf;
}

void* my_malloc(size_t s)
{
	mem_count += s;
	return malloc(s);
}

void* my_calloc(size_t nmemb, size_t size)
{
	mem_count += size;
	return calloc(nmemb, size);
}

void str_free(char *ptr)
{
	mem_count -= strlen(ptr);
	free(ptr);
}

void write_to_cfs(char *buf)
{
	int fd_write;
	int n;

	fd_write = cfs_open(modelname, CFS_WRITE | CFS_APPEND);

	if(fd_write != -1)
	{
		if ((n = cfs_write(fd_write, buf, strlen(buf))) != -1) {
			cfs_close(fd_write);
		} else {
			printf("ERROR: could not write to memory\n");
			cfs_close(fd_write);
		}
	}
	else
	{
		printf("ERROR: could not write to memory\n");
	}
}

void actionstore(char *path, Type type, void *value)
{
	switch(type)
	{
	case STRING:
		sprintf(buffer, "\"%s\" : \"%s\"", path, (char*)value);
		write_to_cfs(buffer);
		break;

	case STRREF:
		sprintf(buffer, "\"%s\"", path);
		write_to_cfs(buffer);
		break;

	case BOOL:
		sprintf(buffer, "\"%s\" : \"%d\"", path, (bool)value);
		write_to_cfs(buffer);
		break;

	case INTEGER:
		sprintf(buffer, "\"%s\" : \"%d\"", path, (int)value);
		write_to_cfs(buffer);
		break;

	case BRACKET:
		sprintf(buffer, "{\n");
		write_to_cfs(buffer);
		break;

	case SQBRACKET:
		sprintf(buffer, "\"%s\" : [\n", path);
		write_to_cfs(buffer);
		break;

	case CLOSEBRACKET:
		sprintf(buffer, "}\n");
		write_to_cfs(buffer);
		break;

	case CLOSESQBRACKET:
		sprintf(buffer, "]\n");
		write_to_cfs(buffer);
		break;

	case CLOSEBRACKETCOLON:
		sprintf(buffer, "},\n");
		write_to_cfs(buffer);
		break;

	case CLOSESQBRACKETCOLON:
		sprintf(buffer, "],\n");
		write_to_cfs(buffer);
		break;

	case COLON:
		sprintf(buffer, ",\n");
		write_to_cfs(buffer);
		break;

	case RETURN:
		sprintf(buffer, "\n");
		write_to_cfs(buffer);
		break;

	case REFERENCE:
		printf("ERROR: non valid type\n");
		break;
	}
}

void actionprintf(char *path, Type type, void* value)
{
	switch(type)
	{
	case STRING:
		printf("\"%s\" : \"%s\"", path, (char*)value);
		break;

	case STRREF:
		printf("\"%s\"", path);
		break;

	case BOOL:
	case INTEGER:
		printf("\"%s\" : \"%d\"", path, (int)value);
		break;

	case BRACKET:
		printf("{\n");
		break;

	case SQBRACKET:
		printf("\"%s\" : [\n", path);
		break;

	case CLOSEBRACKET:
		printf("}\n");
		break;

	case CLOSESQBRACKET:
		printf("]\n");
		break;

	case CLOSEBRACKETCOLON:
		printf("},\n");
		break;

	case CLOSESQBRACKETCOLON:
		printf("],\n");
		break;

	case COLON:
		printf(",\n");
		break;

	case RETURN:
		printf("\n");
		break;

	case REFERENCE:
		printf("ERROR: non valid type\n");
		break;
	}
}

void actionprintpath(char *path, Type type, void *value)
{
	switch(type)
	{
	case STRING:
	case REFERENCE:
		printf("path = %s  value = %s\n",path,(char*)value);
		break;

	case BOOL:
	case INTEGER:
		printf("path = %s  value = %d\n", path, (int)value);
		break;

	case STRREF:
	case BRACKET:
	case SQBRACKET:
	case CLOSEBRACKET:
	case CLOSESQBRACKET:
	case CLOSEBRACKETCOLON:
	case CLOSESQBRACKETCOLON:
	case COLON:
	case RETURN:
		printf("Type non valid!\n");
		break;
	}
}

bool actionRemove(char *_path, char *value)
{
	char *refname = NULL;
	char *src = NULL;
	KMFContainer *container;

	if ((container = new_model->FindByPath(_path, new_model)) != NULL) {
		return true;
	} else if ((container = (KMFContainer*)current_model->FindByPath(_path, current_model)) != NULL) {
		if ((src = strdup(container->eContainer)) != NULL) {
			ModelTrace *mt = newPoly_ModelRemoveTrace(src, value, _path);
			if (mt != NULL)	{
				list_add(model_traces, mt);
				free(src);
				return false;
			} else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", _path, value);
				return true;
			}
		} else {
			PRINTF("ERROR: not enough memory for src!\n");
			return true;
		}
	}

	return true;
}

bool actionAdd(char* _path, char *value)
{
	char *refname = NULL;
	char *src = NULL;
	KMFContainer *container;

	if ((container = current_model->FindByPath(_path, current_model)) != NULL) {
		return true;
	} else if ((container = (KMFContainer*)new_model->FindByPath(_path, new_model)) != NULL) {
		if ((src = strdup(container->eContainer)) != NULL) {
			ModelTrace *mt = newPoly_ModelAddTrace(src, value, _path, container->metaClassName(container));
			if (mt != NULL)	{
				list_add(model_traces, mt);
				free(src);
				return true;
			} else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", _path, (char*)value);
				return true;
			}
		} else {
			PRINTF("ERROR: not enough memory for src!\n");
			return true;
		}
	}

	return true;
}

void actionUpdate(char* _path, Type type, void* value)
{
	char *__path = strdup(_path);
	char path[250];
	char *path2 = strdup(_path);
	char *path3 = NULL;
	char *refname = NULL;
	char *src = NULL;
	KMFContainer *container;
	/*
	 * TODO return if memory is full
	 */


	if ((refname = strtok(path2, "\\")) != NULL) {
		if ((refname = strtok(NULL, "\\")) == NULL) {
			refname = strtok(path2, "\\");
		}
	}

	strcpy(path, strtok(__path, "\\"));

	if (!strcmp("generated_KMF_ID", path)) {
		strcpy(path, "");
	}

	if ((container = new_model->FindByPath(path, new_model)) != NULL) {
		if ((src = strdup(path)) != NULL) {
		} else {
			PRINTF("ERROR: not enough memory for src!\n");
		}
	}

	switch(type)
	{
	case REFERENCE:
		if (container == NULL) {
			if ((container = (KMFContainer*)current_model->FindByPath(path, current_model)) != NULL) {
				if ((src = strdup(path)) != NULL) {
				} else {
					PRINTF("ERROR: not enough memory for src!\n");
				}
			} else {
				PRINTF("ERROR: Cannot retrieve source!\n");
				src = malloc(1);
				strcpy(src, "");
			}

			ModelTrace *mt = newPoly_ModelRemoveTrace(src, refname, path);

			if (mt != NULL)	{
				list_add(model_traces, mt);
			} else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", path, (char*)value);
			}
		}
		break;

	case STRING:
		if (container == NULL) {
			/*
			 * TODO check NULL
			 */
			if ((container = (KMFContainer*)current_model->FindByPath(path, current_model)) != NULL) {
				if ((src = strdup(container->eContainer)) != NULL) {
				} else {
					PRINTF("ERROR: not enough memory for src!\n");
				}
			} else {
				PRINTF("ERROR: Cannot retrieve source!\n");
				src = malloc(1);
				strcpy(src, "");
			}
			/*printf("path = %s  value = %s\n", path, (char*)value);
			printf("Path %s does not exist in new_model, removing...\n\n", path);*/
			ModelTrace *mt = newPoly_ModelRemoveTrace(src, refname, path);
			/*char *strTrace = mt->ToString(mt->pDerivedObj);
				PRINTF(strTrace);
			free(strTrace);*/
			if (mt != NULL)	{
				list_add(model_traces, mt);
			} else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", path, (char*)value);
			}
		} else {
			/*printf("path = %s  value = %s\n", _path, (char*)value);*/
			char* string = current_model->FindByPath(_path, current_model);
			char* string2 = new_model->FindByPath(_path, new_model);
			/*printf("Current attribute value: %s\n", string);
			printf("New attribute value: %s\n", string2);*/
			if(string != NULL && string2 != NULL)
			{
				if(!strcmp(string2, string))
				{
					/*printf("Identical attributes, nothing to change...\n\n");*/
				}
				else
				{
					/*printf("Changing attribute to %s in current_model\n\n", string2);*/
					ModelTrace *mt = newPoly_ModelSetTrace(src, refname, string2);
					/*char *strTrace = mt->ToString(mt->pDerivedObj);
						PRINTF(strTrace);
						free(strTrace);*/
					if(mt != NULL)
					{
						list_add(model_traces, mt);
					}
					else {
						printf("ERROR: ModelTrace cannot be added!\n");
						printf("path = %s  value = %s\n", path, (char*)value);
					}
				}
			}
			else if(string == NULL && string2 != NULL)
			{
				/*printf("Current attribute is NULL, changing to new attribute '%s'\n\n", string2);*/
				ModelTrace *mt = newPoly_ModelSetTrace(src, refname, string2);
				/*
					char *strTrace = mt->ToString(mt->pDerivedObj);
					PRINTF(strTrace);
					free(strTrace);
				 */
				if(mt != NULL)
				{
					list_add(model_traces, mt);
				}
				else {
					printf("ERROR: ModelTrace cannot be added!\n");
					printf("path = %s  value = %s\n", path, (char*)value);
				}
			}
			else if(string != NULL && string2 == NULL)
			{
				/*printf("Changing attribute to NULL\n\n");*/
				ModelTrace *mt = newPoly_ModelSetTrace(src, refname, "");
				/*char *strTrace = mt->ToString(mt->pDerivedObj);
					PRINTF(strTrace);
					free(strTrace);*/
				if(mt != NULL)
				{
					list_add(model_traces, mt);
				}
				else {
					printf("ERROR: ModelTrace cannot be added!\n");
					printf("path = %s  value = %s\n", path, (char*)value);
				}
			}
		}
		break;

	case BOOL:
	case INTEGER:
		if(container == NULL)/*new_model->FindByPath(path, new_model) == NULL)*/
		{
			/*
			 * TODO check NULL
			 */
			container = (KMFContainer*)current_model->FindByPath(path, current_model);
			src = strdup(container->eContainer);
			PRINTF("src: %s\n", src);
			/*printf("path = %s  value = %d\n", path, (int)value);
			printf("Path %s does not exist in new_model, removing...\n\n", path);*/
			ModelTrace *mt = newPoly_ModelRemoveTrace(src, refname, path);
			/*char *strTrace = mt->ToString(mt->pDerivedObj);
				PRINTF(strTrace);
				free(strTrace);*/
			if(mt != NULL)
			{
				list_add(model_traces, mt);
			}
			else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", path, (char*)value);
			}
		}
		else
		{
			/*printf("path = %s  value = %d\n", _path, (int)value);*/
			int v = (int)(current_model->FindByPath(_path, current_model));
			int v2 = (int)(new_model->FindByPath(_path, new_model));
			char v2str[MAX_NUMBER] = {0};
			/*printf("Current attribute value: %d\n", v);
			printf("New attribute value: %d\n", v2);*/
			if(v == v2)
			{
				/*printf("Identical attributes, nothing to change...\n\n");*/
			}
			else
			{
				/*printf("Changing attribute to %d in current_model\n\n", v2);*/
				sprintf(v2str, "%d", v2);
				ModelTrace *mt = newPoly_ModelSetTrace(src, refname, v2str);
				/*char *strTrace = mt->ToString(mt->pDerivedObj);
					PRINTF(strTrace);
					free(strTrace);*/
				if(mt != NULL)
				{
					list_add(model_traces, mt);
				}
				else {
					printf("ERROR: ModelTrace cannot be added!\n");
					printf("path = %s  value = %s\n", path, (char*)value);
				}
			}
		}
		break;

	case STRREF:
	case BRACKET:
	case SQBRACKET:
	case CLOSEBRACKET:
	case CLOSESQBRACKET:
	case CLOSEBRACKETCOLON:
	case CLOSESQBRACKETCOLON:
	case COLON:
	case RETURN:
		printf("Type non valid!\n");
		break;
	}

	free(__path);
	free(path2);
	if (src != NULL) {
		free(src);
	}
}

void actionAddSet(char* _path, Type type, void* value)
{
	char *__path = strdup(_path);
	char path[250];
	char *path2 = strdup(_path);
	char *refname = NULL;
	char *src = NULL;
	char *typename = NULL;
	KMFContainer *container;
	/*
	 * TODO return if memory is full
	 */
	if ((refname = strtok(path2, "\\")) != NULL) {
		if ((refname = strtok(NULL, "\\")) == NULL) {
			refname = strtok(path2, "\\");
		}
	}

	strcpy(path, strtok(__path, "\\"));

	if (!strcmp("generated_KMF_ID", path)) {
		strcpy(path, "");
	}
	/*if ((container = current_model->FindByPath(path, current_model)) != NULL) {
		if ((src = strdup(path)) != NULL) {
		} else {
			PRINTF("ERROR: not enough memory for src!\n");
		}
		typename = strdup(container->metaClassName(container));
	} else {
		PRINTF("INFO: adding %s\n", path);
	}*/

	switch(type)
	{
	case REFERENCE:
		container = current_model->FindByPath(path, current_model);
		if(container == NULL)
		{
			if ((container = (KMFContainer*)new_model->FindByPath(path, new_model)) != NULL) {
				if ((src = strdup(container->path)) != NULL) {
					typename = strdup(container->metaClassName(container));
				} else {
					PRINTF("ERROR: not enough memory for src!\n");
				}
			} else {
				PRINTF("ERROR: Cannot retrieve source!\n");
			}
			/*printf("Path %s does not exist in curent_model, adding...\n\n", path);*/
			ModelTrace *mt = newPoly_ModelAddTrace((char*)value, refname, container->path, NULL);
			/*char *strTrace = mt->ToString(mt->pDerivedObj);
				PRINTF(strTrace);
				free(strTrace);*/
			if(mt != NULL)
			{
				list_add(model_traces, mt);
			}
			else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", path, (char*)value);
			}
		}
		else
		{
			/*printf("Path %s already exists...\n", path);*/
		}
		break;

	case STRING:
		/*printf("path = %s  value = %s\n", path, (char*)value);*/
		/*path = strtok(path, "\\");*/
		container = current_model->FindByPath(path, current_model);
		if(container == NULL)
		{
			if ((container = (KMFContainer*)new_model->FindByPath(path, new_model)) != NULL) {
				if ((src = strdup(container->path)) != NULL) {
					typename = strdup(container->metaClassName(container));
				} else {
					PRINTF("ERROR: not enough memory for src!\n");
				}
			} else {
				PRINTF("ERROR: Cannot retrieve source!\n");
			}

			/*printf("Path %s does not exist in curent_model, adding...\n\n", path);*/
			ModelTrace *mt = newPoly_ModelSetTrace(src, refname, (char*)value);
			/*char *strTrace = mt->ToString(mt->pDerivedObj);
				PRINTF(strTrace);
				free(strTrace);*/
			if(mt != NULL)
			{
				list_add(model_traces, mt);
			}
			else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", path, (char*)value);
			}
		}
		else
		{
			/*printf("Path %s already exists...\n", path);*/
		}
		break;

	case BOOL:
	case INTEGER:
		container = current_model->FindByPath(path, current_model);

		if (container == NULL) {
			if ((container = (KMFContainer*)new_model->FindByPath(path, new_model)) != NULL) {
				if ((src = strdup(container->path)) != NULL) {
					typename = strdup(container->metaClassName(container));
				} else {
					PRINTF("ERROR: not enough memory for src!\n");
				}
			} else {
				PRINTF("ERROR: Cannot retrieve source!\n");
			}

			char v2str[MAX_NUMBER] = {0};
			/*int v2 = (int)(new_model->FindByPath(_path, new_model));*/
			sprintf(v2str, "%d", (int)value);

			ModelTrace *mt = newPoly_ModelSetTrace(src, refname, v2str);

			if(mt != NULL)
			{
				list_add(model_traces, mt);
			}
			else {
				printf("ERROR: ModelTrace cannot be added!\n");
				printf("path = %s  value = %s\n", path, (char*)value);
			}
		}
		else
		{
			/*printf("Path %s already exists...\n", path);*/
		}
		break;

	case STRREF:
	case BRACKET:
	case SQBRACKET:
	case CLOSEBRACKET:
	case CLOSESQBRACKET:
	case CLOSEBRACKETCOLON:
	case CLOSESQBRACKETCOLON:
	case COLON:
	case RETURN:
		printf("Type non valid!\n");
		break;
	}

	free(__path);
	free(path2);
	if (src != NULL) {
		free(src);
	}
	if (typename != NULL) {
		free(typename);
	}
}

/******************************************************************************/
#if REST_RES_HELLO
/*
 * Resources are defined by the RESOURCE macro.
 * Signature: resource name, the RESTful methods it handles, and its URI path (omitting the leading slash).
 */
RESOURCE(helloworld, METHOD_GET, "hello", "title=\"Hello world: ?len=0..\";rt=\"Text\"");

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
void
helloworld_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	const char *len = NULL;
	/* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
	char const * const message = "Hello World! ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxy";
	int length = 12; /*           |<-------->| */

	/* The query string can be retrieved by rest_get_query() or parsed for its key-value pairs. */
	if (REST.get_query_variable(request, "len", &len)) {
		length = atoi(len);
		if (length<0) length = 0;
		if (length>REST_MAX_CHUNK_SIZE) length = REST_MAX_CHUNK_SIZE;
		memcpy(buffer, message, length);
	} else {
		memcpy(buffer, message, length);
	}

	REST.set_header_content_type(response, REST.type.TEXT_PLAIN); /* text/plain is the default, hence this option could be omitted. */
	REST.set_header_etag(response, (uint8_t *) &length, 1);
	REST.set_response_payload(response, buffer, length);
}
#endif
/******************************************************************************/
#if REST_RES_MODELS
/*
 * Resources are defined by the RESOURCE macro.
 * Signature: resource name, the RESTful methods it handles, and its URI path (omitting the leading slash).
 */
RESOURCE(models, METHOD_GET | METHOD_PUT, "models", "title=\"GET or PUT (?length=3000...) a kevoree model\";rt=\"JSON\"");

/*
 * A handler function named [resource name]_handler must be implemented for each RESOURCE.
 * A buffer for the response payload is provided through the buffer pointer. Simple resources can ignore
 * preferred_size and offset, but must respect the REST_MAX_CHUNK_SIZE limit for the buffer.
 * If a smaller block size is requested for CoAP, the REST framework automatically splits the data.
 */
void
models_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	size_t len = 0;
	int32_t strpos = 0;
	coap_packet_t *const coap_req = (coap_packet_t *) request;
	uint8_t method = REST.get_method_type(request);
	char buf[preferred_size];
	const char *modelLengthStr = NULL;
	char modelLengthChar[10];
	int fd_read;
	int32_t n = 0;

	PRINTF("Entering models_handler \n");
	PRINTF("offset: %ld\n", *offset);

	if (*offset >= CHUNKS_TOTAL)
	{
		REST.set_response_status(response, REST.status.BAD_OPTION);
		/* A block error message should not exceed the minimum block size (16).*/
		const char *error_msg = "BlockOutOfScope";
		REST.set_response_payload(response, error_msg, strlen(error_msg));
		PRINTF("ERROR: Block out of scope.\n");
		return;
	}

	if (method & METHOD_GET)
	{
		PRINTF("Method GET model name: %s\n", modelname);

		fd_read = cfs_open(modelname, CFS_READ);

		if (fd_read != -1)
		{
			PRINTF("Sending %s model\n", modelname);
			/*PRINTF("*offset = %ld\npreferred_size = %d\nstrAcc = %ld\n", *offset, preferred_size, strAcc);*/


			/* strAcc is 0 when the request is made for the first time. We read the file and calculate size */
			if (strAcc == 0)
			{
				/*fd_read = cfs_open(modelname, CFS_READ);*/

				if (fd_read != -1)
				{
					length = cfs_seek(fd_read, 0 , CFS_SEEK_END);
					cfs_seek(fd_read, 0, CFS_SEEK_SET);

					PRINTF("Model length: %ld \n", length);
					length2 = length;
				}
				else
				{
					REST.set_response_status(response, REST.status.BAD_OPTION);
					/* A block error message should not exceed the minimum block size (16). */
					const char *error_msg = "FileCouldNotRead";
					REST.set_response_payload(response, error_msg, strlen(error_msg));
					PRINTF("ERROR: could not read from memory.\n");
					return;
				}
			}

			/*fd_read = cfs_open(modelname, CFS_READ);*/

			/* Send data until reaching file lentgh.*/
			if (strpos < preferred_size && fd_read != -1)
			{
				if (length2 - strAcc >= preferred_size)
				{
					/* strAcc is 0 when the request is made for the first time, we must read the first "prefered_size" bytes */
					if (strAcc == 0)
					{
						n = cfs_read(fd_read, buf, sizeof(buf));
						/* Stock the pointer wih the last "preferred_size" */
						pref_size = preferred_size;
						/*PRINTF("FIRST TIME of reading\n");*/
					}
					else
					{
						/* For the second and next requests we seek for the right data in the file, then cumulate the pointer */
						cfs_seek(fd_read, pref_size, CFS_SEEK_SET);
						n = cfs_read(fd_read, buf, sizeof(buf));
						pref_size += preferred_size;
						/*PRINTF("data SEEKED and READED\n");*/
					}
				}
				else
				{
					/* When the last bytes are less than a complete "preferred_size" block, we read only these last bytes */
					cfs_seek(fd_read, pref_size, CFS_SEEK_SET);
					n = cfs_read(fd_read, buf, (length2 - strAcc));
					PRINTF("last read! \n");
					/*strAcc = 0;*/
				}
				/*PRINTF("bytes readed %ld\n", n);*/
				cfs_close(fd_read);
				strpos += snprintf((char *)buffer, preferred_size - strpos + 1, buf);
				length -= strpos;
				/*PRINTF("length = %ld\n", length);
	                    PRINTF("strpos = %ld \n", strpos);*/
			}
			else
			{
				REST.set_response_status(response, REST.status.BAD_OPTION);
				/* A block error message should not exceed the minimum block size (16). */
				const char *error_msg = "FileCouldNotRead";
				REST.set_response_payload(response, error_msg, strlen(error_msg));
				PRINTF("ERROR: could not read from memory.\n");
				return;
			}

			/* snprintf() does not adjust return value if truncated by size.*/
			if (strpos > preferred_size)
			{
				strpos = preferred_size;
				/*PRINTF("strpos = prefered_size, strpos : %ld \n", strpos);*/
			}

			/* Truncate if above CHUNKS_TOTAL bytes. */
			if (/* *offset*/ strAcc + (int32_t)strpos > length2)
			{
				strpos = length2 - strAcc;/* *offset; */
				/*PRINTF("strpos = length2 - *offset : %ld \n", strpos);*/
			}

			/* The query string can be retrieved by rest_get_query() or parsed for its key-value pairs. */
			REST.set_header_content_type(response, REST.type.APPLICATION_JSON); /* text/plain is the default, hence this option could be omitted. */
			REST.set_header_etag(response, (uint8_t *) &strpos, 1);
			REST.set_response_payload(response, buffer, strpos);

			/* IMPORTANT for chunk-wise resources: Signal chunk awareness to REST engine. */
			*offset += strpos;
			strAcc += strpos;
			PRINTF("offset: %ld \nstrAcc = %ld\n", *offset, strAcc);

			PRINTF("Length = %ld\n", length2);
			/* Signal end of resource representation. */
			if (/* *offset*/ strAcc >= length2)
			{
				*offset = -1;
				strAcc = 0;
				length = 0;
				/*PRINTF("*offset >= length, offset : %ld \n", *offset);*/
			}
		}
		else
		{
			REST.set_response_status(response, REST.status.BAD_OPTION);
			/* A block error message should not exceed the minimum block size (16). */
			const char *error_msg = "ModelUnavailable";
			REST.set_response_payload(response, error_msg, strlen(error_msg));
			PRINTF("ERROR: Model unavailable.\n");
			return;
		}
	}
	else if (method & METHOD_PUT)
	{
		PRINTF("Method PUT\nnew model name: %s\n", new_modelname);

		int fd_write = 0;
		int fd_read = 0;
		int n = 0;
		uint8_t *incoming = NULL;

		if ((len = (REST.get_query_variable(request, "length", &modelLengthStr))))
		{
			memcpy(modelLengthChar, modelLengthStr, len);
			PRINTF("String model length = %s\n", modelLengthChar);
			modelLength = atoi(modelLengthChar);
			PRINTF("Integer model length = %d\n", modelLength);
		}
		else
		{
			REST.set_response_status(response, REST.status.BAD_REQUEST);
			const char *error_msg = "ModelLengthReq";
			REST.set_response_payload(response, error_msg, strlen(error_msg));
			PRINTF("ERROR: Model length required.\n");
			return;
		}

		/*unsigned int ct = REST.get_header_content_type(request);

	            if (ct==-1)
	            {
	                REST.set_response_status(response, REST.status.BAD_REQUEST);
	                const char *error_msg = "NoContentType";
	                REST.set_response_payload(response, error_msg, strlen(error_msg));
	                PRINTF("ERROR: No content type.\n");
	                return;
	            }*/
		if (pref_size != 0 && coap_req->block1_num == 0)
		{
			fd_read = cfs_open(new_modelname, CFS_READ);

			if (cfs_seek(fd_read, 0, CFS_SEEK_SET) == -1)
			{
				PRINTF("New file can be written\n");
			}
			else
			{
				cfs_remove(new_modelname);
				fd_read = cfs_open(new_modelname, CFS_READ);
				if (fd_read == -1)
				{
					PRINTF("Same model name has been found, overwritting...\n");
				}
				else
				{
					PRINTF("ERROR: could read from memory, file exists.\n");
					cfs_close(fd_read);
				}
			}
		}

		if ((len = REST.get_request_payload(request, (const uint8_t **) &incoming)))
		{
			if (coap_req->block1_num*coap_req->block1_size+len <= sizeof(large_update_store))
			{
				if (coap_req->block1_num == 0)
				{
					fd_write = cfs_open(new_modelname, CFS_WRITE);
					PRINTF("Writing data...\n");
				}
				else
				{
					fd_write = cfs_open(new_modelname, CFS_WRITE | CFS_APPEND);
					PRINTF("Appending data...\n");
				}
				if(fd_write != -1)
				{
					n = cfs_write(fd_write, incoming, len);
					cfs_close(fd_write);
					length += n;
					PRINTF("Successfully appended data to cfs. wrote %i bytes, Acc=%ld\n", n, length);
				}
				else
				{
					REST.set_response_status(response, REST.status.BAD_OPTION);
					/* A block error message should not exceed the minimum block size (16). */
					const char *error_msg = "CannotWriteCFS";
					REST.set_response_payload(response, error_msg, strlen(error_msg));
					PRINTF("ERROR: could not write to memory \n");
					return;
				}
				large_update_size = coap_req->block1_num*coap_req->block1_size+len;
				large_update_ct = REST.get_header_content_type(request);

				REST.set_response_status(response, REST.status.CHANGED);
				coap_set_header_block1(response, coap_req->block1_num, 0, coap_req->block1_size);

				PRINTF("Chunk num. : %ld Size: %d \n", coap_req->block1_num, coap_req->block1_size);
			}
			else
			{
				REST.set_response_status(response, REST.status.REQUEST_ENTITY_TOO_LARGE);
				REST.set_response_payload(response, buffer, snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%uB max.", sizeof(large_update_store)));
				return;
			}

			if(modelLength == length)
			{
				printf("File transferred successfully\n");
				PROCESS_CONTEXT_BEGIN(&kevoree_adaptations);
				etimer_set(&kev_timer, CLOCK_SECOND * 2);
				PROCESS_CONTEXT_END(&kevoree_adaptations);

				strAcc = length = length2 = 0;
				*offset = -1;
				PRINTF("strAcc: %ld, length: %ld, length2: %ld, *offset: %ld\n", strAcc, length, length2, *offset);
			}
			else if(length > modelLength)
			{
				printf("Error transferring file!\n");
				if((fd_read = cfs_open(new_modelname, CFS_READ)))
				{
					cfs_remove(new_modelname);
					if (!(fd_read = cfs_open(new_modelname, CFS_READ)))
					{
						printf("new_model deleted...\n");
					}
					else
					{
						printf("ERROR: could read from memory, file exists.\n");
						cfs_close(fd_read);
					}
				}
				else
				{
					printf("ERROR: could not read from memory, file does not exist.\n");
				}

				strAcc = length = length2 = 0;
				*offset = -1;
				REST.set_response_status(response, REST.status.BAD_REQUEST);
				const char *error_msg = "IncorrectSize";
				REST.set_response_payload(response, error_msg, strlen(error_msg));
				return;
			}

		}
		else
		{
			REST.set_response_status(response, REST.status.BAD_REQUEST);
			const char *error_msg = "NoPayload";
			REST.set_response_payload(response, error_msg, strlen(error_msg));
			return;
		}
	}
}
#endif

/******************************************************************************/
#if REST_RES_PUT
/*
 * PUT in flash new application modules
 */
RESOURCE(putData, METHOD_PUT, "data", "tile=\"ELF MODULE: ?filename='filename.ce', PUT APPLICATION/OCTET_STREAM\"; rt=\"Control\"");

void
putData_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	coap_packet_t *const coap_req = (coap_packet_t*)request;
	int fd_write = 0;
	int fd_read = 0;
	/*int fd_read = 0;*/
	int n = 0;
	const char *filename = "hello-component.ce";
	int file_length = 0;
	const char *binLength;
	int iBinLength = 0;
	uint8_t *incoming = NULL;
	size_t len = 0;
	size_t len2 = 0;

	/* Check the offset for boundaries of the resource data. */
	if(*offset >= CHUNKS_TOTAL) {
		REST.set_response_status(response, REST.status.BAD_OPTION);
		/* A block error message should not exceed the minimum block size (16). */

		const char *error_msg = "BlockOutOfScope";
		REST.set_response_payload(response, error_msg, strlen(error_msg));
		return;
	}

	if (pref_size != 0 && coap_req->block1_num == 0)
	{
		fd_read = cfs_open(filename, CFS_READ);

		if (cfs_seek(fd_read, 0, CFS_SEEK_SET) == -1)
		{
			PRINTF("New file can be written\n");
		}
		else
		{
			cfs_remove(filename);
			fd_read = cfs_open(filename, CFS_READ);
			if (fd_read == -1)
			{
				PRINTF("Same filename has been found, overwritting...\n");
			}
			else
			{
				PRINTF("ERROR: could read from memory, file exists.\n");
				cfs_close(fd_read);
			}
		}
	}

	if ((len = REST.get_request_payload(request, (const uint8_t **) &incoming)))
	{
		if ((len2 = REST.get_query_variable(request, "length", &binLength)))
		{
			PRINTF("String binary length = %s\n", binLength);
			iBinLength = atoi(binLength);
			PRINTF("Integer binary length = %d\n", iBinLength);
		}
		else
		{
			REST.set_response_status(response, REST.status.BAD_REQUEST);
			const char *error_msg = "ModelLengthReq";
			REST.set_response_payload(response, error_msg, strlen(error_msg));
			PRINTF("ERROR: Model length required.\n");
			return;
		}

		if (coap_req->block1_num*coap_req->block1_size+len <= sizeof(large_update_store))
		{
			if (coap_req->block1_num == 0)
			{
				fd_write = cfs_open(filename, CFS_WRITE);
				PRINTF("Writing data...\n");
			}
			else
			{
				fd_write = cfs_open(filename, CFS_WRITE | CFS_APPEND);
				PRINTF("Appending data...\n");
			}

			if(fd_write != -1)
			{
				n = cfs_write(fd_write, incoming, len);
				cfs_close(fd_write);
				length += n;
				PRINTF("Successfully appended data to cfs. wrote %d bytes, Acc = %ld\n", n, length);
			}
			else
			{
				REST.set_response_status(response, REST.status.BAD_OPTION);
				/* A block error message should not exceed the minimum block size (16). */
				const char *error_msg = "CannotWriteCFS";
				REST.set_response_payload(response, error_msg, strlen(error_msg));
				PRINTF("ERROR: could not write to memory \n");
				return;
			}

			large_update_size = coap_req->block1_num*coap_req->block1_size+len;
			large_update_ct = REST.get_header_content_type(request);

			REST.set_response_status(response, REST.status.CHANGED);
			coap_set_header_block1(response, coap_req->block1_num, 0, coap_req->block1_size);

			PRINTF("Chunk num. : %ld Size: %d \n", coap_req->block1_num, coap_req->block1_size);
		}
		else
		{
			REST.set_response_status(response, REST.status.REQUEST_ENTITY_TOO_LARGE);
			REST.set_response_payload(response, buffer, snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%uB max.", sizeof(large_update_store)));
			return;
		}

		if(iBinLength == length)
		{
			printf("File transferred successfully\n");

			/* Kill any old processes. */
			if(elfloader_autostart_processes != NULL) {
				autostart_exit(elfloader_autostart_processes);
			}

			int fd;
			int ret;
			fd = cfs_open(filename, CFS_READ | CFS_WRITE);
			if (fd != -1)
			{
				ret = elfloader_load(fd);
				PRINTF("ret value:%d\n", ret);
				PRINTF("%s\n", elfloader_unknown);
				cfs_close(fd);

				if(ret == ELFLOADER_OK || ret == ELFLOADER_NO_STARTPOINT)
				{
					int i;
					autostart_start(elfloader_autostart_processes);
					PRINTF("ELF loaded!\n");
					PROCESS_CONTEXT_BEGIN(&instantiator);
					etimer_set(&inst_timer, CLOCK_SECOND * 2);
					PROCESS_CONTEXT_END(&instantiator);
				}
				else
				{
					PRINTF("ELF cannot be loaded!\n");
					PRINTF("ERROR: %d\n", ret);
				}
			}
			else
				PRINTF("Cannot load module\n");

			strAcc = length = length2 = 0;
			*offset = -1;
			PRINTF("strAcc: %ld, length: %ld, length2: %ld, *offset: %ld\n", strAcc, length, length2, *offset);
		}
		else if(length > iBinLength)
		{
			printf("Error transferring file!\n");
			if((fd_write = cfs_open(filename, CFS_READ)))
			{
				cfs_remove(filename);
				if (!(fd_write = cfs_open(filename, CFS_READ)))
				{
					printf("%s deleted...\n", filename);
				}
				else
				{
					printf("ERROR: could read from memory, file exists.\n");
					cfs_close(fd_write);
				}
			}
			else
			{
				printf("ERROR: could not read from memory, file does not exist.\n");
			}

			strAcc = length = length2 = 0;
			*offset = -1;
			REST.set_response_status(response, REST.status.BAD_REQUEST);
			const char *error_msg = "IncorrectSize";
			REST.set_response_payload(response, error_msg, strlen(error_msg));
			return;
		}
	}
	else
	{
		REST.set_response_status(response, REST.status.BAD_REQUEST);
		const char *error_msg = "NoPayload";
		REST.set_response_payload(response, error_msg, strlen(error_msg));
		return;
	}
}
#endif


/******************************************************************************/
#if defined (PLATFORM_HAS_LEDS)
/******************************************************************************/
#if REST_RES_LEDS
/*A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated*/
RESOURCE(leds, METHOD_POST | METHOD_PUT , "actuators/leds", "title=\"LEDs: ?color=r|g|b, POST/PUT mode=on|off\";rt=\"Control\"");

void
leds_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	size_t len = 0;
	const char *color = NULL;
	const char *mode = NULL;
	uint8_t led = 0;
	int success = 1;

	if ((len=REST.get_query_variable(request, "color", &color))) {
		PRINTF("color %.*s\n", len, color);

		if (strncmp(color, "r", len)==0) {
			led = LEDS_RED;
		} else if(strncmp(color,"g", len)==0) {
			led = LEDS_GREEN;
		} else if (strncmp(color,"b", len)==0) {
			led = LEDS_BLUE;
		} else {
			success = 0;
		}
	} else {
		success = 0;
	}

	if (success && (len=REST.get_post_variable(request, "mode", &mode))) {
		PRINTF("mode %s\n", mode);

		if (strncmp(mode, "on", len)==0) {
			leds_on(led);
		} else if (strncmp(mode, "off", len)==0) {
			leds_off(led);
		} else {
			success = 0;
		}
	} else {
		success = 0;
	}

	if (!success) {
		REST.set_response_status(response, REST.status.BAD_REQUEST);
	}
}
#endif

/******************************************************************************/
#if REST_RES_TOGGLE
/* A simple actuator example. Toggles the red led */
RESOURCE(toggle, METHOD_POST, "actuators/toggle", "title=\"Red LED\";rt=\"Control\"");
void
toggle_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	leds_toggle(LEDS_RED);
}
#endif
#endif /* PLATFORM_HAS_LEDS */
/******************************************************************************/

PROCESS_THREAD(instantiator, ev, data)
{
	PROCESS_BEGIN();
	PRINTF("Process instantiaton started!\n");
	int i;

	while(1){
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&inst_timer));
		for (i = 0; elfloader_autostart_processes[i] != NULL; ++i) {
			PRINTF("Process %s is present\n", elfloader_autostart_processes[i]->name);
			if(!strcmp(elfloader_autostart_processes[i]->name, "kev_contiki//hello_world/0.0.1"))
			{
				PRINTF("INFO: Process \"kev_contiki//hello_world/0.0.1\" found!\n");
				PRINTF("Sending pointer %p\n", &compPointer);
				process_post_synch(elfloader_autostart_processes[i], PROCESS_EVENT_POLL, &compPointer);
				etimer_set(&process_timer, CLOCK_SECOND * 2);

				PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&process_timer));
				PRINTF("Receiving pointer %p\n", &compPointer);
				AbstractTypeDefinition *comp;
				comp = (AbstractTypeDefinition*)compPointer;
				if(comp != NULL) {
					comp->start(comp);
				}
				else
					PRINTF("ERROR: Cannot create instance!\n");
			}
			else
			{
				PRINTF("ERROR: Process not found!\n");
			}
		}
	}

	PROCESS_END();
}

uip_ipaddr_t server_ipaddr;

void
client_chunk_handler(void *response)
{
	const uint8_t *chunk;

	int len = coap_get_payload(response, &chunk);

	int fd = -1;

	/*
	 * POC
	 */

	if((fd = cfs_open("new_component", CFS_WRITE | CFS_APPEND)))
	{
		int err;
		if((err = cfs_write(fd, chunk, len)))
		{
			PRINTF("Successfully appended %d bytes of component data!\n", len);
		}
		else
		{
			PRINTF("Writing binary data failed!\n");
			PRINTF("ERROR: %d", err);
		}
	}


}

PROCESS_THREAD(kevoree_adaptations, ev, dt)
{
	PROCESS_BEGIN();

	int binLength = 0;

	static coap_packet_t request[1]; /* This way the packet can be treated as pointer as usual. */
	SERVER_NODE(&server_ipaddr);
	coap_receiver_init();

	while(1)
	{
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&kev_timer));

		int fd_read = -1;
		int listLength = 0;
		int i;
		bool isFirst = true;

		printf("Starting Kevoree adaptations\n");
		printf("INFO: Trying to load new_model with length %d\n", modelLength);

		char *jsonModel = malloc(modelLength + 1);

		if((fd_read = cfs_open(new_modelname, CFS_READ)) != -1)
		{
			length = cfs_seek(fd_read, 0 , CFS_SEEK_END);
			cfs_seek(fd_read, 0, CFS_SEEK_SET);

			if((cfs_read(fd_read, jsonModel, modelLength + 1)) != -1)
			{
				printf("INFO: new_model JSON loaded in RAM\n");
				/*printf("%s\n", jsonModel);*/
			}
			else
			{
				printf("ERROR: Empty model!\n");
			}

		}

		struct jsonparse_state jsonState;

		jsonparse_setup(&jsonState, jsonModel, length + 1);

		new_model = JSONKevDeserializer(&jsonState, jsonparse_next(&jsonState));

		if(new_model != NULL)
		{
			printf("INFO: new_model loaded successfully!\n");
		}
		else
		{
			printf("ERROR: new_model cannot be loaded\n");
		}


		if(new_model != NULL)
		{
			/* Inti es aqui!! */
			visitor_print->action = actionprintpath;
			visitor_print->secondAction = NULL;
			current_model->VisitPaths(current_model, visitor_print);
			PRINTF("\n\n");
			new_model->VisitPaths(new_model, visitor_print);
			printf("INFO: new_model detected, comparing with curent_model\n\n");
			visitor_print->action = actionUpdate;
			visitor_print->secondAction = actionRemove;
			current_model->VisitPaths(current_model, visitor_print);
			visitor_print->action = actionAddSet;
			visitor_print->secondAction = actionAdd;
			new_model->VisitPaths(new_model, visitor_print);

			if((listLength = list_length(model_traces)))
			{
				int fd_write;
				int n;


				/*PRINTF("Creating TraceSequences!\n");
				TraceSequence *ts = new_TraceSequence();
				ts->populate(ts, model_traces);
				printf(ts->toString(ts));*/
				ModelTrace *mt;

				fd_write = cfs_open("json_traces.json", CFS_WRITE | CFS_APPEND);

				if(fd_write != -1)
				{
					if ((n = cfs_write(fd_write, "[\n", strlen("[\n"))) != -1) {
						for (i = 0; i < listLength; ++i) {
							if (isFirst)	{
								mt = list_head(model_traces);
								char *strTrace = mt->ToString(mt->pDerivedObj);
								if ((n = cfs_write(fd_write, strTrace, strlen(strTrace))) == -1) {
									PRINTF("ERROR: could not write to memory\n");
									/*cfs_close(fd_write);*/
									free(strTrace);
									break;
								}
								/*PRINTF(strTrace);*/
								free(strTrace);
								isFirst = false;
							} else {
								mt = list_item_next(mt);
								char *strTrace = mt->ToString(mt->pDerivedObj);
								if ((n = cfs_write(fd_write, strTrace, strlen(strTrace))) == -1) {
									PRINTF("ERROR: could not write to memory\n");
									/*cfs_close(fd_write);*/
									free(strTrace);
									break;
								}
								/*PRINTF(strTrace);*/
								free(strTrace);
							}

							if (i < listLength - 1) {
								if ((n = cfs_write(fd_write, ",", strlen(","))) == -1) {
									PRINTF("ERROR: could not write to memory\n");
									/*cfs_close(fd_write);*/
									break;
								}
							}
						}

					} else {
						PRINTF("ERROR: could not write to memory\n");
						/*cfs_close(fd_write);*/
					}

					if ((n = cfs_write(fd_write, "]\n\n", strlen("]\n\n"))) == -1) {
						PRINTF("ERROR: could not write to memory for \"]\"\n");
						/*cfs_close(fd_write);*/
					}

					cfs_close(fd_write);
				}
				else
				{
					PRINTF("ERROR: could not write to memory\n");
					/*cfs_close(fd_write);*/
				}

			}
		}
		else
		{
			PRINTF("ERROR: New model cannot be visited!\n");
		}

		free(jsonModel);

		fd_read = cfs_open("json_traces.json", CFS_READ);
		if (fd_read != -1) {
			memset(jsonTraces, 0, sizeof(jsonTraces));
			int sz = cfs_read(fd_read, jsonTraces, sizeof(jsonTraces));
			printf("%s", jsonTraces);
			cfs_close(fd_read);
		} else {
			PRINTF("ERROR: could not read from memory \n");
		}

		/*coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
		coap_set_header_uri_path(request, "CoAPKev/");

		printf("INFO: Requesting CoAPKev/\n");

		PRINT6ADDR(&server_ipaddr);
		PRINTF(" : %u\n", UIP_HTONS(REMOTE_PORT));

		COAP_BLOCKING_REQUEST(&server_ipaddr, REMOTE_PORT, request, client_chunk_handler);

		printf("\n INFO: Done\n");

		if((fd_read = cfs_open("new_component", CFS_READ)))
		{
			binLength = cfs_seek(fd_read, 0 , CFS_SEEK_END);
			cfs_seek(fd_read, 0, CFS_SEEK_SET);
			PRINTF("INFO: Received binary of length %d\n", binLength);
		}
		else
		{
			PRINTF("ERROR: Component cannot be received!\n");
		}*/

	}

	PROCESS_END();
}

AUTOSTART_PROCESSES(&rest_server_example);
PROCESS_THREAD(rest_server_example, ev, data)
{
	process_start(&kevoree_adaptations, NULL);
	process_start(&instantiator, NULL);
	elfloader_init();

	int fd_read;

	if((fd_read = cfs_open("hello-component.ce", CFS_READ)) != -1) {
		printf("INFO: removing component \"hello-component.ce\"\n");
		if(!cfs_remove("hello-component.ce")) {
			printf("Success!\n");
		} else {
			printf("ERROR: cannot remove component!\n");
		}
	}

	if((fd_read = cfs_open("json_traces.json", CFS_READ)) != -1) {
		printf("INFO: removing traces \"json_traces.json\"\n");
		if(!cfs_remove("json_traces.json")) {
			printf("Success!\n");
		} else {
			printf("ERROR: cannot remove component!\n");
		}
	}

	AbstractTypeDefinition *dummy = newPoly_AbstractComponent();
	if(dummy != NULL) {
		dummy->delete(dummy);
	}

	PROCESS_BEGIN();

	print_local_addresses();

	if((fd_read = cfs_open(modelname, CFS_READ)) != -1)
	{
		printf("Removing model %s ...\n", modelname);
		if(!cfs_remove(modelname))
		{
			printf("model %s successfully removed!\n", modelname);
		}
		else
		{
			printf("Cannot remove model %s!\n", modelname);
		}
	}
	else
	{
		printf("No current model is present!\n");
	}

	if((fd_read = cfs_open(new_modelname, CFS_READ)) != -1)
	{
		printf("Removing model %s ...\n", new_modelname);
		if(!cfs_remove(new_modelname))
		{
			printf("model %s successfully removed!\n", new_modelname);
		}
		else
		{
			printf("Cannot remove model %s!\n", new_modelname);
		}
	}
	else
	{
		printf("No new model is present!\n");
	}


	PRINTF("Starting Kevoree Example Server\n");

#ifdef RF_CHANNEL
	PRINTF("RF channel: %u\n", RF_CHANNEL);
#endif
#ifdef IEEE802154_PANID
	PRINTF("PAN ID: 0x%04X\n", IEEE802154_PANID);
#endif

	PRINTF("uIP buffer: %u\n", UIP_BUFSIZE);
	PRINTF("LL header: %u\n", UIP_LLH_LEN);
	PRINTF("IP+UDP header: %u\n", UIP_IPUDPH_LEN);
	PRINTF("REST max chunk: %u\n", REST_MAX_CHUNK_SIZE);

	/* Initialize the REST engine. */
	rest_init_engine();

	/* Activate the application-specific resources. */
#if REST_RES_HELLO
	rest_activate_resource(&resource_helloworld);
#endif
#if defined (PLATFORM_HAS_LEDS)
#if REST_RES_LEDS
	rest_activate_resource(&resource_leds);
#endif
#if REST_RES_TOGGLE
	rest_activate_resource(&resource_toggle);
#endif
#endif /* PLATFORM_HAS_LEDS */
#if REST_RES_MODELS
	rest_activate_resource(&resource_models);
#endif
#if REST_RES_PUT
	rest_activate_resource(&resource_putData);
#endif
	current_model = new_ContainerRoot();

	/* ContainerNode contikiNode */
	ContainerNode* contikiNode = new_ContainerNode();
	contikiNode->super->super->name = malloc(sizeof(char) * (strlen("nodeX")) + 1);
	sprintf(contikiNode->super->super->name, "node0");
	contikiNode->super->started = true;
	contikiNode->super->metaData = malloc(sizeof(char) * (strlen("")) + 1);
	strcpy(contikiNode->super->metaData, "");

	current_model->AddNodes(current_model, contikiNode);

	/* NetworkInfo ip */
	NetworkInfo* serverNodeIP = new_NetworkInfo();
	serverNodeIP->super->name = malloc(sizeof(char) * (strlen("ip")) + 1);
	strcpy(serverNodeIP->super->name, "ip");

	contikiNode->AddNetworkInformation(contikiNode, serverNodeIP);
	/* NeworkProperty front
			NetworkProperty* contikiNodeFront = new_NetworkProperty();
			contikiNodeFront->super->name = malloc(sizeof(char) * (strlen("front")) + 1);
			strcpy(contikiNodeFront->super->name, "front");
			contikiNodeFront->value = malloc(sizeof(char) * (strlen("contiki.kevoree.org")) + 1);
			strcpy(contikiNodeFront->value, "contiki.kevoree.org");*/

	/* NeworkProperty local
			NetworkProperty* contikiNodeLocal = new_NetworkProperty();
			contikiNodeLocal->super->name = malloc(sizeof(char) * (strlen("local")) + 1);
			strcpy(contikiNodeLocal->super->name, "local");
			contikiNodeLocal->value = malloc(sizeof(char) * (strlen("aaaa::0:0:3")) + 1);
			strcpy(contikiNodeLocal->value, "aaaa::0:0:3");*/

	/* NeworkProperty front */
	NetworkProperty* serverNodeFront = new_NetworkProperty();
	serverNodeFront->super->name = malloc(sizeof(char) * (strlen("front")) + 1);
	strcpy(serverNodeFront->super->name, "front");
	serverNodeFront->value = malloc(sizeof(char) * (strlen("m3-XX.lille.iotlab.info")) + 1);
	strcpy(serverNodeFront->value, "m3-XX.lille.iotlab.info");

	/* NeworkProperty local */
	NetworkProperty* serverNodeLocal = new_NetworkProperty();
	serverNodeLocal->super->name = malloc(sizeof(char) * (strlen("local")) + 1);
	strcpy(serverNodeLocal->super->name, "local");
	serverNodeLocal->value = malloc(sizeof(char) * strlen("aaaa::0:0:5") + 1);
	strcpy(serverNodeLocal->value, "aaaa::0:0:5");
	/*get_local_addresses(serverNodeLocal->value);*/

	/* NetworkInfo ip
			NetworkInfo* contikiNodeIP = new_NetworkInfo();
			contikiNodeIP->super->name = malloc(sizeof(char) * (strlen("ip")) + 1);
			strcpy(contikiNodeIP->super->name, "ip");

			contikiNodeIP->AddValues(contikiNodeIP, contikiNodeFront);
			contikiNodeIP->AddValues(contikiNodeIP, contikiNodeLocal);*/


	serverNodeIP->AddValues(serverNodeIP, serverNodeFront);
	serverNodeIP->AddValues(serverNodeIP, serverNodeLocal);

	/*serverNode->AddNetworkInformation(serverNode, serverNodeIP);*/

	/* TypeDefinition ContikiNode/1.0.0 */
	TypeDefinition* contikiNodeType = newPoly_NodeType();
	contikiNodeType->super->name = malloc(sizeof(char) * (strlen("ContikiNode")) + 1);
	strcpy(contikiNodeType->super->name, "ContikiNode");
	contikiNodeType->version = malloc(sizeof(char) * (strlen("0.0.1")) + 1);
	strcpy(contikiNodeType->version, "0.0.1");
	contikiNodeType->bean = malloc(sizeof(char) * (strlen("")) + 1);
	strcpy(contikiNodeType->bean, "");
	contikiNodeType->factoryBean = malloc(sizeof(char) * (strlen("")) + 1);
	strcpy(contikiNodeType->factoryBean, "");
	contikiNodeType->abstract = false;

	/*serverNode->super->AddTypeDefinition(serverNode->super, contikiNodeType);*/

	/* TypeDefinition CoAPGroup/1.0.0 */
	TypeDefinition* coapGroupType = newPoly_GroupType();
	coapGroupType->abstract = false;
	coapGroupType->super->name = malloc(sizeof(char) * (strlen("CoAPGroup")) + 1);
	strcpy(coapGroupType->super->name, "CoAPGroup");
	coapGroupType->version = malloc(sizeof(char) * (strlen("0.0.1")) + 1);
	strcpy(coapGroupType->version, "0.0.1");
	coapGroupType->bean = malloc(sizeof(char) * (strlen("")) + 1);
	strcpy(coapGroupType->bean, "");
	coapGroupType->factoryBean = malloc(sizeof(char) * (strlen("")) + 1);
	strcpy(coapGroupType->factoryBean, "");

	current_model->AddTypeDefinitions(current_model, coapGroupType);


	/* TypeLibrary Contiki */
	TypeLibrary* contiki = new_TypeLibrary();
	contiki->super->name = malloc(sizeof(char) * (strlen("ContikiLib")) + 1);
	strcpy(contiki->super->name, "ContikiLib");
	/*contiki->AddSubTypes(contiki, ctFakeConsole);*/
	/*contiki->AddSubTypes(contiki, ctHelloWorld);*/
	contiki->AddSubTypes(contiki, contikiNodeType);
	contiki->AddSubTypes(contiki, coapGroupType);
	/* TypeDefinition CoAPChan/0.0.1
		  	TypeDefinition* coapChanType = newPoly_ChannelType();
		  	coapChanType->abstract = false;
		  	coapChanType->super->name = malloc(sizeof(char) * (strlen("CoAPChan")) + 1);
		  	strcpy(coapChanType->super->name, "CoAPChan");
		  	coapChanType->version = malloc(sizeof(char) * (strlen("0.0.1")) + 1);
		  	strcpy(coapChanType->version, "0.0.1");
		  	coapChanType->bean = malloc(sizeof(char) * (strlen("")) + 1);
		  	strcpy(coapChanType->bean, "");
		  	coapChanType->factoryBean = malloc(sizeof(char) * (strlen("")) + 1);
		  	strcpy(coapChanType->factoryBean, "");
		  	((ChannelType*)coapChanType->pDerivedObj)->upperBindings = 0;
		  	((ChannelType*)coapChanType->pDerivedObj)->lowerBindings = 0;
		  	((ChannelType*)coapChanType->pDerivedObj)->upperFragments = 0;
		  	((ChannelType*)coapChanType->pDerivedObj)->lowerFragments = 0;*/

	/* TypeDefinition FakeConsoleType
		  		TypeDefinition* ctFakeConsole = newPoly_ComponentType();
		  		ctFakeConsole->abstract = false;
		  		ctFakeConsole->super->name = malloc(sizeof(char) * (strlen("FakeConsole")) + 1);
		  		strcpy(ctFakeConsole->super->name, "FakeConsole");
		  		ctFakeConsole->version = malloc(sizeof(char) * (strlen("0.0.1")) + 1);
		  		strcpy(ctFakeConsole->version, "0.0.1");
		  		ctFakeConsole->bean = malloc(sizeof(char) * (strlen("")) + 1);
		  		strcpy(ctFakeConsole->bean, "");
		  		ctFakeConsole->factoryBean = malloc(sizeof(char) * (strlen("")) + 1);
		  		strcpy(ctFakeConsole->factoryBean, "");*/

	/* TypeDefinition HelloWorldType
			TypeDefinition* ctHelloWorld = newPoly_ComponentType();
			ctHelloWorld->abstract = false;
			ctHelloWorld->super->name = malloc(sizeof(char) * (strlen("hello_world")) + 1);
			strcpy(ctHelloWorld->super->name, "hello_world");
			ctHelloWorld->version = malloc(sizeof(char) * (strlen("0.0.1")) + 1);
			strcpy(ctHelloWorld->version, "0.0.1");
			ctHelloWorld->bean = malloc(sizeof(char) * (strlen("")) + 1);
			strcpy(ctHelloWorld->bean, "");
			ctHelloWorld->factoryBean = malloc(sizeof(char) * (strlen("")) + 1);
			strcpy(ctHelloWorld->factoryBean, "");*/


	/* TypeLibrary Default */
	TypeLibrary* defLib = new_TypeLibrary();
	defLib->super->name = malloc(sizeof(char) * (strlen("Default")) + 1);
	strcpy(defLib->super->name, "Default");

	/* Channel CoAPChannel
		  	Channel* defaultChannel = new_Channel();
		  	defaultChannel->super->super->name = malloc(sizeof(char) * (strlen("DefaultChannel")) + 1);
		  	strcpy(defaultChannel->super->super->name, "DefaultChannel");
		  	defaultChannel->super->started = true;
		  	defaultChannel->super->metaData = malloc(sizeof(char) * (strlen("")) + 1);
		  	strcpy(defaultChannel->super->metaData, "");*/

	/* Channel FragmentDictionary server-node
		  		FragmentDictionary* fdChannelServerNode = new_FragmentDictionary();
		  		fdChannelServerNode->name = malloc(sizeof(char) * (strlen("server-node")) + 1);
		  		strcpy(fdChannelServerNode->name, "server-node");*/

	/* Channel FragmentDictionary contiki-node
		  	FragmentDictionary* fdChannelContikiNode = new_FragmentDictionary();
		  	fdChannelContikiNode->name = malloc(sizeof(char) * (strlen("contiki-node")) + 1);
		  	strcpy(fdChannelContikiNode->name, "contiki-node");*/

	/*defaultChannel->super->AddFragmentDictionary(defaultChannel->super, fdChannelContikiNode);*/
	/*defaultChannel->super->AddFragmentDictionary(defaultChannel->super, fdChannelServerNode);*/

	/*Channel FragmentDictionary server-node
		FragmentDictionary* fdContikiNode = new_FragmentDictionary();
		fdContikiNode->name = malloc(sizeof(char) * (strlen("contiki-node")) + 1);
		strcpy(fdContikiNode->name, "contiki-node");*/

	/* Channel DictionaryValue host
		  	DictionaryValue* chanValueHost = new_DictionaryValue();
		  	chanValueHost->name = malloc(sizeof(char) * (strlen("host")) + 1);
		  	strcpy(chanValueHost->name, "host");
		  	chanValueHost->value = malloc(sizeof(char) * (strlen("contiki.kevoree.org")) + 1);
		  	strcpy(chanValueHost->value, "contiki.kevoree.org");*/

	/* Channel DictionaryValue port
		  	DictionaryValue* chanValuePort = new_DictionaryValue();
		  	chanValuePort->name = malloc(sizeof(char) * (strlen("port")) + 1);
		  	strcpy(chanValuePort->name, "port");
		  	chanValuePort->value = malloc(sizeof(char) * (strlen("80")) + 1);
		  	strcpy(chanValuePort->value, "80");*/

	/* Channel DictionaryValue path
		  	DictionaryValue* chanValuePath = new_DictionaryValue();
		  	chanValuePath->name = malloc(sizeof(char) * (strlen("path")) + 1);
		  	strcpy(chanValuePath->name, "path");
		  	chanValuePath->value = malloc(sizeof(char) * (strlen("DefaultChannel")) + 1);
		  	strcpy(chanValuePath->value, "DefaultChannel");*/

	/* Channel DictionaryValue port
		  	DictionaryValue* dvPortContikiNode = new_DictionaryValue();
		  	dvPortContikiNode->name = malloc(sizeof(char) * (strlen("port")) + 1);
		  	strcpy(dvPortContikiNode->name, "port");
		  	dvPortContikiNode->value = malloc(sizeof(char) * (strlen("5683")) + 1);
		  	strcpy(dvPortContikiNode->value, "5683");*/

	/* Channel Dictionary
		  	Dictionary* chanDico = new_Dictionary();
		  	chanDico->AddValues(chanDico, chanValueHost);
		  	chanDico->AddValues(chanDico, chanValuePort);
		  	chanDico->AddValues(chanDico, chanValuePath);*/

	/*defaultChannel->super->AddDictionary(defaultChannel->super, chanDico);
		  	defaultChannel->super->AddTypeDefinition(defaultChannel->super, coapChanType);

		  	fdContikiNode->super->AddValues(fdContikiNode->super, dvPortContikiNode);*/

	/* ComponentType DictionaryAttribute FakeConsole
			DictionaryAttribute *daHelloWorld = new_DictionaryAttribute();
			daHelloWorld->fragmentDependant = false;
			daHelloWorld->optional = false;
			daHelloWorld->super->super->name = malloc(sizeof(char) * (strlen("time")) + 1);
			strcpy(daHelloWorld->super->super->name, "time");
			daHelloWorld->state = false;
			daHelloWorld->datatype = malloc(sizeof(char) * (strlen("int")) + 1);
			strcpy(daHelloWorld->datatype, "int");
			daHelloWorld->defaultValue = malloc(sizeof(char) * (strlen("5")) + 1);
			strcpy(daHelloWorld->defaultValue, "5");*/

	/* ChannelType DictionaryAttribute host
		  	DictionaryAttribute* chanDicoAttrHost = new_DictionaryAttribute();
		  	chanDicoAttrHost->fragmentDependant = false;
		  	chanDicoAttrHost->optional = false;
		  	chanDicoAttrHost->super->super->name = malloc(sizeof(char) * (strlen("host")) + 1);
		  	strcpy(chanDicoAttrHost->super->super->name, "host");
		  	chanDicoAttrHost->state = false;
		  	chanDicoAttrHost->datatype = malloc(sizeof(char) * (strlen("string")) + 1);
		  	strcpy(chanDicoAttrHost->datatype, "string");
		  	chanDicoAttrHost->defaultValue = malloc(sizeof(char) * (strlen("")) + 1);
		  	strcpy(chanDicoAttrHost->defaultValue, "");*/

	/* ChannelType DictionaryAttribute port
		  	DictionaryAttribute* chanDicoAttrPort = new_DictionaryAttribute();
		  	chanDicoAttrPort->fragmentDependant = false;
		  	chanDicoAttrPort->optional = false;
		  	chanDicoAttrPort->super->super->name = malloc(sizeof(char) * (strlen("port")) + 1);
		  	strcpy(chanDicoAttrPort->super->super->name, "port");
		  	chanDicoAttrPort->state = false;
		  	chanDicoAttrPort->datatype = malloc(sizeof(char) * (strlen("number")) + 1);
		  	strcpy(chanDicoAttrPort->datatype, "number");
		  	chanDicoAttrPort->defaultValue = malloc(sizeof(char) * (strlen("")) + 1);
		  	strcpy(chanDicoAttrPort->defaultValue, "");*/

	/* ChannelType DictionaryAttribute path
		  	DictionaryAttribute* chanDicoAttrPath = new_DictionaryAttribute();
		  	chanDicoAttrPath->fragmentDependant = false;
		  	chanDicoAttrPath->optional = false;
		  	chanDicoAttrPath->super->super->name = malloc(sizeof(char) * (strlen("path")) + 1);
		  	strcpy(chanDicoAttrPath->super->super->name, "path");
		  	chanDicoAttrPath->state = false;
		  	chanDicoAttrPath->datatype = malloc(sizeof(char) * (strlen("string")) + 1);
		  	strcpy(chanDicoAttrPath->datatype, "string");
		  	chanDicoAttrPath->defaultValue = malloc(sizeof(char) * (strlen("")) + 1);
		  	strcpy(chanDicoAttrPath->defaultValue, "");*/

	/* GroupType DictionaryAttribute port */
	DictionaryAttribute* gtDicAttrPort = new_DictionaryAttribute();
	gtDicAttrPort->fragmentDependant = true;
	gtDicAttrPort->optional = true;
	gtDicAttrPort->super->super->name = malloc(sizeof(char) * (strlen("port")) + 1);
	strcpy(gtDicAttrPort->super->super->name, "port");
	gtDicAttrPort->state = false;
	gtDicAttrPort->datatype = malloc(sizeof(char) * (strlen("number")) + 1);
	strcpy(gtDicAttrPort->datatype, "number");
	gtDicAttrPort->defaultValue = malloc(sizeof(char) * (strlen("")) + 1);
	strcpy(gtDicAttrPort->defaultValue, "");

	/* GroupType DictionaryAttribute path */
	DictionaryAttribute* gtDicAttrPath = new_DictionaryAttribute();
	gtDicAttrPath->fragmentDependant = true;
	gtDicAttrPath->optional = true;
	gtDicAttrPath->super->super->name = malloc(sizeof(char) * (strlen("path")) + 1);
	strcpy(gtDicAttrPath->super->super->name, "path");
	gtDicAttrPath->state = false;
	gtDicAttrPath->datatype = malloc(sizeof(char) * (strlen("string")) + 1);
	strcpy(gtDicAttrPath->datatype, "string");
	gtDicAttrPath->defaultValue = malloc(sizeof(char) * (strlen("")) + 1);
	strcpy(gtDicAttrPath->defaultValue, "");

	/* GroupType DictionaryAttribute proxy_port */
	DictionaryAttribute* gtDicAttrProxy = new_DictionaryAttribute();
	gtDicAttrProxy->fragmentDependant = true;
	gtDicAttrProxy->optional = true;
	gtDicAttrProxy->super->super->name = malloc(sizeof(char) * (strlen("proxy_port")) + 1);
	strcpy(gtDicAttrProxy->super->super->name, "proxy_port");
	gtDicAttrProxy->state = false;
	gtDicAttrProxy->datatype = malloc(sizeof(char) * (strlen("int")) + 1);
	strcpy(gtDicAttrProxy->datatype, "int");
	gtDicAttrProxy->defaultValue = malloc(sizeof(char) * (strlen("")) + 1);
	strcpy(gtDicAttrProxy->defaultValue, "20000");

	/* GroupType DictionaryAttribute proxy_port
		  		DictionaryAttribute* gtDicAttrProxy = new_DictionaryAttribute();
		  		gtDicAttrProxy->fragmentDependant = true;
		  		gtDicAttrProxy->optional = true;
		  		gtDicAttrProxy->super->super->name = malloc(sizeof(char) * (strlen("proxy_port")) + 1);
		  		strcpy(gtDicAttrProxy->super->super->name, "proxy_port");
		  		gtDicAttrProxy->state = false;
		  		gtDicAttrProxy->datatype = malloc(sizeof(char) * (strlen("int")) + 1);
		  		strcpy(gtDicAttrProxy->datatype, "int");
		  		gtDicAttrProxy->defaultValue = malloc(sizeof(char) * (strlen("")) + 1);
		  		strcpy(gtDicAttrProxy->defaultValue, "9000");*/

	/* ComponentInstance DictionaryType HelloWorld
			DictionaryType* dtHelloWorld = new_DictionaryType();
			dtHelloWorld->AddAttributes(dtHelloWorld, daHelloWorld);*/

	/* ComponentInstance DictionaryType HelloWorld
			DictionaryType* dtHelloWorld = new_DictionaryType();*/


	/* GroupType DictionaryType */
	DictionaryType* gtDicType = new_DictionaryType();
	coapGroupType->AddDictionaryType(coapGroupType, gtDicType);

	gtDicType->AddAttributes(gtDicType, gtDicAttrPort);
	gtDicType->AddAttributes(gtDicType, gtDicAttrPath);
	gtDicType->AddAttributes(gtDicType, gtDicAttrProxy);

	/* ChannelType DictionaryType
		  	DictionaryType* chanDicType = new_DictionaryType();
		  	chanDicType->AddAttributes(chanDicType, chanDicoAttrHost);
		  	chanDicType->AddAttributes(chanDicType, chanDicoAttrPort);
		  	chanDicType->AddAttributes(chanDicType, chanDicoAttrPath);*/

	/* DeployUnit //kevoree-contiki-node/0.0.1 */
	DeployUnit* kevContikiNode = new_DeployUnit();
	kevContikiNode->super->name = malloc(sizeof(char) * (strlen("kevoree-contiki-node")) + 1);
	strcpy(kevContikiNode->super->name, "kevoree-contiki-node");
	kevContikiNode->groupName = malloc(sizeof(char) * (strlen("org.kevoree.library.c")) + 1);
	strcpy(kevContikiNode->groupName, "org.kevoree.library.c");
	kevContikiNode->hashcode = malloc(sizeof(char) * (strlen("")) + 1);
	strcpy(kevContikiNode->hashcode, "");
	kevContikiNode->version = malloc(sizeof(char) * (strlen("0.0.1")) + 1);
	strcpy(kevContikiNode->version,"0.0.1");
	kevContikiNode->url = malloc(sizeof(char) * (strlen("")) + 1);
	strcpy(kevContikiNode->url, "");
	kevContikiNode->type = malloc(sizeof(char) * (strlen("ce")) + 1);
	strcpy(kevContikiNode->type,"ce");

	/* DeployUnit //kevoree-group-coap/0.0.1 */
	DeployUnit* kevGroupCoap = new_DeployUnit();
	kevGroupCoap->super->name = malloc(sizeof(char) * (strlen("kevoree-group-coap")) + 1);
	strcpy(kevGroupCoap->super->name, "kevoree-group-coap");
	kevGroupCoap->groupName = malloc(sizeof(char) * (strlen("")) + 1);
	strcpy(kevGroupCoap->groupName, "");
	kevGroupCoap->hashcode = malloc(sizeof(char) * (strlen("")) + 1);
	strcpy(kevGroupCoap->hashcode, "");
	kevGroupCoap->version = malloc(sizeof(char) * (strlen("0.0.1")) + 1);
	strcpy(kevGroupCoap->version,"0.0.1");
	kevGroupCoap->url = malloc(sizeof(char) * (strlen("")) + 1);
	strcpy(kevGroupCoap->url, "");
	kevGroupCoap->type = malloc(sizeof(char) * (strlen("ce")) + 1);
	strcpy(kevGroupCoap->type,"ce");

	/* DeployUnit //kevoree-chan-coap/0.0.1
		  	DeployUnit* kevChanCoap = new_DeployUnit();
		  	kevChanCoap->super->name = malloc(sizeof(char) * (strlen("kevoree-chan-coap")) + 1);
		  	strcpy(kevChanCoap->super->name, "kevoree-chan-coap");
		  	kevChanCoap->groupName = malloc(sizeof(char) * (strlen("")) + 1);
		  	strcpy(kevChanCoap->groupName, "");
		  	kevChanCoap->hashcode = malloc(sizeof(char) * (strlen("")) + 1);
		  	strcpy(kevChanCoap->hashcode, "");
		  	kevChanCoap->version = malloc(sizeof(char) * (strlen("0.0.1")) + 1);
		  	strcpy(kevChanCoap->version,"0.0.1");
		  	kevChanCoap->url = malloc(sizeof(char) * (strlen("")) + 1);
		  	strcpy(kevChanCoap->url, "");
		  	kevChanCoap->type = malloc(sizeof(char) * (strlen("ce")) + 1);
		  	strcpy(kevChanCoap->type,"ce");*/

	/* DeployUnit //kevoree-comp-fakeconsole/0.0.1
		  		DeployUnit* duFakeConsole = new_DeployUnit();
		  		duFakeConsole->super->name = malloc(sizeof(char) * (strlen("kevoree-comp-fakeconsole")) + 1);
		  		strcpy(duFakeConsole->super->name, "kevoree-comp-fakeconsole");
		  		duFakeConsole->groupName = malloc(sizeof(char) * (strlen("")) + 1);
		  		strcpy(duFakeConsole->groupName, "");
		  		duFakeConsole->hashcode = malloc(sizeof(char) * (strlen("")) + 1);
		  		strcpy(duFakeConsole->hashcode, "");
		  		duFakeConsole->version = malloc(sizeof(char) * (strlen("0.0.1")) + 1);
		  		strcpy(duFakeConsole->version,"0.0.1");
		  		duFakeConsole->url = malloc(sizeof(char) * (strlen("")) + 1);
		  		strcpy(duFakeConsole->url, "");
		  		duFakeConsole->type = malloc(sizeof(char) * (strlen("ce")) + 1);
		  		strcpy(duFakeConsole->type,"ce");*/

	/* DeployUnit //kevoree-comp-helloworld/0.0.1
			DeployUnit* duHelloWorld = new_DeployUnit();
			duHelloWorld->super->name = malloc(sizeof(char) * (strlen("hello_world")) + 1);
			strcpy(duHelloWorld->super->name, "hello_world");
			duHelloWorld->groupName = malloc(sizeof(char) * (strlen("")) + 1);
			strcpy(duHelloWorld->groupName, "kev_contiki");
			duHelloWorld->hashcode = malloc(sizeof(char) * (strlen("")) + 1);
			strcpy(duHelloWorld->hashcode, "");
			duHelloWorld->version = malloc(sizeof(char) * (strlen("0.0.1")) + 1);
			strcpy(duHelloWorld->version,"0.0.1");
			duHelloWorld->url = malloc(sizeof(char) * (strlen("")) + 1);
			strcpy(duHelloWorld->url, "");
			duHelloWorld->type = malloc(sizeof(char) * (strlen("ce")) + 1);
			strcpy(duHelloWorld->type,"ce");*/

	/* PortTypeRef sendMsg
		  		PortTypeRef* ptrSendMsg = new_PortTypeRef();
		  		ptrSendMsg->super->name = malloc(sizeof(char) * (strlen("sendMsg")) + 1);
		  		strcpy(ptrSendMsg->super->name, "sendMsg");
		  		ptrSendMsg->noDependency = true;
		  		ptrSendMsg->optional = true;*/

	/* PortTypeRef inMsg
		  		PortTypeRef* ptrInMsg = new_PortTypeRef();
		  		ptrInMsg->super->name = malloc(sizeof(char) * (strlen("inMsg")) + 1);
		  		strcpy(ptrInMsg->super->name, "inMsg");
		  		ptrInMsg->noDependency = true;
		  		ptrInMsg->optional = true;*/

	/* PortTypeRef sendText
			PortTypeRef* ptrSendText = new_PortTypeRef();
			ptrSendText->super->name = malloc(sizeof(char) * (strlen("sendText")) + 1);
			strcpy(ptrSendText->super->name, "sendText");
			ptrSendText->noDependency = true;
			ptrSendText->optional = true;*/

	/* PortTypeRef fake
			PortTypeRef* ptrFake = new_PortTypeRef();
			ptrFake->super->name = malloc(sizeof(char) * (strlen("fake")) + 1);
			strcpy(ptrFake->super->name, "fake");
			ptrFake->noDependency = true;
			ptrFake->optional = true;*/

	/* Port inMsg
		  		Port* pInMsg = new_Port();
		  		pInMsg->super->name = malloc(sizeof(char) * (strlen("inMsg")) + 1);
		  		strcpy(pInMsg->super->name, "inMsg");
		  		pInMsg->AddPortTypeRef(pInMsg, ptrInMsg);*/

	/* Port sendMsg
		  		Port* pSendMsg = new_Port();
		  		pSendMsg->super->name = malloc(sizeof(char) * (strlen("sendMsg")) + 1);
		  		strcpy(pSendMsg->super->name, "sendMsg");
		  		pSendMsg->AddPortTypeRef(pSendMsg, ptrSendMsg);*/

	/* Port fake
			Port* pFake = new_Port();
			pFake->super->name = malloc(sizeof(char) * (strlen("fake")) + 1);
			strcpy(pFake->super->name, "fake");
			pFake->AddPortTypeRef(pFake, ptrFake);*/

	/* Port sendMsg
			Port* pSendText = new_Port();
			pSendText->super->name = malloc(sizeof(char) * (strlen("sendText")) + 1);
			strcpy(pSendText->super->name, "sendText");
			pSendText->AddPortTypeRef(pSendText, ptrSendText);*/

	/* MBinding inMsg
		  		MBinding* mbInMsg = new_MBinding();
		  		mbInMsg->AddHub(mbInMsg, defaultChannel);
		  		mbInMsg->AddPort(mbInMsg, pInMsg);
		  		pInMsg->AddBindings(pInMsg, mbInMsg);*/

	/* MBinding fake
		  	MBinding* mbFake = new_MBinding();
		  	mbFake->AddHub(mbFake, defaultChannel);
		  	mbFake->AddPort(mbFake, pFake);
		  	pFake->AddBindings(pFake, mbFake);*/

	/* MBinding sendMsg
		  		MBinding* mbSendMsg = new_MBinding();
		  		mbSendMsg->AddHub(mbSendMsg, defaultChannel);
		  		mbSendMsg->AddPort(mbSendMsg, pSendMsg);
		  		pSendMsg->AddBindings(pSendMsg, mbSendMsg);*/

	/* MBinding sendText
		  	MBinding* mbSendText = new_MBinding();
		  	mbSendText->AddHub(mbSendText, defaultChannel);
		  	mbSendText->AddPort(mbSendText, pSendText);
		  	pSendText->AddBindings(pSendText, mbSendText);*/

	/* Group CoAP */
	Group* coapGroup = new_Group();
	coapGroup->super->super->name = malloc(sizeof(char) * (strlen("group0")) + 1);
	strcpy(coapGroup->super->super->name, "group0");
	coapGroup->super->started = true;
	coapGroup->super->metaData = malloc(sizeof(char) * (strlen("")) + 1);
	strcpy(coapGroup->super->metaData, "");

	current_model->AddGroups(current_model, coapGroup);
	/*coapGroup->AddSubNodes(coapGroup, serverNode);*/
	coapGroup->super->AddTypeDefinition(coapGroup->super, coapGroupType);

	/* FragmentDictionary contiki-node */
	FragmentDictionary* coapGroupFragDico = new_FragmentDictionary();
	coapGroupFragDico->name = malloc(sizeof(char) * (strlen("CoAPGroupFragDic")) + 1);
	strcpy(coapGroupFragDico->name, "CoAPGroupFragDic");

	/* Group DictionaryValue port */
	DictionaryValue* groupValuePort = new_DictionaryValue();
	groupValuePort->name = malloc(sizeof(char) * (strlen("port")) + 1);
	strcpy(groupValuePort->name, "port");
	groupValuePort->value = malloc(sizeof(char) * (strlen("5683")) + 1);
	strcpy(groupValuePort->value, "5683");

	/* Group DictionaryValue proxy_port */
	DictionaryValue* groupValueProxy = new_DictionaryValue();
	groupValueProxy->name = malloc(sizeof(char) * (strlen("proxy_port")) + 1);
	strcpy(groupValueProxy->name, "proxy_port");
	groupValueProxy->value = malloc(sizeof(char) * (strlen("20000")) + 1);
	strcpy(groupValueProxy->value, "20000");

	/* Group DictionaryValue path */
	DictionaryValue* groupValuePath = new_DictionaryValue();
	groupValuePath->name = malloc(sizeof(char) * (strlen("path")) + 1);
	strcpy(groupValuePath->name, "path");
	groupValuePath->value = malloc(sizeof(char) * (strlen("CoAPGroup")) + 1);
	strcpy(groupValuePath->value, "CoAPGroup");

	coapGroup->super->AddFragmentDictionary(coapGroup->super, coapGroupFragDico);
	/* Adding values to FragmentDictionary ContikiNode */
	coapGroupFragDico->super->AddValues(coapGroupFragDico->super, groupValuePort);
	coapGroupFragDico->super->AddValues(coapGroupFragDico->super, groupValueProxy);
	coapGroupFragDico->super->AddValues(coapGroupFragDico->super, groupValuePath);


	/* DictionaryType contikiNodeDicType
		DictionaryType* contikiNodeDicType = new_DictionaryType();
		contikiNodeType->AddDictionaryType(contikiNodeType, contikiNodeDicType);*/

	/*ComponentInstance* ciFakeConsole = new_ComponentInstance();
		  		ciFakeConsole->super->super->name = malloc(sizeof(char) * (strlen("fakeconsole")) + 1);
		  		strcpy(ciFakeConsole->super->super->name, "fakeconsole");
		  		ciFakeConsole->super->metaData = malloc(sizeof(char) * (strlen("")) + 1);
		  		strcpy(ciFakeConsole->super->metaData, "");
		  		ciFakeConsole->super->started = true;
		  		ciFakeConsole->super->AddTypeDefinition(ciFakeConsole->super, ctFakeConsole);
		  		ciFakeConsole->AddProvided(ciFakeConsole, pInMsg);
		  		ciFakeConsole->AddRequired(ciFakeConsole, pSendMsg);*/

	/*contikiNode->AddComponents(contikiNode, ciFakeConsole);*/

	/*serverNode->AddGroups(serverNode, coapGroup);
		  		serverNode->AddComponents(serverNode, ciFakeConsole);
		  		serverNode->AddComponents(serverNode, ciHelloWorld);

		  		ctFakeConsole->AddDeployUnit(ctFakeConsole, duFakeConsole);*/
	/*ctHelloWorld->AddDeployUnit(ctHelloWorld, duHelloWorld);*/
	contikiNodeType->AddDeployUnit(contikiNodeType, kevContikiNode);
	coapGroupType->AddDeployUnit(coapGroupType, kevGroupCoap);
	/*coapChanType->AddDeployUnit(coapChanType, kevChanCoap);*/

	/*ctFakeConsole->AddDictionaryType(ctFakeConsole, dtFakeConsole);*/
	/*ctHelloWorld->AddDictionaryType(ctHelloWorld, dtHelloWorld);*/
	/*coapChanType->AddDictionaryType(coapChanType, chanDicType);*/

	/*defaultChannel->AddBindings(defaultChannel, mbInMsg);*/
	/*defaultChannel->AddBindings(defaultChannel, mbFake);*/
	/*defaultChannel->AddBindings(defaultChannel, mbSendMsg);*/
	/*defaultChannel->AddBindings(defaultChannel, mbSendText);*/

	/*((ComponentType*)ctFakeConsole->pDerivedObj)->AddProvided((ComponentType*)ctFakeConsole->pDerivedObj, ptrInMsg);
		  		((ComponentType*)ctFakeConsole->pDerivedObj)->AddRequired((ComponentType*)ctFakeConsole->pDerivedObj, ptrSendMsg);*/
	/*((ComponentType*)ctHelloWorld->pDerivedObj)->AddProvided((ComponentType*)ctHelloWorld->pDerivedObj, ptrFake);
		  	((ComponentType*)ctHelloWorld->pDerivedObj)->AddRequired((ComponentType*)ctHelloWorld->pDerivedObj, ptrSendText);*/


	current_model->AddLibraries(current_model, contiki);
	current_model->AddLibraries(current_model, defLib);

	/*type definition*/
	current_model->AddTypeDefinitions(current_model, contikiNodeType);
	/*current_model->AddTypeDefinitions(current_model, ctHelloWorld);*/

	/*deploy unit*/
	/*modelX->AddDeployUnits(modelX, duHelloWorld);*/
	current_model->AddDeployUnits(current_model, kevContikiNode);
	current_model->AddDeployUnits(current_model, kevGroupCoap);
	/*current_model->AddDeployUnits(current_model, duHelloWorld);*/


	/*instances*/

	int i = 0;
	int j = 0;

	for(i = 0; i < 1; i++)
	{

		/*contiki->AddSubTypes(contiki, coapChanType);*/
		contikiNode->super->AddTypeDefinition(contikiNode->super, contikiNodeType);
		contikiNode->AddGroups(contikiNode, coapGroup);
		//
		//		for(j = 0; j < 1; j++)
		//		{
		//			ComponentInstance* ciHelloWorld = new_ComponentInstance();
		//			ciHelloWorld->super->super->name = malloc(sizeof(char) * (strlen("HelloWorldXX")) + 1);
		//			sprintf(ciHelloWorld->super->super->name, "HelloWorld%d", j);
		//			ciHelloWorld->super->metaData = malloc(sizeof(char) * (strlen("")) + 1);
		//			strcpy(ciHelloWorld->super->metaData, "");
		//			ciHelloWorld->super->started = true;
		//			ciHelloWorld->super->AddTypeDefinition(ciHelloWorld->super, ctHelloWorld);
		//
		//			/*((ComponentType*)ctHelloWorld->pDerivedObj)->AddProvided(ctHelloWorld->pDerivedObj, ptrFake);
		//			((ComponentType*)ctHelloWorld->pDerivedObj)->AddRequired(ctHelloWorld->pDerivedObj, ptrSendText);
		//			contikiNode->AddComponents(contikiNode, ciHelloWorld);
		//			ciHelloWorld->AddProvided(ciHelloWorld, pFake);
		//			ciHelloWorld->AddRequired(ciHelloWorld, pSendText);*/
		//			contikiNode->AddComponents(contikiNode, ciHelloWorld);
		//		}
		//
		coapGroup->AddSubNodes(coapGroup, contikiNode);


		/*printf("%d\n", mem_count);*/
	}

	visitor_print = (Visitor*)malloc(sizeof(Visitor));

	visitor_print->action = actionstore;
	/*visitor_print->action = actionprintf;*/

	/*printf("Elapsed time: %d\n", RTIMER_NOW());*/

	current_model->Visit(current_model, visitor_print);

	if((fd_read = cfs_open(modelname, CFS_READ)) != -1)
	{
		length = cfs_seek(fd_read, 0 , CFS_SEEK_END);
		cfs_seek(fd_read, 0, CFS_SEEK_SET);

		printf("Model created with length: %ld \n", length);
		length = 0;
	}
	/*visitor_print->action = actionprintpath;
	current_model->VisitPaths(current_model, visitor_print);*/


	/* Define application-specific events here. */


	while(1) {
		PROCESS_WAIT_EVENT();
		/*if (ev == PROCESS_EVENT_TIMER) {
			etimer_restart(&ledTimer);
			leds_toggle(LEDS_RED);
		}*/
	} /* while (1) */

	PROCESS_END();
}
