#include "contiki.h"
#include "dev/serial-line.h"
#include <stdbool.h>
#include <stdio.h>

#include "cfs/cfs-coffee.h"


/*
 * Compress an hard-coded model
 */
#define ALPHABET_SIZE 218

#define COMPRESS 0
#define DECOMPRESS 1

//TODO remove extra space and new line characters
const char *const code_attr[ALPHABET_SIZE][2] ={{"eClass","["},{"name","[["},{"values","[]"},{"generated_KMF_ID","[("},{"value","[)"},{"false","[a"},{"true","[b"},{"started","[c"},{"metaData","[d"},{"typeDefinition","[e"},{"dictionary","[f"},{"fragmentDictionary","[g"},{"org.kevoree.Dictionary","[h"},{"org.kevoree.DictionaryValue","[i"},{"provided","[j"},{"required","[k"},{"port","[l"},{"1234","[m"},{"version","[n"},{"0.0.1","[o"},{"groups","[p"},{"org.kevoree.NetworkProperty","[q"},{"org.kevoree.ContainerNode","[r"},{"hosts","[s"},{"host","[t"},{"groups[group0]","[u"},{"components","[v"},{"networkInformation","[w"},{"org.kevoree.NetworkInfo","[x"},{"ip","[y"},{"org.kevoree.ComponentInstance","[z"},{"namespace","[0"},{"org.kevoree.FragmentDictionary","[1"},{"local","[2"},{"interval","[3"},{"url","[4"},{"1000","[5"},{"Inti","[6"},{"abstract","[7"},{"bean","[8"},{"factoryBean","[9"},{"deployUnit","]"},{"superTypes","]["},{"dictionaryType","]]"},{"org.kevoree.DictionaryAttribute","]("},{"fragmentDependant","])"},{"optional","]a"},{"state","]b"},{"datatype","]c"},{"defaultValue","]d"},{"org.kevoree.DeployUnit","]e"},{"groupName","]f"},{"hashcode","]g"},{"type","]h"},{"ce","]i"},{"genericTypes","]j"},{"requiredLibs","]k"},{"org.kevoree.DictionaryType","]l"},{"attributes","]m"},{"int","]n"},{"org.kevoree.ComponentType","]o"},{"kev_contiki","]p"},{"org.kevoree.TypeLibrary","]q"},{"subTypes","]r"},{"3dddTFpd","]s"},{"hello_world","]t"},{"n1759","]u"},{"nb772","]v"},{"n2151","]w"},{"n2251","]x"},{"n1559","]y"},{"blink","]z"},{"sensing","]0"},{"sieve","]1"},{"nb870","]2"},{"n1459","]3"},{"nb771","]4"},{"org.kevoree.ContainerRoot","]5"},{"nodes","]6"},{"typeDefinitions","]7"},{"org.kevoree.NodeType","]8"},{"ContikiNode","]9"},{"org.kevoree.GroupType","("},{"repositories","(["},{"dataTypes","(]"},{"libraries","(("},{"ContikiLib","()"},{"Default","(a"},{"hubs","(b"},{"mBindings","(c"},{"deployUnits","(d"},{"org.kevoree.library.c","(e"},{"nodeNetworks","(f"},{"org.kevoree.Group","(g"},{"group0","(h"},{"subNodes","(i"},{"CtHbJw37","(j"},{"UDPGroup","(k"},{"string","(l"},{"org.kevoree.Repository","(m"},{"nodes[n1759]","(n"},{"0.44028098252601921432122800473","(o"},{"bk1759","(p"},{"0.244668840663507581432123271370","(q"},{"0.280824760207906371432123339315","(r"},{"hwb772","(s"},{"0.125406553270295261432123361237","(t"},{"0.56672404310666021432123659775","(u"},{"bk2151","(v"},{"0.96605161810293791432123715642","(w"},{"lo","(x"},{"n3554","(y"},{"n9989","(z"},{"0.103688640287145971432125140977","(0"},{"hw2251","(1"},{"0.478929068660363551432125174141","(2"},{"0.82558026234619321432125237187","(3"},{"hw1559","(4"},{"0.25617986987344921432125299046","(5"},{"0qE5TygS","(6"},{"oW3cS6Ts","(7"},{"afOM93SD","(8"},{"18000","(9"},{"count",")"},{"100",")["},{"nodes[nb772]",")]"},{"nodes[n2151]",")("},{"nodes[n2251]","))"},{"nodes[n1559]",")a"},{"0.26062655518762771432122795371",")b"},{"0.133597850101068621432123764697",")c"},{"hwb870",")d"},{"0.73711027111858131432123794332",")e"},{"0.70933706359937791432123833544",")f"},{"lx1459",")g"},{"0.70674828044138851432123848469",")h"},{"nodes[nb870]",")i"},{"nodes[n1459]",")j"},{"0.85399847291409971432123883565",")k"},{"hwb771",")l"},{"0.93829955207183961432123899340",")m"},{"n2459",")n"},{"n2650",")o"},{"n9073",")p"},{"n8773",")q"},{"n9877",")r"},{"nodes[nb771]",")s"},{"0.90601870790123941432123946266",")t"},{"lx3554",")u"},{"0.530610746005551432124082140",")v"},{"0.40597744332626461432124065761",")w"},{"hw9989",")x"},{"0.93384177354164421432124102077",")y"},{"nodes[n3554]",")z"},{"nodes[n9989]",")0"},{"0.0915601672604681432124195820",")1"},{"bk2459",")2"},{"0.39016547030769291432124216946",")3"},{"0.53797553176991641432124283059",")4"},{"hw2650",")5"},{"0.39251068630255761432124317104",")6"},{"0.55784740601666271432124376455",")7"},{"bk9073",")8"},{"0.65135152544826271432124390045",")9"},{"0.98144014482386411432124448068","a"},{"hw8773","a["},{"0.68661252455785871432124536416","a]"},{"0.55506877251900731432124571610","a("},{"lx9877","a)"},{"0.51902025728486481432124611813","aa"},{"nodes[n2459]","ab"},{"nodes[n2650]","ac"},{"nodes[n9073]","ad"},{"nodes[n8773]","ae"},{"nodes[n9877]","af"},{"na289","ag"},{"na573","ah"},{"n2152","ai"},{"n1455","aj"},{"na871","ak"},{"0","al"},{"time","am"},{"0.71672565443441271432124688330","an"},{"hwa289","ao"},{"0.96873560803942381432124721324","ap"},{"0.71419032104313371432124762480","aq"},{"bka573","ar"},{"0.92470887908712031432124815954","as"},{"0.123657654505223041432124851613","at"},{"lx2152","au"},{"0.96535480115562681432124871253","av"},{"0.71556304884143171432124953253","aw"},{"bk1455","ax"},{"0.54490464390255511432124999589","ay"},{"0.084562670206651091432125029664","az"},{"bka871","a0"},{"0.75130812753923241432125161541","a1"},{"nodes[na289]","a2"},{"nodes[na573]","a3"},{"nodes[n2152]","a4"},{"nodes[n1455]","a5"},{"nodes[na871]","a6"},{"BXX5q3eV","a7"},{"node0","a8"},{"comp457","a9"},{"0.68263587262481451424775426644","b"},{"6","b["},{"front","b]"},{"9o86ZdvQ","b("},{"CoAPGroup","b)"},{"hytCmvXU","ba"},{"path","bb"},{"number","bc"},{"proxy_port","bd"},{"20000","be"},{"5","bf"},{"nodes[node0]","bg"},{"QoMNUckL","bh"}};

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
        if (attr_index == 0) {
          code = "";
        }
        else {
          if (mode == COMPRESS) {
            code = getSubsituteCode(attr);
          }
          else if (mode == DECOMPRESS) {
            code = getAttributeCode(attr);
          }
        }
	size = strlen(code);
	char tmp_buf[2 + size];
	sprintf(tmp_buf, "\"%s\"", code);
	cfs_write(out, tmp_buf, strlen(tmp_buf));
	attr_index = 0; //reset attribute attr_index
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

