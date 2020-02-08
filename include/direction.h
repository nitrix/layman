#ifndef LEARNGL_DIRECTION_H
#define LEARNGL_DIRECTION_H

typedef unsigned int direction_mask;

enum direction {
    DIRECTION_FORWARD  = 1,
    DIRECTION_BACKWARD = 2,
    DIRECTION_LEFT     = 4,
    DIRECTION_RIGHT    = 8,
};

#endif
