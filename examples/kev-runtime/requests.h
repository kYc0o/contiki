#ifndef __REQUEST_DEPLOY_UNITS__
#define __REQUEST_DEPLOY_UNITS__

#include <stdint.h>

#include "lib/list.h"

/* mapping from artifact name to file name*/
struct MapEntry {
	struct MapEntry* next;
	char* artifact;
	char* filename;
};

struct MapEntry* find_by_artifact(list_t list, const char* a);

#define REQUEST_PACKET_SIZE 58

/* a request may be in many states */
enum State {
	DONE,
	WAITING_FOR_SUMMARY,
	PASSIBALY_WAITING_FOR_SUMMARY,
	RECEIVING_CHUNKS,
	SENDING_SUMMARY,
	SENDING_CHUNKS,
	WAITING_FOR_LOCATION
};

/* represent a request */
struct DeployUnitRequest {
	/* if we want to put them on a list */
	struct DeployUnitRequest* next;
	/* processing state */
	enum State state;
	/* source address of this request (can be NULL) */
	char* source_address;
	/* deploy unit being requested */
	char* deployUnitName;
	/* local file with the deploy unit */
	char* localFilename;
	/* a file descriptor to the local file with the deploy unit */
	int fd;
	/* session id: used in multithread servers */
	uint16_t session_id;
	/* current packet being processed */
	uint16_t current_packet;
	/* number of packets in the file */
	uint16_t nr_packets;
};

/* the request serving protocol has many message types */
enum MessageType {
	/* command types */
	GET_ARTIFACT=3, // the message request an artifact
	GET_CHUNK=4, // the message request a packet
	
	/* response types */
	RESPONSE_SUMMARY=5, // the message is a response to GET_ARTIFACT when the artifact is locally available
	RESPONSE_CHUNK=6, // the message is a response to GET_CHUNK
	RESPONSE_ACK_LOOKING_FOR_PACKET=7 // the message is a response to GET_ARTIFACT when the artifact must be requested to another server
};


/* general packet */
struct KevoreePacket {
	uint16_t crc;
	uint8_t cmd;
	/* length of the data part (what is left) */
	uint8_t len;
	union {
		/* general packet */
		uint8_t data[REQUEST_PACKET_SIZE];
		/* used for GET_ARTIFACT messages */
		struct {
			char artifact[REQUEST_PACKET_SIZE];
		} get_artifact;
		/* summary packet */
		struct {
			uint16_t session_id;
			uint16_t nr_chunks;
		} summary;
		/* used to request a packet */
		struct {
			uint16_t session_id;
			uint16_t chunk_id;
		} get_chunk;
		/* fiel chunk */
		struct {
			uint16_t chunk_id;
			uint8_t data[REQUEST_PACKET_SIZE];
		} chunk;
	} data;
	
};

/* calculate the total length of a message */
uint8_t total_len(struct KevoreePacket* pkt);

/* contains the callbacks to process different message types */
struct RequestProcessingCallback {
	void (*onNewSummary)(uint16_t, uint16_t);
	void (*onNewChunk)(uint16_t, uint16_t, const uint8_t*);
	void (*onAckArtifactRequest)(void);
	void (*onArtifactRequest)(const char*, const char*);
	void (*onChunkRequest)(uint16_t, uint16_t);
};

enum MessageProcessingErroCode {
	NONE = 0,
	WRONG_CRC = 1,
	UNKNOWN_MESSAGE_TYPE = 2
};

/**
	\brief Process an incoming message
	\ret   0 if everything is Ok, a negative value otherwise
 */
enum MessageProcessingErroCode process_message(const char* source_address, uint16_t msg_len, const uint8_t* msg, const struct RequestProcessingCallback* callbacks);

/* create packets */
void build_get_artifact_packet(struct KevoreePacket* dst, const char* artifact);
void build_get_chunk_packet(struct KevoreePacket* dst, uint16_t session_id, uint16_t chunk_id);
void build_summary_packet(struct KevoreePacket* dst, uint16_t session_id, uint16_t nr_chunks);
void build_chunk_packet(struct KevoreePacket* dst, uint16_t chunk_id, uint16_t msg_len, const uint8_t* msg);
void build_ack_packet(struct KevoreePacket* dst);

/* dealing with requests */
struct DeployUnitRequest* find_request_by_source(list_t list, const char* source_address, const char* artifact);
struct DeployUnitRequest* find_request_by_session(list_t list, uint16_t session_id);
struct DeployUnitRequest* create_request(const char* source_address, const char* artifact, enum State initial_state);

#endif
