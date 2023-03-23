#ifndef __TRADUCTEUR__
#define __TRADUCTEUR__

    #include <stdio.h>
    #include <stdlib.h>
    #include "table.h"

    /* évalue une expression */
    void evaluate(FILE * file, Node * node, Table * table, int nb_label, Table * global_table);

    /* fonction qui gère la comparaison lors de if ou de while */
    void print_comp(FILE * file, Node * node, Table* table, int nb_label, Table * global);

    /* affiche les instructions pour un while */
    void print_while(FILE *file, Node * node, Table * table, int *nb_label, Table * global);

    /* affiche les instructions pour un if */
    void print_if(FILE *file, Node * node, Table * table, int *nb_label, Table * global);

    /* affiche les instructions pour un return */
    void print_return(FILE *file, Node * node, Table * table, int *nb_label, Table * global);

    /* affiche les instructions pour un '=' */
    void print_equal(FILE *file, Node * node, Table * table, int *nb_label, Table * global);

    /* utilisé avant l'appel des fonctions afin de donner les arguments */
    void print_arg(FILE * file, Node * node, Table * table, int nb_arg, int nb_label, Table * global);

    /* utilisé dans les fonctions afin de récupéres les paramètres */
    void print_param(FILE * file, Node * node, Table * table, int nb_arg, Table * global);

    /* affiche les differentes instructions de corps d'une fonction */
    void print_body(FILE * file, Node *node, Table* table, int *nb_label, Table * global);

    /* affiche l'ensemble des instructions du programme */
    void print_all_asm(FILE * file, Node *node, Table** table, int nb_table, int *nb_label);

     /* FONCTION D'AFFICHAGE */
    void print_start(FILE * file);


    void print_end(FILE * file);


    void print_start_function(FILE * file, Table * table);


    void print_end_function(FILE * file, Table * table);


    void print_assignment_num(FILE * file, int val);


    void print_assignment_byte(FILE * file, char val);


    void print_add(FILE * file);


    void print_sub(FILE * file);


    void print_neg(FILE * file);


    void print_mul(FILE * file);


    void print_div(FILE * file);


    void print_or(FILE * file);
    

    void print_and(FILE * file);


    void print_var(FILE * file, int adresse);   

    /* FONCTIONS PUTCHAR / PUTINT / GETCHAR / GETINT */
    void my_putchar(FILE * file);


    void my_getchar(FILE * file);


    void my_putint(FILE * file);
    

    void my_getint(FILE * file);

 #endif