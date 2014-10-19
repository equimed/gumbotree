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

#define _GNU_SOURCE

#include <config.h>
#include <stdio.h>
#include <string.h>
#include "gumbo.h"
#include "gumbotree.h"

/**
 * Prints the package string via puts
 */
void
gt_version()
{
  puts(PACKAGE_STRING);
}

// Gumbo human readable lookup arrays
const char* node_types[] = {
  "GUMBO_NODE_DOCUMENT",
  "GUMBO_NODE_ELEMENT",
  "GUMBO_NODE_TEXT",
  "GUMBO_NODE_CDATA",
  "GUMBO_NODE_COMMENT",
  "GUMBO_NODE_WHITESPACE"
};

const char* quirks_types[] = {
  "GUMBO_DOCTYPE_NO_QUIRKS",
  "GUMBO_DOCTYPE_QUIRKS",
  "GUMBO_DOCTYPE_LIMITED_QUIRKS"
};

const char* namespace_types[] = {
  "GUMBO_NAMESPACE_HTML",
  "GUMBO_NAMESPACE_SVG",
  "GUMBO_NAMESPACE_MATHML"
};

const char* parse_flag_types[] = {
  "GUMBO_INSERTION_NORMAL",
  "GUMBO_INSERTION_BY_PARSER",
  "GUMBO_INSERTION_IMPLICIT_END_TAG",
  "GUMBO_INSERTION_IMPLIED",
  "GUMBO_INSERTION_CONVERTED_FROM_END_TAG",
  "GUMBO_INSERTION_FROM_ISINDEX",
  "GUMBO_INSERTION_FROM_IMAGE",
  "GUMBO_INSERTION_RECONSTRUCTED_FORMATTING_ELEMENT",
  "GUMBO_INSERTION_ADOPTION_AGENCY_CLONED",
  "GUMBO_INSERTION_ADOPTION_AGENCY_MOVED",
  "GUMBO_INSERTION_FOSTER_PARENTED"
};

// Templates used by printf
const char* GUMBO_NODE_STRING = "(GumboNode*) node (0x%x){\n\
  GumboNodeType type          = %s\n\
  GumboNode* parent           = 0x%x\n\
  size_t index_within_parent  = %d\n\
  GumboParseFlags parse_flags = %s\n\
%s\n \
}\n";

const char* GUMBO_DOCUMENT_STRING = "  {\n\
  bool v.document.has_doctype = TRUE\n\
  const char * name = %s\n \
  const char * public_identifier = %s\n\
  const char * system_identifier = %s\n\
  GumboQuirksModeEnum doc_type_quirks_mode = %s\n\
}";

const char* GUMBO_ELEMENT_STRING = "  v.element {\n\
  GumboVector children              = %s (%d)\n\
  GumboTag tag                      = %s\n\
  GumboNamespaceEnum tag_namespace  = %s\n\
  GumboStringPiece original_tag     = %s\n\
  GumboStringPiece original_end_tag = %s\n\
  GumboSourcePosition start_pos     = line=%d column=%d offset=%d\n\
  GumboSourcePosition end_pos       = line=%d column=%d offset=%d\n\
  GumboVector attributes            = %s\n\
}";

const char* GUMBO_TEXT_STRING = "  v.text {\n\
    const char* text                  = %s\n\
    GumboStringPiece original_text    = %s\n\
    GumboSourcePosition start_pos     = line=%d column=%d offset=%d\n\
  }";

void
stringpiece_to_string(const char* istr, size_t istrlen, char* ostr)
{
  int i;

  for (i = 0; i < istrlen; i++)
  {
    ostr[i] = istr[i];
  }
  ostr[i] = '\0';
}

/**
 * Print relevant data from a node
 *
 * @param GumboNode* node
 *   The node to inspect
 */
void
gt_print_node(GumboNode* node)
{
  GumboAttribute* attribute;
  char attributes[256];
  char original_tag[128];
  char original_end_tag[128];
  char parse_flags[256];
  char vstr[1024];
  unsigned int flagbits;

  vstr[0] = '\0';
  if (node->type == GUMBO_NODE_ELEMENT)
  {
    // Element node
    stringpiece_to_string(node->v.element.original_tag.data, node->v.element.original_tag.length, original_tag);
    stringpiece_to_string(node->v.element.original_end_tag.data, node->v.element.original_end_tag.length, original_end_tag);

    attributes[0] = '\0';
    for (int i = 0; i < node->v.element.attributes.length; i++)
    {
      attribute = node->v.element.attributes.data[i];
      strcat(attributes, attribute->name);
      strcat(attributes, "=\"");
      strcat(attributes, attribute->value);
      strcat(attributes, "\" ");
    }
    strcat(attributes, "\n");

    sprintf(vstr, GUMBO_ELEMENT_STRING, (node->v.element.children.length)? "Yes": "No", node->v.element.children.length, gumbo_normalized_tagname(node->v.element.tag), namespace_types[node->v.element.tag_namespace],original_tag, original_end_tag, node->v.element.start_pos.line, node->v.element.start_pos.column, node->v.element.start_pos.offset, node->v.element.end_pos.line, node->v.element.end_pos.column, node->v.element.end_pos.offset, attributes);
  }
  else
  {
    // Text based node (terminal node)
    sprintf(vstr, GUMBO_TEXT_STRING, node->v.text.text, node->v.text.text, node->v.text.start_pos.line, node->v.text.start_pos.column, node->v.text.start_pos.offset);
  }

  // Build parse flags string
  strcpy(parse_flags, "None");
  if (node->parse_flags)
  {
    flagbits = (unsigned int)node->parse_flags;
    parse_flags[0] = '\0';
    for (int i = 0; i < sizeof(parse_flag_types)/sizeof(parse_flag_types[0]); i++)
    {
      if (flagbits&0x1)
      {
        strcat(parse_flags, parse_flag_types[i]);
        strcat(parse_flags, " "); 
      }
      flagbits >>= 1;
    }
  }

  printf(GUMBO_NODE_STRING, (unsigned long)node, node_types[node->type], node->parent, node->index_within_parent, parse_flags, vstr);
}

/**
 * Find the occurance of a text string within a text node
 *
 * @param char* str
 *   The search string
 * @param GumboNode* node
 *   The node to search or the parent of nodes to search
 * @param GTSearchDescend descend
 *   Flag to include or exclude children in search
 * @param GTSearchCase kase
 *   Flag to require case match or ignore case
 *
 * @returns GumboNode*
 *   Pointer to text node containing the text or NULL for not found
 */
GumboNode*
gt_search_text(char* str, GumboNode* node, GTSearchDescend descend,
               GTSearchCase kase)
{
  if (node->type == GUMBO_NODE_TEXT)
  {
    if (kase == GT_MATCH_CASE)
    {
      // Case sensitive
      if (strstr(node->v.text.text, str))
        return node;
    }
    else
    {
      // Case insensitive
      if (strcasestr(node->v.text.text, str))
        return node;
    }
  }
  else if ((node->type == GUMBO_NODE_ELEMENT) &&
           (descend == GT_INCLUDE_CHILDREN))
  {
    NodeIterator* ni;
    ni = gt_create_node_iterator(node, 0xffffffff, NULL, true);
    while ((node = gt_next_node(ni)) != NULL)
    {
      if (node->type == GUMBO_NODE_TEXT)
      {
        if (kase == GT_MATCH_CASE)
        {
          // Case sensitive
          if (strstr(node->v.text.text, str))
            return node;
        }
        else
        {
          // Case insensitive
          if (strcasestr(node->v.text.text, str))
            return node;
        }
      }
    }
  }

  return NULL;
}

