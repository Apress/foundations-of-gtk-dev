#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "browser.h"

void on_back_clicked (GtkToolButton*);
void on_forward_clicked (GtkToolButton*);
void on_up_clicked (GtkToolButton*);
void on_refresh_clicked (GtkToolButton*);
void on_home_clicked (GtkToolButton*);
void on_go_clicked (GtkButton*);
void on_location_activate (GtkEntry*);
void on_row_activated (GtkTreeView*, GtkTreePath*, GtkTreeViewColumn*);

#endif
