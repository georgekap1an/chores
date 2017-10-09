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

#include <gtkmm/treemodel.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treemodelfilter.h>
#include <gtkmm/treeview.h>




struct Chore
{
  Glib::ustring name;
  Glib::ustring date;
  int frequency = 0;
  
  Chore(Glib::ustring const& name, Glib::ustring const& date, int frequency);
};

struct ChangeInfo
{
  std::unique_ptr<Chore> before;
  std::unique_ptr<Chore> after;
  
  void clear();
  explicit operator bool();
};


class ChoreColumns : public Gtk::TreeModelColumnRecord
{
public:
  ChoreColumns();

  Gtk::TreeModelColumn<Glib::ustring> name;
  Gtk::TreeModelColumn<Glib::ustring> date;
  Gtk::TreeModelColumn<int> frequency;
};

class ChoreList
{
public:
  ChoreColumns columns_;
  
  ChoreList();
  ~ChoreList();
  
  Gtk::TreeRow get_selected_row();
  void set_search_text(Glib::ustring text);
  void mark_selection_complete();
  void update_selection(Glib::ustring const& name, Glib::ustring const& date, int frequency);
  void delete_selection();
  void add_chore(Glib::ustring const& name, Glib::ustring const& date, int frequency);
  
  void undo();
  bool empty();

  Gtk::TreeView& view();
  
  void save();
  void load();
  
private:
  
  Glib::RefPtr<Gtk::ListStore> list_store_ = Gtk::ListStore::create(columns_);
  Glib::RefPtr<Gtk::TreeModelFilter> list_filter_ = Gtk::TreeModelFilter::create(list_store_);
  Gtk::TreeView tree_view_;
  Glib::ustring search_text_;
  std::string datapath_;
  
  ChangeInfo last_change_;
  
  void select_first_chore();
  bool should_be_visible(Gtk::TreeIter i);
  std::unique_ptr<Chore> chore_from_row(Gtk::TreeRow const& row);
  
};




