/* Name, event_tables.c, CS 24000, Spring 2020
 * Last updated March 27, 2020
 */

#include "event_tables.h"

#include <string.h>

/*
 * Function called prior to main that sets up META_TABLE and MIDI_TABLE as
 * lookup tables that map event types to default events
 */

void build_event_tables() {
  //  META Events
  memset(META_TABLE, 0, sizeof(META_TABLE));
  META_TABLE[0x00] = (meta_event_t) {"Sequence Number", 3, NULL};
  META_TABLE[0x01] = (meta_event_t) {"Text Event", 0, NULL};
  META_TABLE[0x02] = (meta_event_t) {"Copyright Notice", 0, NULL};
  META_TABLE[0x03] = (meta_event_t) {"Sequence/Track Name", 0, NULL};
  META_TABLE[0x04] = (meta_event_t) {"Instrument Name", 0, NULL};
  META_TABLE[0x05] = (meta_event_t) {"Lyric", 0, NULL};
  META_TABLE[0x06] = (meta_event_t) {"Marker", 0, NULL};
  META_TABLE[0x07] = (meta_event_t) {"Cue Point", 0, NULL};
  META_TABLE[0x20] = (meta_event_t) {"MIDI Channel Prefix", 1, NULL};
  META_TABLE[0x21] = (meta_event_t) {"MIDI Port Prefix", 1, NULL};
  META_TABLE[0x2f] = (meta_event_t) {"End of Track", 0, NULL};
  META_TABLE[0x51] = (meta_event_t) {"Set Tempo", 3, NULL};
  META_TABLE[0x54] = (meta_event_t) {"SMTPE Offset", 5, NULL};
  META_TABLE[0x58] = (meta_event_t) {"Time Signature", 4, NULL};
  META_TABLE[0x59] = (meta_event_t) {"Key Signature", 2, NULL};
  META_TABLE[0x7f] = (meta_event_t) {"Sequencer-Specific Meta-event", 0, NULL};

  //  MIDI Events
  memset(MIDI_TABLE, 0, sizeof(MIDI_TABLE));
  //  Channel messages
  for (uint8_t i = 0x80; i <= 0x8F; i++) {
    MIDI_TABLE[i] = (midi_event_t) {"Note Off", i, 2, NULL};
  }
  for (uint8_t i = 0x90; i <= 0x9F; i++) {
    MIDI_TABLE[i] = (midi_event_t) {"Note On", i, 2, NULL};
  }
  for (uint8_t i = 0xA0; i <= 0xAF; i++) {
    MIDI_TABLE[i] = (midi_event_t) {"Polyphonic Key", i, 2, NULL};
  }
  for (uint8_t i = 0xB0; i <= 0xBF; i++) {
    MIDI_TABLE[i] = (midi_event_t) {"Control Change", i, 2, NULL};
  }
  for (uint8_t i = 0xC0; i <= 0xCF; i++) {
    MIDI_TABLE[i] = (midi_event_t) {"Program Change", i, 1, NULL};
  }
  for (uint8_t i = 0xD0; i <= 0xDF; i++) {
    MIDI_TABLE[i] = (midi_event_t) {"After-touch", i, 1, NULL};
  }
  for (uint8_t i = 0xE0; i <= 0xEF; i++) {
    MIDI_TABLE[i] = (midi_event_t) {"Pitch Wheel Change", i, 2, NULL};
  }
  MIDI_TABLE[0xF1] = (midi_event_t) {"Undefined", 0xF1, 0, NULL};
  MIDI_TABLE[0xF2] = (midi_event_t) {"Song Position Pointer", 0xF2, 2, NULL};
  MIDI_TABLE[0xF3] = (midi_event_t) {"Song Select", 0xF3, 1, NULL};
  MIDI_TABLE[0xF4] = (midi_event_t) {"Undefined", 0xF4, 0, NULL};
  MIDI_TABLE[0xF5] = (midi_event_t) {"Undefined", 0xF5, 0, NULL};
  MIDI_TABLE[0xF6] = (midi_event_t) {"Tune Request", 0xF6, 0, NULL};
  MIDI_TABLE[0xF8] = (midi_event_t) {"Timing Clock", 0xF8, 0, NULL};
  MIDI_TABLE[0xF9] = (midi_event_t) {"Undefined", 0xF9, 0, NULL};
  MIDI_TABLE[0xFA] = (midi_event_t) {"Start", 0xFA, 0, NULL};
  MIDI_TABLE[0xFB] = (midi_event_t) {"Continue", 0xFB, 0, NULL};
  MIDI_TABLE[0xFC] = (midi_event_t) {"Stop", 0xFC, 0, NULL};
  MIDI_TABLE[0xFD] = (midi_event_t) {"Undefined", 0xFD, 0, NULL};
  MIDI_TABLE[0xFE] = (midi_event_t) {"Active Sensing", 0xFE, 0, NULL};
} /* build_event_tables() */
