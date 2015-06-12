#include "contiki.h"
#include "dev/serial-line.h"
#include "sys/node-id.h"
#include <stdbool.h>
#include "deluge-udp.h"
#include <stdio.h>

#include "cfs/cfs-coffee.h"

#ifndef SINK_ID
#define SINK_ID 1
#endif

#ifndef PLAIN_FILE_SIZE
#define PLAIN_FILE_SIZE 2817
#endif

#ifndef COMPRESSED_FILE_SIZE
#define COMPRESSED_FILE_SIZE 1713
#endif

static const char *DEFAULTMODEL = "{\"eClass\" : \"org.kevoree.ContainerRoot\",\"generated_KMF_ID\" : \"BXX5q3eV\",\"nodes\" : [{\"eClass\" : \"org.kevoree.ContainerNode\",\"name\" : \"node0\",\"metaData\" : \"\",\"started\" : \"1\",\"components\" : [],\"hosts\" : [],\"host\" : [],\"groups\" : [\"groups[group0]\"],\"networkInformation\" : [{\"eClass\" : \"org.kevoree.NetworkInfo\",\"name\" : \"ip\",\"values\" : [{\"eClass\" : \"org.kevoree.NetworkProperty\",\"name\" : \"front\",\"value\" : \"m3-XX.lille.iotlab.info\"},{\"eClass\" : \"org.kevoree.NetworkProperty\",\"name\" : \"local\",\"value\" : \"fe80:0000:0000:0000:0323:4501:4471:0343\"}]}],\"typeDefinition\" : [\"typeDefinitions[ContikiNode/0.0.1]\"],\"dictionary\" : [],\"fragmentDictionary\" : []}],\"typeDefinitions\" : [{\"eClass\" : \"org.kevoree.NodeType\",\"name\" : \"ContikiNode\",\"version\" : \"0.0.1\",\"factoryBean\" : \"\",\"bean\" : \"\",\"abstract\" : \"0\",\"deployUnit\" : [\"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]\"],\"dictionaryType\" : [{\"eClass\" : \"org.kevoree.DictionaryType\",\"generated_KMF_ID\" : \"o8AVQY3e\",\"attributes\" : []}],\"superTypes\" : []},{\"eClass\" : \"org.kevoree.GroupType\",\"name\" : \"CoAPGroup\",\"version\" : \"0.0.1\",\"factoryBean\" : \"\",\"bean\" : \"\",\"abstract\" : \"0\",\"deployUnit\" : [\"deployUnits[//kevoree-group-coap/0.0.1]\"],\"dictionaryType\" : [{\"eClass\" : \"org.kevoree.DictionaryType\",\"generated_KMF_ID\" : \"3dddTFpd\",\"attributes\" : [{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"proxy_port\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"int\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"20000\"},{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"port\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"number\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"\"},{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"path\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"string\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"\"}]}],\"superTypes\" : []}],\"repositories\" : [],\"dataTypes\" : [],\"libraries\" : [{\"eClass\" : \"org.kevoree.TypeLibrary\",\"name\" : \"ContikiLib\",\"subTypes\" : [\"typeDefinitions[ContikiNode/0.0.1]\",\"typeDefinitions[CoAPGroup/0.0.1]\"]},{\"eClass\" : \"org.kevoree.TypeLibrary\",\"name\" : \"Default\",\"subTypes\" : []}],\"hubs\" : [],\"mBindings\" : [],\"deployUnits\" : [{\"eClass\" : \"org.kevoree.DeployUnit\",\"name\" : \"kevoree-group-coap\",\"groupName\" : \"\",\"version\" : \"0.0.1\",\"url\" : \"\",\"hashcode\" : \"\",\"type\" : \"ce\"},{\"eClass\" : \"org.kevoree.DeployUnit\",\"name\" : \"kevoree-contiki-node\",\"groupName\" : \"org.kevoree.library.c\",\"version\" : \"0.0.1\",\"url\" : \"\",\"hashcode\" : \"\",\"type\" : \"ce\"}],\"nodeNetworks\" : [],\"groups\" : [{\"eClass\" : \"org.kevoree.Group\",\"name\" : \"group0\",\"metaData\" : \"\",\"started\" : \"1\",\"subNodes\" : [\"nodes[node0]\"],\"typeDefinition\" : [\"typeDefinitions[CoAPGroup/0.0.1]\"],\"dictionary\" : [],\"fragmentDictionary\" : [{\"eClass\" : \"org.kevoree.FragmentDictionary\",\"generated_KMF_ID\" : \"VEj2RlNr\",\"name\" : \"contiki-node\",\"values\" : []}]}]}";

unsigned short node_id = 1;

PROCESS(compression, "kev-compress");
AUTOSTART_PROCESSES(&compression);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(compression, ev, data)
{
  static uint8_t buf[257];
  char model_buffer[PLAIN_FILE_SIZE];
  static struct cfs_dirent dirent;
  static struct cfs_dir dir;
  static uint32_t fdFile;
  static char *filename;
  char* in;
  char* out;
  char * f_name = "m";
  int fd, s;
  PROCESS_BEGIN();

  while(1) {
    PROCESS_YIELD();
    if (ev == serial_line_event_message) {
      if (!strcmp(data, "ls")) {
	if(cfs_opendir(&dir, ".") == 0) {
	  while(cfs_readdir(&dir, &dirent) != -1) {
	    printf("File: %s (%ld bytes)\n",
		dirent.name, (long)dirent.size);
	  }
	  cfs_closedir(&dir);
	}
      }
      else if (strstr(data, "cat") == data) {
	int n, jj;
	char* tmp = strstr(data, " ");
	tmp++;
	fdFile = cfs_open(tmp, CFS_READ);
	if (fdFile < 0) printf("error opening the file %s\n", tmp);
	while ((n = cfs_read(fdFile, buf, 60)) > 0) {
	  for (jj = 0 ; jj < n ; jj++) printf("%c", (char)buf[jj]);
	}
	printf("\n");
	cfs_close(fdFile);
	if (n!=0)
	  printf("Some error reading the file\n");

      }
      else if (strstr(data, "rm") == data) {
	char* tmp = strstr(data, " ");
	tmp++;
	cfs_remove(tmp);
      }
      else if (strstr(data, "format") == data) {
	printf("Formatting\n");
	printf("It takes around 3 minutes\n");
	printf("...\n");
	fdFile = cfs_coffee_format();
	printf("Formatted with result %ld\n", fdFile);
      }
      else if (!strcmp(data, "write")) {
	int fd_model = cfs_open("m", CFS_WRITE);
	cfs_write(fd_model, DEFAULTMODEL, strlen(DEFAULTMODEL));
	cfs_close(fd_model);
      }
      else if (strstr(data, "compress") == data) {
	in = "m";
	out = "c";
	printf("Compressing '%s' to '%s'\n", in, out);
	compress(in, out);
	printf("Done compressing!\n");
      }
      else if (strstr(data, "decompress") == data) {
	in = "c";
	out = "d";
	printf("Decompressing '%s' to '%s'\n", in, out);
	decompress(in, out);
	printf("Done decompressing!\n");
      }
      else
      {
	printf("Unknown command.\n");
      }
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
