/*
* Copyright (c) 2017 Jonathan Hebert
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public
* License along with this program; if not, write to the
* Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301 USA
*
* Authored by: Jonathan Hebert <jonathan.hebert@icloud.com>
*/

#include <gtkmm/menu.h>
#include <glibmm/i18n.h>
#include <gtkmm/window.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/searchentry.h>
#include <gtkmm/revealer.h>
#include <gtkmm/scrolledwindow.h>
#include <giomm/settings.h>
#include <gtkmm/popover.h>
#include "choredialog.h"
#include "chorelist.h"

class ChoreMenu : public Gtk::Menu
{
public:
  Gtk::MenuItem finished_item;
  Gtk::MenuItem edit_item;
  Gtk::MenuItem remove_item;
  
  ChoreMenu();
};


class ChoresMainWindow : public Gtk::Window
{

public:
  ChoresMainWindow();
  virtual ~ChoresMainWindow();

private:
  void mark_selection_complete();
  void edit_selection();
  void delete_selection();
  void add_chore();
  void undo();

  //Signal handlers:
  void on_list_button_press(GdkEventButton* event);
  bool on_key_press_event(GdkEventKey* key_event) override;
  void on_search_button_click();

  //Member widgets:
  Gtk::HeaderBar header_bar_;
  Gtk::Button close_button_;
  Gtk::Box box_;
  Gtk::Button add_button_;
  Gtk::ToggleButton search_toggle_button_;
  Gtk::SearchEntry search_entry_;
  Gtk::Revealer search_entry_revealer_;
  Gtk::ScrolledWindow scrolled_window_;
  Gtk::Button undo_button_;
  Gtk::Label welcome_message_;
  Gtk::Popover welcome_popover_;
  ChoreList list_;
  ChoreMenu menu_;
  
  // window state
  int current_height_;
  Glib::RefPtr<Gio::Settings> window_height_settings_;
};