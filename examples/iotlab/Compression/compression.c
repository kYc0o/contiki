#include "contiki.h"
#include "dev/serial-line.h"
#include <stdbool.h>
#include <stdio.h>

#include "cfs/cfs-coffee.h"


/*
 * Compress an hard-coded model
 */
#define ALPHABET_SIZE 196

#define COMPRESS 0
#define DECOMPRESS 1

//TODO remove extra space and new line characters
const char *const code_attr[ALPHABET_SIZE][2] ={{"eClass","a"},{"name","aa"},{"values","ab"},{"generated_KMF_ID","ac"},{"value","ad"},{"false","ae"},{"true","af"},{"started","ag"},{"metaData","ah"},{"typeDefinition","ai"},{"dictionary","aj"},{"fragmentDictionary","ak"},{"org.kevoree.Dictionary","al"},{"org.kevoree.DictionaryValue","am"},{"provided","an"},{"required","ao"},{"port","ap"},{"1234","aq"},{"version","ar"},{"0.0.1","as"},{"groups","at"},{"org.kevoree.NetworkProperty","au"},{"org.kevoree.ContainerNode","av"},{"hosts","aw"},{"host","ax"},{"groups[group0]","ay"},{"components","az"},{"networkInformation","a0"},{"org.kevoree.NetworkInfo","a1"},{"org.kevoree.ComponentInstance","a2"},{"namespace","a3"},{"org.kevoree.FragmentDictionary","a4"},{"local","a5"},{"interval","a6"},{"url","a7"},{"1000","a8"},{"Inti","a9"},{"abstract","b"},{"bean","ba"},{"factoryBean","bb"},{"deployUnit","bc"},{"superTypes","bd"},{"dictionaryType","be"},{"org.kevoree.DictionaryAttribute","bf"},{"fragmentDependant","bg"},{"optional","bh"},{"state","bi"},{"datatype","bj"},{"defaultValue","bk"},{"org.kevoree.DeployUnit","bl"},{"groupName","bm"},{"hashcode","bn"},{"type","bo"},{"genericTypes","bp"},{"requiredLibs","bq"}};

static const char *DEFAULTMODEL = "{\"eClass\":\"org.kevoree.ContainerRoot\",\"generated_KMF_ID\":\"CtHbJw37\",\"nodes\":[{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n1759\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":803,\\\"y\\\":52}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.44028098252601921432122800473\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"bk1759\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":922,\\\"y\\\":115}\",\"typeDefinition\":[\"typeDefinitions[blink/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.244668840663507581432123271370\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::1759\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"nb772\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":653,\\\"y\\\":51}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.280824760207906371432123339315\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hwb772\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":761,\\\"y\\\":118}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.125406553270295261432123361237\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::b772\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n2151\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":499,\\\"y\\\":50}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.56672404310666021432123659775\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"bk2151\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":576,\\\"y\\\":113}\",\"typeDefinition\":[\"typeDefinitions[blink/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.96605161810293791432123715642\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"lo\",\"value\":\"fe80::2151\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"nb870\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":354,\\\"y\\\":153}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.133597850101068621432123764697\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hwb870\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":498,\\\"y\\\":217}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.73711027111858131432123794332\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::b870\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n1459\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":354,\\\"y\\\":252}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.70933706359937791432123833544\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"lx1459\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":474,\\\"y\\\":307}\",\"typeDefinition\":[\"typeDefinitions[sensing/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.70674828044138851432123848469\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::1459\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"nb771\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":353,\\\"y\\\":353}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.85399847291409971432123883565\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hwb771\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":448,\\\"y\\\":419}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.93829955207183961432123899340\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::b771\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n3554\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":354,\\\"y\\\":454}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.90601870790123941432123946266\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"lx3554\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":475,\\\"y\\\":520}\",\"typeDefinition\":[\"typeDefinitions[sensing/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.530610746005551432124082140\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::3554\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n9989\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":354,\\\"y\\\":553}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.40597744332626461432124065761\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hw9989\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":450,\\\"y\\\":611}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.93384177354164421432124102077\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::9989\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n2251\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1128,\\\"y\\\":55}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.103688640287145971432125140977\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hw2251\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1322,\\\"y\\\":132}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.478929068660363551432125174141\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::2251\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n1559\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":957,\\\"y\\\":54}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.82558026234619321432125237187\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hw1559\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1048,\\\"y\\\":117}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.25617986987344921432125299046\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::1559\"}]}]}],\"typeDefinitions\":[{\"eClass\":\"org.kevoree.NodeType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"ContikiNode\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[]},{\"eClass\":\"org.kevoree.GroupType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"UDPGroup\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[//kevoree-group-udp/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"3dddTFpd\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"port\",\"state\":\"true\",\"datatype\":\"int\",\"defaultValue\":\"1234\",\"genericTypes\":[]}]}]},{\"eClass\":\"org.kevoree.ComponentType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"blink\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[kev_contiki//blink/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"0qE5TygS\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"interval\",\"state\":\"false\",\"datatype\":\"int\",\"defaultValue\":\"1000\",\"genericTypes\":[]}]}],\"required\":[],\"provided\":[]},{\"eClass\":\"org.kevoree.ComponentType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"sensing\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[kev_contiki//sensing/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"oW3cS6Ts\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"interval\",\"state\":\"false\",\"datatype\":\"int\",\"defaultValue\":\"1000\",\"genericTypes\":[]}]}],\"required\":[],\"provided\":[]},{\"eClass\":\"org.kevoree.ComponentType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"hello_world\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[kev_contiki//hello_world/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"3dddTFpd\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"name\",\"state\":\"false\",\"datatype\":\"string\",\"defaultValue\":\"Inti\",\"genericTypes\":[]}]}],\"required\":[],\"provided\":[]},{\"eClass\":\"org.kevoree.ComponentType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"sieve\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[kev_contiki//sieve/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"afOM93SD\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"interval\",\"state\":\"false\",\"datatype\":\"int\",\"defaultValue\":\"18000\",\"genericTypes\":[]},{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"count\",\"state\":\"false\",\"datatype\":\"int\",\"defaultValue\":\"100\",\"genericTypes\":[]}]}],\"required\":[],\"provided\":[]}],\"repositories\":[{\"eClass\":\"org.kevoree.Repository\",\"url\":\"[aaaa::1]:1234\"}],\"dataTypes\":[],\"libraries\":[{\"eClass\":\"org.kevoree.TypeLibrary\",\"name\":\"ContikiLib\",\"subTypes\":[\"typeDefinitions[ContikiNode/0.0.1]\",\"typeDefinitions[UDPGroup/0.0.1]\",\"typeDefinitions[blink/0.0.1]\",\"typeDefinitions[sensing/0.0.1]\",\"typeDefinitions[hello_world/0.0.1]\",\"typeDefinitions[sieve/0.0.1]\"]},{\"eClass\":\"org.kevoree.TypeLibrary\",\"name\":\"Default\",\"subTypes\":[]}],\"hubs\":[],\"mBindings\":[],\"deployUnits\":[{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"\",\"name\":\"kevoree-group-udp\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"org.kevoree.library.c\",\"name\":\"kevoree-contiki-node\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"kev_contiki\",\"name\":\"blink\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"kev_contiki\",\"name\":\"sensing\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"kev_contiki\",\"name\":\"hello_world\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"kev_contiki\",\"name\":\"sieve\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]}],\"nodeNetworks\":[],\"groups\":[{\"eClass\":\"org.kevoree.Group\",\"name\":\"group0\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":897,\\\"y\\\":397}\",\"typeDefinition\":[\"typeDefinitions[UDPGroup/0.0.1]\"],\"subNodes\":[\"nodes[n1759]\",\"nodes[nb772]\",\"nodes[n2151]\",\"nodes[nb870]\",\"nodes[n1459]\",\"nodes[nb771]\",\"nodes[n3554]\",\"nodes[n9989]\",\"nodes[n2251]\",\"nodes[n1559]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.26062655518762771432122795371\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]}],\"fragmentDictionary\":[{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n1759\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"nb772\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n2151\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"nb870\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n1459\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"nb771\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n3554\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n9989\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n2251\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n1559\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]}]}]}";

PROCESS(compression, "Compression");
AUTOSTART_PROCESSES(&compression);

const char* getSubsituteCode(char* code)
{
  int i;
  for (i=0; i<ALPHABET_SIZE; i++)
  {
    if (strcmp(code, code_attr[i][0]) == 0) {
      return code_attr[i][1];
    }
  }
  return code;
}

const char* getAttributeCode(char* code)
{
  int i;
  for (i=0; i<ALPHABET_SIZE; i++)
  {
    if (strcmp(code, code_attr[i][1]) == 0) {
      return code_attr[i][0];
    }
  }
  return code;
}


/**
 * in, out: CFS file descriptor
 * mode: COMPRESS or DECOMPRESS
 */
void substitutionCode(int in, int out, int mode)
{
  static uint8_t buf[100];

  int n, j; //buffer indexes
  const char* code;
  size_t size;

  char attr[100];
  int attr_index = 0;
  bool in_attribute = false;

  while ((n = cfs_read(in, buf, 60)) >0)
  {
    for (j=0; j<n; j++) {

      // begining of an attribute name
      if (buf[j] =='"' && !in_attribute)
      {
	in_attribute = true;
      }
      // end of an attribute name
      else if (buf[j] == '"' && in_attribute)
      {
	in_attribute = false;
	attr[attr_index] = '\0';
	attr_index = 0; //reset attribute attr_index
	if (mode == COMPRESS) {
	  code = getSubsituteCode(attr);
	}
	else if (mode == DECOMPRESS) {
	  code = getAttributeCode(attr);
	}
	size = strlen(code);
	char tmp_buf[2 + size];
	sprintf(tmp_buf, "\"%s\"", code);
	cfs_write(out, tmp_buf, strlen(tmp_buf));
      }
      // inside an attribute
      else if (buf[j] != '"' && in_attribute)
      {
	attr[attr_index] = buf[j];
	attr_index++;
      }
      else
      {
	char c[1];
	sprintf(c, "%c", buf[j]); //avoid encoding error
	cfs_write(out, c, sizeof(c));
      }

    }
  }
}

void compress(char* input, char* output)
{
  int in_file = cfs_open(input, CFS_READ);
  int out_file = cfs_open(output, CFS_WRITE);

  if (in_file >= 0 && out_file >= 0) {
    substitutionCode(in_file, out_file, COMPRESS);
  }

  if (in_file >= 0)
    cfs_close(in_file);
  if (out_file >= 0)
    cfs_close(out_file);
}

void decompress(char* input, char* output)
{
  int in_file = cfs_open(input, CFS_READ);
  int out_file = cfs_open(output, CFS_WRITE);

  if (in_file >= 0 && out_file >= 0)
    substitutionCode(in_file, out_file, DECOMPRESS);

  if (in_file >= 0)
    cfs_close(in_file);
  if (out_file >= 0)
    cfs_close(out_file);
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(compression, ev, data)
{
  static uint8_t buf[257];
  static struct cfs_dirent dirent;
  static struct cfs_dir dir;
  static uint32_t fdFile;
  static char *filename;
  char* in;
  char* out;
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
      else if (!strcmp(data, "write")) {
	int fd_model = cfs_open("model.json", CFS_WRITE);
	cfs_write(fd_model, DEFAULTMODEL, strlen(DEFAULTMODEL));
	cfs_close(fd_model);
      }
      else if (strstr(data, "compress") == data) {
	in = "model.json";
	out = "model.json-compressed";
	printf("Compressing '%s' to '%s'\n", in, out);
	compress(in, out);
	printf("Done compressing!\n");
      }
      else if (strstr(data, "decompress") == data) {
	in = "model.json-compressed";
	out = "model.json-decompressed";
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
