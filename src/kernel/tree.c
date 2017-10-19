/*
  tree.c -- tree module;

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

#include "mininim.h"

int
insert_tree_node (struct tree *tree, int ref_id, enum tree_node_type type,
                  void *data, m_comparison_fn_t cmp)
{
  int new_id = ref_id + 1;
  if (ref_id >= 0 && tree->node[ref_id].type == TREE_NODE_TYPE_BRANCH) {
    while (new_id < tree->nmemb
           && tree->node[new_id].depth > tree->node[ref_id].depth)
      new_id++;
  }
  struct tree_node node;
  node.depth = ref_id >= 0 ? tree->node[ref_id].depth : 0;
  node.type = type;
  node.data = data;
  node.cmp = cmp;
  tree->node = add_to_array (&node, 1, tree->node, &tree->nmemb,
                             new_id, sizeof (*tree->node));
  return new_id;
}

int
add_tree_node (struct tree *tree, int ref_id, enum tree_node_type type,
               void *data, m_comparison_fn_t cmp)
{
  int new_id = ref_id + 1;
  struct tree_node node;
  if (ref_id >= 0)
    node.depth = tree->node[ref_id].type == TREE_NODE_TYPE_BRANCH
      ? tree->node[ref_id].depth + 1
      : tree->node[ref_id].depth;
  else node.depth = 0;
  node.type = type;
  node.data = data;
  node.cmp = cmp;
  tree->node = add_to_array (&node, 1, tree->node, &tree->nmemb,
                             new_id, sizeof (*tree->node));
  return new_id;
}

int
get_tree_branch_last_child_id (struct tree *tree, int parent_id)
{
  int last_id = -1;
  int parent_depth = parent_id >= 0 ? tree->node[parent_id].depth : -1;
  for (int id = parent_id + 1; id < tree->nmemb; id++) {
    if (tree->node[id].depth == parent_depth + 1)
      last_id = id;
    if (tree->node[id].depth <= parent_depth
        || id == tree->nmemb - 1) return last_id;
  }
  return -1;
}

int
get_tree_branch_child_id (struct tree *tree, int parent_id, void *data,
                          m_comparison_fn_t cmp)
{
  int parent_depth = parent_id >= 0 ? tree->node[parent_id].depth : -1;
  for (int id = parent_id + 1; id < tree->nmemb; id++) {
    if (tree->node[id].depth > parent_depth + 1) continue;
    else if (tree->node[id].depth <= parent_depth) break;
    if (cmp (data, tree->node[id].data)) return id;
  }
  return -1;
}

int
get_or_put_tree_branch_child (struct tree *tree, int parent_id,
                              enum tree_node_type type,
                              void *data,
                              size_t size,
                              m_comparison_fn_t cmp)
{
  int id = get_tree_branch_child_id (tree, parent_id, data, cmp);
  if (id >= 0) return id;
  id = get_tree_branch_last_child_id (tree, parent_id);
  void *new_data = xmalloc (size);
  memcpy (new_data, data, size);
  id = id < 0
    ? add_tree_node (tree, parent_id, type, new_data, cmp)
    : insert_tree_node (tree, id, type, new_data, cmp);
  return id;
}

int
get_tree_node_id_by_data (struct tree *tree, int depth, void *data)
{
  for (int id = 0; id < tree->nmemb; id++) {
    if (tree->node[id].depth != depth) continue;
    if (tree->node[id].cmp (tree->node[id].data, data)) return id;
  }
  return -1;
}

bool
tree_eq (struct tree *a, struct tree *b)
{
  /* if (a->nmemb != b->nmemb) */
  /*   fprintf (stderr, "a->nmemb: %ji, b->nmemb: %ji\n", a->nmemb, b->nmemb); */

  if (a->nmemb != b->nmemb) return false;

  for (int id = 0; id < a->nmemb; id++) {
    if (a->node[id].depth != b->node[id].depth
        || a->node[id].type != b->node[id].type
        || a->node[id].cmp != b->node[id].cmp
        || ! a->node[id].cmp (a->node[id].data, a->node[id].data))
      return false;
  }

  return true;
}

void
populate_tree_ctrl (Ihandle *tree_ctrl, struct tree *tree)
{
  for (int id = -1; id < (int) tree->nmemb - 1; id++) {
    char *add_command = tree->node[id + 1].type == TREE_NODE_TYPE_BRANCH
      ? "ADDBRANCH"
      : "ADDLEAF";
    char *insert_command = tree->node[id + 1].type == TREE_NODE_TYPE_BRANCH
      ? "INSERTBRANCH"
      : "INSERTLEAF";
    if (id <= -1)
      IupSetStrfId (tree_ctrl, add_command, id, "%030i", id + 1);
    else if (tree->node[id + 1].depth > tree->node[id].depth)
      IupSetStrfId (tree_ctrl, add_command, id, "%030i", id + 1);
    else if (tree->node[id + 1].depth == tree->node[id].depth)
      IupSetStrfId (tree_ctrl, insert_command, id, "%030i", id + 1);
    else for (int bid = id - 1; bid >= 0; bid--)
           if (tree->node[id + 1].depth == tree->node[bid].depth) {
             IupSetStrfId (tree_ctrl, insert_command, bid, "%030i", id + 1);
             break;
           }
  }
}

void
destroy_tree (struct tree *tree)
{
  for (int id = 0; id < tree->nmemb ; id++) {
    al_free (tree->node[id].data);
    tree->node[id].data = NULL;
  }
  destroy_array ((void **) &tree->node, &tree->nmemb);
}
