#include "songs.h"

// D6F C6F B5 C6F D6F E D6F C6F
// C-flat D-flat F-flat G-flat A-flat
#define D6F 1244.51
#define C6F 1108.73
#define B5 987.77
#define E6 1318.51
#define N1 309
#define N12 155
#define N14 77

static float notes[1][20] = {{D6F, C6F, B5, C6F, D6F, E6, D6F, C6F, D6F, C6F, B5, C6F, D6F, E6, D6F, C6F}};
static int durations[1][20] = {{N1, N12, N1, N12, N12+N14, N14, N12, N1+N12, N1, N12, N1, N12, N12+N14, N14, N12, N1+N12}};
static int lengths[1] = {16};

void play_song(int song_id){
    for (int i = 0; i < lengths[song_id]; ++i) {
        bark(notes[song_id][i], durations[song_id][i]);
    }
}