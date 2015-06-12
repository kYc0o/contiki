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
const char *const code_attr[ALPHABET_SIZE][2] ={{"eClass","a"},{"name","aa"},{"values","ab"},{"generated_KMF_ID","ac"},{"value","ad"},{"false","ae"},{"true","af"},{"started","ag"},{"metaData","ah"},{"typeDefinition","ai"},{"dictionary","aj"},{"fragmentDictionary","ak"},{"org.kevoree.Dictionary","al"},{"org.kevoree.DictionaryValue","am"},{"provided","an"},{"required","ao"},{"port","ap"},{"1234","aq"},{"version","ar"},{"0.0.1","as"},{"groups","at"},{"org.kevoree.NetworkProperty","au"},{"org.kevoree.ContainerNode","av"},{"hosts","aw"},{"host","ax"},{"groups[group0]","ay"},{"components","az"},{"networkInformation","a0"},{"org.kevoree.NetworkInfo","a1"},{"org.kevoree.ComponentInstance","a2"},{"namespace","a3"},{"org.kevoree.FragmentDictionary","a4"},{"local","a5"},{"interval","a6"},{"url","a7"},{"1000","a8"},{"Inti","a9"},{"abstract","b"},{"bean","ba"},{"factoryBean","bb"},{"deployUnit","bc"},{"superTypes","bd"},{"dictionaryType","be"},{"org.kevoree.DictionaryAttribute","bf"},{"fragmentDependant","bg"},{"optional","bh"},{"state","bi"},{"datatype","bj"},{"defaultValue","bk"},{"org.kevoree.DeployUnit","bl"},{"groupName","bm"},{"hashcode","bn"},{"type","bo"},{"genericTypes","bp"},{"requiredLibs","bq"},{"org.kevoree.DictionaryType","br"},{"attributes","bs"},{"int","bt"},{"org.kevoree.ComponentType","bu"},{"kev_contiki","bv"},{"org.kevoree.TypeLibrary","bw"},{"subTypes","bx"},{"3dddTFpd","by"},{"hello_world","bz"},{"n1759","b0"},{"nb772","b1"},{"n2151","b2"},{"n2251","b3"},{"n1559","b4"},{"blink","b5"},{"sensing","b6"},{"sieve","b7"},{"nb870","b8"},{"n1459","b9"},{"nb771","c"},{"org.kevoree.ContainerRoot","ca"},{"nodes","cb"},{"typeDefinitions","cc"},{"org.kevoree.NodeType","cd"},{"ContikiNode","ce"},{"org.kevoree.GroupType","cf"},{"repositories","cg"},{"dataTypes","ch"},{"libraries","ci"},{"ContikiLib","cj"},{"Default","ck"},{"hubs","cl"},{"mBindings","cm"},{"deployUnits","cn"},{"org.kevoree.library.c","co"},{"nodeNetworks","cp"},{"org.kevoree.Group","cq"},{"group0","cr"},{"subNodes","cs"}};

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

