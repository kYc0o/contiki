#include "contiki.h"
//#include "dev/serial-line.h"
#include "sys/node-id.h"
//#include "sys/etimer.h"
#include <stdbool.h>
//#include "deluge-udp.h"
#include <stdio.h>

#include "cfs/cfs-coffee.h"

#ifndef SINK_ID
#define SINK_ID 1
#endif

#ifndef PLAIN_FILE_SIZE
#define PLAIN_FILE_SIZE 2817
#endif

#ifndef COMPRESSED_FILE_SIZE
#define COMPRESSED_FILE_SIZE 1484
#endif

static const char *DEFAULTMODEL = "{\"eClass\" : \"org.kevoree.ContainerRoot\",\"generated_KMF_ID\" : \"BXX5q3eV\",\"nodes\" : [{\"eClass\" : \"org.kevoree.ContainerNode\",\"name\" : \"node0\",\"metaData\" : \"\",\"started\" : \"1\",\"components\" : [],\"hosts\" : [],\"host\" : [],\"groups\" : [\"groups[group0]\"],\"networkInformation\" : [{\"eClass\" : \"org.kevoree.NetworkInfo\",\"name\" : \"ip\",\"values\" : [{\"eClass\" : \"org.kevoree.NetworkProperty\",\"name\" : \"front\",\"value\" : \"m3-XX.lille.iotlab.info\"},{\"eClass\" : \"org.kevoree.NetworkProperty\",\"name\" : \"local\",\"value\" : \"fe80:0000:0000:0000:0323:4501:4471:0343\"}]}],\"typeDefinition\" : [\"typeDefinitions[ContikiNode/0.0.1]\"],\"dictionary\" : [],\"fragmentDictionary\" : []}],\"typeDefinitions\" : [{\"eClass\" : \"org.kevoree.NodeType\",\"name\" : \"ContikiNode\",\"version\" : \"0.0.1\",\"factoryBean\" : \"\",\"bean\" : \"\",\"abstract\" : \"0\",\"deployUnit\" : [\"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]\"],\"dictionaryType\" : [{\"eClass\" : \"org.kevoree.DictionaryType\",\"generated_KMF_ID\" : \"o8AVQY3e\",\"attributes\" : []}],\"superTypes\" : []},{\"eClass\" : \"org.kevoree.GroupType\",\"name\" : \"CoAPGroup\",\"version\" : \"0.0.1\",\"factoryBean\" : \"\",\"bean\" : \"\",\"abstract\" : \"0\",\"deployUnit\" : [\"deployUnits[//kevoree-group-coap/0.0.1]\"],\"dictionaryType\" : [{\"eClass\" : \"org.kevoree.DictionaryType\",\"generated_KMF_ID\" : \"3dddTFpd\",\"attributes\" : [{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"proxy_port\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"int\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"20000\"},{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"port\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"number\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"\"},{\"eClass\" : \"org.kevoree.DictionaryAttribute\",\"name\" : \"path\",\"optional\" : \"1\",\"state\" : \"0\",\"datatype\" : \"string\",\"fragmentDependant\" : \"1\",\"defaultValue\" : \"\"}]}],\"superTypes\" : []}],\"repositories\" : [],\"dataTypes\" : [],\"libraries\" : [{\"eClass\" : \"org.kevoree.TypeLibrary\",\"name\" : \"ContikiLib\",\"subTypes\" : [\"typeDefinitions[ContikiNode/0.0.1]\",\"typeDefinitions[CoAPGroup/0.0.1]\"]},{\"eClass\" : \"org.kevoree.TypeLibrary\",\"name\" : \"Default\",\"subTypes\" : []}],\"hubs\" : [],\"mBindings\" : [],\"deployUnits\" : [{\"eClass\" : \"org.kevoree.DeployUnit\",\"name\" : \"kevoree-group-coap\",\"groupName\" : \"\",\"version\" : \"0.0.1\",\"url\" : \"\",\"hashcode\" : \"\",\"type\" : \"ce\"},{\"eClass\" : \"org.kevoree.DeployUnit\",\"name\" : \"kevoree-contiki-node\",\"groupName\" : \"org.kevoree.library.c\",\"version\" : \"0.0.1\",\"url\" : \"\",\"hashcode\" : \"\",\"type\" : \"ce\"}],\"nodeNetworks\" : [],\"groups\" : [{\"eClass\" : \"org.kevoree.Group\",\"name\" : \"group0\",\"metaData\" : \"\",\"started\" : \"1\",\"subNodes\" : [\"nodes[node0]\"],\"typeDefinition\" : [\"typeDefinitions[CoAPGroup/0.0.1]\"],\"dictionary\" : [],\"fragmentDictionary\" : [{\"eClass\" : \"org.kevoree.FragmentDictionary\",\"generated_KMF_ID\" : \"VEj2RlNr\",\"name\" : \"contiki-node\",\"values\" : []}]}]}";

unsigned short node_id = 1;

PROCESS(compression, "Model compression");
AUTOSTART_PROCESSES(&compression);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(compression, ev, data)
{
  char plain_buffer[PLAIN_FILE_SIZE];
  char model_buffer[COMPRESSED_FILE_SIZE];
  static uint32_t fdFile;
  static struct etimer et;
  static struct cfs_dirent dirent;
  static struct cfs_dir dir;
  static char *filename;
  char * f_plain_name = "model.json";
  char * f_name = "model-comp.json";
  int fd, s;
  PROCESS_BEGIN();


  printf("node_id: %d\n", node_id);

  //printf("Formatting\n");
  //fdFile = cfs_coffee_format();
  //printf("Formatted with result %ld\n", fdFile);

  cfs_remove(f_name);
  memset(model_buffer, 0, sizeof(model_buffer));

  if(node_id == SINK_ID) {

    // write default model to disk
    memset(plain_buffer, 0, sizeof(plain_buffer));
    strcpy(plain_buffer, DEFAULTMODEL);
    fd = cfs_open(f_plain_name, CFS_WRITE);
    if(fd < 0) {
      printf("going to exit now\n");
      process_exit(NULL);
    }
    if((s = cfs_write(fd, plain_buffer, sizeof(plain_buffer))) != sizeof(plain_buffer)) {
      cfs_close(fd);
      printf("ERROR: File written incorrectly\n");
      process_exit(NULL);
    } else {
      printf("Written %d bytes\n", s);
    }
    cfs_close(fd);

    // compress model
    printf("Compressing %s to %s\n", f_plain_name, f_name);
    int size = compress(f_plain_name, f_name);
    printf("done compressing\n");
    printf("compressed size is %d\n", size);

    // print compressed file
    fd = cfs_open(f_name, CFS_READ);
    s = cfs_read(fd, model_buffer, sizeof(model_buffer));
    model_buffer[sizeof(model_buffer) - 1] = '\0';
    if(s <= 0) {
      printf("failed to read data from the file\n");
    } else {
      printf("File contents: \n%s\n", model_buffer);
    }
    cfs_close(fd);

    // list root directory
    if(cfs_opendir(&dir, ".") == 0) {
      while(cfs_readdir(&dir, &dirent) != -1) {
        printf("File: %s (%ld bytes)\n",
            dirent.name, (long)dirent.size);
      }
      cfs_closedir(&dir);
    }

    // remove uncompressed model
    cfs_remove(f_plain_name);
  } else {
    memset(model_buffer, 'x', sizeof(model_buffer));
    fd = cfs_open(f_name, CFS_WRITE);
    if(fd < 0) {
      printf("going to exit now\n");
      process_exit(NULL);
    }
    if((s = cfs_write(fd, model_buffer, sizeof(model_buffer))) != sizeof(model_buffer)) {
      cfs_close(fd);
      printf("ERROR: File written incorrectly\n");
      process_exit(NULL);
    } else {
      printf("Written %d bytes\n", s);
    }
    cfs_close(fd);

  }


  if (!deluge_disseminate(f_name, (node_id == SINK_ID ? 1 : 0), NULL))
  {
    printf("Disseminating %s\n", f_name);
  } else {
    printf("Error disseminating\n");
  }

  etimer_set(&et, CLOCK_SECOND*30);

  while(1) {
    PROCESS_WAIT_EVENT();

    if (ev == PROCESS_EVENT_TIMER) {
      if(node_id != SINK_ID) {
        fd = cfs_open(f_name, CFS_READ);
        if(fd < 0) {
          printf("failed to open %s file\n", f_name);
        } else {
          s = cfs_read(fd, model_buffer, sizeof(model_buffer));
          model_buffer[sizeof(model_buffer) - 1] = '\0';
          if(s <= 0) {
            printf("failed to read data from the file\n");
          } else {
            printf("File contents: \n%s\n", model_buffer);
          }
          cfs_close(fd);
        }
      }
      etimer_restart(&et);
    }

  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
