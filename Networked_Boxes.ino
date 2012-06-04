/*
 * Box networking
 */

#include "messaging.h"

/* "Unique" ID for the node */
node_id_t local_ID = 0x4321;

void setup() {
  
}

void loop() {
  unsigned char buffer[MSG_MAX_LENGTH];
  msg_header_t *hdr = (msg_header_t *)buffer;
  unsigned short read;

  /* Receive a message directed to this node */
  read = messaging_return_packet(buffer, sizeof (buffer), local_ID);

  /* TODO: Do something */

  /* Send a response to the message */
  messaging_ack_packet(hdr, local_ID, NULL, 0);
}
