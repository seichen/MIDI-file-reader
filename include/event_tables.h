#ifndef _TABLES_H
#define _TABLES_H

#include <inttypes.h>

typedef struct sys_event_s {
  uint32_t data_len;
  uint8_t *data;
} sys_event_t;

typedef struct meta_event_s {
  const char *name;
  uint32_t data_len;
  uint8_t *data;  
} meta_event_t;

typedef struct midi_event_s {
  const char *name;
  uint8_t status;
  uint8_t data_len;
  uint8_t *data;
} midi_event_t;

meta_event_t META_TABLE[256];
midi_event_t MIDI_TABLE[256];

void __attribute__ ((constructor)) build_event_tables();

#endif // _TABLES_H
