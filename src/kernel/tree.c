/*
  tree.c -- tree module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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
  /*   eprintf ("a->nmemb: %zu, b->nmemb: %zu\n", */
  /* 	     a->nmemb, b->nmemb); */

  if (a->nmemb != b->nmemb) return false;

  for (int id = 0; id < a->nmemb; id++) {
    if (a->node[id].depth != b->node[id].depth
        || a->node[id].type != b->node[id].type
        || a->node[id].cmp != b->node[id].cmp
        || ! a->node[id].cmp (a->node[id].data, b->node[id].data))
      return false;
  }

  return true;
}

int
tree_node_parent_id (struct tree *tree, int child_id)
{
  for (int id = child_id - 1; id >= 0; id--)
    if (tree->node[id].depth < tree->node[child_id].depth)
      return id;
  return -1;
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


/**************************************************************************
 * Tree control
 **************************************************************************/

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
select_node_by_id (Ihandle *tree_ctrl, int id)
{
  if (id < 0) return;

  IupSetInt (tree_ctrl, "VALUE", id);

  /* Motif bug workaround */
  int depth = IupGetIntId (tree_ctrl, "DEPTH", id);
  if (depth > 0) {
    int parent_id = IupGetIntId (tree_ctrl, "PARENT", id);
    IupSetAttributeId (tree_ctrl, "STATE", parent_id, "COLLAPSED");
    IupSetAttributeId (tree_ctrl, "STATE", parent_id, "EXPANDED");
    IupSetInt (tree_ctrl, "VALUE", id);
  }
}

void
select_node_by_title (Ihandle *tree_ctrl, char *s_title, int s_depth)
{
  bool selected = false;

  int count = IupGetInt (tree_ctrl, "COUNT");
  for (int id = 0; id < count; id++) {
    char *title = IupGetAttributeId (tree_ctrl, "TITLE", id);
    int depth = IupGetIntId (tree_ctrl, "DEPTH", id);
    if (s_depth >= 0 && s_depth != depth) continue;
    if (! strcmp (s_title, title)) {
      select_node_by_id (tree_ctrl, id);
      selected = true;
      break;
    }
  }

  if (! selected) IupSetAttribute (tree_ctrl, "VALUE", "FIRST");
}

void
propagate_children_nodes_color (Ihandle *tree_ctrl)
{
  unsigned char dr, dg, db;
  IupGetRGB (tree_ctrl, "FGCOLOR", &dr, &dg, &db);

  int count = IupGetInt (tree_ctrl, "COUNT");
  for (int id = 0; id < count; id++) {
    unsigned char r, g, b;
    IupGetRGBId (tree_ctrl, "COLOR", id, &r, &g, &b);
    if ((r != dr || g != dg || b != db)
        && IupGetIntId (tree_ctrl, "DEPTH", id) > 0) {
      int parent_id = id;
      do {
        /* check depth rather than node id to workaround a bug in IUP
           that makes all parent chains end at node id 0 instead of -1 */
        parent_id = IupGetIntId (tree_ctrl, "PARENT", parent_id);
        unsigned char pr, pg, pb;
        IupGetRGBId (tree_ctrl, "COLOR", parent_id, &pr, &pg, &pb);
        pr = max_int (pr, r);
        pg = max_int (pg, g);
        pb = max_int (pb, b);
        IupSetRGBId (tree_ctrl, "COLOR", parent_id, pr, pg, pb);
      } while (IupGetIntId (tree_ctrl, "DEPTH", parent_id) > 0);
    }
  }
}
