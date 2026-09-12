#ifndef RAYCASTER_CORE_H
#define RAYCASTER_CORE_H

void rc_init(void);
void rc_set_texture(int slot, const unsigned char *rgba, int w, int h);
void rc_render(unsigned char *out, int w, int h);
void rc_tick(int fwd, int back, int left, int right, double moveSpeed, double rotSpeed, double extraTurn);

#endif