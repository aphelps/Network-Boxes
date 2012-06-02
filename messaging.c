/*******************************************************************************
 *
 ******************************************************************************/

#include "messaging.h"

/*
 * Compute a checksum for a data buffer
 * TODO: Lookup a good & past CRC algorithm
 */
unsigned short
messaging_checksum(unsigned char *data, unsigned short length) 
{
  unsigned short checksum = 0;
  int i;
  for (i = 0; i < length; i++) {
    checksum = checksum + data[i];
  }

  return checksum;
}

/* Verify that the version-involatile header is valid */
char
messaging_valid_prefix(msg_header_t *hdr)
{
  if (hdr->magic != MSG_START_MAGIC)
    return 0;
  if (hdr->version != MSG_VERSION)
    return 0;
  if (hdr->length > MSG_MAX_LENGTH)
    return 0;

  return 1;
}

/* Validate a received message */
char
messaging_validate(unsigned char *data, unsigned short length) 
{
  msg_header_t *hdr = (msg_header_t *)data;
  
  if (!messaging_valid_prefix(hdr)) {
    return 0;
  }

  if (hdr->length != length) {
    return 0;
  }

  if (hdr->type >= MSG_TYPE_MAX) {
    return 0;
  }

  if (messaging_checksum(data, length) != hdr->checksum) {
    return 0;
  }

  if (data[hdr->length] != MSG_END_MAGIC) {
    return 0;
  }

  return 1;
}

/*
 * Blocking call to read the next byte on the stream
 */
unsigned char
messaging_next_byte(void) 
{
  // TODO: How due we read data?  Serial.read()?
  return 0;
}


/*
 * Read and return the next valid packet 
 */
void
messaging_get_packet(unsigned short local_address) 
{
  unsigned char buffer[MSG_MAX_LENGTH];
  msg_header_t *hdr = (msg_header_t *)buffer;
  unsigned short read = 1;

MSG_START:
  while ((buffer[0] = messaging_next_byte()) != MSG_START_MAGIC) {
    /* Skip data until we see a package start */
  }
  read = 1;

  /* Read the rest of the message prefix */
  while (read < MSG_PREFIX_LEN) {
    buffer[read] = messaging_next_byte();
    read++;
  }

  if (messaging_valid_prefix(hdr) != 1) {
    goto MSG_START;
  }

  
}


