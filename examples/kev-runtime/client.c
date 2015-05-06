#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "myencode.h"

#include "cmds.h"

// known commands
int uploadfile (char* params[]);
int noArgumentCommand(char* params[]);
int oneArgumentCommand(char* params[]);
int twoArgumentsCommand(char* params[]);
int exitCommand (char* params[]);
int helpCommand (char* params[]);
Command commands[] = {
  {
    .name = "upload",
    .exec = &uploadfile
  },
  {
	.name = "createInstance",
	.exec = twoArgumentsCommand
  },
  {
    .name = "ls",
    .exec = &noArgumentCommand
  },
  {
    .name = "loadelf",
    .exec = &oneArgumentCommand
  },
  {
    .name = "startInstance",
    .exec = &oneArgumentCommand
  },
  {
    .name = "stopInstance",
    .exec = &oneArgumentCommand
  },
  {
	.name = "removeInstance",
	.exec = oneArgumentCommand
  },
  {
    .name = "cat",
    .exec = &oneArgumentCommand
  },
  {
    .name = "rm",
    .exec = &oneArgumentCommand
  },
  {
	.name = "pushModel",
	.exec = &noArgumentCommand
  },
  {
    .name = "format",
    .exec = &noArgumentCommand
  },
  {
    .name = "exit",
    .exec = &exitCommand
  },
  {
    .name = "help",
    .exec = &helpCommand
  }
};

// global variables
static int sockfd = 0;

// global methods
void send_data(char* msg);

// implementing commands
int uploadfile (char* params[])
{
  unsigned char buf[4096];
  char out[4096 + 1000];
  int n;
  sprintf(buf, "upload %s\n", params[1]);
  send_data(buf);
  int fd = open(params[1], O_RDONLY);
  while ((n = read(fd, buf, 4096)) > 0) {
    int r = encode(buf, n, out);
    out[r] = 0;
    send_data(out);
    send_data("\n");
  }
  close(fd);
  send_data("endupload\n");
  return 1;
}

static char*
getDeployUnitLocalPath(const char * deployUnitName)
{
	FILE* fp;
	char * line = NULL;
	size_t len = 0;
    ssize_t read;

	fp = fopen("kev-components/repository.repo", "r");
	if (fp == NULL)
	   return NULL;

	while ((read = getline(&line, &len, fp)) != -1) {
		if (strstr(line, deployUnitName) == line) {
			fclose(fp);
			char* retTmp = line + strlen(deployUnitName) + 1;
			char* ret = retTmp + strlen(retTmp);
			ret --;
			*ret = 0;
			ret = strdup(retTmp);
			free(line);
			return ret;
		}
	}

	fclose(fp);
	if (line)
	   free(line);
	return NULL;
}

int uploadUnit (char* params[])
{
  unsigned char buf[4096];
  char out[4096 + 1000];
  char b[100];
  int n;

  /* get Deploy Unit Location */
  char* deployUnitLocalPath = getDeployUnitLocalPath(params[1]);
  if (!deployUnitLocalPath) {
	fprintf(stderr, "CLIENT-ERR: couldn't find local deploy unit %s\n", params[1]);
	return 0;
  }
  else printf("Found in %s\n", deployUnitLocalPath);

  sprintf(b, "kev-components/%s", deployUnitLocalPath);
  int fd = open(b, O_RDONLY);
  if (fd < 0 ) {
  	fprintf(stderr, "CLIENT-ERR: couldn't open file %s which contains the deploy unit\n", b);
	return 0;
  }
  sprintf(buf, "uploadUnit %s\n", "pepe");
  send_data(buf);
  while ((n = read(fd, buf, 4096)) > 0) {
    int r = encode(buf, n, out);
    out[r] = 0;
    send_data(out);
    send_data("\n");
  }
  close(fd);
  printf("Ok, everything was sent\n");
  send_data("enduploadUnit\n");
  return 1;
}

int noArgumentCommand(char* params[])
{
  send_data(params[0]); send_data("\n");
  return 1;
}
int oneArgumentCommand(char* params[])
{
  send_data(params[0]); send_data(" "); send_data(params[1]); send_data("\n");
  return 1;
}
int twoArgumentsCommand(char* params[])
{
  send_data(params[0]); send_data(" "); send_data(params[1]); send_data(" "); send_data(params[2]); send_data("\n");
  return 1;
}
int helpCommand (char* params[])
{
  int nb = sizeof(commands) / sizeof(Command);
  for (int i = 0 ; i < nb ; i++)
    printf("Command: %s\n", commands[i].name);
  return 1;
}
int exitCommand (char* params[])
{
  close(sockfd);
  exit(0);
}

// working with the sockets
void error(const char *msg)
{
  perror(msg);
  fprintf(stderr, "\n");
  exit(1);
}

void send_data(char* msg)
{
  struct timespec delay = {
    .tv_sec  = 0,
    .tv_nsec = 300000000
  }; // 300 milliseconds
  char* s = msg;
  int ended = 0;
  while (!ended) {
    char* sTmp = strstr(s, "\n");
    if (sTmp) {
      sTmp ++;
      write(sockfd, s, (sTmp - s));
      nanosleep(&delay, NULL);
      s = sTmp;
    }
    else {
      if (strlen(s)) {
        write(sockfd, s, strlen(s));
        nanosleep(&delay, NULL);
      }
      ended = 1;
    }
  }
}

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define TOKEN_START "__012345"
#define TOKEN_END "543210__"

/* I am too lazy , hehehe*/
int total = 0;
char hugeBuffer[40*1024*1024];

int isDeployUnitRequest(char* buff, int len, char* deployUnit)
{
	buff[len] = 0;
	strcpy(&hugeBuffer[total], buff);
	total += len;
	hugeBuffer[total] = 0;
	
	char* init = strstr(hugeBuffer, TOKEN_START);
	if (init) {
		char* end = strstr(hugeBuffer, TOKEN_END);
		if (end) {
			init += 1 + strlen(TOKEN_START);
			end --;
			*end = 0;
			strcpy(deployUnit, init);
			// clean te buffer
			total = 0;
			return 1;
		}	
	}
	return 0;
}

void *answering_to_remote(void *x_void_ptr)
{
	char* params[] = {
		"uploadUnit",
		""
	};
	
	params[1] = (char*) x_void_ptr;
	uploadUnit(params);
}

void receive_data()
{
  char buff[101];
  char deployUnitName[100];
  int n = read(sockfd,buff,100);
  while (n > 0) {
	if (!isDeployUnitRequest(buff, n, deployUnitName))
		
    	for (int i = 0 ; i < n ; i++)
      		printf(KGRN "%c", buff[i]);
	else {
		printf(KNRM "\nWell ... someone is requesting %s; so let's upload the file\n", deployUnitName);
		pthread_t receiver;
		/* create a second thread which executes inc_x(&x) */
		if (pthread_create(&receiver, NULL, answering_to_remote, strdup(deployUnitName))) {

		}
	}
    n = read(sockfd,buff,100);
  }
}

void *reading_from_remote(void *x_void_ptr)
{

  receive_data();

  /* the function must return something - NULL will do */
  return NULL;
}


static int isSeparator(char c) {
  return c == '\t' || c == '\n' || c == ' ';
}


void parse_args(int argc, char* argv[], char **hostname, char **port)
{
  /*  client <hostname> <port> */
  char *endptr;

  if (3 != argc)
    goto usage;

  *hostname = argv[1];
  *port = argv[2];

  // Check integer
  strtol(argv[2], &endptr, 10);
  if ('\0' != *endptr) {
    fprintf(stderr, "Invalid port '%s'\n", argv[2]);
    goto usage;
  }
  return;

usage:
  fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
  exit(EINVAL);
}


int connect_server(char *hostname, char *port)
{
  int ret;
  int fd = -1;
  struct addrinfo *serverinfo, *p;
  struct addrinfo hints = {0};
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;


  // Resolve hostname
  if ((ret = getaddrinfo(hostname, port, &hints, &serverinfo))) {
    fprintf(stderr, "getaddrinfo: %s", gai_strerror(ret));
    error("Hostname resolution failed");
  }

  // Loop through all results and connect to the first possible
  for (p = serverinfo; p != NULL; p = p->ai_next) {
    if (-1 == (fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))) {
      perror("client: socket");
      continue;
    }

    if (-1 == connect(fd, p->ai_addr, p->ai_addrlen)) {
      close(fd);
      perror("client: connect");
      continue;
    }

    // Connected
    return fd;
  }

  error("Connection failed");
  return -1;
}


int main(int argc, char* argv[])
{
  char *hostname;
  char *port;

  parse_args(argc, argv, &hostname, &port);
  fprintf(stderr, "Connecting to %s:%s\n", hostname, port);

  sockfd = connect_server(hostname, port);


  printf(KNRM "> ");
  fflush(stdout);

  pthread_t receiver;
  /* create a second thread which executes inc_x(&x) */
  if (pthread_create(&receiver, NULL, reading_from_remote, NULL)) {
    fprintf(stderr, "Error creating thread\n");
    return 1;
  }

  char command[100];
  const int nbCommands = sizeof(commands) / sizeof(Command);
  char* params[20];
  for (int i = 0 ; i < 20 ; i ++)
    params[i] = (char*)malloc(100*sizeof(char));

  while (1) {
    fgets(command, 100, stdin);
    char* tmp = command;
    int idx = 0;
    while ((*tmp)) {
      int tt = 0;
      while ((*tmp) && !isSeparator(*tmp)) {
        params[idx][tt++] = (*tmp);
        tmp++;
      }
      params[idx][tt] = 0;
      if (isSeparator(*tmp))
        tmp++;
      idx ++;

    }
    for (int i = 0 ; i < nbCommands ; i++) {
      if (strcmp(commands[i].name, params[0]) == 0) {
        commands[i].exec(params);
      }
    }
    printf(KNRM "> ");
    fflush(stdout);
  }
  close(sockfd);
  return 0;
}
