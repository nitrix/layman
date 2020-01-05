#ifndef LEARNGL_MAIN_H
#define LEARNGL_MAIN_H

#include "renderer.h"
#include "toolkit.h"
#include "window.h"

int main(int argc, char *argv[]);
void main_loop(struct window *window, struct renderer *renderer);

#endif