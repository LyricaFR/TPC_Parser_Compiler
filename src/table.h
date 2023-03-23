#ifndef __TABLE__
#define __TABLE__

    #include "tree.h"
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>

    typedef enum {
        type_int,
        type_char,
        type_void
    }Type;

    typedef enum {
        VAR,
        ARG,
        FUN,
        GLB
    }Class;

    typedef struct table{
        char symbol[256];
        Type type;
        int pos;
        Class classe;
        struct table * next;
    }Table;


    typedef struct cases{
        Type type;
        int num;
        char byte;
        struct cases * next;
    }Cases;

    /* renvoie la version chaine de charactère du type mis en paramètre */
    char * type_to_char(Type type);

    /* renvoie la version chaine de charactère de la classe mis en paramètre */
    char * class_to_char(Class classe);

    /* renvoie la version typé de la chaine de charactere mis en paramètre */
    Type char_to_type(char * c);

    /* vérifie si le symbole est dans la table (1 : present , 0 : absent) */
    int is_in_table(char * symbol, Table  * table);

    /* initialise une nouvelle ligne d'une table */
    Table * init_table(char * symbol, Type type, int pos, Class classe);

    /* retourne la ligne du symbole mis en paramètre dans la table (renvoie null si le symbole n'a pas été trouvé */
    Table *find(Table * table, char * symbol);

    /* utilise la fonction find mais sur 2 tables (si le symbole n'a pas été trouvé dans la première table on lance la recherche sur la deuxième */
    Table *find_in_2_tables(Table * table, Table * global_table, char * symbol);

    /* ajoute dans la table si le symbole est présent et le retourne */
    Table * add_symbol(char * symbol, Type type, int pos, Class classes, Table  * table, int lineno, int * sortie);

    /* affiche un message d'avertissement si on trouve un entier dans l'expression de droite (utiliser la fonction si la (lvalue est un char) */
    void verif_rvalue(Node * tmp, Table * table, Table * global_table, Table *Lvalue, int * sortie);

    /* initialise une case (ref aux switch) */
    Cases *init_case(Type type, int num, char byte);

    /* ajoute à cases une nouvelle case, si deja présente affiche un message d'erreur et la sortie devient 2 */
    Cases * add_case(Type type, int num, char byte, Cases  * cases, int lineno, int * sortie);

    /* utilise la fonction add_case en fonction du type de la constante de case */
    Cases * verif_case(Node * node, Cases * cases, int * sortie);
    /* fonction qui verifie l'écriture du switch */
    void verif_switch(Node * node, Cases * cases, int cpt_def, int * sortie);

    /* ajoute tout les symboles d'une partie (param ou corps) */
    Table *add_all_symbol(Table * gobal_table, Table * table, Node *tmp, int *pos, Class classes, int * sortie, Type retour);

    /* renvoie la table globale rempli */
    Table *create_global_table(Node *node, Table * gobal_table, int * sortie);

    /* renvoie une table de fonction rempli */
    Table *create_function_table(Node *node, Table * gobal_table, int * sortie);

    /* affiche une table */
    void print_table(Table * table);

    /* affiche toute les tables mis en liste en paramètre */
    void print_all_function_table(Table * list_table[], int nb_table);

    /* Cette fonction vérifie si la variable représentée par node est déclarée dans la table globale ou la table de fonction */
    void verif_decl(Table * table_globale, Table * table_fun, Node *node, int * sortie);

    /*  libère une table */
    void free_table(Table* table);

    /* libère l'ensemble des tables */
    void free_table_list(Table** lst, int size);

    /* renvoie la taille que prend une table */
    int block_size(Table* table);

    /* ajoute dans la table global les fontions */
    Table * add_function_symbol(Node * node, Table * table, int * sortie);

    /* verifie s'il y a des erreurs sémantiques ou des warnings lors d'une affectation */
    void verify_equals(Node * node, Table * table, Table * global_table, int * sortie);

    /* renvoie la derniere ligne d'une table mis en paramètre */
    Table * last_table(Table * table);

    /* compte le nombre d'argument d'une fonction */
    int count_args(Table * table);

    /* verifie les types des paramètres et affiche les warning si necessaire 
        on utilise cette fonction lors de l appel d une fonction quand le type demandé est un char */
    void verif_param(Node * node, Table * paramtable, Table * table, Table * global_table);

    /* Fonction qui vérifie le nombre d'arguments d'une fonction et affiche un message d'erreur*/
    void verify_nb_args(Node * node, Table * paramtable, Table * table, Table * global_table, int * sortie);

    /* renvoie la position dans la table globale du symbole mis en paramètre */
    int pos_fun_table(char * name, Table * table_globale);
    
    /* exploration de l'arbre afin de créer la table des symboles et de detecter les erreurs sémantiques et warning */
    Table** explo_tree(Node *node, Table** table, int *nb_table, int * sortie);

    /* verifie si un mais est présent et qu'il est de type int */
    void verify_main(Table * table_globale, int * sortie);


#endif
