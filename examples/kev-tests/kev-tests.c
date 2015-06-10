/*
 * kev-tests.c
 * This file is part of Kevoree-Contiki
 *
 * Copyright (C) 2015 - Francisco Acosta
 *
 * Kevoree-Contiki is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Kevoree-Contiki is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kevoree-Contiki. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file
 *         A very simple Contiki application showing how kevoree works
 * \author
 *         Francisco Acosta <fco.ja.ac@gmail.com>
 */

#include "contiki.h"
#include "lib/list.h"
#include "jsonparse.h"
#include "cfs/cfs.h"

#include "ContainerRoot.h"
#include "JSONModelLoader.h"
#include "TraceSequence.h"
#include "ModelCompare.h"
#include "AdaptationPrimitive.h"
#include "Planner.h"
#include "Visitor.h"


#include <stdio.h> /* For printf() */

static ContainerRoot *current_model = NULL;
static ContainerRoot *new_model = NULL;

/*
 * Initial model with one node and no components
 */
static const char *DEFAULTMODEL = "{\"eClass\":\"org.kevoree.ContainerRoot\",\"generated_KMF_ID\":\"BXX5q3eV\",\"nodes\":[{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n1759\",\"metaData\":\"\",\"started\":\"true\",\"components\":[],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::1759\"}]}],\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"dictionary\":[],\"fragmentDictionary\":[]}],\"typeDefinitions\":[{\"eClass\":\"org.kevoree.NodeType\",\"name\":\"ContikiNode\",\"version\":\"0.0.1\",\"factoryBean\":\"\",\"bean\":\"\",\"abstract\":\"0\",\"deployUnit\":[\"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]\"],\"dictionaryType\":[],\"superTypes\":[]},{\"eClass\":\"org.kevoree.GroupType\",\"name\":\"UDPGroup\",\"version\":\"0.0.1\",\"factoryBean\":\"\",\"bean\":\"\",\"abstract\":\"0\",\"deployUnit\":[\"deployUnits[//kevoree-group-udp/0.0.1]\"],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"3dddTFpd\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"name\":\"port\",\"optional\":\"false\",\"state\":\"true\",\"datatype\":\"int\",\"fragmentDependant\":\"false\",\"defaultValue\":\"1234\"}]}],\"superTypes\":[]}],\"repositories\":[],\"dataTypes\":[],\"libraries\":[{\"eClass\":\"org.kevoree.TypeLibrary\",\"name\":\"ContikiLib\",\"subTypes\":[\"typeDefinitions[ContikiNode/0.0.1]\",\"typeDefinitions[UDPGroup/0.0.1]\"]},{\"eClass\":\"org.kevoree.TypeLibrary\",\"name\":\"Default\",\"subTypes\":[]}],\"hubs\":[],\"mBindings\":[],\"deployUnits\":[{\"eClass\":\"org.kevoree.DeployUnit\",\"name\":\"kevoree-group-udp\",\"groupName\":\"\",\"version\":\"0.0.1\",\"url\":\"\",\"hashcode\":\"\",\"type\":\"ce\"},{\"eClass\":\"org.kevoree.DeployUnit\",\"name\":\"kevoree-contiki-node\",\"groupName\":\"org.kevoree.library.c\",\"version\":\"0.0.1\",\"url\":\"\",\"hashcode\":\"\",\"type\":\"ce\"}],\"nodeNetworks\":[],\"groups\":[{\"eClass\":\"org.kevoree.Group\",\"name\":\"group0\",\"metaData\":\"\",\"started\":\"true\",\"subNodes\":[\"nodes[n1759]\"],\"typeDefinition\":[\"typeDefinitions[UDPGroup/0.0.1]\"],\"dictionary\":[],\"fragmentDictionary\":[]}]}";

/*
 * Proposed new model with 7 nodes one component each
 */
static const char *NEWMODEL = "{\"eClass\":\"org.kevoree.ContainerRoot\",\"generated_KMF_ID\":\"CtHbJw37\",\"nodes\":[{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n1759\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":803,\\\"y\\\":52}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.44028098252601921432122800473\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"bk1759\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":922,\\\"y\\\":115}\",\"typeDefinition\":[\"typeDefinitions[blink/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.244668840663507581432123271370\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::1759\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n2151\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":499,\\\"y\\\":50}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.56672404310666021432123659775\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"bk2151\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":576,\\\"y\\\":113}\",\"typeDefinition\":[\"typeDefinitions[blink/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.96605161810293791432123715642\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"lo\",\"value\":\"fe80::2151\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"nb771\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":353,\\\"y\\\":353}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.85399847291409971432123883565\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hwb771\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":448,\\\"y\\\":419}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.93829955207183961432123899340\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::b771\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n9989\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":354,\\\"y\\\":553}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.40597744332626461432124065761\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hw9989\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":450,\\\"y\\\":611}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.93384177354164421432124102077\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::9989\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n2459\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":494,\\\"y\\\":654}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.0915601672604681432124195820\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"bk2459\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":645,\\\"y\\\":722}\",\"typeDefinition\":[\"typeDefinitions[blink/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.39016547030769291432124216946\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::2459\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n2650\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":655,\\\"y\\\":653}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.53797553176991641432124283059\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hw2650\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":794,\\\"y\\\":712}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.39251068630255761432124317104\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::2650\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n9073\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":817,\\\"y\\\":653}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.55784740601666271432124376455\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"bk9073\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":943,\\\"y\\\":718}\",\"typeDefinition\":[\"typeDefinitions[blink/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.65135152544826271432124390045\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::9073\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n8773\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":982,\\\"y\\\":655}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.98144014482386411432124448068\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hw8773\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1118,\\\"y\\\":716}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.68661252455785871432124536416\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::8773\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n9877\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1138,\\\"y\\\":656}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.55506877251900731432124571610\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"lx9877\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1256,\\\"y\\\":638}\",\"typeDefinition\":[\"typeDefinitions[sensing/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.51902025728486481432124611813\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::9877\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n2251\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1128,\\\"y\\\":55}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.103688640287145971432125140977\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hw2251\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1322,\\\"y\\\":132}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.478929068660363551432125174141\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::2251\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n1559\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":957,\\\"y\\\":54}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.82558026234619321432125237187\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hw1559\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1048,\\\"y\\\":117}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.25617986987344921432125299046\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::1559\"}]}]}],\"typeDefinitions\":[{\"eClass\":\"org.kevoree.NodeType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"ContikiNode\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[]},{\"eClass\":\"org.kevoree.GroupType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"UDPGroup\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[//kevoree-group-udp/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"3dddTFpd\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"port\",\"state\":\"true\",\"datatype\":\"int\",\"defaultValue\":\"1234\",\"genericTypes\":[]}]}]},{\"eClass\":\"org.kevoree.ComponentType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"blink\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[kev_contiki//blink/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"0qE5TygS\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"interval\",\"state\":\"false\",\"datatype\":\"int\",\"defaultValue\":\"1000\",\"genericTypes\":[]}]}],\"required\":[],\"provided\":[]},{\"eClass\":\"org.kevoree.ComponentType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"sensing\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[kev_contiki//sensing/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"oW3cS6Ts\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"interval\",\"state\":\"false\",\"datatype\":\"int\",\"defaultValue\":\"1000\",\"genericTypes\":[]}]}],\"required\":[],\"provided\":[]},{\"eClass\":\"org.kevoree.ComponentType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"hello_world\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[kev_contiki//hello_world/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"3dddTFpd\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"name\",\"state\":\"false\",\"datatype\":\"string\",\"defaultValue\":\"Inti\",\"genericTypes\":[]}]}],\"required\":[],\"provided\":[]},{\"eClass\":\"org.kevoree.ComponentType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"sieve\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[kev_contiki//sieve/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"afOM93SD\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"interval\",\"state\":\"false\",\"datatype\":\"int\",\"defaultValue\":\"18000\",\"genericTypes\":[]},{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"count\",\"state\":\"false\",\"datatype\":\"int\",\"defaultValue\":\"100\",\"genericTypes\":[]}]}],\"required\":[],\"provided\":[]}],\"repositories\":[{\"eClass\":\"org.kevoree.Repository\",\"url\":\"[aaaa::1]:1234\"}],\"dataTypes\":[],\"libraries\":[{\"eClass\":\"org.kevoree.TypeLibrary\",\"name\":\"ContikiLib\",\"subTypes\":[\"typeDefinitions[ContikiNode/0.0.1]\",\"typeDefinitions[UDPGroup/0.0.1]\",\"typeDefinitions[blink/0.0.1]\",\"typeDefinitions[sensing/0.0.1]\",\"typeDefinitions[hello_world/0.0.1]\",\"typeDefinitions[sieve/0.0.1]\"]},{\"eClass\":\"org.kevoree.TypeLibrary\",\"name\":\"Default\",\"subTypes\":[]}],\"hubs\":[],\"mBindings\":[],\"deployUnits\":[{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"\",\"name\":\"kevoree-group-udp\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"org.kevoree.library.c\",\"name\":\"kevoree-contiki-node\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"kev_contiki\",\"name\":\"blink\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"kev_contiki\",\"name\":\"sensing\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"kev_contiki\",\"name\":\"hello_world\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"kev_contiki\",\"name\":\"sieve\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]}],\"nodeNetworks\":[],\"groups\":[{\"eClass\":\"org.kevoree.Group\",\"name\":\"group0\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":897,\\\"y\\\":397}\",\"typeDefinition\":[\"typeDefinitions[UDPGroup/0.0.1]\"],\"subNodes\":[\"nodes[n1759]\",\"nodes[n2151]\",\"nodes[nb771]\",\"nodes[n9989]\",\"nodes[n2459]\",\"nodes[n2650]\",\"nodes[n9073]\",\"nodes[n8773]\",\"nodes[n9877]\",\"nodes[n2251]\",\"nodes[n1559]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.26062655518762771432122795371\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]}],\"fragmentDictionary\":[{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n1759\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n2151\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"nb771\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n9989\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n2459\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n2650\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n9073\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n8773\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n9877\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n2251\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n1559\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]}]}]}";

struct jsonparse_state jsonState;
int fd;
TraceSequence *ts;

/*---------------------------------------------------------------------------*/
PROCESS(kev_tests_process, "Kevoree test process");
AUTOSTART_PROCESSES(&kev_tests_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(kev_tests_process, ev, data)
{
	printf("INFO: Starting kevoree-contiki implementation\n");

	if ((fd = cfs_open("model.json", CFS_READ)) != -1) {
		printf("INFO: removing model.json \n");
		if(!cfs_remove("model.json")) {
			printf("Success!\n");
		} else {
			printf("ERROR: cannot remove component!\n");
		}
	}

	if ((fd = cfs_open("newModel.json", CFS_READ)) != -1) {
		printf("INFO: removing newModel.json \n");
		if(!cfs_remove("newModel.json")) {
			printf("Success!\n");
		} else {
			printf("ERROR: cannot remove component!\n");
		}
	}

	PROCESS_BEGIN();

	/*
	 * Write constant DEFAULTMODEL to a file in order to de-serialize it
	 */
	fd = cfs_open("model.json", CFS_WRITE);
	if (fd != -1) {
		cfs_write(fd, DEFAULTMODEL, strlen(DEFAULTMODEL));
		cfs_close(fd);
		printf("INFO: model.json successfully written\n");
	}

	/*
	 * Write constant NEWMODEL to a file in order to de-serialize it
	 */
	fd = cfs_open("newModel.json", CFS_WRITE);
	if (fd != -1) {
		cfs_write(fd, NEWMODEL, strlen(NEWMODEL));
		cfs_close(fd);
		printf("INFO: newModel.json successfully written\n");
	}

	/*
	 * De-serialize models
	 */
	jsonparse_setup(&jsonState, "model.json");
	/*jsonparse_setup(&jsonState, DEFAULTMODEL, strlen(DEFAULTMODEL));*/
	current_model = JSONKevDeserializer(&jsonState, jsonparse_next(&jsonState));
	cfs_close(jsonState.fd);
	cfs_remove("model.json");

	if (current_model == NULL) {
		printf("ERROR: current_model cannot be loaded\n");
		PROCESS_EXIT();
	} else {
		printf("INFO: model.json successfully loaded\n");
	}

	/*jsonparse_setup(&jsonState, NEWMODEL, strlen(NEWMODEL));*/
	jsonparse_setup(&jsonState, "newModel.json");
	new_model = JSONKevDeserializer(&jsonState, jsonparse_next(&jsonState));
	cfs_close(jsonState.fd);
	cfs_remove("newModel.json");

	if (new_model == NULL) {
		printf("ERROR: new_model cannot be loaded\n");
		PROCESS_EXIT();
	} else {
		printf("INFO: newModel.json successfully loaded\n");
		int length = hashmap_length(new_model->nodes);
		printf("INFO: newModel with %d nodes is ready!\n", length);
	}

	/*
	 * Create differences in form of traces, assuming node id "n1759"
	 */
	ts = ModelCompare(new_model, current_model, "n1759");
	if (ts == NULL) {
		printf("ERROR: Cannot create traceSequence\n");\
		PROCESS_EXIT();
	}

	/*
	 * Create a list of adaptations
	 */
	LIST(plannedAdaptations);
	list_init(plannedAdaptations);

	Planner_compareModels(current_model, new_model, "n1759", ts);
	plannedAdaptations = Planner_schedule();

	if (plannedAdaptations != NULL) {
		int adaptListLength = list_length(plannedAdaptations);
		printf("INFO: Number of adaptations: %d\n", adaptListLength);
		while (list_length(plannedAdaptations) > 0) {
			AdaptationPrimitive *c = (AdaptationPrimitive*)list_pop(plannedAdaptations);
			printf("%s: Priority: %d Type: %d\n", c->ref->VT->getPath(c->ref), c->priority, c->primitiveType);
			c->delete(c);
		}
	} else {
		printf("ERROR: cannot create Adaptation primitives\n");
	}
	ts->delete(ts);

	delete((KMFContainer*)current_model);
	delete((KMFContainer*)new_model);

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
