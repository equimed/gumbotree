/**
 * @file
 *
 * Library functions to facilitate using the Gumbo HTML 5 parser
 *
 * These functions are modeled on the W3C Recommendations for
 * Document Object Model Traversal.
 *
 * see http://www.w3.org/TR/2000/REC-DOM-Level-2-Traversal-Range-20001113/traversal.html#Traversal-NodeFilter
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "gumbo.h"
#include "gumbotree.h"

TreeWalker*
gt_create_tree_walker(GumboNode* root, unsigned int what_to_show, short (*filter)(GumboNode* node), bool expand_entity_references)
{
  TreeWalker* tw = NULL;

  // Allocate memory for the node iterator structure
  tw = (TreeWalker*)malloc(sizeof(TreeWalker));

  // Set defaults
  if (tw && root) {
    tw->root_node = root->parent;
    tw->what_to_show = SHOW_ALL;
    tw->expand_entity_references = expand_entity_references;
    tw->filter = filter;
    tw->current_node = root;
  }
  return tw;
}

/**
 * Return parent of reference node
 *
 * @param TreeWalker* tw
 *   Pointer to TreeWalker structure
 * @returns GumboNode* node
 *   Pointer to parent node or NULL for no parent
 */
GumboNode*
gt_parent_node(TreeWalker* tw)
{
  GumboNode* node = tw->current_node;

  do {
    node = node->parent;
  } while ( (gt_accept_node(tw, node) != FILTER_ACCEPT) && (node != tw->root_node) );

  tw->current_node = node;
  if (node == tw->root_node)
  {
    node = NULL;
  }

  return node;
}

/**
 * Return first child of reference node
 *
 * @param TreeWalker* tw
 *   Pointer to TreeWalker structure
 * @returns GumboNode* node
 *   Pointer to first child or NULL for no first child
 */
GumboNode*
gt_first_child(TreeWalker* tw)
{
  GumboNode* node = tw->current_node;

  if (node->type == GUMBO_NODE_ELEMENT)
  {
    do {
      if (node->v.element.children.length)
      {
        node = node->v.element.children.data[0];
      }
      else
      {
        node = NULL;
        break;
      }
    } while ( (gt_accept_node(tw, node) != FILTER_ACCEPT) && (node != tw->root_node) );
  }
  else
  {
    node = NULL;
  }
 
  if (node)
  {
    tw->current_node = node;
  }

  return node;
}

GumboNode*
gt_last_child(TreeWalker* tw)
{
  GumboNode* node = tw->current_node;

  if (node->type == GUMBO_NODE_ELEMENT)
  {
    do {
      if (node->v.element.children.length)
      {
        node = node->v.element.children.data[node->v.element.children.length - 1];
      }
      else
      {
        node = NULL;
        break;
      }
    } while ( (gt_accept_node(tw, node) != FILTER_ACCEPT) && (node != tw->root_node) );
  }
  else
  {
    node = NULL;
  }

  if (node)
  {
    tw->current_node = node;
  }

  return node;
}

GumboNode*
gt_previous_sibling(TreeWalker* tw)
{
  GumboNode* gn = NULL;

  return gn;
}

GumboNode*
gt_next_sibling(TreeWalker* tw)
{
  GumboNode* gn =NULL;

  return gn;
}
