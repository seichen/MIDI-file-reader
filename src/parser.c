/* Name, parser.c, CS 24000, Spring 2020
 * Last updated March 27, 2020
 */

/* Add any includes here */

#include "parser.h"
#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

uint8_t status_byte;

/* This function starts the process of 
 * parsing the file sent in
 */

song_data_t *parse_file(const char *file) {
  assert(file != NULL);

  FILE* in = NULL;
  in = fopen(file, "r");
  assert(in);

  build_event_tables();

  //allocate space for song list

  song_data_t *list = malloc(sizeof(song_data_t));
  assert(list);
  list->track_list = NULL;

  char *p = malloc(strlen(file) + 1);
  assert(p);
  strcpy(p, file);

  list->path = p;

  //parse the header

  parse_header(in, list);

  //parse the track(s)

  for (int i = 0; i < list->num_tracks; i++) {
    parse_track(in, list);
  }

  int8_t temp = 0;
  int check = fread(&temp, 1, 1, in);
  assert(check == 0);

  fclose(in);
  in = NULL;

  return list;
} /* parse_file() */

/* This function parses the header
 * portion of the file sent in
 */

void parse_header(FILE *in, song_data_t *song) {
  int check = 0;

  uint32_t chunk = 0;
  check = fread(&chunk, 1, 4, in);
  assert(check > 0);
  chunk = end_swap_32((uint8_t *)&chunk);

  assert(chunk == 0x4d546864);

  uint32_t len = 0;
  fread(&len, 1, 4, in);
  len = end_swap_32((uint8_t *)&len);
  assert(check > 0);
  assert(len == 6);

  uint16_t form = 0;

  check = fread(&form, 1, 2, in);
  assert(check > 0);
  form = end_swap_16((uint8_t *)&form);
  assert((form == 0) || (form == 1) || (form == 2));
  song->format = form;

  check = fread(&(song->num_tracks), 1, 2, in);
  song->num_tracks = end_swap_16((uint8_t *)
    &song->num_tracks);
  assert(check > 0);


  division_t new = {};

  uint8_t byte[2];
  fread(&byte, 1, 2, in);

  uint16_t swap = end_swap_16(byte);

  if (swap & 0x8000) {
    new.uses_tpq = false;
    new.ticks_per_frame = (swap & 0xFF);
    new.frames_per_sec = ((swap & 0x7F00) >> 8);
  }
  else {
    new.uses_tpq = true;
    new.ticks_per_qtr = swap;
  }

  song->division = new;
} /* parse_header() */

/* This function starts the process of
 * parsing tracks in the file
 */

void parse_track(FILE *in, song_data_t *song) {
  assert(in);
  assert(song);

  uint32_t chunk = 0;
  fread(&chunk, 1, 4, in);
  chunk = end_swap_32((uint8_t *)&chunk);
  assert(chunk == 0x4d54726b);


  track_node_t *new_t = malloc(sizeof(track_node_t));
  assert(new_t);
  new_t->next_track = NULL;
  new_t->track = NULL;

  track_t *new = malloc(sizeof(track_t));
  assert(new);
  new->event_list = NULL;

  new_t->track = new;

  fread(&new->length, 1, 4, in);
  new->length = end_swap_32((uint8_t *)&new->length);

  event_node_t *new_e_n = malloc(sizeof(event_node_t));
  assert(new_e_n);
  new_e_n->next_event = NULL;
  new_e_n->event = NULL;

  new->event_list = new_e_n;

  new_e_n->event = parse_event(in);

  event_node_t *next = 0;

  while (true) {
    next = malloc(sizeof(event_node_t));
    new_e_n->next_event = next;

    next->event = parse_event(in);
    new_e_n = next;
    if (new_e_n->event->type == 0xff) {
      if (strcmp(new_e_n->event->meta_event.name,
          "End of Track") == 0) {
        break;
      }
    }
  }

  new_e_n->next_event = NULL;

  if (song->track_list == NULL) {
    song->track_list = new_t;
  }
  else {
    track_node_t *temp = song->track_list;
    while (temp->next_track != NULL) {
      temp = temp->next_track;
    }
    temp->next_track = new_t;
  }
} /* parse_track() */

/* This function parses events
 * in the file
 */

event_t *parse_event(FILE *in) {
  assert(in);

  event_t *new = malloc(sizeof(event_t));
  assert(new);


  new->delta_time = parse_var_len(in);

  fread(&(new->type), 1, 1, in);

  uint8_t type = event_type(new);


  if (type == META_EVENT_T) {
    new->meta_event = parse_meta_event(in);
  }
  else if (type == MIDI_EVENT_T) {
    new->midi_event = parse_midi_event(in, new->type);
  }
  else if (type == SYS_EVENT_T) {
    new->sys_event = parse_sys_event(in, type);
  }

  return new;

} /* parse_event() */

/* This function parses events
 * of the type system
 */

sys_event_t parse_sys_event(FILE *in, uint8_t data) {
  sys_event_t new = {};

  new.data_len = parse_var_len(in);

  new.data = malloc(new.data_len);

  fread(new.data, 1, new.data_len, in);

  return new;
} /* parse_sys_event() */

/* This function parses events
 * of the type meta
 */

meta_event_t parse_meta_event(FILE *in) {
  meta_event_t new = {};

  uint8_t n = 0;
  fread(&n, 1, 1, in);

  new = META_TABLE[n];

  uint32_t len = parse_var_len(in);

  if (new.data_len != 0) {
    assert(len == new.data_len);
  }
  else {
    new.data_len = len;
  }

  if (new.data_len > 0) {
    new.data = malloc(new.data_len);

    fread(new.data, 1, new.data_len, in);
  }

  assert(new.name);

  return new;
} /* parse_meta_event() */

/* This function parses events
 * of the type midi
 */

midi_event_t parse_midi_event(FILE *in, uint8_t data) {
  midi_event_t new = {};


  if (data & 0x80) {


    new = MIDI_TABLE[data];
    assert(new.name);

    new.status = data;

    status_byte = data;

    if (new.data_len > 0) {

      new.data = malloc(new.data_len);

      fread(new.data, 1, new.data_len, in);
    }
  }
  else {

    new = MIDI_TABLE[status_byte];

    assert(new.name);

    new.status = status_byte;

    if (new.data_len > 0) {

      new.data = malloc(new.data_len);
      *(new.data) = data;

      fread(&new.data[1], 1, new.data_len - 1, in);
    }
  }

  return new;
} /* parse_midi_event() */

/* This function parses the
 * variable length quantity
 */

uint32_t parse_var_len(FILE *in) {
  assert(in);

  uint8_t n = 0;
  uint8_t check = 0;
  uint32_t len = 0;

  fread(&n, 1, 1, in);
  check = n;

  n = n & 0b01111111;

  len = len + n;


  while ((check & 0b10000000) != 0x00) {
    fread(&n, 1, 1, in);
    check = n;
    len = len << 7;
    n = n & 0b01111111;
    len = len + n;
  }

  return len;
} /* parse_var_len() */

/* This function determines the type
 * of event
 */

uint8_t event_type(event_t *event) {

  if (event->type == 0xff) {
    return META_EVENT_T;
  }
  if ((event->type == 0xF7) || (event->type == 0xF0)) {
    return SYS_EVENT_T;
  }
  return MIDI_EVENT_T;

} /* event_type() */

/* This function frees
 * all memory in song_data_t
 */

void free_song(song_data_t *song) {

  if (song == NULL) {
    return;
  }

  track_node_t *temp = NULL;

  if (song->track_list != NULL) {
    temp = song->track_list->next_track;
  }

  while (song->track_list != NULL) {
    free_track_node(song->track_list);
    song->track_list = temp;
    if (temp != NULL) {
      temp = temp->next_track;
    }
  }

  free(song->path);
  free(song);
} /* free_song() */

/* This function frees
 * all memory in track_node_t
 */

void free_track_node(track_node_t *track_t) {

  if (track_t->track != NULL) {
    free_event_node(track_t->track->event_list);
    free(track_t->track);
  }
  free(track_t);

} /* free_track_node() */

/* This function frees
 * all memory in event_node_t
 */

void free_event_node(event_node_t *event) {


  event_node_t *temp = event;
  uint8_t type = 0;

  while (event != NULL) {
    event = event->next_event;
    type = event_type(temp->event);
    if (type == SYS_EVENT_T) {
      free(temp->event->sys_event.data);
    }
    else if (type == META_EVENT_T) {
      free(temp->event->meta_event.data);
    }
    else {
      free(temp->event->midi_event.data);
    }
    free(temp->event);
    free(temp);
    temp = event;
  }

} /* free_event_node() */

/* This function swaps the
 * endians of a 16 bit number
 */

uint16_t end_swap_16(uint8_t s[2]) {


  uint16_t new = s[0];

  new = new << 8;

  new = new | s[1];


  return new;
} /* end_swap_16() */

/* This function swaps the
 * endians of a 32 bit number
 */

uint32_t end_swap_32(uint8_t s[4]) {

  uint32_t new = s[0] << 24;

  new = new | (s[1] << 16);
  new = new | (s[2] << 8);
  new = new | s[3];
  return new;
} /* end_swap_32() */
