%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "src/table.h"
#include "src/traducteur.h"
int yylex();
int yyerror(char option, char* msg);
extern int lineno;
extern int tok_end;
Node *t, *n, *root;

%}

%union {
       Node *node;
       char byte;
       int num;
       char ident[64];
       char comp[3];
       char type[4];
}
%type <node> Prog DeclVars Declarateurs DeclFoncts DeclFonct EnTeteFonct Parametres ListTypVar Corps SuiteInstr SuiteCaseDefault InstrBreak SuiteInstrBreak Instr Exp Const TB FB M E T F LValue Arguments ListExp SwitchInstr SuiteSwitchInstr
%token <byte> CHARACTER ADDSUB DIVSTAR
%token <num> NUM
%token <ident> IDENT
%token <comp> ORDER EQ
%token <type> TYPE
%token OR AND IF RETURN WHILE ELSE VOID FOR SWITCH CASE DEFAULT BREAK
%precedence ')'
%precedence ELSE

%parse-param {int tree}

%%
Prog:  DeclVars DeclFoncts                {$$ = makeNode(Prog); if ($1 != NULL) { t = makeNode(DeclVars); addChild($$, t); addChild(t, $1);} if ($2 != NULL){ n = makeNode(DeclFoncts); addChild($$, n); addChild(n, $2);} root = $$; if (tree) { printTree($$); }}
    ;
DeclVars:
       DeclVars TYPE Declarateurs ';'     {t = makeNode(type); strcpy(t->type, $2); t->category = TYPE_C; addChild(t, $3); if ($1 != NULL) {$$ = $1; addSibling($$, t);} else {$$ = t;}}
    |                                     {$$ = NULL;}
    ;
Declarateurs:
       Declarateurs ',' IDENT             {$$ = $1; t = makeNode(ident); strcpy(t->ident, $3); t->category = IDENT_C; addSibling($$, t);}
    |  IDENT                              {$$ = makeNode(ident); strcpy($$->ident, $1); $$->category = IDENT_C;}
    ;
DeclFoncts:
       DeclFoncts DeclFonct               {$$ = $1; addSibling($$, $2);}
    |  DeclFonct                          {$$ = $1;}
    ;
DeclFonct:
       EnTeteFonct Corps                  {$$ = makeNode(DeclFonct); addChild($$, $1); addChild($$, $2);}
    ;
EnTeteFonct:
       TYPE IDENT '(' Parametres ')'      {$$ = makeNode(type); strcpy($$->type, $1); $$->category = TYPE_C; t = makeNode(ident); strcpy(t->ident, $2); t->category = IDENT_C; addSibling($$, t); addSibling($$, $4);}
    |  VOID IDENT '(' Parametres ')'      {$$ = makeNode(Void); t = makeNode(ident); strcpy(t->ident, $2); t->category = IDENT_C; addSibling($$, t); addSibling($$, $4);}
    ;
Parametres:
       VOID                               {$$ = makeNode(Parametres); addChild($$, makeNode(Void));}
    |  ListTypVar                         {$$ = makeNode(Parametres); addChild($$, $1);}
    ;
ListTypVar:
       ListTypVar ',' TYPE IDENT          {$$ = $1; t = makeNode(type); strcpy(t->type, $3); t->category = TYPE_C; addSibling($$, t); n = makeNode(ident); strcpy(n->ident, $4); n->category = IDENT_C; addChild(t, n);}
    |  TYPE IDENT                         {$$ = makeNode(type); strcpy($$->type, $1); $$->category = TYPE_C; t = makeNode(ident); strcpy(t->ident, $2); t->category = IDENT_C; addChild($$, t);}
    ;
Corps: '{' DeclVars SuiteInstr '}'        {$$ = makeNode(Corps); if ($2 != NULL) {addChild($$, $2); addChild($$, $3);} else { addChild($$, $3);}}
    ;
SuiteInstr:
       SuiteInstr Instr                   {if ($1 == NULL) {$$ = $2;} else { $$ = $1; addSibling($$, $2);}}
    |                                     {$$ = NULL;}
    ;

InstrBreak:
         Instr                            {$$ = $1;}
       | BREAK ';'                        {$$ = makeNode(Break);}
       ;

SuiteInstrBreak:
        SuiteInstrBreak InstrBreak          {if ($1 == NULL) {$$ = $2;} else { $$ = $1; addSibling($$, $2);}}
       |                                  {$$ = NULL;}
       ;

Instr:
       LValue '=' Exp ';'                 {$$ = makeNode(equal); addChild($$, $1); addChild($$, $3);}
    |  IF '(' Exp ')' Instr               {$$ = makeNode(If); addChild($$, $3); addChild($$, $5);}
    |  IF '(' Exp ')' Instr ELSE Instr    {$$ = makeNode(If); addChild($$, $3); addChild($$, $5); t = makeNode(Else); addSibling($$, t); addChild(t, $7);}
    |  WHILE '(' Exp ')' Instr            {$$ = makeNode(While); addChild($$, $3); addChild($$, $5);}
    |  IDENT '(' Arguments  ')' ';'       {$$ = makeNode(ident); strcpy($$->ident, $1); $$->category = IDENT_C; addChild($$, $3);}
    |  RETURN Exp ';'                     {$$ = makeNode(Return); addChild($$, $2);}
    |  RETURN ';'                         {$$ = makeNode(Return);}
    |  '{' SuiteInstr '}'                 {$$ = $2;}
    |  SWITCH '(' Exp ')' '{' SuiteInstrBreak SuiteSwitchInstr '}' {$$ = makeNode(Switch); addChild($$, $3); if ($6 != NULL) {addChild($$, $6);} if ($7 != NULL) {addChild($3, $7);}}
    |  ';'                                {$$ = makeNode(semicolon);}
    ;
SuiteSwitchInstr:
    SuiteSwitchInstr SwitchInstr          {if ($1 != NULL) {$$ = $1; if ($2 != NULL) {addSibling($$, $2);}} else if ($2 != NULL) {$$ = $2;} else {$$ = NULL;}}
    |                                     {$$ = NULL;}
    ;
Const :
       ADDSUB Const                       {$$ = makeNode(addsub); $$->byte = $1; $$->category = BYTE_C; addChild($$, $2);}
    |  NUM                                {$$ = makeNode(num); $$->num = $1; $$->category = NUM_C;}
    |  CHARACTER                          {$$ = makeNode(character); $$->byte = $1; $$->category = BYTE_C;}   
    ;

SuiteCaseDefault:
        InstrBreak SuiteCaseDefault                 {if ($1 != NULL) {$$ = $1; if ($2 != NULL) {addSibling($$, $2);}} else if ($2 != NULL) {$$ = $2;} else {$$ = NULL;}}
       |InstrBreak                       {if ($1 != NULL) {$$ = $1;}}
       ;

SwitchInstr:
       CASE Const ':' SuiteCaseDefault             {$$ = makeNode(Case); addChild($$, $2); if ($4 != NULL) {addChild($$,$4);}}
    |  CASE Const ':' SwitchInstr             {$$ = makeNode(Case); addChild($$, $2); if ($4 != NULL) {addSibling($$,$4);}}       
    |  DEFAULT ':' SuiteCaseDefault               {$$ = makeNode(Default); if ($3 != NULL) {addChild($$,$3);}}
    |  DEFAULT ':' SwitchInstr               {$$ = makeNode(Default); if ($3 != NULL) {addSibling($$,$3);}}    
    ;
Exp :  Exp OR TB                          {$$ = makeNode(Or); addChild($$, $1); addChild($$, $3);}
    |  TB                                 {$$ = $1;}
    ;
TB  :  TB AND FB                          {$$ = makeNode(And); addChild($$, $1); addChild($$, $3);}
    |  FB                                 {$$ = $1;}
    ;
FB  :  FB EQ M                            {$$ = makeNode(eq); strcpy($$->comp, $2); $$->category = COMP_C; addChild($$, $1); addChild($$, $3);}
    |  M                                  {$$ = $1;}
    ;
M   :  M ORDER E                          {$$ = makeNode(order); strcpy($$->comp, $2); $$->category = COMP_C; addChild($$, $1); addChild($$, $3);}
    |  E                                  {$$ = $1;}
    ;
E   :  E ADDSUB T                         {$$ = makeNode(addsub); $$->byte = $2; $$->category = BYTE_C; addChild($$, $1); addChild($$, $3);}
    |  T                                  {$$ = $1;}
    ;    
T   :  T DIVSTAR F                        {$$ = makeNode(divstar); $$->byte = $2; $$->category = BYTE_C; addChild($$, $1); addChild($$, $3);}
    |  F                                  {$$ = $1;}
    ;
F   :  ADDSUB F                           {$$ = makeNode(addsub); $$->byte = $1; $$->category = BYTE_C; addChild($$, $2);}
    |  '!' F                              {$$ = makeNode(Not); addChild($$, $2);}
    |  '(' Exp ')'                        {$$ = $2;}
    |  NUM                                {$$ = makeNode(num); $$->num = $1; $$->category = NUM_C;}
    |  CHARACTER                          {$$ = makeNode(character); $$->byte = $1; $$->category = BYTE_C;}    
    |  LValue                             {$$ = $1;}
    |  IDENT '(' Arguments  ')'           {$$ = makeNode(ident); strcpy($$->ident, $1); $$->category = IDENT_C; if ($3 != NULL) {addChild($$, $3);}}
    ;
LValue:
       IDENT                              {$$ = makeNode(ident); strcpy($$->ident, $1); $$->category = IDENT_C;}
    ;
Arguments:
       ListExp                            {$$ = makeNode(Arguments); addChild($$, $1);}
    |                                     {$$ = NULL;}
    ;
ListExp:
       ListExp ',' Exp                    {$$ = $1; addSibling($$, $3);}
    |  Exp                                {$$ = $1;}
    ;
%%

/* Gere les options */
void options(int argc, char **argv, int * tree, int * symtabs){
       int opt, option_index = 0;
       static struct option long_options[] = {
              {"tree", 0, 0, 't'},
              {"symtabs", 0, 0, 's'}, 
              {"help", 0, 0, 'h'},
              {0, 0, 0, 0}
       };
       while((opt = getopt_long(argc, argv, "tsh", long_options, &option_index)) != -1){
              switch(opt){
                     case 't':
                            (*tree) = 1;
                            break;
                     case 's':
                            (*symtabs) = 1;
                            break;
                     case 'h':
                            printf("./tpcas < [FILE]           lance l'analysateur syntaxique sur [FILE]\n");
                            printf("./tpcas -t,--tree < [FILE] affiche l'arbre syntaxique\n");
                            printf("./tpcas -s,--symtabs < [FILE] affiche les tables des symboles\n");                           
                            printf("./tpcas -h,--help          affiche l'interface utilisateur\n");
                            exit(0);
                     default:
                            exit(3);              
              }
       }
}

/* Gere le fichier asm (creation + ecriture) */
void nasm(Node * root, Table **table){
       int nb_label = 0;
       FILE * file;
       file = fopen("_anonymous.asm", "w");
       if (file == NULL){
           fprintf(stderr, "Error : _anonymous.asm\n");
           exit(EXIT_FAILURE);
       }
       print_start(file);
       print_end(file);
       print_all_asm(file, root, table, 1, &nb_label);
       fclose(file);
}

int main(int argc, char** argv){
       int symtabs = 0, tree = 0;
       int sortie = 0, nb_table = 1;

       options(argc, argv, &tree, &symtabs);

       if (yyparse(tree) == 0){
              Node *tmp = root;
              Table ** list_table = malloc(sizeof(Table*));

              list_table = explo_tree(tmp, list_table, &nb_table, &sortie);
              verify_main(list_table[0], &sortie);

              if (symtabs) print_all_function_table(list_table, nb_table);

              tmp = root;
              if (sortie == 0) nasm(tmp, list_table);


              free_table_list(list_table, nb_table);
              return sortie;
       }
       return 1;
}


int yyerror(char option, char* msg){
       fprintf(stderr, "Erreur à la ligne %d : %d\n", lineno, tok_end);
       return 1;
}
