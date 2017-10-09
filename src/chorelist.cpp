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

#include <iostream>
#include <glibmm/date.h>
#include <glibmm/i18n.h>
#include <glibmm/keyfile.h>
#include <glibmm/miscutils.h>
#include "chorelist.h"

using namespace std;
using namespace Gtk;
using namespace Glib;

Chore::Chore(ustring const& name, ustring const& date, int frequency)
{
  this->name = name;
  this->date = date;
  this->frequency = frequency;
}

void ChangeInfo::clear()
{
  before.reset();
  after.reset();
}

ChangeInfo::operator bool()
{
  return before || after;
}

ChoreColumns::ChoreColumns()
{
  add(name);
  add(date);
  add(frequency);
}

ChoreList::ChoreList() :
  datapath_(build_filename(get_home_dir(), ".chores"))
{
  list_store_->set_sort_column(columns_.date.index(), Gtk::SortType::SORT_ASCENDING);
  list_filter_->set_visible_func([this](Gtk::TreeIter i){ return should_be_visible(i);});
  tree_view_.set_model(list_filter_);
  tree_view_.append_column(_("Chore"), columns_.name);
  tree_view_.append_column(_("Due Date"), columns_.date);
  // built-in search doesn't behave
  // quiet like I want; disable it
  tree_view_.set_search_column(-1);
  load();
}

ChoreList::~ChoreList()
{
  save();
}

TreeRow ChoreList::get_selected_row()
{
  return *(tree_view_.get_selection()->get_selected());
}

void ChoreList::set_search_text(ustring text)
{
  search_text_ = text;
  list_filter_->refilter();
}

void ChoreList::mark_selection_complete()
{
  auto selected_row = get_selected_row();
  
  last_change_.clear();
  last_change_.before = chore_from_row(selected_row);
  
  Date next_due_date;
  next_due_date.set_time_current();
  next_due_date.add_days(selected_row[columns_.frequency]);
  selected_row[columns_.date] = next_due_date.format_string("%Y-%m-%d");
  
  last_change_.after = chore_from_row(selected_row);
  
  select_first_chore();
}

void ChoreList::update_selection(ustring const& name, ustring const& date, int frequency)
{
  auto selected_row = get_selected_row();
  
  last_change_.clear();
  last_change_.before = chore_from_row(selected_row);
  
  selected_row[columns_.name] = name;
  selected_row[columns_.date] = date;
  selected_row[columns_.frequency] = frequency;
  
  last_change_.after = chore_from_row(selected_row);
}

void ChoreList::delete_selection()
{
  auto selected_row = tree_view_.get_selection()->get_selected();
  
  // record change
  last_change_.clear();
  last_change_.before = chore_from_row(*selected_row);
  
  // make change
  auto child_iter = list_filter_->convert_iter_to_child_iter(selected_row);
  list_store_->erase(child_iter);
  
  select_first_chore();
}


void ChoreList::add_chore(ustring const& name, ustring const& date, int frequency)
{
  // make change
  auto new_row = *(list_store_->append());
  new_row[columns_.name] = name;
  new_row[columns_.date] = date;
  new_row[columns_.frequency] = frequency;
  
  // record change
  last_change_.clear();
  last_change_.after = chore_from_row(new_row);
}

void ChoreList::undo()
{
  if(last_change_)
  {
    if(!last_change_.before)
    {
      // undo add be removing
      list_store_->foreach_iter([&](ListStore::iterator const& i) -> bool{
        if(i->get_value(columns_.name) == last_change_.after->name)
        {
          list_store_->erase(i);
          return true;
        }
        return false;
      });
      
    }
    else if(!last_change_.after)
    {
      // undo remove by adding
      auto new_row = *(list_store_->append());
      new_row[columns_.name] = last_change_.before->name;
      new_row[columns_.date] = last_change_.before->date;
      new_row[columns_.frequency] = last_change_.before->frequency;
    }
    else
    {
      // undo edit by restoring original
      for(auto&& row : list_store_->children())
      {
        if(row[columns_.name] == last_change_.after->name)
        {
          row[columns_.name] = last_change_.before->name;
          row[columns_.date] = last_change_.before->date;
          row[columns_.frequency] = last_change_.before->frequency;
          break;
        }
      }
    }
    last_change_.clear();
  }
}

bool ChoreList::empty()
{
  return list_store_->children().empty();
}


TreeView& ChoreList::view()
{
  return tree_view_;
}

void ChoreList::save()
{
  KeyFile keyfile;
  for(auto&& row : list_store_->children())
  {
    keyfile.set_value(row[columns_.name], "due_date", row[columns_.date]);
    keyfile.set_integer(row[columns_.name], "frequency", row[columns_.frequency]);
  }
  bool success = keyfile.save_to_file(datapath_);
  if(!success)
  {
    //TODO use glib logging facility
    cerr << "failed to save data to " << datapath_ << endl;
  }
}

void ChoreList::load()
{
  try
  {
    KeyFile keyfile;
    keyfile.load_from_file(datapath_);
    auto chore_names = keyfile.get_groups();
    for(auto&& chore_name : chore_names)
    {
      auto new_row = *(list_store_->append());
      new_row[columns_.name] = chore_name;
      new_row[columns_.date] = keyfile.get_string(chore_name, "due_date");
      new_row[columns_.frequency] = max(1, keyfile.get_integer(chore_name, "frequency"));
    }
  }
  catch(Exception& e)
  {
    // treat as first use 
  }
}

void ChoreList::select_first_chore()
{
  TreeModel::Path newpath(list_store_->children().begin());
  tree_view_.set_cursor(newpath);
  tree_view_.grab_focus();
}

bool ChoreList::should_be_visible(TreeIter i)
{
  if(search_text_.empty())
  {
    return true;
  }
  else
  {
    auto& row = *i;
    ustring const& name =  row[columns_.name];
    return name.casefold().find(search_text_) !=  ustring::npos;
  }
}

unique_ptr<Chore> ChoreList::chore_from_row(TreeRow const& row)
{
  return make_unique<Chore>(row[columns_.name], row[columns_.date], row[columns_.frequency]);
}







