#ifndef _PARSER_H
#define _PARSER_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

#include "event_tables.h"

//  Used internally
#define SYS_EVENT_T (1)
#define META_EVENT_T (2)
#define MIDI_EVENT_T (3)

//  Used for parsing
#define SYS_EVENT_1 (0xF0)
#define SYS_EVENT_2 (0xF7)
#define META_EVENT (0xFF)

//  Forward declarations
typedef struct division_s division_t;
typedef struct track_s track_t;
typedef struct event_s event_t;
typedef struct track_node_s track_node_t;
typedef struct event_node_s event_node_t;
typedef struct song_data_s song_data_t;

//  MIDI Structures
typedef struct division_s {
  bool uses_tpq;
  union {
    //  If uses_tpq == true
    uint16_t ticks_per_qtr;
    //  If uses_tpq == false
    struct {
      uint8_t ticks_per_frame;
      uint8_t frames_per_sec;
    };
  };
} division_t;

typedef struct track_s {
  uint32_t length;
  event_node_t *event_list;
} track_t;

typedef struct event_s {
  uint32_t delta_time;
  uint8_t type;
  union {
    sys_event_t sys_event;
    meta_event_t meta_event;
    midi_event_t midi_event;
  };
} event_t;

//  Internal structures
typedef struct track_node_s {
  struct track_node_s *next_track;
  track_t *track;
} track_node_t;

typedef struct event_node_s {
  struct event_node_s *next_event;
  event_t *event;
} event_node_t;

typedef struct song_data_s {
  //  Relative path to MIDI file
  char *path;

  //  MIDI Header info
  uint8_t format;
  uint16_t num_tracks;
  division_t division;

  //  MIDI Track info
  track_node_t *track_list;
} song_data_t;

//  Parsing functions
song_data_t *parse_file(const char *);
void parse_header(FILE *, song_data_t *);
void parse_track(FILE *, song_data_t *);
event_t *parse_event(FILE *);
sys_event_t parse_sys_event(FILE *, uint8_t);
meta_event_t parse_meta_event(FILE *);
midi_event_t parse_midi_event(FILE *, uint8_t);
uint32_t parse_var_len(FILE *);

//  Interpreting data internally
uint8_t event_type(event_t *);

//  Data manipulation
void free_song(song_data_t *);
void free_track_node(track_node_t *);
void free_event_node(event_node_t *);

//  Functions for swapping endian-ness
uint16_t end_swap_16(uint8_t [2]);
uint32_t end_swap_32(uint8_t [4]);

#endif // _PARSER_H
