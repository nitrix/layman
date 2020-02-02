#ifndef LEARNGL_DIRECTION_H
#define LEARNGL_DIRECTION_H

typedef unsigned int direction_mask;

enum direction {
    DIRECTION_FORWARD  = 1u,
    DIRECTION_BACKWARD = 2u,
    DIRECTION_LEFT     = 4u,
    DIRECTION_RIGHT    = 8u,
};

#endif