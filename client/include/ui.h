#ifndef UI_H
#define UI_H

struct ui *ui_create(struct renderer *renderer);
void ui_destroy(struct ui *ui);
void ui_render(struct ui *ui);

#endif
