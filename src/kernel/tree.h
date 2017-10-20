/*
  tree.h -- tree module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MININIM_TREE_H
#define MININIM_TREE_H

struct tree {
  struct tree_node {
    int depth;
    enum tree_node_type {
      TREE_NODE_TYPE_BRANCH, TREE_NODE_TYPE_LEAF,
    } type;
    void *data;
    m_comparison_fn_t cmp;
  } *node;
  size_t nmemb;
};

int insert_tree_node (struct tree *tree, int ref_id, enum tree_node_type type,
                      void *data, m_comparison_fn_t cmp);
int add_tree_node (struct tree *tree, int ref_id, enum tree_node_type type,
                   void *data, m_comparison_fn_t cmp);
int get_tree_branch_last_child_id (struct tree *tree, int parent_id);
int get_tree_branch_child_id (struct tree *tree, int parent_id, void *data,
                              m_comparison_fn_t cmp);
int get_or_put_tree_branch_child (struct tree *tree, int parent_id,
                                  enum tree_node_type type,
                                  void *data,
                                  size_t size,
                                  m_comparison_fn_t cmp);
int get_tree_node_id_by_data (struct tree *tree, int depth, void *data);
bool tree_eq (struct tree *a, struct tree *b);
void destroy_tree (struct tree *tree);

/* Tree control */
void populate_tree_ctrl (Ihandle *tree_ctrl, struct tree *tree);
void select_node_by_id (Ihandle *tree_ctrl, int id);
void select_node_by_title (Ihandle *tree_ctrl, char *title, int depth);
void propagate_children_nodes_color (Ihandle *tree_ctrl);

#endif	/* MININIM_TREE_H */
