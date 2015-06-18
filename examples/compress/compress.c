#include "contiki.h"
#include "sys/node-id.h"
#include <stdbool.h>
#include <stdio.h>

#include "cfs/cfs-coffee.h"

#ifndef PLAIN_FILE_SIZE
#define PLAIN_FILE_SIZE 2375
#endif

#ifndef COMPRESSED_FILE_SIZE
#define COMPRESSED_FILE_SIZE 869
#endif

static const char *DEFAULTMODEL = "{\"eClass\":\"org.kevoree.ContainerRoot\",\"generated_KMF_ID\":\"CtHbJw37\",\"nodes\":[{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n3554\",\"started\":\"true\",\"metaData\":\"\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[],\"fragmentDictionary\":[],\"components\":[],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"lo\",\"value\":\"fe80::3554\"}]}]}],\"typeDefinitions\":[{\"eClass\":\"org.kevoree.NodeType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"ContikiNode\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[]},{\"eClass\":\"org.kevoree.GroupType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"UDPGroup\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[//kevoree-group-udp/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"3dddTFpd\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"port\",\"state\":\"true\",\"datatype\":\"int\",\"defaultValue\":\"1234\",\"genericTypes\":[]}]}]}],\"repositories\":[{\"eClass\":\"org.kevoree.Repository\",\"url\":\"[aaaa::1]:1234\"}],\"dataTypes\":[],\"libraries\":[{\"eClass\":\"org.kevoree.TypeLibrary\",\"name\":\"ContikiLib\",\"subTypes\":[\"typeDefinitions[ContikiNode/0.0.1]\",\"typeDefinitions[UDPGroup/0.0.1]\"]}],\"hubs\":[],\"mBindings\":[],\"deployUnits\":[{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"\",\"name\":\"kevoree-group-udp\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"org.kevoree.library.c\",\"name\":\"kevoree-contiki-node\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]}],\"nodeNetworks\":[],\"groups\":[{\"eClass\":\"org.kevoree.Group\",\"name\":\"group0\",\"started\":\"true\",\"metaData\":\"\",\"typeDefinition\":[\"typeDefinitions[UDPGroup/0.0.1]\"],\"subNodes\":[\"nodes[n3554]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.26062655518762771432122795371\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]}],\"fragmentDictionary\":[{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n3554\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]}]}]}\0";

char * f_plain_name = "model.json";
char * f_name = "model-comp.json";
char plain_buffer[PLAIN_FILE_SIZE];
char model_buffer[COMPRESSED_FILE_SIZE];
static uint32_t fdFile;
int fd, s;
static struct cfs_dirent dirent;
static struct cfs_dir dir;

// list root directory
static void listDir()
{
  if(cfs_opendir(&dir, ".") == 0) {
    while(cfs_readdir(&dir, &dirent) != -1) {
      printf("File: %s (%ld bytes)\n", dirent.name, (long)dirent.size);
    }
    cfs_closedir(&dir);
  }
}

PROCESS(compression, "Model compression");
AUTOSTART_PROCESSES(&compression);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(compression, ev, data)
{
  //static struct etimer et;
  PROCESS_BEGIN();

  //printf("Formatting\n");
  //fdFile = cfs_coffee_format();
  //printf("Formatted with result %ld\n", fdFile);

  cfs_remove(f_name);

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

  // print decompressed file
  fd = cfs_open(f_plain_name, CFS_READ);
  s = cfs_read(fd, plain_buffer, sizeof(plain_buffer));
  if(s <= 0) {
    printf("failed to read data from the file\n");
  } else {
    printf("Decompressed file contents: \n%s\n", plain_buffer);
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
  if(s <= 0) {
    printf("failed to read data from the file\n");
  } else {
    printf("Compressed file contents: \n%s\n", model_buffer);
  }
  cfs_close(fd);

  // remove uncompressed model
  cfs_remove(f_plain_name);

  listDir();

  // decompress file
  printf("Decompressing %s to %s\n", f_name, f_plain_name);
  size = decompress(f_name, f_plain_name);
  printf("done decompressing\n");
  printf("decompressed size is %d\n", size);

  listDir();

  // print decompressed file
  fd = cfs_open(f_plain_name, CFS_READ);
  s = cfs_read(fd, plain_buffer, sizeof(plain_buffer));
  if(s <= 0) {
    printf("failed to read data from the file\n");
  } else {
    printf("Decompressed file contents: \n%s\n", plain_buffer);
  }
  cfs_close(fd);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
