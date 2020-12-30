#include <assert.h>
#include <unistd.h>
#include <getopt.h>

#include "parser.h"
#include "library.h"

#include "song_writer.h"

#define USAGE \
"Usage instructions:\n\n"\
"  flags:\n"\
"    -d directory_path   Reads in all of the .mid files in the specified"\
" directory to create the library.\n"\
"    -s song_path        Parses the specified midi file.\n"\
"    -w write_path       Writes the parsed midi file to the path specified"\
" here. If the -s option is not also used, the -w option is ignored.\n"\
"    -h                  Display information on the options and"\
" arguments supported.\n\n"\
"  example usage:\n"\
"    ./proj1_main_p1 -d \"songs\"\n      Reads in all .mid files in the "\
" ./songs directory\n"\
"    ./proj1_main_p1 -s \"songs/reflect.mid\" -w"\
" \"new_reflect.mid\"\n"\
"        Performs a loopback test by parsing /songs/reflect.mid then"\
" writing it back to ./new_reflect.mid\n"\

int main(int argc, char **argv) {
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  setvbuf(stdin, NULL, _IONBF, 0);
  
  if (argc == 1) {
    printf(USAGE);
    return -1;
  }

  int opt;

  char *lib_dir_path = NULL;
  char *song_path = NULL;
  char *new_song_path = NULL;
  song_data_t *song = NULL;

  while ((opt = getopt(argc, argv, ":hd:s:w:")) != -1) {
    switch (opt) {
      case 'h':
        printf(USAGE);
        break;
      case 'd':
        lib_dir_path = optarg;
        break;
      case 's':
        song_path = optarg;
        break;
      case 'w':
        new_song_path = optarg;
        break;
      case ':':
        printf("option needs a value\n");
        break;
      case '?':
        printf("unknown option: %c\n", optopt);
        break;
    }
  }

  if (lib_dir_path) {
    make_library(lib_dir_path);
    printf("Songs in %s:\n\n", lib_dir_path);
    write_song_list(stdout, g_song_library);
  }

  if (song_path) {
    song = parse_file(song_path);
    assert(song);

    if (new_song_path) {
      printf("Writing %s to %s\n", song_path, new_song_path);
      write_song_data(song, new_song_path);
    }
  }

  if (lib_dir_path) {
    free_library(g_song_library);
  }
  if (song_path) {
    free_song(song);
  }

  return 0;
}