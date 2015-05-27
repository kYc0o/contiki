/*
 * kev-runtime.c
 * This file is part of Kevoree-Contiki
 *
 * Copyright (C) 2015 - Inti Gonzalez-Herrera
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


#include "contiki.h"
#include "dev/serial-line.h"
#include "cfs/cfs.h"
#include "loader/elfloader.h"
#include "net/netstack.h"

#include "contiki-net.h"
#include "net/uip.h"
#include "net/uip-ds6.h"
#include "net/rpl/rpl.h"

#include "simple-udp.h"
#include "net/netstack.h"


#include "jsonparse.h"

#include "rtkev.h"
#include "shell_group.h"
#include "deluge_group.h"
#include "udpComponent.h"
#include "ShellBasedDeployUnitRetriever.h"
#include "NaiveUDPBasedDeployUnitRetriever.h"
#include "requests.h"

#include "ContainerRoot.h"
#include "JSONModelLoader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* list of retrieving strategies */
#define NAIVE_UDP_BASED_RETRIEVER 0
#define SMART_UDP_BASED_RETRIEVER 1
#define SHELL_BASED_RETRIEVER 2

/* strategy to use */
#define DEPLOY_UNIT_RETRIEVER_STRATEGY NAIVE_UDP_BASED_RETRIEVER

#define FLASH_FORMATTED "formated.txt"
#define MAGIC_NUMBER 0xFFEEDDCC

/* used to find out if the flash is formatted */
static int
isFormatted()
{
	int r = 0;
	int fd =  cfs_open(FLASH_FORMATTED, CFS_READ);
	uint32_t x = 0;
	if (fd < 0) return r;
	if ((cfs_read(fd, &x, sizeof(uint32_t)) == sizeof(uint32_t)) && (x == MAGIC_NUMBER)) {
		r = 1;
	}
	cfs_close(fd);
	return r;
}

/* used to write down the fact hat the flash is formatted */
static void
mark_as_formatted()
{
	int fd =  cfs_open(FLASH_FORMATTED, CFS_WRITE);
	uint32_t x = MAGIC_NUMBER;
	cfs_write(fd, &x, sizeof(uint32_t));
	cfs_close(fd);
}

const char* new_model_const = "{\"eClass\": \"org.kevoree.ContainerRoot\",\"generated_KMF_ID\": \"CtHbJw37\",\"nodes\": [{\"eClass\": \"org.kevoree.ContainerNode\",\"name\": \"n1759\",\"started\": \"false\",\"metaData\": \"{\\\"x\\\":296,\\\"y\\\":167}\",\"typeDefinition\": [\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\": [],\"host\": [],\"groups\": [\"groups[group0]\"],\"dictionary\": [],\"fragmentDictionary\": [],\"components\": [{\"eClass\": \"org.kevoree.ComponentInstance\",\"name\": \"comp457\",\"started\": \"true\",\"metaData\": \"{\\\"x\\\":408,\\\"y\\\":239}\",\"typeDefinition\": [\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\": [],\"dictionary\": [{\"eClass\": \"org.kevoree.Dictionary\",\"generated_KMF_ID\": \"0.68263587262481451424775426644\",\"values\": [{\"eClass\": \"org.kevoree.DictionaryValue\",\"name\": \"time\",\"value\": \"6\"}]}],\"fragmentDictionary\": [],\"provided\": [],\"required\": []}],\"networkInformation\": [{\"eClass\": \"org.kevoree.NetworkInfo\",\"name\": \"ip\",\"values\": [{\"eClass\": \"org.kevoree.NetworkProperty\",\"name\": \"local\",\"value\": \"aaaa::0:0:5\"},{\"eClass\": \"org.kevoree.NetworkProperty\",\"name\": \"front\",\"value\": \"m3-XX.lille.iotlab.info\"}]}]}],\"typeDefinitions\": [{\"eClass\": \"org.kevoree.NodeType\",\"abstract\": \"false\",\"bean\": \"\",\"name\": \"ContikiNode\",\"factoryBean\": \"\",\"version\": \"0.0.1\",\"deployUnit\": [\"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]\"],\"superTypes\": [],\"dictionaryType\": [{\"eClass\": \"org.kevoree.DictionaryType\",\"generated_KMF_ID\": \"9o86ZdvQ\",\"attributes\": []}]},{\"eClass\": \"org.kevoree.GroupType\",\"abstract\": \"false\",\"bean\": \"\",\"name\": \"CoAPGroup\",\"factoryBean\": \"\",\"version\": \"0.0.1\",\"deployUnit\": [\"deployUnits[//kevoree-group-coap/0.0.1]\"],\"superTypes\": [],\"dictionaryType\": [{\"eClass\": \"org.kevoree.DictionaryType\",\"generated_KMF_ID\": \"hytCmvXU\",\"attributes\": [{\"eClass\": \"org.kevoree.DictionaryAttribute\",\"fragmentDependant\": \"false\",\"optional\": \"false\",\"name\": \"path\",\"state\": \"false\",\"datatype\": \"string\",\"defaultValue\": \"\",\"genericTypes\": []},{\"eClass\": \"org.kevoree.DictionaryAttribute\",\"fragmentDependant\": \"false\",\"optional\": \"false\",\"name\": \"port\",\"state\": \"false\",\"datatype\": \"number\",\"defaultValue\": \"\",\"genericTypes\": []},{\"eClass\": \"org.kevoree.DictionaryAttribute\",\"fragmentDependant\": \"false\",\"optional\": \"false\",\"name\": \"proxy_port\",\"state\": \"false\",\"datatype\": \"int\",\"defaultValue\": \"20000\",\"genericTypes\": []}]}]},{\"eClass\": \"org.kevoree.ComponentType\",\"abstract\": \"false\",\"bean\": \"\",\"name\": \"hello_world\",\"factoryBean\": \"\",\"version\": \"0.0.1\",\"deployUnit\": [\"deployUnits[kev_contiki//hello_world/0.0.1]\"],\"superTypes\": [],\"dictionaryType\": [{\"eClass\": \"org.kevoree.DictionaryType\",\"generated_KMF_ID\": \"3dddTFpd\",\"attributes\": [{\"eClass\": \"org.kevoree.DictionaryAttribute\",\"fragmentDependant\": \"false\",\"optional\": \"false\",\"name\": \"time\",\"state\": \"false\",\"datatype\": \"int\",\"defaultValue\": \"5\",\"genericTypes\": []}]}],\"required\": [],\"provided\": []}],\"repositories\": [{\"eClass\": \"org.kevoree.Repository\",\"url\": \"coap://[bbbb::1]:5683/libraries\"}],\"dataTypes\": [],\"libraries\": [{\"eClass\": \"org.kevoree.TypeLibrary\",\"name\": \"ContikiLib\",\"subTypes\": [\"typeDefinitions[ContikiNode/0.0.1]\",\"typeDefinitions[CoAPGroup/0.0.1]\"]},{\"eClass\": \"org.kevoree.TypeLibrary\",\"name\": \"Default\",\"subTypes\": [\"typeDefinitions[hello_world/0.0.1]\"]}],\"hubs\": [],\"mBindings\": [],\"deployUnits\": [{\"eClass\": \"org.kevoree.DeployUnit\",\"groupName\": \"\",\"name\": \"kevoree-group-coap\",\"hashcode\": \"\",\"type\": \"ce\",\"url\": \"\",\"version\": \"0.0.1\",\"requiredLibs\": []},{\"eClass\": \"org.kevoree.DeployUnit\",\"groupName\": \"org.kevoree.library.c\",\"name\": \"kevoree-contiki-node\",\"hashcode\": \"\",\"type\": \"ce\",\"url\": \"\",\"version\": \"0.0.1\",\"requiredLibs\": []},{\"eClass\": \"org.kevoree.DeployUnit\",\"groupName\": \"kev_contiki\",\"name\": \"hello_world\",\"hashcode\": \"\",\"type\": \"ce\",\"url\": \"\",\"version\": \"0.0.1\",\"requiredLibs\": []}],\"nodeNetworks\": [],\"groups\": [{\"eClass\": \"org.kevoree.Group\",\"name\": \"group0\",\"started\": \"false\",\"metaData\": \"{\\\"x\\\":504,\\\"y\\\":259}\",\"typeDefinition\": [\"typeDefinitions[CoAPGroup/0.0.1]\"],\"subNodes\": [\"nodes[n1759]\"],\"dictionary\": [],\"fragmentDictionary\": [{\"eClass\": \"org.kevoree.FragmentDictionary\",\"name\": \"contiki-node\",\"generated_KMF_ID\": \"QoMNUckL\",\"values\": []}]}]}";

/*const char *new_model_const = "{\"eClass\":\"org.kevoree.ContainerRoot\",\"generated_KMF_ID\":\"CtHbJw37\",\"nodes\":[{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n1759\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":803,\\\"y\\\":52}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.44028098252601921432122800473\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"bk1759\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":922,\\\"y\\\":115}\",\"typeDefinition\":[\"typeDefinitions[blink/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.244668840663507581432123271370\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::1759\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"nb772\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":653,\\\"y\\\":51}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.280824760207906371432123339315\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hwb772\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":761,\\\"y\\\":118}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.125406553270295261432123361237\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::b772\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n2151\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":499,\\\"y\\\":50}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.56672404310666021432123659775\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"bk2151\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":576,\\\"y\\\":113}\",\"typeDefinition\":[\"typeDefinitions[blink/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.96605161810293791432123715642\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"lo\",\"value\":\"fe80::2151\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"nb870\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":354,\\\"y\\\":153}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.133597850101068621432123764697\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hwb870\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":498,\\\"y\\\":217}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.73711027111858131432123794332\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::b870\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n1459\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":354,\\\"y\\\":252}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.70933706359937791432123833544\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"lx1459\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":474,\\\"y\\\":307}\",\"typeDefinition\":[\"typeDefinitions[sensing/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.70674828044138851432123848469\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::1459\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"nb771\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":353,\\\"y\\\":353}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.85399847291409971432123883565\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hwb771\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":448,\\\"y\\\":419}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.93829955207183961432123899340\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::b771\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n3554\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":354,\\\"y\\\":454}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.90601870790123941432123946266\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"lx3554\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":475,\\\"y\\\":520}\",\"typeDefinition\":[\"typeDefinitions[sensing/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.530610746005551432124082140\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::3554\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n9989\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":354,\\\"y\\\":553}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.40597744332626461432124065761\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hw9989\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":450,\\\"y\\\":611}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.93384177354164421432124102077\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::9989\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n2459\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":494,\\\"y\\\":654}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.0915601672604681432124195820\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"bk2459\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":645,\\\"y\\\":722}\",\"typeDefinition\":[\"typeDefinitions[blink/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.39016547030769291432124216946\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::2459\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n2650\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":655,\\\"y\\\":653}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.53797553176991641432124283059\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hw2650\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":794,\\\"y\\\":712}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.39251068630255761432124317104\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::2650\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n9073\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":817,\\\"y\\\":653}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.55784740601666271432124376455\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"bk9073\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":943,\\\"y\\\":718}\",\"typeDefinition\":[\"typeDefinitions[blink/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.65135152544826271432124390045\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::9073\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n8773\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":982,\\\"y\\\":655}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.98144014482386411432124448068\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hw8773\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1118,\\\"y\\\":716}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.68661252455785871432124536416\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::8773\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n9877\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1138,\\\"y\\\":656}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.55506877251900731432124571610\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"lx9877\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1256,\\\"y\\\":638}\",\"typeDefinition\":[\"typeDefinitions[sensing/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.51902025728486481432124611813\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::9877\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"na289\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1268,\\\"y\\\":557}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.71672565443441271432124688330\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hwa289\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1222,\\\"y\\\":526}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.96873560803942381432124721324\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::a289\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"na573\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1268,\\\"y\\\":458}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.71419032104313371432124762480\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"bka573\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1223,\\\"y\\\":394}\",\"typeDefinition\":[\"typeDefinitions[blink/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.92470887908712031432124815954\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::a573\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n2152\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1267,\\\"y\\\":360}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.123657654505223041432124851613\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"lx2152\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1222,\\\"y\\\":319}\",\"typeDefinition\":[\"typeDefinitions[sensing/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.96535480115562681432124871253\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::2152\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n1455\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1265,\\\"y\\\":259}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.71556304884143171432124953253\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"bk1455\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1199,\\\"y\\\":219}\",\"typeDefinition\":[\"typeDefinitions[blink/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.54490464390255511432124999589\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::1455\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"na871\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1264,\\\"y\\\":155}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.084562670206651091432125029664\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"bka871\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1193,\\\"y\\\":114}\",\"typeDefinition\":[\"typeDefinitions[blink/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.75130812753923241432125161541\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"interval\",\"value\":\"1000\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::a871\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n2251\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1128,\\\"y\\\":55}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.103688640287145971432125140977\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hw2251\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1322,\\\"y\\\":132}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.478929068660363551432125174141\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::2251\"}]}]},{\"eClass\":\"org.kevoree.ContainerNode\",\"name\":\"n1559\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":957,\\\"y\\\":54}\",\"typeDefinition\":[\"typeDefinitions[ContikiNode/0.0.1]\"],\"hosts\":[],\"host\":[],\"groups\":[\"groups[group0]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.82558026234619321432125237187\",\"values\":[]}],\"fragmentDictionary\":[],\"components\":[{\"eClass\":\"org.kevoree.ComponentInstance\",\"name\":\"hw1559\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":1048,\\\"y\\\":117}\",\"typeDefinition\":[\"typeDefinitions[hello_world/0.0.1]\"],\"namespace\":[],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.25617986987344921432125299046\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"name\",\"value\":\"Inti\"}]}],\"fragmentDictionary\":[],\"provided\":[],\"required\":[]}],\"networkInformation\":[{\"eClass\":\"org.kevoree.NetworkInfo\",\"name\":\"ip\",\"values\":[{\"eClass\":\"org.kevoree.NetworkProperty\",\"name\":\"local\",\"value\":\"fe80::1559\"}]}]}],\"typeDefinitions\":[{\"eClass\":\"org.kevoree.NodeType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"ContikiNode\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[]},{\"eClass\":\"org.kevoree.GroupType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"UDPGroup\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[//kevoree-group-udp/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"3dddTFpd\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"port\",\"state\":\"true\",\"datatype\":\"int\",\"defaultValue\":\"1234\",\"genericTypes\":[]}]}]},{\"eClass\":\"org.kevoree.ComponentType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"blink\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[kev_contiki//blink/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"0qE5TygS\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"interval\",\"state\":\"false\",\"datatype\":\"int\",\"defaultValue\":\"1000\",\"genericTypes\":[]}]}],\"required\":[],\"provided\":[]},{\"eClass\":\"org.kevoree.ComponentType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"sensing\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[kev_contiki//sensing/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"oW3cS6Ts\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"interval\",\"state\":\"false\",\"datatype\":\"int\",\"defaultValue\":\"1000\",\"genericTypes\":[]}]}],\"required\":[],\"provided\":[]},{\"eClass\":\"org.kevoree.ComponentType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"hello_world\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[kev_contiki//hello_world/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"3dddTFpd\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"name\",\"state\":\"false\",\"datatype\":\"string\",\"defaultValue\":\"Inti\",\"genericTypes\":[]}]}],\"required\":[],\"provided\":[]},{\"eClass\":\"org.kevoree.ComponentType\",\"abstract\":\"false\",\"bean\":\"\",\"name\":\"sieve\",\"factoryBean\":\"\",\"version\":\"0.0.1\",\"deployUnit\":[\"deployUnits[kev_contiki//sieve/0.0.1]\"],\"superTypes\":[],\"dictionaryType\":[{\"eClass\":\"org.kevoree.DictionaryType\",\"generated_KMF_ID\":\"afOM93SD\",\"attributes\":[{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"interval\",\"state\":\"false\",\"datatype\":\"int\",\"defaultValue\":\"18000\",\"genericTypes\":[]},{\"eClass\":\"org.kevoree.DictionaryAttribute\",\"fragmentDependant\":\"false\",\"optional\":\"false\",\"name\":\"count\",\"state\":\"false\",\"datatype\":\"int\",\"defaultValue\":\"100\",\"genericTypes\":[]}]}],\"required\":[],\"provided\":[]}],\"repositories\":[{\"eClass\":\"org.kevoree.Repository\",\"url\":\"[aaaa::1]:1234\"}],\"dataTypes\":[],\"libraries\":[{\"eClass\":\"org.kevoree.TypeLibrary\",\"name\":\"ContikiLib\",\"subTypes\":[\"typeDefinitions[ContikiNode/0.0.1]\",\"typeDefinitions[UDPGroup/0.0.1]\",\"typeDefinitions[blink/0.0.1]\",\"typeDefinitions[sensing/0.0.1]\",\"typeDefinitions[hello_world/0.0.1]\",\"typeDefinitions[sieve/0.0.1]\"]},{\"eClass\":\"org.kevoree.TypeLibrary\",\"name\":\"Default\",\"subTypes\":[]}],\"hubs\":[],\"mBindings\":[],\"deployUnits\":[{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"\",\"name\":\"kevoree-group-udp\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"org.kevoree.library.c\",\"name\":\"kevoree-contiki-node\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"kev_contiki\",\"name\":\"blink\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"kev_contiki\",\"name\":\"sensing\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"kev_contiki\",\"name\":\"hello_world\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]},{\"eClass\":\"org.kevoree.DeployUnit\",\"groupName\":\"kev_contiki\",\"name\":\"sieve\",\"hashcode\":\"\",\"type\":\"ce\",\"url\":\"\",\"version\":\"0.0.1\",\"requiredLibs\":[]}],\"nodeNetworks\":[],\"groups\":[{\"eClass\":\"org.kevoree.Group\",\"name\":\"group0\",\"started\":\"true\",\"metaData\":\"{\\\"x\\\":897,\\\"y\\\":397}\",\"typeDefinition\":[\"typeDefinitions[UDPGroup/0.0.1]\"],\"subNodes\":[\"nodes[n1759]\",\"nodes[nb772]\",\"nodes[n2151]\",\"nodes[nb870]\",\"nodes[n1459]\",\"nodes[nb771]\",\"nodes[n3554]\",\"nodes[n9989]\",\"nodes[n2459]\",\"nodes[n2650]\",\"nodes[n9073]\",\"nodes[n8773]\",\"nodes[n9877]\",\"nodes[na289]\",\"nodes[na573]\",\"nodes[n2152]\",\"nodes[n1455]\",\"nodes[na871]\",\"nodes[n2251]\",\"nodes[n1559]\"],\"dictionary\":[{\"eClass\":\"org.kevoree.Dictionary\",\"generated_KMF_ID\":\"0.26062655518762771432122795371\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]}],\"fragmentDictionary\":[{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n1759\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"nb772\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n2151\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"nb870\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n1459\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"nb771\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n3554\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n9989\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n2459\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n2650\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n9073\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n8773\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n9877\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"na289\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"na573\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n2152\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n1455\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"na871\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n2251\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]},{\"eClass\":\"org.kevoree.FragmentDictionary\",\"name\":\"n1559\",\"generated_KMF_ID\":\"\",\"values\":[{\"eClass\":\"org.kevoree.DictionaryValue\",\"name\":\"port\",\"value\":\"1234\"}]}]}]}";
*/
/* built-in kevoree types */
extern const GroupInterface ShellGroupInterface;
extern struct process shellGroupP;

/* declaring built-in instances */
DECLARE_KEV_TYPES(3, &ShellGroupInterface, &DelugeRimeGroupInterface, &UDPClientInterface)
struct Built_In_Instance {
	const char* type_name;
	const char* instance_name;
	const bool enabled;
};
static const struct Built_In_Instance built_in_instances [] ={
	{
		.type_name = "ShellGroupType",
		.instance_name = "shellGroup0",
		.enabled = true
	},
	{
		.type_name = DELUGE_GROUP_TYPENAME,
		.instance_name = "delugeGroup0",
		.enabled = true
	},
	{
		.type_name = UDP_CLIENT_COMPONENT_TYPE_NAME,
		.instance_name = "udpClient0",
		.enabled = false
	}
};


static char*
get_local_address(void)
{
	int i;
	uint8_t state;
	char* r = (char*)malloc(sizeof(char)*10);
	
	for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
		state = uip_ds6_if.addr_list[i].state;
		if(uip_ds6_if.addr_list[i].isused &&
				(state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
			uip_ipaddr_t *ip_addr = &uip_ds6_if.addr_list[i].ipaddr;
			uint16_t a = (ip_addr->u8[14] << 8) + ip_addr->u8[15];
			sprintf(r, "%x", a);
			return r; // please, just one address, I don't care if the interface has more than one address
		}
	}
	
	return  NULL;
}

PROCESS(kevRuntime, "KevRuntime");
AUTOSTART_PROCESSES(&kevRuntime);
PROCESS_THREAD(kevRuntime, ev, data)
{
	static uint8_t buf[257];
	static uint8_t processingFile = 0;
	static uint32_t received = 0;
	static struct cfs_dirent dirent;
	static struct cfs_dir dir;
	static uint32_t fdFile;
	static char *filename;
	static int i;
	
	void* instTmp;
	
	int result;

	PROCESS_BEGIN();

	printf("Size of KevoreePacket: %d\n", sizeof(struct KevoreePacket));
	
	NETSTACK_MAC.off(1);

	/* definitively we want to dynamically load modules */
	elfloader_init();
	
	/* ensure that the flash is formatted */
	if (!isFormatted()) {
		printf("Formating the flash ... this may take a while, so go for a coffee ... around three minutes\n");
		int r_format = cfs_coffee_format();
		printf("Formatting result is %d\n", r_format);
		if (r_format) {
			printf("Some error formatting ... Kevoree cannot run\n");
			PROCESS_EXIT();
		}
		else {
			mark_as_formatted();
		}
	}
	
	/* create model from constant */
	fdFile = cfs_open("new_model-compact.json", CFS_WRITE);
	cfs_write(fdFile, new_model_const, strlen(new_model_const));
	cfs_close(fdFile);
	printf("File with new model was created 000\n");
	
	/* get local address */
	char* local_address = get_local_address();
	
	/* initialize Kevoree Runtime */
	result = 1;
#if DEPLOY_UNIT_RETRIEVER_STRATEGY == NAIVE_UDP_BASED_RETRIEVER
	result = initKevRuntime(local_address, &naive_udp_retriever);
#elif DEPLOY_UNIT_RETRIEVER_STRATEGY == SMART_UDP_BASED_RETRIEVER

#elif DEPLOY_UNIT_RETRIEVER_STRATEGY == SHELL_BASED_RETRIEVER
	result = initKevRuntime(local_address, &shellBasedRetriever);
#endif

	/* free some memory */
	free(local_address);
	
	/* is everything ok? */
	if (result != 0) {
		printf("Runtime initialization error\n");
		PROCESS_EXIT();
	}
	
	/* let's register core components */
	REGISTER_KEV_TYPES_NOW();

	printf("Kevoree server started !\n");

	/* create built-in types */
	for (i = 0 ; i < sizeof(built_in_instances)/sizeof(struct Built_In_Instance); i++) {
		// skip instance if it is disabled
		if (!built_in_instances[i].enabled) continue;
		
		/* install instance */
		do {
			static struct etimer et;
			/* Listen for announcements every one second. */
			etimer_set(&et, CLOCK_SECOND * 1);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
			instTmp = NULL;
			createInstance(built_in_instances[i].type_name, built_in_instances[i].instance_name, &instTmp);
		} while (instTmp == NULL);
		printf("The instance %s is located at %p\n", built_in_instances[i].instance_name, instTmp);
		startInstance(built_in_instances[i].instance_name);
	}

	while(1) {

		PROCESS_WAIT_EVENT();
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
			else if (!strcmp(data, "pushModel")) {
				//notifyNewModel(NULL);
				process_post(&shellGroupP, NEW_MODEL_IN_JSON, NULL);
			}
			else if (strstr(data, "createInstance") == data) {
				printf("Executing createInstance\n");
				char* tmp = strstr(data, " ");
				tmp++;
				char* tmp2 = strstr(tmp, " ");
				*tmp2 = 0;
				printf("\tParam 0 : %s\n", tmp);
				tmp2++;
				printf("\tParam 1 : %s\n", tmp2);
				void* ins;
				createInstance(tmp, tmp2, &ins);
				printf("\tThe created instance has address %p\n", ins);
			}
			else if (!strcmp(data, "format")) {
				/* format the flash */
				printf("Formatting\n");
				printf("It takes around 3 minutes\n");
				printf("...\n");

				fdFile = cfs_coffee_format();
				printf("Formatted with result %ld\n", fdFile);
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
				printf("DONE WITH CAT\n");
				cfs_close(fdFile);
				if (n!=0)
					printf("Some error reading the file\n");
			}
			else if (strstr(data, "rm") == data) {
				int n, jj;
				char* tmp = strstr(data, " ");
				tmp++;
				cfs_remove(tmp);
			}
			else if (strstr(data, "startInstance") == data) {
				filename = strstr(data, " ");
				filename++;
				startInstance(filename);
			}
			else if (strstr(data, "stopInstance") == data) {
				filename = strstr(data, " ");
				filename++;
				stopInstance(filename);
			}
			else if (strstr(data, "removeInstance") == data) {
				filename = strstr(data, " ");
				filename++;
				removeInstance(filename);
			}
			else if (strstr(data, "loadelf") == data) {
				filename = strstr(data, " ");
				filename++;
				loadElfFile(filename);
			}
			else if (strstr(data, "uploadUnit") == data) {
				char* tmp = strstr(data, " ");
				tmp++;
				fdFile = cfs_open(tmp, CFS_READ | CFS_WRITE);
				printf("Uploading deploy unit %s\n", tmp);
				processingFile = 1;
				filename = strdup(tmp);
			}
			else if (!strcmp(data, "enduploadUnit")) {
				cfs_close(fdFile);
				printf("File %s uploaded (%ld bytes)\n", filename, received);
				received = 0;
				processingFile = 0;
				// so we can notify to the other DeployUnitRetriever
				notifyDeployUnitDownloaded(filename);
				filename = 0;
			}
			else if (strstr(data, "upload") == data) {
				char* tmp = strstr(data, " ");
				tmp++;
				fdFile = cfs_open(tmp, CFS_READ | CFS_WRITE);
				printf("Uploading file %s\n", tmp);
				processingFile = 1;
			}
			else if (!strcmp(data, "endupload")) {
				cfs_close(fdFile);
				printf("File uploaded (%ld bytes)\n", received);
				received = 0;
				processingFile = 0;
			}
			else if (processingFile) {
				int n = strlen(data);
				int r = decode(data, n, buf);
				received += r;
				cfs_write(fdFile, buf, r);
			}
			else  {
				printf("Unknown commnad => %s\n", (char*)data);
			}
		}
	}

	PROCESS_END();
}
