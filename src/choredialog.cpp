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

#include "choredialog.h"

using namespace std;
using namespace Glib;
using namespace Gtk;


ChoreDialog::ChoreDialog(Window& window) :
  Dialog(_("Add Chore"), window),
  name_label(_("Name")),
  freq_label(_("Frequency (In Days)")),
  date_label(_("First Due Date")),
  save_button(_("Save")),
  popover_message(_("Enter a Name")),
  popover(name_entry)
{
  name_label.set_halign(Align::ALIGN_END);
  freq_label.set_halign(Align::ALIGN_END);
  date_label.set_halign(Align::ALIGN_END);
  freq_entry.set_range(1, numeric_limits<int>::max());
  freq_entry.set_increments(1,7);
  freq_entry.set_value(7); // default to weekly chore
  
  grid.set_column_spacing(12);
  grid.set_row_spacing(6);
  grid.attach(name_label, 0, 0, 1, 1);
  grid.attach(name_entry, 1, 0, 1, 1);
  grid.attach(freq_label, 0, 1, 1, 1);
  grid.attach(freq_entry, 1, 1, 1, 1);
  grid.attach(date_label, 0, 2, 1, 1);
  grid.attach(date_picker, 1, 2, 1, 1);
  grid.set_border_width(18);
  get_vbox()->add(grid);
  add_button(_("Cancel"), RESPONSE_CANCEL);
  popover.add(popover_message);
  popover.set_border_width(18);
  popover.set_modal(false);
  
  name_entry.signal_changed().connect([this](){
    if(name_entry.get_text_length() > 0 && popover.is_visible())
    {
      popover.hide();
    }
  });
  save_button.signal_clicked().connect([this](){
    if(name_entry.get_text().empty())
    {
      popover.show_all();
      name_entry.grab_focus();
    }
    else
    {
      response(RESPONSE_APPLY);
    }
  });
  this->get_action_area()->pack_end(save_button);
}

void ChoreDialog::set_edit_mode(ustring name, int frequency, Date date)
{
  name_entry.set_text(name);
  freq_entry.set_value(frequency),
  set_date(date);
  set_title(_("Edit Chore"));
  date_label.set_text(_("Next Due Date"));
}

Date ChoreDialog::get_date()
{
  Date date;
  date_picker.get_date(date);
  return date;
}

void ChoreDialog::set_date(Date date)
{
  date_picker.select_month(date.get_month() - 1, date.get_year());
  date_picker.select_day(date.get_day());
}