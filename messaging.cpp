/*******************************************************************************
 *
 ******************************************************************************/

#include <Arduino.h>

#include "Debug.h"
#include "messaging.h"

/*
 * Compute a checksum for a data buffer
 * TODO: Lookup a good & past CRC algorithm
 */
checksum_t
messaging_calculate_checksum(uint8_t *data, length_t length) 
{
  checksum_t checksum = 0;
  length_t i;
  for (i = 0; i < length; i++) {
    checksum = checksum + data[i];
  }

  return checksum;
}

/* Get the checksum for a message */
checksum_t
messaging_checksum(msg_header_t *hdr) 
{
  /* Swap out the checksum for zero during computation */
  checksum_t check_in = hdr->checksum;
  checksum_t check;

  hdr->checksum = 0;
  check = messaging_calculate_checksum((uint8_t *)hdr, hdr->length);
  hdr->checksum = check_in;

  return check;
}


/* Verify that the version-involatile header is valid */
status_t
messaging_valid_prefix(msg_header_t *hdr)
{
  if (hdr->marker != MSG_START_MARKER)
    return 0;
  if (hdr->version != MSG_VERSION)
    return 0;
  if (hdr->length > MSG_MAX_LENGTH)
    return 0;

  return 1;
}

/* Validate a received message */
status_t
messaging_validate(uint8_t *data, length_t length) 
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

  if (messaging_checksum(hdr) != hdr->checksum) {
    return 0;
  }

  if (data[hdr->length] != MSG_END_MARKER) {
    return 0;
  }

  return 1;
}

/*
 * Blocking call to read the next byte on the stream
 */
uint8_t
messaging_next_byte(void) 
{
  // TODO: How do we read data?  Serial.read()?
  return 0;
}

status_t
messaging_send_byte(uint8_t data_byte) 
{
  // TODO: How do we send data?
  return 0;
}

uint8_t
messaging_send_data(uint8_t *data, length_t data_len) 
{
  length_t sent = 0;
  for (sent = 0; sent < data_len; sent++) {
    messaging_send_byte(data[sent]);
  }
  return sent;
}

/* Send a message packet */
void
messaging_send_packet(msg_header_t *hdr) 
{
  messaging_send_data((uint8_t *)hdr, hdr->length);
}

/*
 * Read and return the next valid packet:
 * 1) Watch for start byte
 * 2) Read the message prefix
 * 3) Read the entire message
 * 4) Validate message
 */
length_t
messaging_get_packet(uint8_t *buffer,
                     length_t buffer_length)
{
  msg_header_t *hdr = (msg_header_t *)buffer;
  length_t read = 1;
#ifdef MSG_WATCH_MARKERS
  uint8_t marker_seen = 0;
#endif

MSG_START:
  while ((buffer[0] = messaging_next_byte()) != MSG_START_MARKER) {
    /* Skip data until we see a package start */
  }
  read = 1;

  /* Read the rest of the message prefix */
  while (read < MSG_PREFIX_LEN) {
    buffer[read] = messaging_next_byte();
    read++;
  }

  /* Validate the prefix */
  if (!messaging_valid_prefix(hdr)) {
    DEBUG_PRINT(1, "messaging_get_packet: read invalid prefix\n");
    goto MSG_START;
  }

  if (hdr->length > buffer_length) {
    DEBUG_PRINT(1, "messaging_get_packet: hdr->length longer than buffer\n");
    goto MSG_START;
  }

  /* Read the remainder of the packet */
  while (read < hdr->length) {
    buffer[read] = messaging_next_byte();

#ifdef MSG_WATCH_MARKERS
    /* Watch for unexpected end + start markers */
    if (buffer[read] == MSG_END_MARKER)
      marker_seen |= 1;
    else if (buffer[read] == MSG_START_MARKER)
      marker_seen |= 2;
    else {
      marker_seen = 0;
    }
#endif

    read++;
  }

  if (!messaging_validate(buffer, read)) {
    DEBUG_PRINT(1, "messaging_get_packet: read invalid message\n");
    goto MSG_START;
  }

  return read;
}

/*
 * Read packets.  If the packet is for the local address then return with it,
 * otherwise forward it.
 */
length_t
messaging_return_packet(uint8_t *buffer,
                        length_t buffer_length,
                        node_id_t local_id)
{
  msg_header_t *hdr = (msg_header_t *)buffer;
  length_t read;

MSG_GET:
  read = messaging_get_packet(buffer, buffer_length);

  if (hdr->destination != local_id) {
    messaging_send_packet(hdr);
    
    if (hdr->destination != MSG_ID_BROADCAST) {
      goto MSG_GET;
    }
  }

  return read;
}

/*
 * Build and construct an acknowledgement message
 */
status_t
messaging_ack_packet(msg_header_t *hdr_in,
                     node_id_t source,
                     uint8_t *data,
                     uint8_t data_len) 
{
  uint8_t buffer[MSG_MAX_LENGTH];
  msg_header_t *hdr_out = (msg_header_t *)buffer;

  hdr_out->marker = MSG_START_MARKER;
  hdr_out->version = hdr_in->version;
  hdr_out->length = sizeof (msg_header_t) + data_len + sizeof (uint8_t);
  hdr_out->source = source;
  hdr_out->destination = hdr_in->source;
  hdr_out->sequence = hdr_in->sequence;
  hdr_out->type = hdr_in->type;
  hdr_out->flags = MSG_FLAG_ACK;

  if (data_len) {
    memcpy(buffer + sizeof (msg_header_t), data, data_len);
  }

  buffer[hdr_out->length - 1] = MSG_END_MARKER;

  /* Compute the checksum on the message */
  hdr_out->checksum = messaging_checksum(hdr_out);

  /* Transmit */
  messaging_send_packet(hdr_out);

  return 1;
}

