#ifndef PCAP_READER_H
#define PCAP_READER_H

#include <stdint.h>
#include <stdbool.h>

// Callback function type for processing Modbus TCP payloads
// Parameters: payload data, payload length, user data pointer
typedef void (*modbus_payload_callback_t)(const uint8_t *payload, uint32_t length, 
                                            const char *src_ip, uint16_t src_port,
                                            const char *dst_ip, uint16_t dst_port,
                                            double timestamp,
                                            void *user_data);

// Process PCAP file and extract Modbus TCP payloads
bool pcap_process_file(const char *filename, modbus_payload_callback_t callback, void *user_data);

#endif // PCAP_READER_H
