/* tree.h */
typedef enum {
  Prog,
  DeclVars, 
  Declarateurs, 
  DeclFoncts, 
  DeclFonct, 
  EnTeteFonct, 
  Parametres, 
  ListTypVar, 
  Corps, 
  SuiteInstr, 
  Instr, 
  Exp, 
  TB, 
  FB, 
  M, 
  E, 
  T, 
  F, 
  LValue, 
  Arguments, 
  ListExp,
  character, 
  num, 
  If, 
  Else, 
  Return, 
  While, 
  Void, 
  ident, 
  type, 
  eq, 
  order, 
  addsub, 
  divstar, 
  Or, 
  And, 
  For, 
  SuiteSwitchInstr,
  SwitchInstr,
  Switch,
  Case,
  Default,
  Break,
  comma,
  semicolon,
  parenthesis1,
  parenthesis2,
  Not,
  brace1,
  brace2,
  equal,
  epsilon
  /* list all other node labels, if any */
  /* The list must coincide with the string array in tree.c */
  /* To avoid listing them twice, see https://stackoverflow.com/a/10966395 */
} label_t;

typedef enum {
  BYTE_C,
  NUM_C,
  IDENT_C,
  COMP_C,
  TYPE_C
} Category;

typedef struct Node {
  label_t label;
  struct Node *firstChild, *nextSibling;
  int lineno;
  char byte;
  int num;
  char ident[64];
  char comp[3];
  char type[4];
  Category category;
} Node;

Node *makeNode(label_t label);
void addSibling(Node *node, Node *sibling);
void addChild(Node *parent, Node *child);
void deleteTree(Node*node);
void printTree(Node *node);

#define FIRSTCHILD(node) node->firstChild
#define SECONDCHILD(node) node->firstChild->nextSibling
#define THIRDCHILD(node) node->firstChild->nextSibling->nextSibling
