#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// BEGIN DEBUG INCLUDE
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
// END DEBUG INCLUDE

#define ALPHABET_SIZE 196

#define COMPRESS 0
#define DECOMPRESS 1

//TODO remove extra space and new line characters
const char *const code_attr[ALPHABET_SIZE][2] =
{
  {"eClass","a"},
  {"name","aa"},
  {"values","ab"},
  {"generated_KMF_ID","ac"},
  {"value","ad"},
  {"false","ae"},
  {"true","af"},
  {"started","ag"},
  {"metaData","ah"},
  {"typeDefinition","ai"},
  {"dictionary","aj"},
  {"fragmentDictionary","ak"},
  {"org.kevoree.Dictionary","al"},
  {"org.kevoree.DictionaryValue","am"},
  {"provided","an"},
  {"required","ao"},
  {"port","ap"},
  {"1234","aq"},
  {"version","ar"},
  {"0.0.1","as"},
  {"groups","at"},
  {"org.kevoree.NetworkProperty","au"},
  {"org.kevoree.ContainerNode","av"},
  {"hosts","aw"},
  {"host","ax"},
  {"components","ay"},
  {"networkInformation","az"},
  {"org.kevoree.NetworkInfo","a0"},
  {"ip","a1"},
  {"org.kevoree.ComponentInstance","a2"},
  {"namespace","a3"},
  {"org.kevoree.FragmentDictionary","a4"},
  {"local","a5"},
  {"interval","a6"},
  {"url","a7"},
  {"1000","a8"},
  {"Inti","a9"},
  {"abstract","b"},
  {"bean","ba"},
  {"factoryBean","bb"},
  {"deployUnit","bc"},
  {"superTypes","bd"},
  {"dictionaryType","be"},
  {"org.kevoree.DictionaryAttribute","bf"},
  {"fragmentDependant","bg"},
  {"optional","bh"},
  {"state","bi"},
  {"datatype","bj"},
  {"defaultValue","bk"},
  {"org.kevoree.DeployUnit","bl"},
  {"groupName","bm"},
  {"hashcode","bn"},
  {"type","bo"},
  {"ce","bp"},
  {"genericTypes","bq"},
  {"requiredLibs","br"},
  {"org.kevoree.DictionaryType","bs"},
  {"attributes","bt"},
  {"int","bu"},
  {"org.kevoree.ComponentType","bv"},
  {"kev_contiki","bw"},
  {"org.kevoree.TypeLibrary","bx"},
  {"subTypes","by"},
  {"3dddTFpd","bz"},
  {"hello_world","b0"},
  {"n1759","b1"},
  {"nb772","b2"},
  {"n2151","b3"},
  {"n2251","b4"},
  {"n1559","b5"},
  {"blink","b6"},
  {"sensing","b7"},
  {"sieve","b8"},
  {"nb870","b9"},
  {"n1459","c"},
  {"nb771","ca"},
  {"org.kevoree.ContainerRoot","cb"},
  {"nodes","cc"},
  {"typeDefinitions","cd"},
  {"org.kevoree.NodeType","ce"},
  {"ContikiNode","cf"},
  {"org.kevoree.GroupType","cg"},
  {"repositories","ch"},
  {"dataTypes","ci"},
  {"libraries","cj"},
  {"ContikiLib","ck"},
  {"Default","cl"},
  {"hubs","cm"},
  {"mBindings","cn"},
  {"deployUnits","co"},
  {"org.kevoree.library.c","cp"},
  {"nodeNetworks","cq"},
  {"org.kevoree.Group","cr"},
  {"group0","cs"},
  {"subNodes","ct"},
  {"CtHbJw37","cu"},
  {"UDPGroup","cv"},
  {"string","cw"},
  {"org.kevoree.Repository","cx"},
  {"0.44028098252601921432122800473","cy"},
  {"bk1759","cz"},
  {"0.244668840663507581432123271370","c0"},
  {"0.280824760207906371432123339315","c1"},
  {"hwb772","c2"},
  {"0.125406553270295261432123361237","c3"},
  {"0.56672404310666021432123659775","c4"},
  {"bk2151","c5"},
  {"0.96605161810293791432123715642","c6"},
  {"lo","c7"},
  {"n3554","c8"},
  {"n9989","c9"},
  {"0.103688640287145971432125140977","d"},
  {"hw2251","da"},
  {"0.478929068660363551432125174141","db"},
  {"0.82558026234619321432125237187","dc"},
  {"hw1559","dd"},
  {"0.25617986987344921432125299046","de"},
  {"0qE5TygS","df"},
  {"oW3cS6Ts","dg"},
  {"afOM93SD","dh"},
  {"18000","di"},
  {"count","dj"},
  {"100","dk"},
  {"0.26062655518762771432122795371","dl"},
  {"0.133597850101068621432123764697","dm"},
  {"hwb870","dn"},
  {"0.73711027111858131432123794332","do"},
  {"0.70933706359937791432123833544","dp"},
  {"lx1459","dq"},
  {"0.70674828044138851432123848469","dr"},
  {"0.85399847291409971432123883565","ds"},
  {"hwb771","dt"},
  {"0.93829955207183961432123899340","du"},
  {"n2459","dv"},
  {"n2650","dw"},
  {"n9073","dx"},
  {"n8773","dy"},
  {"n9877","dz"},
  {"0.90601870790123941432123946266","d0"},
  {"lx3554","d1"},
  {"0.530610746005551432124082140","d2"},
  {"0.40597744332626461432124065761","d3"},
  {"hw9989","d4"},
  {"0.93384177354164421432124102077","d5"},
  {"0.0915601672604681432124195820","d6"},
  {"bk2459","d7"},
  {"0.39016547030769291432124216946","d8"},
  {"0.53797553176991641432124283059","d9"},
  {"hw2650","e"},
  {"0.39251068630255761432124317104","ea"},
  {"0.55784740601666271432124376455","eb"},
  {"bk9073","ec"},
  {"0.65135152544826271432124390045","ed"},
  {"0.98144014482386411432124448068","ee"},
  {"hw8773","ef"},
  {"0.68661252455785871432124536416","eg"},
  {"0.55506877251900731432124571610","eh"},
  {"lx9877","ei"},
  {"0.51902025728486481432124611813","ej"},
  {"na289","ek"},
  {"na573","el"},
  {"n2152","em"},
  {"n1455","en"},
  {"na871","eo"},
  {"0","ep"},
  {"time","eq"},
  {"0.71672565443441271432124688330","er"},
  {"hwa289","es"},
  {"0.96873560803942381432124721324","et"},
  {"0.71419032104313371432124762480","eu"},
  {"bka573","ev"},
  {"0.92470887908712031432124815954","ew"},
  {"0.123657654505223041432124851613","ex"},
  {"lx2152","ey"},
  {"0.96535480115562681432124871253","ez"},
  {"0.71556304884143171432124953253","e0"},
  {"bk1455","e1"},
  {"0.54490464390255511432124999589","e2"},
  {"0.084562670206651091432125029664","e3"},
  {"bka871","e4"},
  {"0.75130812753923241432125161541","e5"},
  {"BXX5q3eV","e6"},
  {"node0","e7"},
  {"comp457","e8"},
  {"0.68263587262481451424775426644","e9"},
  {"6","f"},
  {"front","fa"},
  {"9o86ZdvQ","fb"},
  {"CoAPGroup","fc"},
  {"hytCmvXU","fd"},
  {"path","fe"},
  {"number","ff"},
  {"proxy_port","fg"},
  {"20000","fh"},
  {"5","fi"},
  {"QoMNUckL","fj"}
};

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

void substitutionCode(FILE* in, FILE* out, int mode)
{
  char c, attr[100];
  int index = 0;
  bool in_attribute = false;

  while ((c=fgetc(in)) != EOF)
  {
    // begining of an attributae name
    if (c =='"' && !in_attribute)
    {
      in_attribute = true;
    }
    // end of an attribute name
    else if (c == '"' && in_attribute)
    {
      in_attribute = false;
      attr[index] = '\0';
      index = 0; //reset attribute index
      if (mode == COMPRESS)
	fprintf(out, "\"%s\"", getSubsituteCode(attr));
      else if (mode == DECOMPRESS)
	fprintf(out, "\"%s\"", getAttributeCode(attr));
      else
	exit(-1);
    }
    // inside an attribute
    else if (c != '"' && in_attribute)
    {
      attr[index] = c;
      index++;
    }
    else
    {
      fputc(c, out);
    }
  }
}

void compress(char* input, char* output)
{
  FILE *in = fopen(input, "r");
  FILE *out = fopen(output, "w");

  if (in != NULL && out != NULL)
    substitutionCode(in, out, COMPRESS);

  if (in != NULL)
    fclose(in);
  if (out != NULL)
    fclose(out);
}

void decompress(char* input, char* output)
{
  FILE* in  = fopen(input, "r");
  FILE* out = fopen(output, "w");

  if (in != NULL && out != NULL)
    substitutionCode(in, out, DECOMPRESS);

  if (in != NULL)
    fclose(in);
  if (out != NULL)
    fclose(out);
}

int main(int argc, char *argv[])
{
  if (argc == 4)
  {
    char* source = argv[1];
    char* compressed = argv[2];
    char* decompressed = argv[3];

    // Compress model
    //compress("../models/7nodes1component.json", "7nodes1component.compress.json");
    compress(source, compressed);

    // Decompress model
    //decompress("7nodes1component.compress.json", "7nodes1component.decompress.json");
    decompress(compressed, decompressed);

    // BEGIN STAT INSTRUCTION
    struct stat buf;
    stat(source, &buf);
    int sourc_size = buf.st_size;
    stat(compressed, &buf);
    int comp_size = buf.st_size;
    float ratio = comp_size/(float)sourc_size;
    printf("%d, %d\n", sourc_size, comp_size);
    // END STAT INSTRUCTION
  }
  return 0;
}
