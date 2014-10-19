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
#include "gumbo.h"
#include "gumbotree.h"

/**
 * Executes a user define "filter" function and returns results to iterator or walker
 *
 * @param NodeIterator* ni
 *   This documents node iterator
 * @param GumboNode* node
 *   The node to be accepted or rejected
 * @returns (short)
 *   FILTER_ACCEPT - accept the node, NodeIterator and TreeWalker will return
 *     this node (Default)
 *   FILTER_REJECT - reject the node, NodeIterator and TreeWalker will not
 *     return this node
 *   FILTER_SKIP - skip the node, NodeInterator and TreeWalker will not return
 *     this node, but may return children of this node
 */
short
gt_accept_node(NodeIterator* ni, GumboNode* node)
{
  short status;

  if (!ni->what_to_show)
  {
    status = FILTER_REJECT;
  }
  else if (ni->what_to_show == SHOW_ALL) {
    status = FILTER_ACCEPT;
  }
  else
  {
    status = FILTER_REJECT;;
    switch (node->type)
    {
    case GUMBO_NODE_DOCUMENT:
      if (ni->what_to_show  & SHOW_GUMBO_NODE_DOCUMENT)
      {
        status = FILTER_ACCEPT;
      }
      break;
    case GUMBO_NODE_ELEMENT:
      if (ni->what_to_show  & SHOW_GUMBO_NODE_ELEMENT)
      {
        status = FILTER_ACCEPT;
      }
      break;
    case GUMBO_NODE_TEXT:
      if (ni->what_to_show  & SHOW_GUMBO_NODE_TEXT)
      {
        status = FILTER_ACCEPT;
      }
      break;
    case GUMBO_NODE_CDATA:
      if (ni->what_to_show  & SHOW_GUMBO_NODE_CDATA)
      {
        status = FILTER_ACCEPT;
      }
      break;
    case GUMBO_NODE_WHITESPACE:
      if (ni->what_to_show  & SHOW_GUMBO_NODE_WHITESPACE)
      {
        status = FILTER_ACCEPT;
      }
      break;
    }
  }

  if (status == FILTER_ACCEPT && ni->filter) {
    return ni->filter(node);
  }
  return status;
}
