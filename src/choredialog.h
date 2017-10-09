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

#include <gtkmm/button.h>
#include <glibmm/date.h>
#include <gtkmm/dialog.h>
#include <gtkmm/calendar.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/grid.h>
#include <gtkmm/popover.h>
#include <glibmm/i18n.h>


class ChoreDialog : public Gtk::Dialog
{
public:
  Gtk::Grid grid;
  Gtk::Label name_label;
  Gtk::Entry name_entry;
  Gtk::Label freq_label;
  Gtk::SpinButton freq_entry;
  Gtk::Label date_label;
  Gtk::Calendar date_picker;
  Gtk::Entry date_entry;
  Gtk::Button save_button;
  Gtk::Label popover_message;
  Gtk::Popover popover;
  
  ChoreDialog(Gtk::Window& window);
  void set_edit_mode(Glib::ustring name, int frequency, Glib::Date date);
  Glib::Date get_date();
  void set_date(Glib::Date date);
};