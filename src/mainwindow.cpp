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

#include "mainwindow.h"

using namespace std;
using namespace Glib;
using namespace Gdk;
using namespace Gtk;
  
ChoreMenu::ChoreMenu() :
    finished_item(_("Mark Finished")),
    edit_item(_("Edit")),
    remove_item(_("Delete"))
{
  append(finished_item);
  append(edit_item);
  append(remove_item);
}




ChoresMainWindow::ChoresMainWindow() :
  welcome_message_(_("Start by Adding a Chore")),
  welcome_popover_(add_button_)
{
  search_entry_.signal_search_changed().connect([this](){
    list_.set_search_text(search_entry_.get_text().casefold());
  });
  search_entry_.set_visible(true);
  search_entry_.set_visibility(true);
  
  search_entry_revealer_.add(search_entry_);
  search_entry_revealer_.set_transition_duration(500);
  search_entry_revealer_.set_transition_type(Gtk::RevealerTransitionType::REVEALER_TRANSITION_TYPE_SLIDE_DOWN);
  search_entry_revealer_.set_reveal_child(false);
  
  list_.view().signal_row_activated().connect([this](const Gtk::TreeModel::Path& path,
                                                const Gtk::TreeViewColumn* col) { edit_selection(); });
  
  list_.view().signal_button_press_event().connect_notify([&](GdkEventButton* event) { on_list_button_press(event); });
  
  
  menu_.finished_item.signal_activate().connect([this](){ mark_selection_complete(); });
  menu_.edit_item.signal_activate().connect([this](){ edit_selection(); });
  menu_.remove_item.signal_activate().connect([this](){ delete_selection(); });
  menu_.show_all(); 
  
  scrolled_window_.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
  scrolled_window_.add(list_.view());
  
  add_button_.set_image_from_icon_name("list-add");
  add_button_.signal_clicked().connect([this]() { add_chore(); });
  add_button_.set_tooltip_text(_("Add a New Chore"));
  
  welcome_popover_.add(welcome_message_);
  welcome_popover_.set_border_width(18);
  welcome_popover_.set_modal(false);
  
  
  close_button_.set_image_from_icon_name("window-close-symbolic");
  close_button_.signal_clicked().connect([this](){ close(); });
  
  search_toggle_button_.set_image_from_icon_name("edit-find-symbolic");
  search_toggle_button_.signal_clicked().connect([this](){ on_search_button_click(); });
  search_toggle_button_.set_tooltip_text(_("Find a Chore …")); //need to use real ellipses
  
  undo_button_.set_image_from_icon_name("edit-undo-symbolic");
  undo_button_.set_tooltip_text(_("Undo Last Change"));
  undo_button_.signal_clicked().connect([this]() { undo(); });
  undo_button_.set_sensitive(false);
  
  box_.set_orientation(Gtk::Orientation::ORIENTATION_VERTICAL);
  box_.set_homogeneous(false);
  box_.pack_start(search_entry_revealer_, Gtk::PACK_SHRINK);
  box_.pack_start(scrolled_window_);
  
  header_bar_.set_title(_("Chores"));
  header_bar_.pack_start(close_button_);
  
  header_bar_.pack_end(add_button_);
  header_bar_.pack_end(undo_button_);
  header_bar_.pack_start(search_toggle_button_);
  
  set_title(_("Chores"));
  set_border_width(0);
  
  Gdk::Geometry geometry;
  geometry.min_width = get_width();
  geometry.max_width = get_width();
  geometry.min_height = 400;
  geometry.max_height = 4000;
  
  set_geometry_hints(*this, geometry, Gdk::WindowHints::HINT_MIN_SIZE | Gdk::WindowHints::HINT_MAX_SIZE);
  add(box_);
  
  set_titlebar(header_bar_);
  
  signal_size_allocate().connect([&](Gtk::Allocation& allocation){
    int width;
    get_size(width, current_height_);
  });
  
  window_height_settings_ = Gio::Settings::create("com.github.georgekap1an.chores");
  current_height_ = window_height_settings_->get_int("window-height");
  set_default_size(0, current_height_);
  
  add_events(Gdk::KEY_PRESS_MASK);
  
  this->signal_show().connect([this](){
    if(list_.empty())
    {
      welcome_popover_.show_all();
      search_toggle_button_.set_sensitive(false);
    }
  });
  
  show_all();
}

ChoresMainWindow::~ChoresMainWindow()
{
  window_height_settings_->set_int("window-height", current_height_);
}


void ChoresMainWindow::mark_selection_complete()
{
  list_.mark_selection_complete();
  undo_button_.set_sensitive(true);
}

void ChoresMainWindow::edit_selection()
{
  auto selected_row = list_.get_selected_row();
  Date selected_date;
  selected_date.set_parse(selected_row[list_.columns_.date]);
  ChoreDialog dialog(*this);
  dialog.set_edit_mode(selected_row[list_.columns_.name],
                       selected_row[list_.columns_.frequency],
                       selected_date);
  dialog.show_all();
  
  int response = dialog.run();
  
  if(response == RESPONSE_APPLY)
  {
    list_.update_selection(dialog.name_entry.get_text(),
                           dialog.get_date().format_string("%Y-%m-%d"),
                           dialog.freq_entry.get_value_as_int());
    undo_button_.set_sensitive(true);
  }
}

void ChoresMainWindow::delete_selection()
{
  list_.delete_selection();
  undo_button_.set_sensitive(true);
}

void ChoresMainWindow::add_chore()
{
  if(welcome_popover_.is_visible())
  {
    welcome_popover_.hide();
    search_toggle_button_.set_sensitive(true);
  }
  
  ChoreDialog dialog(*this);
  dialog.show_all();
  int response = dialog.run();
  if(response == RESPONSE_APPLY)
  {
    list_.add_chore(dialog.name_entry.get_text(),
                    dialog.get_date().format_string("%Y-%m-%d"),
                    dialog.freq_entry.get_value_as_int());
    undo_button_.set_sensitive(true);
  }
}

void ChoresMainWindow::undo()
{
  list_.undo();
  undo_button_.set_sensitive(false);
}


void ChoresMainWindow::on_list_button_press(GdkEventButton* event)
{
  if(event->type == GDK_BUTTON_PRESS && event->button == 3)
  {
    menu_.popup(event->button, event->time);
  }
}

bool ChoresMainWindow::on_key_press_event(GdkEventKey* key_event)
{
  if(key_event->keyval == GDK_KEY_Escape)
  {
    if(search_entry_revealer_.get_reveal_child())
    {
      search_entry_revealer_.set_reveal_child(false);
      search_entry_.set_text("");
    }
    return true;
  }
  if(key_event->keyval == GDK_KEY_Delete)
  {
    delete_selection();
    return true;
  }
  if((key_event->state & (CONTROL_MASK | SHIFT_MASK | MOD1_MASK)) == CONTROL_MASK) // ctl ..
  {
    switch(key_event->keyval)
    {
      case GDK_KEY_f: // find
      {
        bool current_state = search_toggle_button_.get_active();
        search_toggle_button_.set_active(!current_state);
        //on_search_button_click();
        return true;
      }
      case GDK_KEY_m:
      {
        mark_selection_complete();
        return true;
      }
      case GDK_KEY_e: // edit
      {
        edit_selection(); // ?
        return true;
      }
      case GDK_KEY_z: // undo
      {
        undo();
        return true;
      }
      case GDK_KEY_n: // new
      {
        add_chore();
        return true;
      }
      case GDK_KEY_q: // quit
      {
        //cout << "ctl-q" << endl;
        close();
        return true;
      }
      case GDK_KEY_w: // close
      {
        //cout << "ctl-w" << endl;
        close();
        return true;
      }
      case GDK_KEY_s: // save
      {
        list_.save();
        return true;
      }
      default:
      {
        break;
      }
    }
  }
  //if the event has not been handled, call the base class
  return Gtk::Window::on_key_press_event(key_event);  
}

void ChoresMainWindow::on_search_button_click()
{
  if(search_entry_revealer_.get_reveal_child())
  {
    search_entry_revealer_.set_reveal_child(false);
    search_entry_.set_text("");
  }
  else
  {
    search_entry_revealer_.set_reveal_child(true);
    search_entry_.grab_focus();
  }
}


  

