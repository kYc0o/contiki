
#include "requests.h"

#include <string.h>
#include <stdlib.h>

struct MapEntry* find_by_artifact(list_t list, const char* a)
{
	struct MapEntry* r = NULL;
	for (r=list_head(list) ; r != NULL ; r=list_item_next(r)) {
		if (strcmp(r->artifact, a) == 0) {
			return r;
		}
	}
	return r;
}

static uint16_t
crc16_add(unsigned char b, uint16_t acc)
{
  /*
    acc  = (unsigned char)(acc >> 8) | (acc << 8);
    acc ^= b;
    acc ^= (unsigned char)(acc & 0xff) >> 4;
    acc ^= (acc << 8) << 4;
    acc ^= ((acc & 0xff) << 4) << 1;
  */

  acc ^= b;
  acc  = (acc >> 8) | (acc << 8);
  acc ^= (acc & 0xff00) << 4;
  acc ^= (acc >> 8) >> 4;
  acc ^= (acc & 0xff00) >> 5;
  return acc;
}
/*---------------------------------------------------------------------------*/
static unsigned short
crc16_data(const unsigned char *data, int len, uint16_t acc)
{
  int i;
  
  for(i = 0; i < len; ++i) {
    acc = crc16_add(*data, acc);
    ++data;
  }
  return acc;
}

/* calculate the total length of a message */
uint8_t total_len(struct KevoreePacket* pkt)
{
	return pkt->len + ((char*)&pkt->data - (char*)pkt); 
}

/**
	\brief Process an incoming message
	\ret   0 if everything is Ok, a negative value otherwise
 */
enum MessageProcessingErroCode
process_message(
	const char* source_address, 
	uint16_t msg_len, 
	const uint8_t* msg, 
	const struct RequestProcessingCallback* callbacks)
{
	uint16_t crcC;
	struct KevoreePacket* pkt = (struct KevoreePacket*)msg;
	
	/* first check if it is a vlid packet (no transmission errors) */
	uint16_t crc = pkt->crc;
	pkt->crc = 0;
	crcC = crc16_data((unsigned char*)pkt, msg_len, 0);
#if 0
	printf("Data received from ");
	uip_debug_ipaddr_print(sender_addr);
	printf(" on port %d from port %d with length %d, crc0 %d and crc1 %d\n",
			receiver_port, sender_port, datalen, crc, crcC);
#endif

	/* stop processecing if the packet has errors */
	if (crc != crcC) {
		return WRONG_CRC;
	}
	
	/* process message */
	if (pkt->cmd == RESPONSE_SUMMARY) {
		callbacks->onNewSummary(pkt->data.summary.session_id, pkt->data.summary.nr_chunks);
	}
	else if (pkt->cmd == RESPONSE_CHUNK) {
		callbacks->onNewChunk(pkt->data.chunk.chunk_id, pkt->len - sizeof(uint16_t), pkt->data.chunk.data);
	}
	else if (pkt->cmd == RESPONSE_ACK_LOOKING_FOR_PACKET) {
		callbacks->onAckArtifactRequest();
	}
	else if (pkt->cmd == GET_ARTIFACT) {
		callbacks->onArtifactRequest(source_address, pkt->data.get_artifact.artifact);
	}
	else if (pkt->cmd == GET_CHUNK) {
		callbacks->onChunkRequest(pkt->data.get_chunk.session_id, pkt->data.get_chunk.chunk_id);
	}
	else {
		int tt = pkt->cmd;
		printf("Unknown type : %d\n", tt);
		return UNKNOWN_MESSAGE_TYPE;
	}
	return NONE;
}

void
build_get_artifact_packet(struct KevoreePacket* pkt, const char* artifact)
{
	pkt->crc = 0;
	pkt->cmd = GET_ARTIFACT;
	pkt->len = strlen(artifact) + 1;
	strcpy(pkt->data.get_artifact.artifact, artifact);
	pkt->crc = crc16_data((unsigned char*)pkt, total_len(pkt), 0);
}

void
build_get_chunk_packet(struct KevoreePacket* pkt, uint16_t session_id, uint16_t chunk_id)
{
	pkt->crc = 0;
	pkt->cmd = GET_CHUNK;
	pkt->len = sizeof(uint16_t)*2;
	pkt->data.get_chunk.session_id = session_id;
	pkt->data.get_chunk.chunk_id = chunk_id;
	pkt->crc = crc16_data((unsigned char*)pkt, total_len(pkt), 0);
}

void
build_summary_packet(struct KevoreePacket* pkt, uint16_t session_id, uint16_t nr_chunks)
{
	pkt->crc = 0;
	pkt->cmd = RESPONSE_SUMMARY;
	pkt->len = sizeof(uint16_t)*2;
	pkt->data.summary.session_id = session_id;
	pkt->data.summary.nr_chunks = nr_chunks;
	pkt->crc = crc16_data((unsigned char*)pkt, total_len(pkt), 0);	
}

void
build_chunk_packet(struct KevoreePacket* pkt, uint16_t chunk_id, uint16_t msg_len, const uint8_t* msg)
{
	pkt->crc = 0;
	pkt->cmd = RESPONSE_CHUNK;
	pkt->len = sizeof(uint16_t) + msg_len;
	pkt->data.chunk.chunk_id = chunk_id;
	memcpy(pkt->data.chunk.data, msg, msg_len);
	pkt->crc = crc16_data((unsigned char*)pkt, total_len(pkt), 0);
}

void
build_ack_packet(struct KevoreePacket* pkt)
{
	pkt->crc = 0;
	pkt->cmd = RESPONSE_ACK_LOOKING_FOR_PACKET;
	pkt->len = 0;
	pkt->crc = crc16_data((unsigned char*)pkt, total_len(pkt), 0);
}

/* dealing with requests */
struct DeployUnitRequest*
find_request_by_source(list_t list, const char* source_address, const char* artifact)
{
	struct DeployUnitRequest* r;
	for (r = list_head(list) ; r != NULL ; r = list_item_next(r)) {
		if (strcmp(r->deployUnitName, artifact) == 0 && strcmp(r->source_address, source_address) == 0)
			return r;
	}
	return NULL;
}

struct DeployUnitRequest*
find_request_by_session(list_t list, uint16_t session_id)
{
	struct DeployUnitRequest* r;
	for (r = list_head(list) ; r != NULL ; r = list_item_next(r)) {
		if (r->session_id == session_id)
			return r;
	}
	return NULL;
}

struct DeployUnitRequest*
create_request(const char* source_address, const char* artifact, enum State initial_state)
{
	static uint16_t last_session_id = 1;
	struct DeployUnitRequest* r = (struct DeployUnitRequest*)malloc(sizeof(struct DeployUnitRequest));
	r->source_address = strdup(source_address);
	r->deployUnitName = strdup(artifact);
	r->session_id = last_session_id ++;
	r->state = initial_state;
	r->current_packet = 0;
	r->localFilename = NULL;
	r->fd = 0;
	r->nr_packets = 0;
	return r;
}

void
dispose_request(struct DeployUnitRequest* r)
{
	if (r->source_address != NULL) free(r->source_address);
	
	if (r->deployUnitName != NULL) free(r->deployUnitName);
	
	if (r->localFilename != NULL) free(r->localFilename);
	
	free(r);
}

