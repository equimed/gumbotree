/**
 * @file
 *
 * libgumbotree library include file
 *
 */

#ifndef GUMBOTREE_H
#define GUMBOTREE_H 1

/* BEGIN_C_DECLS should be used at the beginning of your declarations,
 * so that C++ compilers don't mangle their names.  Use END_C_DECLS at
 * the end of C declarations. */
#undef BEGIN_C_DECLS
#undef END_C_DECLS
#ifdef __cplusplus
# define BEGIN_C_DECLS extern "C" {
# define END_C_DECLS }
#else
# define BEGIN_C_DECLS /* empty */
# define END_C_DECLS /* empty */
#endif
     
/* PARAMS is a macro used to wrap function prototypes, so that
 *         compilers that don't understand ANSI C prototypes still work,
 *                 and ANSI C compilers can issue warnings about type mismatches. */
#undef PARAMS
#if defined (__STDC__) || defined (_AIX) \
        || (defined (__mips) && defined (_SYSTYPE_SVR4)) \
        || defined(WIN32) || defined(__cplusplus)
# define PARAMS(protos) protos
#else
# define PARAMS(protos) ()
#endif

// Structures
typedef struct {
  GumboNode*      root_node;                   // Start reference node
  unsigned long   what_to_show;                // See bit-mapped values below
  bool            expand_entity_references;    // TRUE - follow children, FALSE - Don't follow childrn
  short           (*filter)(GumboNode* node);  // User supplied filter function
  GumboNode*      current_node;                // Current reference node
} NodeIterator, TreeWalker;

// Enumerations
typedef enum {
  GT_EXCLUDE_CHILDREN,
  GT_INCLUDE_CHILDREN
} GTSearchDescend;

typedef enum {
  GT_MATCH_CASE,
  GT_CASE_INSENSITIVE
} GTSearchCase;

// Functions
BEGIN_C_DECLS
NodeIterator*   gt_create_node_iterator(GumboNode* root, unsigned int what_to_show, short (*filter)(GumboNode* node), bool expand_entity_references);
void            gt_detach_node_iterator(NodeIterator* ni);
GumboNode*      gt_next_node(NodeIterator* ni);
GumboNode*      gt_previous_node(NodeIterator* ni);


TreeWalker*     gt_create_tree_walker(GumboNode* root, unsigned int what_to_show, short (*filter)(GumboNode* node), bool expand_entity_references);
void            gt_detach_tree_walker(TreeWalker* tw);
GumboNode*      gt_parent_node(TreeWalker* tw);
GumboNode*      gt_first_child(TreeWalker* tw);
GumboNode*      gt_last_child(TreeWalker* tw);
GumboNode*      gt_previous_sibling(TreeWalker* tw);
GumboNode*      gt_next_sibling(TreeWalker* tw);

short           gt_accept_node(NodeIterator* ni, GumboNode* node);

// Misc functions
void            gt_version();
void            gt_print_node(GumboNode* node);
GumboNode*      gt_search_text(char* str, GumboNode* node, GTSearchDescend descend, GTSearchCase kase);

END_C_DECLS

// Definitions
// Returned values of nodeAccept(); 
#define FILTER_ACCEPT                  1
#define FILTER_REJECT                  2
#define FILTER_SKIP                    3

// Bit-mapped values for whatToShow
// Note - these vary widely from the W3C specification for
// level 2 node traversal specification. This is because
// the Gumbo library has a different value for and number of
// node types
#define SHOW_GUMBO_NODE_DOCUMENT       (1 << 0)
#define SHOW_GUMBO_NODE_ELEMENT        (1 << 1)
#define SHOW_GUMBO_NODE_TEXT           (1 << 2)
#define SHOW_GUMBO_NODE_CDATA          (1 << 3)
#define SHOW_GUMBO_NODE_COMMENT        (1 << 4)
#define SHOW_GUMBO_NODE_WHITESPACE     (1 << 5)
#define SHOW_ALL                       0xffffff

#endif /* GUMBOTREE_H */
