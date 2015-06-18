#include "contiki.h"
#include "dev/serial-line.h"
#include <stdbool.h>
#include <stdio.h>

#include "cfs/cfs-coffee.h"


/*
 * Compress an hard-coded model
 */
#define ALPHABET_SIZE 237

#define COMPRESS 0
#define DECOMPRESS 1

#define COMPRESS_BUFFER_SIZE 60

const char *const code_attr[ALPHABET_SIZE][2] ={{"eClass","["},{"name","]"},{"values","("},{"generated_KMF_ID",")"},{"value","a"},{"false","b"},{"true","c"},{"started","d"},{"metaData","e"},{"typeDefinition","f"},{"dictionary","g"},{"fragmentDictionary","h"},{"org.kevoree.Dictionary","i"},{"org.kevoree.DictionaryValue","j"},{"provided","k"},{"required","l"},{"port","m"},{"1234","n"},{"version","o"},{"0.0.1","p"},{"typeDefinitions[ContikiNode/0.0.1]","q"},{"groups","r"},{"org.kevoree.NetworkProperty","s"},{"org.kevoree.ContainerNode","t"},{"hosts","u"},{"host","v"},{"groups[group0]","w"},{"components","x"},{"networkInformation","y"},{"org.kevoree.NetworkInfo","z"},{"ip","A"},{"org.kevoree.ComponentInstance","B"},{"namespace","C"},{"org.kevoree.FragmentDictionary","D"},{"local","E"},{"interval","F"},{"url","G"},{"1000","H"},{"typeDefinitions[hello_world/0.0.1]","I"},{"Inti","J"},{"abstract","K"},{"bean","L"},{"factoryBean","M"},{"deployUnit","N"},{"superTypes","O"},{"dictionaryType","P"},{"org.kevoree.DictionaryAttribute","Q"},{"fragmentDependant","R"},{"optional","S"},{"state","T"},{"datatype","U"},{"defaultValue","V"},{"org.kevoree.DeployUnit","W"},{"groupName","X"},{"hashcode","Y"},{"type","Z"},{"ce","[["},{"genericTypes","[]"},{"requiredLibs","[("},{"org.kevoree.DictionaryType","[)"},{"attributes","[a"},{"int","[b"},{"typeDefinitions[blink/0.0.1]","[c"},{"org.kevoree.ComponentType","[d"},{"kev_contiki","[e"},{"typeDefinitions[sensing/0.0.1]","[f"},{"org.kevoree.TypeLibrary","[g"},{"subTypes","[h"},{"3dddTFpd","[i"},{"hello_world","[j"},{"typeDefinitions[UDPGroup/0.0.1]","[k"},{"n1759","[l"},{"nb772","[m"},{"n2151","[n"},{"n2251","[o"},{"n1559","[p"},{"blink","[q"},{"sensing","[r"},{"sieve","[s"},{"nb870","[t"},{"n1459","[u"},{"nb771","[v"},{"org.kevoree.ContainerRoot","[w"},{"nodes","[x"},{"typeDefinitions","[y"},{"org.kevoree.NodeType","[z"},{"ContikiNode","[A"},{"deployUnits[org.kevoree.library.c//kevoree-contiki-node/0.0.1]","[B"},{"org.kevoree.GroupType","[C"},{"repositories","[D"},{"dataTypes","[E"},{"libraries","[F"},{"ContikiLib","[G"},{"Default","[H"},{"hubs","[I"},{"mBindings","[J"},{"deployUnits","[K"},{"org.kevoree.library.c","[L"},{"kevoree-contiki-node","[M"},{"nodeNetworks","[N"},{"org.kevoree.Group","[O"},{"group0","[P"},{"subNodes","[Q"},{"CtHbJw37","[R"},{"UDPGroup","[S"},{"deployUnits[//kevoree-group-udp/0.0.1]","[T"},{"deployUnits[kev_contiki//hello_world/0.0.1]","[U"},{"string","[V"},{"org.kevoree.Repository","[W"},{"kevoree-group-udp","[X"},{"nodes[n1759]","[Y"},{"0.44028098252601921432122800473","[Z"},{"bk1759","]["},{"0.244668840663507581432123271370","]]"},{"0.280824760207906371432123339315","]("},{"hwb772","])"},{"0.125406553270295261432123361237","]a"},{"0.56672404310666021432123659775","]b"},{"bk2151","]c"},{"0.96605161810293791432123715642","]d"},{"lo","]e"},{"n3554","]f"},{"n9989","]g"},{"0.103688640287145971432125140977","]h"},{"hw2251","]i"},{"0.478929068660363551432125174141","]j"},{"0.82558026234619321432125237187","]k"},{"hw1559","]l"},{"0.25617986987344921432125299046","]m"},{"deployUnits[kev_contiki//blink/0.0.1]","]n"},{"0qE5TygS","]o"},{"deployUnits[kev_contiki//sensing/0.0.1]","]p"},{"oW3cS6Ts","]q"},{"deployUnits[kev_contiki//sieve/0.0.1]","]r"},{"afOM93SD","]s"},{"18000","]t"},{"count","]u"},{"100","]v"},{"typeDefinitions[sieve/0.0.1]","]w"},{"nodes[nb772]","]x"},{"nodes[n2151]","]y"},{"nodes[n2251]","]z"},{"nodes[n1559]","]A"},{"0.26062655518762771432122795371","]B"},{"0.133597850101068621432123764697","]C"},{"hwb870","]D"},{"0.73711027111858131432123794332","]E"},{"0.70933706359937791432123833544","]F"},{"lx1459","]G"},{"0.70674828044138851432123848469","]H"},{"nodes[nb870]","]I"},{"nodes[n1459]","]J"},{"0.85399847291409971432123883565","]K"},{"hwb771","]L"},{"0.93829955207183961432123899340","]M"},{"n2459","]N"},{"n2650","]O"},{"n9073","]P"},{"n8773","]Q"},{"n9877","]R"},{"nodes[nb771]","]S"},{"0.90601870790123941432123946266","]T"},{"lx3554","]U"},{"0.530610746005551432124082140","]V"},{"0.40597744332626461432124065761","]W"},{"hw9989","]X"},{"0.93384177354164421432124102077","]Y"},{"nodes[n3554]","]Z"},{"nodes[n9989]","(["},{"0.0915601672604681432124195820","(]"},{"bk2459","(("},{"0.39016547030769291432124216946","()"},{"0.53797553176991641432124283059","(a"},{"hw2650","(b"},{"0.39251068630255761432124317104","(c"},{"0.55784740601666271432124376455","(d"},{"bk9073","(e"},{"0.65135152544826271432124390045","(f"},{"0.98144014482386411432124448068","(g"},{"hw8773","(h"},{"0.68661252455785871432124536416","(i"},{"0.55506877251900731432124571610","(j"},{"lx9877","(k"},{"0.51902025728486481432124611813","(l"},{"nodes[n2459]","(m"},{"nodes[n2650]","(n"},{"nodes[n9073]","(o"},{"nodes[n8773]","(p"},{"nodes[n9877]","(q"},{"na289","(r"},{"na573","(s"},{"n2152","(t"},{"n1455","(u"},{"na871","(v"},{"0","(w"},{"time","(x"},{"typeDefinitions[CoAPGroup/0.0.1]","(y"},{"0.71672565443441271432124688330","(z"},{"hwa289","(A"},{"0.96873560803942381432124721324","(B"},{"0.71419032104313371432124762480","(C"},{"bka573","(D"},{"0.92470887908712031432124815954","(E"},{"0.123657654505223041432124851613","(F"},{"lx2152","(G"},{"0.96535480115562681432124871253","(H"},{"0.71556304884143171432124953253","(I"},{"bk1455","(J"},{"0.54490464390255511432124999589","(K"},{"0.084562670206651091432125029664","(L"},{"bka871","(M"},{"0.75130812753923241432125161541","(N"},{"nodes[na289]","(O"},{"nodes[na573]","(P"},{"nodes[n2152]","(Q"},{"nodes[n1455]","(R"},{"nodes[na871]","(S"},{"BXX5q3eV","(T"},{"node0","(U"},{"comp457","(V"},{"0.68263587262481451424775426644","(W"},{"6","(X"},{"front","(Y"},{"m3-XX.lille.iotlab.info","(Z"},{"9o86ZdvQ",")["},{"CoAPGroup",")]"},{"deployUnits[//kevoree-group-coap/0.0.1]",")("},{"hytCmvXU","))"},{"path",")a"},{"number",")b"},{"proxy_port",")c"},{"20000",")d"},{"5",")e"},{"kevoree-group-coap",")f"},{"nodes[node0]",")g"},{"contiki-node",")h"},{"QoMNUckL",")i"}};

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
  char buf[COMPRESS_BUFFER_SIZE];

  int n, j; //buffer indexes
  const char* code;
  size_t size;

  int out_size = 0;
  char attr[100];
  int attr_index = 0;
  bool in_attribute = false;

  while ((n = cfs_read(in, buf, COMPRESS_BUFFER_SIZE)) >0)
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
        out_size += cfs_write(out, tmp_buf, strlen(tmp_buf));
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
        if (buf[j] != ' ') {
          char c[1];
          sprintf(c, "%c", buf[j]); //avoid encoding error
          out_size += cfs_write(out, c, sizeof(c));
        }
      }

    }
  }
  return out_size;
}

int compress(char* input, char* output)
{
  int in_file = cfs_open(input, CFS_READ);
  int out_file = cfs_open(output, CFS_WRITE | CFS_APPEND);
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
  int out_file = cfs_open(output, CFS_WRITE | CFS_APPEND);
  int out_size = -1;

  if (in_file >= 0 && out_file >= 0) {
    out_size = substitutionCode(in_file, out_file, DECOMPRESS);
  }

  if (in_file >= 0)
    cfs_close(in_file);
  if (out_file >= 0)
    cfs_close(out_file);
  return out_size;
}
