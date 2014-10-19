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

/**
 * Create the node iterator structure, initialize to default
 * values, and return to caller.
 *
 * Defaults:
 *   what_to_show - SHOW_ALL
 *   expand_entity_references - True
 *
 * @param GumboNode* root
 *   Pointer to "root" within the gumbo structue
 *
 * @returns
 *   Pointer to node iterator structure
 *   NULL if couldn't allocate memory
 *   NULL if root is NULL
 */
NodeIterator*
gt_create_node_iterator(GumboNode* root, unsigned int what_to_show, short (*filter)(GumboNode* node), bool expand_entity_references)
{
  NodeIterator* ni = NULL;

  // Allocate memory for the node iterator structure
  ni = (NodeIterator*)malloc(sizeof(NodeIterator));

  // Set defaults
  if (ni && root) {
    ni->root_node = root->parent;
    ni->what_to_show = SHOW_ALL;
    ni->expand_entity_references = expand_entity_references;
    ni->filter = filter;
    ni->current_node = root;
  }
  return ni;
}

void
gt_detach_node_iterator(NodeIterator* ni)
{
  free(ni);
}

/**
 * Find and return the next node
 *
 * Algorithm:
 *   Do I have a first child? Yes, return it
 *     No, Do I have a right sibling? Yes, return it
 *       No, Does my parent have a right sibling? Yes, return it
 *         No, Does my grandparent have a right sibling? Yes, return it
 *           Etc.
 *
 * @param NodeIterator* ni
 *   Pointer to the node iterator
 *
 * @returns GumboNode* gn
 *   Pointer to the next node or NULL
 */
GumboNode*
gt_next_node(NodeIterator* ni)
{
  GumboNode* node = ni->current_node;
  do
  {
    if (node->type == GUMBO_NODE_ELEMENT && node->v.element.children.length)
    {
      // Has children - Return first child
      node = (GumboNode*)node->v.element.children.data[0];
    }
    else
    {
      do
      {
        if ( node->index_within_parent < node->parent->v.element.children.length - 1 )
        {
          // Has sibling - Return next sibling
          node = (GumboNode*)node->parent->v.element.children.data[node->index_within_parent + 1];
          break;
        }
      } while ((node = node->parent) && (node != ni->root_node)); // Move up tree and try again
    }
  } while ( (gt_accept_node(ni, node) != FILTER_ACCEPT) && (node != ni->root_node) ); 

  ni->current_node = node;
  if (node == ni->root_node) {
    node = NULL;
  }

  return node;
}

/**
 *  Find and return the previous node
 *
 *  Algorithm:
 *  Do I not have a left sibling? Yes, return parent
 *    No, does the left sibling not have a child? Yes, return left sibling
 *      No, does the siblings child not have a child? Yes, return sibling child
 *        No, Does the child of the siblings child not have a child? Yes, return child of sibing child
 *          Etc.
 * 
 * @param NodeIterator* ni
 *   Pointer to the node iterator
 *
 * @returns GumboNode* gn
 *   Pointer to the previous node or NULL
 */
GumboNode*
gt_previous_node(NodeIterator* ni)
{
  GumboNode* node = ni->current_node;

  do {
    if ( node->index_within_parent == 0 )
    {
      // Doesn't have left sibling - Return parent
      node = node->parent;
    }
    else
    {
      // Get left sibling
      node = node->parent->v.element.children.data[node->index_within_parent - 1];
      do
      {
        if ((node->type != GUMBO_NODE_ELEMENT) || (!node->v.element.children.length))
        {
           // Left sibling doesn't have child - return left sibling
           break;
        }
      } while ((node = node->v.element.children.data[node->v.element.children.length - 1])); // Move down tree
    }
  } while ( (gt_accept_node(ni, node) != FILTER_ACCEPT) && (node != ni->root_node) ); 

  ni->current_node = node;
  if (node == ni->root_node)
  {
    node = NULL;
  }

  return node;
}
