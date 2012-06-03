/*
 * Box networking
 */

#include "messaging.h"

uint16_t local_ID = 0x4321;

void setup() {
  
}

void loop() {
  unsigned char buffer[MSG_MAX_LENGTH];
  unsigned short read;

  read = messaging_return_packet(buffer, sizeof (buffer), local_ID);
}
