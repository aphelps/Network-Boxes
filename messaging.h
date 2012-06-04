/*******************************************************************************
 * Message format version 1
 * +--------+--------+--------+--------+ 
 * | marker |version |     length      | // Constant across versions
 * +--------+--------+--------+--------+
 * |      source     |   destination   |
 * +--------+--------+--------+--------+
 * |     sequence    |msg type| flags  |
 * +--------+--------+--------+--------+ 
 * |    checksum     |   data ...      |            
 * +--------+--------+--------+--------+
 * |  data ...                | marker |
 * +--------+--------+--------+--------+
 ******************************************************************************/

#include <Arduino.h>

/* Messaging data types */
typedef uint16_t node_id_t;
typedef uint16_t length_t;
typedef uint16_t checksum_t;
typedef uint8_t  status_t;

/* Beginning and end delimitors */
#define MSG_START_MARKER 0xF3
#define MSG_END_MARKER   0xE3

/* Current message version */
#define MSG_VERSION 1

/* Message types */
typedef enum {
  MSG_TYPE_NONE,
  MSG_TYPE_MAX
} msg_type_t;

#define MSG_MAX_LENGTH 64    /* Maximum length of a message */

#define MSG_ID_BROADCAST 0xFFFF /* Broadcast destination ID */

/* Message flags */
#define MSG_FLAG_ACK 0x1

/* Message header */
typedef struct msg_header {
  uint8_t  marker;      /* Message start indicator */
  uint8_t  version;
  length_t length;      /* Length of entire message, including the header */

  node_id_t source;      /* Source ID */
  node_id_t destination; /* Destination ID */
  uint16_t sequence;    /* Sequence number */
  uint8_t type;
  uint8_t flags;
  checksum_t checksum;    /* Checksum of entire message */
} msg_header_t; /* 14 bytes */

#define MSG_PREFIX_LEN 4

/* Read and return a valid packet */
length_t messaging_get_packet(uint8_t *buffer, length_t buffer_length);

/* Return a packet to be processed */
length_t messaging_return_packet(uint8_t *buffer, length_t buffer_length,
                                 uint16_t local_id);

/* Send an acknowledgement */
status_t messaging_ack_packet(msg_header_t *hdr,
                              node_id_t source,
                              uint8_t *data,
                              uint8_t data_len);

/*
 * #define's for optional functionality
 */

/* If set the packet code will watch for unexpected end markers followed by
 * start markers */
#define MSG_WATCH_MARKERS
