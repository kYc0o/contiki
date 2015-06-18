#include "contiki.h"
#include "sys/node-id.h"
#include <stdbool.h>
#include <stdio.h>

#include "cfs/cfs-coffee.h"

#ifndef SINK_ID
#define SINK_ID 1
#endif

#ifndef PLAIN_FILE_SIZE
//#define PLAIN_FILE_SIZE 16042
#define PLAIN_FILE_SIZE 2374
#endif

#ifndef COMPRESSED_FILE_SIZE
//#define COMPRESSED_FILE_SIZE 5909
#define COMPRESSED_FILE_SIZE 869
#endif

//static const char *DEFAULTMODEL = "{\"eClass\":\"org.kevoree.ContainerRoot\",\"generated_KMF_ID\":\"CtHbJw37\",\"nodes\":[{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n1759\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":803,\\\"y\\\":52}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.44028098252601921432122800473\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"bk1759\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":922,\\\"y\\\":115}\",\"typeDefinition\":[\"typeDefinitions[blink/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.244668840663507581432123271370\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::1759\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"nb772\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":653,\\\"y\\\":51}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.280824760207906371432123339315\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hwb772\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":761,\\\"y\\\":118}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.125406553270295261432123361237\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::b772\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n2151\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":499,\\\"y\\\":50}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.56672404310666021432123659775\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"bk2151\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":576,\\\"y\\\":113}\",\"typeDefinition\":[\"typeDefinitions[blink/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.96605161810293791432123715642\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"lo\",\"value\":\"fe80::2151\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"nb870\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":354,\\\"y\\\":153}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.133597850101068621432123764697\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hwb870\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":498,\\\"y\\\":217}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.73711027111858131432123794332\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::b870\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n1459\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":354,\\\"y\\\":252}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.70933706359937791432123833544\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"lx1459\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":474,\\\"y\\\":307}\",\"typeDefinition\":[\"typeDefinitions[sensing/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.70674828044138851432123848469\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::1459\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"nb771\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":353,\\\"y\\\":353}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.85399847291409971432123883565\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hwb771\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":448,\\\"y\\\":419}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.93829955207183961432123899340\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::b771\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n3554\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":354,\\\"y\\\":454}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.90601870790123941432123946266\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"lx3554\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":475,\\\"y\\\":520}\",\"typeDefinition\":[\"typeDefinitions[sensing/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.530610746005551432124082140\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::3554\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n9989\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":354,\\\"y\\\":553}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.40597744332626461432124065761\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hw9989\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":450,\\\"y\\\":611}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.93384177354164421432124102077\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::9989\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n2251\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1128,\\\"y\\\":55}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.103688640287145971432125140977\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hw2251\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1322,\\\"y\\\":132}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.478929068660363551432125174141\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::2251\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n1559\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":957,\\\"y\\\":54}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.82558026234619321432125237187\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hw1559\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1048,\\\"y\\\":117}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.25617986987344921432125299046\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::1559\"}]}]}],\"typeDefinitions\":[{\"eClass\":\"org.kevoree.NodeType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"ContikiNode\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[]},{\"eClass\":\"org.kevoree.GroupType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"UDPGroup\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[//kevoree-group-udp/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"3dddTFpd\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"port\",\"state\":\"true\",\"datatype\":\"int\",\"defaultValue\":\"1234\",\"genericTypes\":[]}]}]},{\"eClass\":\"org.kevoree.ComponentType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"blink\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[kev_contiki//blink/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"0qE5TygS\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"interval\",\"state\":\"false\",\"datatype\":\"int\",\"defaultValue\":\"1000\",\"genericTypes\":[]}]}],\"required\":[],\"provided\":[]},{\"eClass\":\"org.kevoree.ComponentType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"sensing\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[kev_contiki//sensing/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"oW3cS6Ts\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"interval\",\"state\":\"false\",\"datatype\":\"int\",\"defaultValue\":\"1000\",\"genericTypes\":[]}]}],\"required\":[],\"provided\":[]},{\"eClass\":\"org.kevoree.ComponentType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"hello_world\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[kev_contiki//hello_world/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"3dddTFpd\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"name\",\"state\":\"false\",\"datatype\":\"string\",\"defaultValue\":\"Inti\",\"genericTypes\":[]}]}],\"required\":[],\"provided\":[]},{\"eClass\":\"org.kevoree.ComponentType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"sieve\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[kev_contiki//sieve/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"afOM93SD\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"interval\",\"state\":\"false\",\"datatype\":\"int\",\"defaultValue\":\"18000\",\"genericTypes\":[]},{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"count\",\"state\":\"false\",\"datatype\":\"int\",\"defaultValue\":\"100\",\"genericTypes\":[]}]}],\"required\":[],\"provided\":[]}],\"repositories\":[{\"eClass\":\"org.kevoree.Repository\",\"url\":\"[aaaa::1]:1234\"}],\"dataTypes\":[],\"libraries\":[{\"eClass\":\"org.kevoree.TypeLibrary\",\"name\":\"ContikiLib\",\"subTypes\":[\"typeDefinitions[ContikiNode/0.0.1]\",\"typeDefinitions[UDPGroup/0.0.1]\",\"typeDefinitions[blink/0.0.1]\",\"typeDefinitions[sensing/0.0.1]\",\"typeDefinitions[hello_world/0.0.1]\",\"typeDefinitions[sieve/0.0.1]\"]},{\"eClass\":\"org.kevoree.TypeLibrary\",\"name\":\"Default\",\"subTypes\":[]}],\"hubs\":[],\"mBindings\":[],\"deployUnits\":[{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"\",\"name\":\"kevoree-group-udp\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"org.kevoree.library.c\",\"name\":\"kevoree-contiki-node\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"kev_contiki\",\"name\":\"blink\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"kev_contiki\",\"name\":\"sensing\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"kev_contiki\",\"name\":\"hello_world\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"kev_contiki\",\"name\":\"sieve\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]}],\"nodeNetworks\":[],\"groups\":[{\"eClass\":\"org.kevoree.Group\",\"name\":\"group0\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":897,\\\"y\\\":397}\",\"typeDefinition\":[\"typeDefinitions[UDPGroup/0.0.1]\"],\"subNodes\":[\"nodes[n1759]\",\"nodes[nb772]\",\"nodes[n2151]\",\"nodes[nb870]\",\"nodes[n1459]\",\"nodes[nb771]\",\"nodes[n3554]\",\"nodes[n9989]\",\"nodes[n2251]\",\"nodes[n1559]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.26062655518762771432122795371\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]}],\"fragmentDictionary\":[{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n1759\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"nb772\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n2151\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"nb870\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n1459\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"nb771\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n3554\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n9989\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n2251\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n1559\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]}]}]}\0";

static const char *DEFAULTMODEL = "{\"eClass\":\"org.kevoree.ContainerRoot\",\"generated_KMF_ID\":\"CtHbJw37\",\"nodes\":[{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n3554\",\"started\":\"true\",\"metaData\":\"\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[],\"fragmentDictionary\":[],\"components\":[],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"lo\",\"value\":\"fe80::3554\"}]}]}],\"typeDefinitions\":[{\"eClass\":\"org.kevoree.NodeType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"ContikiNode\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[]},{\"eClass\":\"org.kevoree.GroupType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"UDPGroup\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[//kevoree-group-udp/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"3dddTFpd\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"port\",\"state\":\"true\",\"datatype\":\"int\",\"defaultValue\":\"1234\",\"genericTypes\":[]}]}]}],\"repositories\":[{\"eClass\":\"org.kevoree.Repository\",\"url\":\"[aaaa::1]:1234\"}],\"dataTypes\":[],\"libraries\":[{\"eClass\":\"org.kevoree.TypeLibrary\",\"name\":\"ContikiLib\",\"subTypes\":[\"typeDefinitions[ContikiNode/0.0.1]\",\"typeDefinitions[UDPGroup/0.0.1]\"]}],\"hubs\":[],\"mBindings\":[],\"deployUnits\":[{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"\",\"name\":\"kevoree-group-udp\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"org.kevoree.library.c\",\"name\":\"kevoree-contiki-node\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]}],\"nodeNetworks\":[],\"groups\":[{\"eClass\":\"org.kevoree.Group\",\"name\":\"group0\",\"started\":\"true\",\"metaData\":\"\",\"typeDefinition\":[\"typeDefinitions[UDPGroup/0.0.1]\"],\"subNodes\":[\"nodes[n3554]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.26062655518762771432122795371\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]}],\"fragmentDictionary\":[{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n3554\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]}]}]}";

unsigned short node_id = 2;
char * f_plain_name = "model.json";
char * f_name = "model-comp.json";
char plain_buffer[PLAIN_FILE_SIZE];
char model_buffer[COMPRESSED_FILE_SIZE];
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

static void modelDownloaded(unsigned version)
{
  printf("\n\nDownload of version %d is completed\n", version);
  cfs_remove(f_plain_name);
  //listDir();
  printf("\nDecompressing file %s to %s\n", f_name, f_plain_name);
  int size_decompress = decompress(f_name, f_plain_name);
  printf("Plain size is %d\n\n", size_decompress);
  //listDir();

  fd = cfs_open(f_name, CFS_READ);
  s = cfs_read(fd, model_buffer, sizeof(model_buffer));
  if(s <= 0) {
    printf("failed to read data from the file\n");
  } else {
    printf("Compressed file contents: \n%s\n", model_buffer);
  }
  cfs_close(fd);

  fd = cfs_open(f_plain_name, CFS_READ);
  s = cfs_read(fd, plain_buffer, sizeof(plain_buffer));
  if(s <= 0) {
    printf("failed to read data from the file\n");
  } else {
    printf("decompressed file contents: \n%s\n", plain_buffer);
  }
  cfs_close(fd);
  listDir();
}

PROCESS(compression, "Model compression");
AUTOSTART_PROCESSES(&compression);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(compression, ev, data)
{
  //static struct etimer et;
  PROCESS_BEGIN();


  printf("node_id: %d\n", node_id);

  //printf("Formatting\n");
  //fdFile = cfs_coffee_format();
  //printf("Formatted with result %ld\n", fdFile);

  cfs_remove(f_name);
  memset(model_buffer, '\0', sizeof(model_buffer));

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
    size = compress(f_name, f_plain_name);
    printf("done decompressing\n");
    printf("decompressed size is %d\n", size);

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
    cfs_seek(fd, 0, CFS_SEEK_SET);
    cfs_close(fd);

  }


  if (!deluge_disseminate(f_name, (node_id == SINK_ID ? 1 : 0), modelDownloaded))
  {
    printf("Disseminating %s\n", f_name);
  } else {
    printf("Error disseminating\n");
  }

  //etimer_set(&et, CLOCK_SECOND*30);

  //while(1) {
  //  PROCESS_WAIT_EVENT();

  //  if (ev == PROCESS_EVENT_TIMER) {
  //    if(node_id != SINK_ID) {
  //      fd = cfs_open(f_name, CFS_READ);
  //      if(fd < 0) {
  //        printf("failed to open %s file\n", f_name);
  //      } else {
  //        s = cfs_read(fd, model_buffer, sizeof(model_buffer));
  //        model_buffer[sizeof(model_buffer) - 1] = '\0';
  //        if(s <= 0) {
  //          printf("failed to read data from the file\n");
  //        } else {
  //          printf("File contents: \n%s\n", model_buffer);
  //        }
  //        cfs_close(fd);
  //      }
  //    }
  //    etimer_restart(&et);
  //  }

  //}
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
