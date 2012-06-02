/*******************************************************************************
 * Message format version 1
 * +--------+--------+--------+--------+ 
 * |  magic |version |     length      | // Constant across versions
 * +--------+--------+--------+--------+
 * |   destination   |msg type| flags  |
 * +--------+--------+--------+--------+ 
 * |   checksum      | data ...        |            
 * +--------+--------+--------+--------+
 * |  data ...                | magic  |
 * +--------+--------+--------+--------+
 ******************************************************************************/

/* Beginning and end delimitors */
#define MSG_START_MAGIC 0xF3
#define MSG_END_MAGIC   0xE3

/* Current message version */
#define MSG_VERSION 1

/* Message types */
typedef enum {
  MSG_TYPE_NONE,
  MSG_TYPE_MAX
} msg_type_t;

#define MSG_MAX_LENGTH 64    /* Maximum length of a message */

#define MSG_BROADCAST 0xFFFF /* Broadcast destination ID */

/* Message header */
typedef struct msg_header {
  unsigned char  magic;        /* Message start indicator */
  unsigned char  version;
  unsigned short length;       /* Length of entire message, including the header */

  unsigned short destination;  /* Destination ID */
  unsigned char  type;
  unsigned char  flags;
  unsigned short checksum;     /* Checksum of entire message, including the header */
} msg_header_t; /* 10 bytes */

#define MSG_PREFIX_LEN 4

/* Compute a checksum for a data buffer */
unsigned short messaging_checksum(unsigned char *data, unsigned short length);

/* Validate a received message */
char messaging_validate(unsigned char *data, unsigned short length);
