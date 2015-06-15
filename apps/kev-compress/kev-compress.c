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
const char *const code_attr[ALPHABET_SIZE][2] ={{"eClass","["},{"name","]"},{"values","("},{"generated_KMF_ID",")"},{"value","a"},{"false","b"},{"true","c"},{"started","d"},{"metaData","e"},{"typeDefinition","f"},{"dictionary","g"},{"fragmentDictionary","h"},{"org.kevoree.Dictionary","i"},{"org.kevoree.DictionaryValue","j"},{"provided","k"},{"required","l"},{"port","m"},{"1234","n"},{"version","o"},{"0.0.1","p"},{"groups","q"},{"org.kevoree.NetworkProperty","r"},{"org.kevoree.ContainerNode","s"},{"hosts","t"},{"host","u"},{"groups[group0]","v"},{"components","w"},{"networkInformation","x"},{"org.kevoree.NetworkInfo","y"},{"ip","z"},{"org.kevoree.ComponentInstance","A"},{"namespace","B"},{"org.kevoree.FragmentDictionary","C"},{"local","D"},{"interval","E"},{"url","F"},{"1000","G"},{"Inti","H"},{"abstract","I"},{"bean","J"},{"factoryBean","K"},{"deployUnit","L"},{"superTypes","M"},{"dictionaryType","N"},{"org.kevoree.DictionaryAttribute","O"},{"fragmentDependant","P"},{"optional","Q"},{"state","R"},{"datatype","S"},{"defaultValue","T"},{"org.kevoree.DeployUnit","U"},{"groupName","V"},{"hashcode","W"},{"type","X"},{"ce","Y"},{"genericTypes","Z"},{"requiredLibs","0"},{"org.kevoree.DictionaryType","1"},{"attributes","2"},{"int","3"},{"org.kevoree.ComponentType","4"},{"kev_contiki","5"},{"org.kevoree.TypeLibrary","6"},{"subTypes","7"},{"3dddTFpd","8"},{"hello_world","9"},{"n1759","[["},{"nb772","[]"},{"n2151","[("},{"n2251","[)"},{"n1559","[a"},{"blink","[b"},{"sensing","[c"},{"sieve","[d"},{"nb870","[e"},{"n1459","[f"},{"nb771","[g"},{"org.kevoree.ContainerRoot","[h"},{"nodes","[i"},{"typeDefinitions","[j"},{"org.kevoree.NodeType","[k"},{"ContikiNode","[l"},{"org.kevoree.GroupType","[m"},{"repositories","[n"},{"dataTypes","[o"},{"libraries","[p"},{"ContikiLib","[q"},{"Default","[r"},{"hubs","[s"},{"mBindings","[t"},{"deployUnits","[u"},{"org.kevoree.library.c","[v"},{"nodeNetworks","[w"},{"org.kevoree.Group","[x"},{"group0","[y"},{"subNodes","[z"},{"CtHbJw37","[A"},{"UDPGroup","[B"},{"string","[C"},{"org.kevoree.Repository","[D"},{"nodes[n1759]","[E"},{"0.44028098252601921432122800473","[F"},{"bk1759","[G"},{"0.244668840663507581432123271370","[H"},{"0.280824760207906371432123339315","[I"},{"hwb772","[J"},{"0.125406553270295261432123361237","[K"},{"0.56672404310666021432123659775","[L"},{"bk2151","[M"},{"0.96605161810293791432123715642","[N"},{"lo","[O"},{"n3554","[P"},{"n9989","[Q"},{"0.103688640287145971432125140977","[R"},{"hw2251","[S"},{"0.478929068660363551432125174141","[T"},{"0.82558026234619321432125237187","[U"},{"hw1559","[V"},{"0.25617986987344921432125299046","[W"},{"0qE5TygS","[X"},{"oW3cS6Ts","[Y"},{"afOM93SD","[Z"},{"18000","[0"},{"count","[1"},{"100","[2"},{"nodes[nb772]","[3"},{"nodes[n2151]","[4"},{"nodes[n2251]","[5"},{"nodes[n1559]","[6"},{"0.26062655518762771432122795371","[7"},{"0.133597850101068621432123764697","[8"},{"hwb870","[9"},{"0.73711027111858131432123794332","]["},{"0.70933706359937791432123833544","]]"},{"lx1459","]("},{"0.70674828044138851432123848469","])"},{"nodes[nb870]","]a"},{"nodes[n1459]","]b"},{"0.85399847291409971432123883565","]c"},{"hwb771","]d"},{"0.93829955207183961432123899340","]e"},{"n2459","]f"},{"n2650","]g"},{"n9073","]h"},{"n8773","]i"},{"n9877","]j"},{"nodes[nb771]","]k"},{"0.90601870790123941432123946266","]l"},{"lx3554","]m"},{"0.530610746005551432124082140","]n"},{"0.40597744332626461432124065761","]o"},{"hw9989","]p"},{"0.93384177354164421432124102077","]q"},{"nodes[n3554]","]r"},{"nodes[n9989]","]s"},{"0.0915601672604681432124195820","]t"},{"bk2459","]u"},{"0.39016547030769291432124216946","]v"},{"0.53797553176991641432124283059","]w"},{"hw2650","]x"},{"0.39251068630255761432124317104","]y"},{"0.55784740601666271432124376455","]z"},{"bk9073","]A"},{"0.65135152544826271432124390045","]B"},{"0.98144014482386411432124448068","]C"},{"hw8773","]D"},{"0.68661252455785871432124536416","]E"},{"0.55506877251900731432124571610","]F"},{"lx9877","]G"},{"0.51902025728486481432124611813","]H"},{"nodes[n2459]","]I"},{"nodes[n2650]","]J"},{"nodes[n9073]","]K"},{"nodes[n8773]","]L"},{"nodes[n9877]","]M"},{"na289","]N"},{"na573","]O"},{"n2152","]P"},{"n1455","]Q"},{"na871","]R"},{"0","]S"},{"time","]T"},{"0.71672565443441271432124688330","]U"},{"hwa289","]V"},{"0.96873560803942381432124721324","]W"},{"0.71419032104313371432124762480","]X"},{"bka573","]Y"},{"0.92470887908712031432124815954","]Z"},{"0.123657654505223041432124851613","]0"},{"lx2152","]1"},{"0.96535480115562681432124871253","]2"},{"0.71556304884143171432124953253","]3"},{"bk1455","]4"},{"0.54490464390255511432124999589","]5"},{"0.084562670206651091432125029664","]6"},{"bka871","]7"},{"0.75130812753923241432125161541","]8"},{"nodes[na289]","]9"},{"nodes[na573]","(["},{"nodes[n2152]","(]"},{"nodes[n1455]","(("},{"nodes[na871]","()"},{"BXX5q3eV","(a"},{"node0","(b"},{"comp457","(c"},{"0.68263587262481451424775426644","(d"},{"6","(e"},{"front","(f"},{"9o86ZdvQ","(g"},{"CoAPGroup","(h"},{"hytCmvXU","(i"},{"path","(j"},{"number","(k"},{"proxy_port","(l"},{"20000","(m"},{"5","(n"},{"nodes[node0]","(o"},{"QoMNUckL","(p"}};

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
int substitutionCode(int in, int out, int mode)
{
  static uint8_t buf[100];

  int n, j; //buffer indexes
  const char* code;
  size_t size;

  int out_size = 0;
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
        out_size += (2 + size);
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
        out_size++;
	sprintf(c, "%c", buf[j]); //avoid encoding error
	cfs_write(out, c, sizeof(c));
      }

    }
  }
  return out_size;
}

int compress(char* input, char* output)
{
  int in_file = cfs_open(input, CFS_READ);
  int out_file = cfs_open(output, CFS_WRITE);
  int out_size = -1;

  if (in_file >= 0 && out_file >= 0) {
    out_size = substitutionCode(in_file, out_file, COMPRESS);
  }

  if (in_file >= 0)
    cfs_close(in_file);
  if (out_file >= 0)
    cfs_close(out_file);
  return out_size;
}

int decompress(char* input, char* output)
{
  int in_file = cfs_open(input, CFS_READ);
  int out_file = cfs_open(output, CFS_WRITE);
  int out_size = -1;

  if (in_file >= 0 && out_file >= 0)
    out_size = substitutionCode(in_file, out_file, DECOMPRESS);

  if (in_file >= 0)
    cfs_close(in_file);
  if (out_file >= 0)
    cfs_close(out_file);
  return out_size;
}
