#include "table.h"


/* Affichage des avertissements */
static void print_warning_assigment(int lineno){
    fprintf(stderr, "warning: assigment to 'char' from 'int' (line %d)\n", lineno);    
}

static void print_warning_assigment_param(int lineno){
    fprintf(stderr, "warning: expected 'int' but arguments is of type 'char' (line %d)\n", lineno);    
}

static void print_warning_return_void_fun_non_void(int lineno){
    fprintf(stderr, "warning: 'return' with no value, in function returning non-void (line %d)\n", lineno);    
}

static void print_warning_return_no_void_fun_void(int lineno){
    fprintf(stderr, "warning: 'return' with a value, in function returning void (line %d)\n", lineno);    
}

/* Affichage des erreurs semantiques */
static void print_error_redeclaration(char * symbol, int lineno){
    fprintf(stderr, "error: redeclaration of '%s' (line %d)\n", symbol, lineno);   
}

static void print_error_duplicate_cases(int lineno){
    fprintf(stderr, "error: duplicate cases value (line %d)\n", lineno);
}

static void print_error_multiple_default(int lineno){
    fprintf(stderr, "error: multiple default labels in one switch (line %d)\n", lineno);
}

static void print_error_undeclared(char * symbol, int lineno){
    fprintf(stderr, "error: '%s' undeclared (line %d)\n", symbol, lineno);  
}

static void print_error_undefined_main(){
    fprintf(stderr, "error : undefined reference to `main'\n");
}

static void print_error_too_many_argument(char * symbol, int lineno){
    fprintf(stderr, "error: too many arguments to function '%s' (line %d)\n", symbol, lineno); 
}

static void print_error_too_few_argument(char * symbol, int lineno){
    fprintf(stderr, "error: too few arguments to function '%s' (line %d)\n", symbol, lineno); 
}

static void print_error_no_return(int lineno){
    fprintf(stderr, "error: void value not ignored as it ought to be (line %d)\n", lineno);     
}

/* renvoie la version chaine de charactère du type mis en paramètre */
char * type_to_char(Type type){
    if (type == type_char) return "char";
    else if (type == type_int) return "int";
    return "void";
}
/* renvoie la version chaine de charactère de la classe mis en paramètre */
char * class_to_char(Class classe){
    if (classe == VAR) return "var";
    else if (classe == ARG) return "arg";
    else if (classe == FUN) return "fun";
    else if (classe == GLB) return "glb";  
    return "" ;    
}

/* renvoie la version typé de la chaine de charactere mis en paramètre */
Type char_to_type(char * c){
    if (strcmp(c, "char") == 0) return type_char;
    else if (strcmp(c, "int") == 0) return type_int;
    return type_void;
}

/* vérifie si le symbole est dans la table (1 : present , 0 : absent) */
int is_in_table(char * symbol, Table  * table){
    Table  * tmp = table;
    while (tmp != NULL){
        if (strcmp(symbol, tmp->symbol) == 0) return 1;
        tmp = tmp->next;
    }
    return 0;
}

/* initialise une nouvelle ligne d'une table */
Table * init_table(char * symbol, Type type, int pos, Class classe){
    Table * table =  malloc(sizeof(Table));
    if (table == NULL){
        fprintf(stderr, "Error: table is NULL\n");
        return NULL;
    }
    strcpy(table->symbol, symbol);
    table->type = type;
    table->pos = pos;
    table->classe = classe;
    table->next = NULL;
    return table;
}

/* retourne la ligne du symbole mis en paramètre dans la table (renvoie null si le symbole n'a pas été trouvé */
Table *find(Table * table, char * symbol){
    Table * tmp = table;
    while (tmp != NULL){
        if (strcmp(tmp->symbol, symbol) == 0)
            return tmp;
        tmp = tmp->next;
    }

    return NULL;
}

/* utilise la fonction find mais sur 2 tables (si le symbole n'a pas été trouvé dans la première table on lance la recherche sur la deuxième */
Table *find_in_2_tables(Table * table, Table * global_table, char * symbol){
    Table * tmp = find(table, symbol);
    if (tmp == NULL){
        tmp = find(global_table, symbol);
    }
    return tmp;
}

/* ajoute dans la table si le symbole est présent et le retourne */
Table * add_symbol(char * symbol, Type type, int pos, Class classes, Table  * table, int lineno, int * sortie){
    if (is_in_table(symbol, table)){
        (*sortie) = 2;
        print_error_redeclaration(symbol, lineno);
        return table;
    }
    if (table == NULL)
        return init_table(symbol, type, pos, classes);
    table->next = add_symbol(symbol, type, pos, classes, table->next, lineno, sortie);
    return table;
}

/* affiche un message d'avertissement si on trouve un entier dans l'expression de droite (utiliser la fonction si la (lvalue est un char) */
void verif_rvalue(Node * tmp, Table * table, Table * global_table, Table *Lvalue, int * sortie){
    Table * Rvalue;
    if (tmp->label == Arguments) return;
    if (Lvalue->type == type_char && tmp->category == NUM_C){                /* on verifie si c'est un entier */
        print_warning_assigment(tmp->lineno);
        return;
    }

    if (tmp->category == IDENT_C){              /* on verifie si c'est une identificateur */
        Rvalue = find_in_2_tables(table, global_table, tmp->ident);
        if (Rvalue->type == type_void){
            print_error_no_return(tmp->lineno);
            (*sortie) = 2;
            return;            
        }
        if (Lvalue->type == type_char && Rvalue->type == type_int){          /* on verifie si c'est un entier */
            print_warning_assigment(tmp->lineno);
            return;
        }
    }
    /* on parcours l'expression */
    if (tmp->firstChild != NULL)
        verif_rvalue(tmp->firstChild, table, global_table, Lvalue, sortie);
    if (tmp->nextSibling != NULL)
        verif_rvalue(tmp->nextSibling, table, global_table, Lvalue, sortie);
       
}

/* initialise une case (ref aux switch) */
Cases *init_case(Type type, int num, char byte){
    Cases * cases = (Cases*)malloc(sizeof(Case));
    if (cases == NULL){
        fprintf(stderr, "Error: cases is NULL\n");
        return NULL;
    }
    cases->type = type;
    cases->num = num;
    cases->byte = byte;
    cases->next = NULL;
    return cases;
}

/* ajoute à cases une nouvelle case, si deja présente affiche un message d'erreur et la sortie devient 2 */
Cases * add_case(Type type, int num, char byte, Cases  * cases, int lineno, int * sortie){
    if (cases == NULL)
        return init_case(type, num, byte);

    /* verifie si on a deux case avec un même entier */
    if ((cases->type == type_int) && (cases->num == num)){
        print_error_duplicate_cases(lineno);
        (*sortie) = 2;
        return cases;  
    }
    /* verifie si on a deux case avec un même charactère */
    if ((cases->type == type_char) && (cases->byte == byte)){
        print_error_duplicate_cases(lineno);
        (*sortie) = 2;
        return cases;
    }
    cases->next = add_case(type, num, byte, cases->next, lineno, sortie);
    return cases;
}

/* utilise la fonction add_case en fonction du type de la constante de case */
Cases * verif_case(Node * node, Cases * cases, int * sortie){
    Node * tmp = node->firstChild;
    if (tmp->category == NUM_C){
        cases = add_case(type_int, tmp->num, ' ', cases, node->lineno, sortie);
    }
    if (tmp->category == BYTE_C){
        cases = add_case(type_char, 0, tmp->byte, cases, node->lineno, sortie);
    }
    return cases;
}
/* fonction qui verifie l'écriture du switch */
void verif_switch(Node * node, Cases * cases, int cpt_def, int * sortie){
    if (node->label == Default) {                       /* On compte le nombre de default d'un switch*/
        cpt_def++;
        if (cpt_def > 1){                               /* si il y en a plus d'un on affiche une erreur et la sortie devient 2 */
            print_error_multiple_default(node->lineno);
            (*sortie) = 2;
        }
    }
    if (node->label == Case){                 
        cases = verif_case(node, cases, sortie);                /* On verifie que 2 cases ne sont pas pareille */
    }

    if (node->firstChild != NULL) {
        if (node->firstChild->label != Switch)
            verif_switch(node->firstChild, cases, cpt_def, sortie);
    }
    if (node->nextSibling != NULL) {
        if (node->nextSibling->label != Switch)
            verif_switch(node->nextSibling, cases, cpt_def, sortie);
    }
}

/* ajoute tout les symboles d'une partie (param ou corps) */
Table *add_all_symbol(Table * gobal_table, Table * table, Node *tmp, int *pos, Class classes, int * sortie, Type retour){
    Node *tmp2, *start = tmp;
    Type type;
    while(start != NULL) {  
        /* On parcourt toute les declarations */
        if ((strcmp(start->type, "char") == 0 || strcmp(start->type, "int") == 0)){
            type = char_to_type(start->type);         /* On recupere le type */       
            tmp2 = start->firstChild;                 /* On parcours toute les variable globale qui sont fils de type */
            while(tmp2 != NULL){
                if (classes == VAR || classes == ARG){
                    if (type == type_char) (*pos)--;
                    if (type == type_int) (*pos) -= 4;
                }                
                table = add_symbol(tmp2->ident, type, *pos, classes,table, tmp2->lineno, sortie);
                if (classes == GLB){
                    if (type == type_char) (*pos)++;
                    if (type == type_int) (*pos) += 4;
                }
                tmp2 = tmp2->nextSibling;
            }
        }
        else 
            verif_decl(gobal_table, table, start, sortie);   
        
        start = start->nextSibling;   
    } 
    return table;
}

/* renvoie la table globale rempli */
Table *create_global_table(Node *node, Table * gobal_table, int * sortie){
    int pos = 0;
    Node * tmp = node;
    Table *table = NULL;
    if (node->label != DeclVars)
        return NULL;
    /* On decend en dessous de DeclVars */
    if (tmp->firstChild != NULL)
        return add_all_symbol(gobal_table, table, tmp->firstChild, &pos, GLB, sortie, type_void);
    return table;
}

/* renvoie une table de fonction rempli */
Table *create_function_table(Node *node, Table * gobal_table, int * sortie){
    int pos = 0;
    Table * table = NULL;
    Node * tmp = node->firstChild->nextSibling->nextSibling;
    /* ajoute les symboles paramètres */
    table = add_all_symbol(gobal_table, table, tmp->firstChild, &pos, ARG, sortie, char_to_type(node->firstChild->type));  
    /* ajoute les symbole du corps */
    table = add_all_symbol(gobal_table, table, tmp->nextSibling->firstChild, &pos, VAR, sortie, char_to_type(node->firstChild->type)); 
    return table;
}

/* affiche une table */
void print_table(Table * table){
    Table * tmp = table;
    printf("\n");
    printf("==================================================================\n");
    printf("====================== Table des symboles ========================\n");
    printf("==================================================================\n");
    printf(" Symbole       |  Type          | Adresse        | Classe        \n");
    while(tmp != NULL){
        printf("%14s | %14s | %14d | %14s \n", tmp->symbol, type_to_char(tmp->type), tmp->pos, class_to_char(tmp->classe));
        tmp = tmp->next;

    }
    printf("==================================================================\n");
    printf("\n");  

}

/* affiche toute les tables mis en liste en paramètre */
void print_all_function_table(Table * list_table[], int nb_table){
    int i;
    for (i = 0; i < nb_table; i++){
       print_table(list_table[i]); 
    }
}

/* Cette fonction vérifie si la variable représentée par node est déclarée dans la table globale ou la table de fonction */
void verif_decl(Table * table_globale, Table * table_fun, Node *node, int * sortie){
    if (node == NULL) return;  
    if (node->category == IDENT_C){
        if ((is_in_table(node->ident, table_fun) == 0) && (is_in_table(node->ident, table_globale) == 0)) {
            (*sortie) = 2;
            print_error_undeclared(node->ident, node->lineno);
        }
    }
    if (node->firstChild != NULL){
        verif_decl(table_globale, table_fun, node->firstChild, sortie);
        if (node->firstChild->nextSibling != NULL) 
            verif_decl(table_globale, table_fun, node->firstChild->nextSibling, sortie);    
    }
}

/*  libère une table */
void free_table(Table* table){
    if (table->next != NULL){
        free_table(table->next);
    }
    free(table);
}

/* libère l'ensemble des tables */
void free_table_list(Table** lst, int size){
    int i;
    for (i = size - 1; i >= 0; i--){
        if (lst[i] != NULL)
            free_table(lst[i]);
    }
}

/* renvoie la taille que prend une table */
int block_size(Table* table){
    Table * tmp = table;
    if (tmp == NULL) return 0;
    while (tmp->next != NULL){
        tmp = tmp->next;
    }
    return tmp->pos + (tmp->type == type_int ? 4 : 1);
}

/* ajoute dans la table global les fontions */
Table * add_function_symbol(Node * node, Table * table, int * sortie){
    return add_symbol(node->firstChild->nextSibling->ident, char_to_type(node->firstChild->type), block_size(table), FUN, table, node->lineno, sortie);
}

/* verifie s'il y a des erreurs sémantiques ou des warnings lors d'une affectation */
void verify_equals(Node * node, Table * table, Table * global_table, int * sortie){
    Table * Lvalue;
    Lvalue = find_in_2_tables(table, global_table, node->firstChild->ident);
    if (Lvalue == NULL) return;
        /* verif_decl(global_table, table, node, sortie); */
    verif_rvalue(node->firstChild->nextSibling, table, global_table, Lvalue, sortie);
}

/* renvoie la derniere ligne d'une table mis en paramètre */
Table * last_table(Table * table){
    Table * tmp = table;
    while (tmp->next != NULL)
        tmp = tmp->next;
    return tmp;
}

/* compte le nombre d'argument d'une fonction */
int count_args(Table * table){
    int count = 0;
    Table * tmp = table;
    while (tmp != NULL){
        if (tmp->classe == ARG) count++;
        tmp = tmp->next;    
    }
    return count;
}

/* verifie les types des paramètres et affiche les warning si necessaire 
    on utilise cette fonction lors de l appel d une fonction quand le type demandé est un char */
void verif_param(Node * node, Table * paramtable, Table * table, Table * global_table){
    /* si c'est une variable */
    if (node->category == IDENT_C){ 
        Table *tmp = find_in_2_tables(table, global_table, node->ident);
        if (tmp == NULL) return;
        if (tmp->type == type_int){
            print_warning_assigment_param(node->lineno);
            return;
        }
        return;
    }
    /* si c'est un entier */
    if (node->category == NUM_C){
        print_warning_assigment_param(node->lineno);
        return;
    }
    if (node->firstChild != NULL) {
        verif_param(node->firstChild, paramtable, table,global_table);
        if (node->firstChild->nextSibling != NULL) 
            verif_param(node->firstChild->nextSibling, paramtable, table,global_table);
    }
}

/* Fonction qui vérifie le nombre d'arguments d'une fonction et affiche un message d'erreur*/
void verify_nb_args(Node * node, Table * paramtable, Table * table, Table * global_table, int * sortie){

    Table * tmp2 = paramtable;
    int nb_args, nb_param;
    nb_args = count_args(paramtable);
    if (node->nextSibling != NULL){
        if (node->nextSibling->label == Parametres) return;
    }

    /* si il n'y a pas l'argument donné lors 2 l'appel 
    mais que la fonction demande 1 ou des arguments */
    if (node->firstChild == NULL && nb_args != 0) {
        print_error_too_few_argument(node->ident, node->lineno);
        (*sortie) = 2;
        return; 
    }
    /* si il n'y a pas l'argument donné lors 2 l'appel 
    et que la fonction ne demande pas d'argument */
    if (node->firstChild == NULL && nb_args == 0)return;    
    Node * tmp = node->firstChild->firstChild;
    while(tmp != NULL && tmp2 != NULL){
        if (tmp2->classe != ARG) break;
        if (tmp2->type == type_char){
            verif_param(tmp,paramtable, table, global_table);       /* verifie les paramètres si le type demandé était un char */
        }
        tmp2 = tmp2->next;
        tmp = tmp->nextSibling;
    }
    /* si le nombre d'arguments est correcte */
    if (tmp == NULL && tmp2 == NULL) return;
    /* si il y a plus d'argument que demandé */
    if (tmp != NULL){
        print_error_too_many_argument(node->ident, node->lineno);
        (*sortie) = 2; 
        return;        
    } 
    /* si il y a moins d'argument que demandé */
    if (tmp == NULL && tmp2->classe == ARG){
        print_error_too_few_argument(node->ident, node->lineno);
        (*sortie) = 2;
        return;        
    } 
    
}

/* renvoie la position dans la table globale du symbole mis en paramètre */
int pos_fun_table(char * name, Table * table_globale){
    int pos = 1;
    Table * tmp = table_globale;
    while(tmp != NULL){
        if (strcmp(tmp->symbol, name) == 0 && tmp->classe == FUN) return pos;
        if (tmp->classe == FUN) pos++;
        tmp = tmp->next;
    }
    return 0;
}

/* exploration de l'arbre afin de créer la table des symboles et de detecter les erreurs sémantiques et warning */
Table** explo_tree(Node *node, Table** table, int *nb_table, int * sortie){
    /* Si on rencontre un identificateur, on vérifie qu'il a été déclaré */
    if (node->label == DeclVars){
        table[0] = create_global_table(node, table[0], sortie);                 /* creer la table globales */
        if (node->nextSibling != NULL)
            return explo_tree(node->nextSibling, table, nb_table, sortie);
        return table;
    }
    if (node->label == DeclFonct){
        table[0] = add_function_symbol(node, table[0], sortie);                 /* ajoute le symbole de la fonction à la table globale */
        table = realloc(table, sizeof(Table*) * ((*nb_table) + 1 ));
        table[(*nb_table)] = create_function_table(node, table[0], sortie);     /* creer la table de la fonction (arguments + variables ) */
        (*nb_table)++;
    } 

    if (node->label == equal){
        verify_equals(node, table[(*nb_table) - 1], table[0], sortie);
    }

    if (node->label == Return && node->firstChild != NULL)
        verif_rvalue(node->firstChild, table[(*nb_table) - 1], table[0], last_table(table[0]), sortie);

    if (node->label == Switch){
        Cases * cases = NULL;
        verif_switch(node, cases, 0, sortie);
    }

    /* vérifie les fonction (nombre d'arguments et type) */
    if (node->category == IDENT_C){
        Table * tmp;
        tmp = find(table[0], node->ident);
        if (tmp != NULL){
            if (tmp->classe == FUN){
                verify_nb_args(node, table[pos_fun_table(node->ident, table[0])], table[(*nb_table) - 1], table[0], sortie);
            }
        }
    }
    if (node->firstChild != NULL) 
        table = explo_tree(node->firstChild, table, nb_table, sortie);
    if (node->nextSibling != NULL) 
        table = explo_tree(node->nextSibling, table, nb_table, sortie);
    return table;

}

/* verifie si un mais est présent et qu'il est de type int */
void verify_main(Table * table_globale, int * sortie){
    Table * tmp = table_globale;
    while (tmp != NULL){
        if ((strcmp(tmp->symbol, "main") == 0) && (tmp->type == type_int)) return;
        tmp = tmp->next;
    }    
    print_error_undefined_main();
    (*sortie) = 2;
}
