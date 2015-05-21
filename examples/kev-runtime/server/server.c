
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>

#include "requests.h"

#include "lib/list.h"

LIST(art_to_file);
LIST(requests_as_server);

static const char* reposotory_base_path = "kev-components";

int sockfd;

/* create artifact list */
static int
fill_repository()
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char path[255];
	
	sprintf(path, "%s/repository.repo", reposotory_base_path);

	fp = fopen(path, "r");
	if (fp == NULL)
	   exit(EXIT_FAILURE);

	while ((read = getline(&line, &len, fp)) != -1) {
		char* tmp = strstr(line, "\n");
		*tmp = 0;
		tmp = strstr(line, " ");
		*tmp = 0;
		tmp++;
		
		if (find_by_artifact(art_to_file, line) == NULL) {
			
		
			struct MapEntry* entry = (struct MapEntry*)malloc(sizeof(struct MapEntry));
			sprintf(path, "%s/%s", reposotory_base_path, tmp);
			entry->artifact = strdup(line);
			entry->filename = strdup(path);
			list_add(art_to_file, entry);
			printf("%s => %s\n", entry->artifact, entry->filename);
		}
	}

	free(line);
	
	fclose(fp);
}

static void server_onNewSummary(uint16_t session_id, uint16_t nr_chunks);
static void server_onNewChunk(uint16_t chunk_id, uint16_t len, const uint8_t* data);
static void server_onAckArtifactRequest(void);
static void server_onArtifactRequest(const char* source_address, const char* artifact);
static void server_onChunkRequest(uint16_t session_id, uint16_t chunk_id);

static const struct RequestProcessingCallback server_request_callbacks = {
	.onNewSummary = server_onNewSummary,
	.onNewChunk = server_onNewChunk,
	.onAckArtifactRequest = server_onAckArtifactRequest,
	.onArtifactRequest = server_onArtifactRequest,
	.onChunkRequest = server_onChunkRequest
};

static void
server_onNewSummary(uint16_t session_id, uint16_t nr_chunks)
{
	printf("ERROR: Everything is wrong %s:%d\n", __FILE__, __LINE__);
}
static void
server_onNewChunk(uint16_t chunk_id, uint16_t len, const uint8_t* data)
{
	printf("ERROR: Everything is wrong %s:%d\n", __FILE__, __LINE__);
}

static void
server_onAckArtifactRequest(void)
{
	printf("ERROR: Everything is wrong %s:%d\n", __FILE__, __LINE__);
}

static int
is_artifact_location_known(struct DeployUnitRequest* req)
{
	struct MapEntry* entry = find_by_artifact(art_to_file, req->deployUnitName);
	return entry != NULL;
}

static void
prepare_request_with_artifact(struct DeployUnitRequest* req, const char* artifact)
{
	struct MapEntry* entry = find_by_artifact(art_to_file, req->deployUnitName);
	req->fd = open(entry->filename, O_RDONLY);

	struct stat buf;
	stat(entry->filename, &buf);
	uint32_t s = buf.st_size;
	req->current_packet = 0;
	req->nr_packets = s / REQUEST_PACKET_SIZE;
	if (s % REQUEST_PACKET_SIZE) req->nr_packets++;
	
}

static void
server_onArtifactRequest(const char* source_address, const char* artifact)
{
	printf("Address '%s', Artifact: '%s'\n", source_address, artifact);
	
	
	struct DeployUnitRequest* req = find_request_by_source(requests_as_server, source_address, artifact);
	
	int is_new = req == NULL;
	
	if (req == NULL) {
		/* a new request */
		req = create_request(source_address, artifact, SENDING_SUMMARY);
		list_add(requests_as_server, req);
	}
	
	/* requesting the same */
	if (req->state == SENDING_SUMMARY || req->state == WAITING_FOR_LOCATION) {
		/* send summary to the source */
		
		if (is_artifact_location_known(req)) {
			printf("\tSending summary\n");
			/* filling the information about the packet */
			prepare_request_with_artifact(req, artifact);
			struct KevoreePacket pkt;
			build_summary_packet(&pkt, req->session_id, req->nr_packets);
			
			/* modifying the state */
			req->state = SENDING_CHUNKS;
			
			/* send response back */
			struct sockaddr_in6 cliaddr;
			cliaddr.sin6_family = AF_INET6;
			inet_pton(AF_INET6, source_address, &cliaddr.sin6_addr);
			cliaddr.sin6_port=htons(1234);
			sendto(sockfd, &pkt, total_len(&pkt), 0, (struct sockaddr *)&cliaddr,sizeof(cliaddr));
		}
		else {
		
			printf("Wrong <=======\n");
		
			while(1) {  };
		}
	}
}
static void
server_onChunkRequest(uint16_t session_id, uint16_t chunk_id)
{
	struct DeployUnitRequest* req = find_request_by_session(requests_as_server, session_id);
	
	if (req != NULL && req->state == SENDING_CHUNKS && req->current_packet <= chunk_id) {
	
		req->current_packet = chunk_id;
	
		printf("Session Id '%d', Chunk Id: '%d'. %d %d '%s'\n", session_id, chunk_id, req->state, req->current_packet, req->source_address);
		/* read from the file */
		uint8_t buf [REQUEST_PACKET_SIZE];
		lseek(req->fd, chunk_id * REQUEST_PACKET_SIZE, SEEK_SET);
		int n = read(req->fd, buf, REQUEST_PACKET_SIZE);
		
		/* build the packet */
		struct KevoreePacket pkt;
		build_chunk_packet(&pkt, chunk_id, n, buf);
		
		/* send response back */
		struct sockaddr_in6 cliaddr;
		cliaddr.sin6_family = AF_INET6;
		inet_pton(AF_INET6, req->source_address, &cliaddr.sin6_addr);
		cliaddr.sin6_port=htons(1234);
		sendto(sockfd, &pkt, total_len(&pkt), 0, (struct sockaddr *)&cliaddr,sizeof(cliaddr));
	}
}

int
main (int argc, char *argv[])
{
	list_init(art_to_file);
	fill_repository();
	
	list_init(requests_as_server);

	printf("Server started\n");
	
	int n;
	struct sockaddr_in6 servaddr;
	struct sockaddr_in6 cliaddr;
	socklen_t len;
	uint8_t mesg[1000];
	char address_name[80];

	sockfd = socket(AF_INET6,SOCK_DGRAM,0);
	
	if (sockfd < 0)  {
		printf("ERROR: no socket available\n");
		return -1;
	}

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin6_family = AF_INET6;
	servaddr.sin6_addr=in6addr_any; //IN6ADDR_ANY_INIT;//htonl(IN6ADDR_ANY_INIT);
	servaddr.sin6_port=htons(1234);
	if (bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) != 0) {
		printf("ERROR: no binding available %d:%s\n", errno, strerror(errno));
		return -1;
	} 

	/* loop for ever */
	for (;;)
	{
	  len = sizeof(cliaddr);
	  n = recvfrom(sockfd,(char*)mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
	  inet_ntop(AF_INET6, (struct sockaddr *)&cliaddr.sin6_addr, address_name, 80);
	  enum MessageProcessingErroCode err = process_message(address_name, n, mesg, &server_request_callbacks);
	  if (err != NONE) {
	  	printf("Error %d processing message\n", err);
	  }
	}

	return 0;
}
