/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
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
 *
 */

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */
#include "contiki.h"
#include "cfs/cfs.h"
#include "deluge.h"
#include "sys/node-id.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#ifndef FILE_SIZE
#define FILE_SIZE 2818
#endif

#ifndef SINK_ID
#define SINK_ID	1
#endif

/*---------------------------------------------------------------------------*/
PROCESS(deluge_test_process, "Deluge test process");
AUTOSTART_PROCESSES(&deluge_test_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(deluge_test_process, ev, data)
{
	int fd_write, i;
	/*bool isFirst = true;
	char tmpBuf[64];*/
	/*char buf[1024];*/
	char buf[FILE_SIZE];
	char *filename = "curr_model.json";
	static struct etimer et;

	PROCESS_BEGIN();
	printf("node_id: %d\n", node_id);


	/*
	sprintf(buf, "{\"eClass\" : \"org.kevoree.ContainerRoot\",\"generated_KMF_ID\" : \"BXX5q3eV\",\"nodes\" : [{\"eClass\" : \"org.kevoree.ContainerNode\",\"name\" : \"node0\",\"metaData\" : \"\",\"started\" : \"1\",\"components\" : [],\"hosts\" : [],\"host\" : [],\"groups\" : [\"groups[group0]\"],\"networkInformation\" : [{\"eClass\" : \"org.kevoree.NetworkInfo\",\"name\" : \"ip\",\"values\" : [{\"eClass\" : \"org.kevoree.NetworkProperty\",\"name\" : \"front\",\"value\" : \"m3-XX.lille.iotlab.info\"},{\"eClass\" : \"org.kevoree.NetworkProperty\",\"name\" : \"local\",\"value\" : \"fe80:0000:0000:0000:0323:4501:4471:0343\"}]}],\"typeDefinition\" : [\"typeDefinitions[ContikiNode/0.0.1]\"],\"dictionary\" : [],\"fragmentDictionary\" : []}],\"typeDefinitions\" : [{\"eClass\" : \"org.kevoree.NodeType\",\"name\" : \"ContikiNode\",\"version\" : \"0.0.1\",\"factoryBean\" : \"\",\"bean\" : \"\",\"abstract\" : \"0\",\"deployUnit\" : [\"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]\"],\"dictionaryType\" : [{\"eClass\" : \"org.kevoree.DictionaryType\",\"generated_KMF_ID\" : \"o8AVQY3e\",\"attributes\" : []}],\"superTypes\" : []},{\"eClass\" : \"org.kevoree.GroupType\",\"name\" : \"CoAPGroup\",\"version\" : \"0.0.1\",\"factoryBean\" : \"\",\"bean\" : \"\",\"abstract\" : \"0\",\"deployUnit\" : [\"deployUnits[//kevoree-group-coap/0.0.1]\"],\"dictionaryType\" : [{\"eClass\" : \"org.kevoree.DictionaryType\",\"generated_KMF_ID\" : \"3dddTFpd\",\"attributes\" : [{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"proxy_port\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"int\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"20000\"},{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"port\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"number\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"\"},{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"path\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"string\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"\"}]}],\"superTypes\" : []}],\"repositories\" : [],\"dataTypes\" : [],\"libraries\" : [{\"eClass\" : \"org.kevoree.TypeLibrary\",\"name\" : \"ContikiLib\",\"subTypes\" : [\"typeDefinitions[ContikiNode/0.0.1]\",\"typeDefinitions[CoAPGroup/0.0.1]\"]},{\"eClass\" : \"org.kevoree.TypeLibrary\",\"name\" : \"Default\",\"subTypes\" : []}],\"hubs\" : [],\"mBindings\" : [],\"deployUnits\" : [{\"eClass\" : \"org.kevoree.DeployUnit\",\"name\" : \"kevoree-group-coap\",\"groupName\" : \"\",\"version\" : \"0.0.1\",\"url\" : \"\",\"hashcode\" : \"\",\"type\" : \"ce\"},{\"eClass\" : \"org.kevoree.DeployUnit\",\"name\" : \"kevoree-contiki-node\",\"groupName\" : \"org.kevoree.library.c\",\"version\" : \"0.0.1\",\"url\" : \"\",\"hashcode\" : \"\",\"type\" : \"ce\"}],\"nodeNetworks\" : [],\"groups\" : [{\"eClass\" : \"org.kevoree.Group\",\"name\" : \"group0\",\"metaData\" : \"\",\"started\" : \"1\",\"subNodes\" : [\"nodes[node0]\"],\"typeDefinition\" : [\"typeDefinitions[CoAPGroup/0.0.1]\"],\"dictionary\" : [],\"fragmentDictionary\" : [{\"eClass\" : \"org.kevoree.FragmentDictionary\",\"generated_KMF_ID\" : \"VEj2RlNr\",\"name\" : \"contiki-node\",\"values\" : []}]}]}");
	 */
	/*for (i = 0; i < 16; ++i) {
		if (isFirst) {
			strcpy(buf, "[{empty model for testing purposes abcdefghijklmnopqrstuvwxyz}");
			isFirst = false;
		} else {
			strcpy(tmpBuf, "{empty model for testing purposes abcdefghijklmnopqrstuvwxyz}");
			sprintf(buf, "%s,%s", buf, tmpBuf);
		}
	}*/
	if(node_id == SINK_ID) {
		memset(buf, 0, sizeof(buf));
		strcpy(buf, "{\"eClass\" : \"org.kevoree.ContainerRoot\",\"generated_KMF_ID\" : \"BXX5q3eV\",\"nodes\" : [{\"eClass\" : \"org.kevoree.ContainerNode\",\"name\" : \"node0\",\"metaData\" : \"\",\"started\" : \"1\",\"components\" : [],\"hosts\" : [],\"host\" : [],\"groups\" : [\"groups[group0]\"],\"networkInformation\" : [{\"eClass\" : \"org.kevoree.NetworkInfo\",\"name\" : \"ip\",\"values\" : [{\"eClass\" : \"org.kevoree.NetworkProperty\",\"name\" : \"front\",\"value\" : \"m3-XX.lille.iotlab.info\"},{\"eClass\" : \"org.kevoree.NetworkProperty\",\"name\" : \"local\",\"value\" : \"fe80:0000:0000:0000:0323:4501:4471:0343\"}]}],\"typeDefinition\" : [\"typeDefinitions[ContikiNode/0.0.1]\"],\"dictionary\" : [],\"fragmentDictionary\" : []}],\"typeDefinitions\" : [{\"eClass\" : \"org.kevoree.NodeType\",\"name\" : \"ContikiNode\",\"version\" : \"0.0.1\",\"factoryBean\" : \"\",\"bean\" : \"\",\"abstract\" : \"0\",\"deployUnit\" : [\"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]\"],\"dictionaryType\" : [{\"eClass\" : \"org.kevoree.DictionaryType\",\"generated_KMF_ID\" : \"o8AVQY3e\",\"attributes\" : []}],\"superTypes\" : []},{\"eClass\" : \"org.kevoree.GroupType\",\"name\" : \"CoAPGroup\",\"version\" : \"0.0.1\",\"factoryBean\" : \"\",\"bean\" : \"\",\"abstract\" : \"0\",\"deployUnit\" : [\"deployUnits[//kevoree-group-coap/0.0.1]\"],\"dictionaryType\" : [{\"eClass\" : \"org.kevoree.DictionaryType\",\"generated_KMF_ID\" : \"3dddTFpd\",\"attributes\" : [{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"proxy_port\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"int\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"20000\"},{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"port\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"number\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"\"},{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"path\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"string\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"\"}]}],\"superTypes\" : []}],\"repositories\" : [],\"dataTypes\" : [],\"libraries\" : [{\"eClass\" : \"org.kevoree.TypeLibrary\",\"name\" : \"ContikiLib\",\"subTypes\" : [\"typeDefinitions[ContikiNode/0.0.1]\",\"typeDefinitions[CoAPGroup/0.0.1]\"]},{\"eClass\" : \"org.kevoree.TypeLibrary\",\"name\" : \"Default\",\"subTypes\" : []}],\"hubs\" : [],\"mBindings\" : [],\"deployUnits\" : [{\"eClass\" : \"org.kevoree.DeployUnit\",\"name\" : \"kevoree-group-coap\",\"groupName\" : \"\",\"version\" : \"0.0.1\",\"url\" : \"\",\"hashcode\" : \"\",\"type\" : \"ce\"},{\"eClass\" : \"org.kevoree.DeployUnit\",\"name\" : \"kevoree-contiki-node\",\"groupName\" : \"org.kevoree.library.c\",\"version\" : \"0.0.1\",\"url\" : \"\",\"hashcode\" : \"\",\"type\" : \"ce\"}],\"nodeNetworks\" : [],\"groups\" : [{\"eClass\" : \"org.kevoree.Group\",\"name\" : \"group0\",\"metaData\" : \"\",\"started\" : \"1\",\"subNodes\" : [\"nodes[node0]\"],\"typeDefinition\" : [\"typeDefinitions[CoAPGroup/0.0.1]\"],\"dictionary\" : [],\"fragmentDictionary\" : [{\"eClass\" : \"org.kevoree.FragmentDictionary\",\"generated_KMF_ID\" : \"VEj2RlNr\",\"name\" : \"contiki-node\",\"values\" : []}]}]}");
	} else {
		/*strcpy(buf, "[{\"empty model for testing purposes\" : \"abcdefghijklmnopqrstuv\"},{\"empty model for testing purposes\" : \"abcdefghijklmnopqrstuv\"},{\"empty model for testing purposes\" : \"abcdefghijklmnopqrstuv\"},{\"empty model for testing purposes\" : \"abcdefghijklmnopqrstuv\"},{\"empty model for testing purposes\" : \"abcdefghijklmnopqrstuv\"},{\"empty model for testing purposes\" : \"abcdefghijklmnopqrstuv\"},{\"empty model for testing purposes\" : \"abcdefghijklmnopqrstuv\"},{\"empty model for testing purposes\" : \"abcdefghijklmnopqrstuv\"},{\"empty model for testing purposes\" : \"abcdefghijklmnopqrstuv\"},{\"empty model for testing purposes\" : \"abcdefghijklmnopqrstuv\"},{\"empty model for testing purposes\" : \"abcdefghijklmnopqrstuv\"},{\"empty model for testing purposes\" : \"abcdefghijklmnopqrstuv\"},{\"empty model for testing purposes\" : \"abcdefghijklmnopqrstuv\"},{\"empty model for testing purposes\" : \"abcdefghijklmnopqrstuv\"},{\"empty model for testing purposes\" : \"abcdefghijklmnopqrstuv\"},{\"empty model for testing purposes\" : \"abcdefghijklmnopqrstu\"}]");*/
		/*for (j = 0; j < FILE_SIZE - 1; j++) {
			if (isFirst) {
				strcpy(buf, "x");
				isFirst = false;
			} else {
				sprintf(buf, "%sx", buf);
			}
		}*/
		memset(buf, 'x', sizeof(buf));
	}

	printf("file size is: %d\n", strlen(buf));

	cfs_remove(filename);
	fd_write = cfs_open(filename, CFS_WRITE);
	if(fd_write < 0) {
		printf("ERROR: Cannot open file for writing\n");
		process_exit(NULL);
	}
	if((i = cfs_write(fd_write, buf, strlen(buf) + 1)) != strlen(buf) + 1) {
		printf("ERROR: Cannot write file\n");
		cfs_close(fd_write);
		process_exit(NULL);
	} else {
		printf("%d bytes written\n", i);
		cfs_close(fd_write);
	}

	/*if(cfs_seek(fd, FILE_SIZE, CFS_SEEK_SET) != FILE_SIZE) {
		printf("failed to seek to the end\n");
	}

	cfs_seek(fd, 0, CFS_SEEK_SET);*/



	/*fd_read = cfs_open(filename, CFS_READ);
	if (fd_read < 0) {
		printf("ERROR: Cannot open file\n");
	}

	i = cfs_read(fd_read, auxBuf, sizeof(auxBuf));
	if (i < 0) {
		printf("ERROR: Cannot read file\n");
		cfs_close(fd_read);
	} else {
		printf("%d bytes read\n", i);
	}

	printf("%s\n", auxBuf);*/



	if (!deluge_disseminate(filename, node_id == SINK_ID))
	{
		printf("Disseminating %s\n", filename);
	} else {

	}

	etimer_set(&et, CLOCK_SECOND * 180);
	while (true) {
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		if(node_id != SINK_ID) {
			fd_write = cfs_open(filename, CFS_READ);
			if(fd_write < 0) {
				printf("failed to open %s file\n", filename);
			} else {
				i = cfs_read(fd_write, buf, sizeof(buf));
				buf[sizeof(buf) - 1] = '\0';
				if(i <= 0) {
					printf("failed to read data from the file\n");
				} else {
					printf("File contents: \n%s\n", buf);
				}
				cfs_close(fd_write);
			}
		}
		etimer_reset(&et);
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
