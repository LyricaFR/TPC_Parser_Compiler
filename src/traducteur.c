#include "traducteur.h"

/* FONCTIONS D'AFFICHAGE */

void print_start(FILE * file){
	fprintf(file, "extern printf, scanf\n");
    fprintf(file, "section .data\n");
    fprintf(file, "\tstringchar : db \"%%c\", 10, 0\n");
    fprintf(file, "\tscanstringchar : db \" %%c\", 0\n");
    fprintf(file, "\tstringint : db \"%%d\", 10, 0\n");
    fprintf(file, "\tscanstringint : db \"%%d\", 0\n");
    fprintf(file, "global _start\n");
    fprintf(file, "section .text\n");
    fprintf(file, "_start:\n");
    fprintf(file, "\tcall main\n");
}

void print_end(FILE * file){
	fprintf(file, "mov rax, 60\n");
	fprintf(file, "mov rdi, 0\n");
	fprintf(file, "syscall\n");	
}

void print_start_function(FILE * file, Table * table){
    fprintf(file, "\tpush rbp\n");
	fprintf(file, "\tmov rbp, rsp\n");
	fprintf(file, "\tsub rsp, %d\n", 16);
}

void print_end_function(FILE * file, Table * table){
    fprintf(file, "\tadd rsp, %d\n", 16);
    fprintf(file, "\tmov rsp, rbp\n");
    fprintf(file, "\tpop rbp\n"); 
    fprintf(file, "\tret\n");  
}

void print_assignment_num(FILE * file, int val){
	fprintf(file, "\tmov eax, %d\n", val);
	fprintf(file, "\tpush rax\n");
}

void print_assignment_byte(FILE * file, char val){
	fprintf(file, "\tmov eax, '%c'\n", val);
	fprintf(file, "\tpush rax\n");
}

void print_add(FILE * file){
	fprintf(file, "\tpop rcx\n");
	fprintf(file, "\tpop rax\n");
	fprintf(file, "\tadd rax, rcx\n");	
	fprintf(file, "\tpush rax\n");
}

void print_sub(FILE * file){
	fprintf(file, "\tpop rcx\n");
	fprintf(file, "\tpop rax\n");
	fprintf(file, "\tsub rax, rcx\n");	
	fprintf(file, "\tpush rax\n");
}

void print_neg(FILE * file){
	fprintf(file, "\tpop rax\n");	
	fprintf(file, "\tneg rax\n");
	fprintf(file, "\tpush rax\n");			
}

void print_mul(FILE * file){
	fprintf(file, "\tpop rcx\n");
	fprintf(file, "\tpop rax\n");
	fprintf(file, "\timul rax, rcx\n");	
	fprintf(file, "\tpush rax\n");
}
void print_div(FILE * file){
	fprintf(file, "\tpop rcx\n");
	fprintf(file, "\tpop rax\n");
	fprintf(file, "\tidiv rax, rcx\n");	
	fprintf(file, "\tpush rax\n");
}

void print_or(FILE * file){
	fprintf(file, "\tpop rcx\n");
	fprintf(file, "\tpop rax\n");
	fprintf(file, "\tor rax, rcx\n");	
	fprintf(file, "\tpush rax\n");
}

void print_and(FILE * file){
	fprintf(file, "\tpop rcx\n");
	fprintf(file, "\tpop rax\n");
	fprintf(file, "\tand rax, rcx\n");	
	fprintf(file, "\tpush rax\n");
}

void print_not(FILE * file){
	fprintf(file, "\tpop rax\n");
	fprintf(file, "\tnot rax\n");	
	fprintf(file, "\tpush rax\n");
}

void print_var(FILE * file, int adresse){
	if (adresse < 0)fprintf(file, "\tmov eax, dword [rbp%d]\n", adresse);   
	else fprintf(file, "\tmov eax, dword [rbp+%d]\n", adresse);
	fprintf(file, "\tpush rax\n");
}

void print_glb(FILE * file, int adresse){
	fprintf(file, "\tmov eax, dword [glb+%d]\n", adresse);
	fprintf(file, "\tpush rax\n");
}

void print_debcl_glb(FILE * file, Node * node){
	if (strcmp(node->type, type_to_char(type_int)) == 0) fprintf(file, "0");
	if (strcmp(node->type, type_to_char(type_char)) == 0) fprintf(file, "''");
	if (node->nextSibling != NULL) {
		fprintf(file, ", ");
		print_debcl_glb(file, node->nextSibling);
	}
}

/* évalue une expression */
void evaluate(FILE * file, Node * node, Table * table, int nb_label, Table * global_table){
	Table * table_tmp;
	if (node->category == NUM_C)
		print_assignment_num(file, node->num);
	

	else if (node->category == BYTE_C && node->firstChild == NULL)
		print_assignment_byte(file, node->byte);

	else if (node->label == ident){
		table_tmp = find_in_2_tables(table, global_table, node->ident);
		if (table_tmp->classe == FUN) {
			if (node->firstChild != NULL) print_arg(file, node->firstChild->firstChild, table, 0, nb_label, global_table);
			fprintf(file, "\tcall %s\n", node->ident);	
			return;
		}
		if (table_tmp->classe == VAR) print_var(file, table_tmp->pos);
		if (table_tmp->classe == GLB) print_glb(file, table_tmp->pos);
	}	
	/*
	else if (node->category == COMP_C){
		print_comp(file, node, table, nb_label, global_table);
	}*/

	if (node->firstChild != NULL){
		evaluate(file, node->firstChild, table,nb_label, global_table); 	
		if (node->firstChild->nextSibling)
			evaluate(file, node->firstChild->nextSibling, table, nb_label, global_table); 
	}
	if (node->byte == '+') print_add(file);
	else if (node->byte == '-' && node->firstChild->nextSibling == NULL) print_neg(file);
	else if (node->byte == '-') print_sub(file);	
	else if (node->byte == '*') print_mul(file);	
	else if (node->byte == '/') print_div(file);	
	else if (node->byte == '!') print_not(file);	
	else if (node->label == And )print_and(file);	
	else if (node->label == Or )print_or(file);					
}

/* fonction qui gère la comparaison lors de if ou de while */
void print_comp(FILE * file, Node * node, Table* table, int nb_label, Table * global){
	if (node->category == COMP_C) {
		evaluate(file, node->firstChild, table, nb_label, global);
		evaluate(file, node->firstChild->nextSibling, table, nb_label, global);
		fprintf(file, "\tpop rcx\n");
		fprintf(file, "\tpop rax\n");		
		fprintf(file, "\tcmp rcx, rax\n");
		if (strcmp(node->comp, "<") == 0) fprintf(file, "\tjl label%d\n", nb_label);
		if (strcmp(node->comp, "<=") == 0) fprintf(file, "\tjle label%d\n", nb_label);	
		if (strcmp(node->comp, ">") == 0) fprintf(file, "\tjg label%d\n", nb_label);
		if (strcmp(node->comp, ">=") == 0) fprintf(file, "\tjge label%d\n", nb_label);	
		if (strcmp(node->comp, "==") == 0) fprintf(file, "\tje label%d\n", nb_label);
		if (strcmp(node->comp, "!=") == 0) fprintf(file, "\tjne label%d\n", nb_label);	
		return;	
	}	
	evaluate(file, node, table, nb_label, global);
	fprintf(file, "\tpop rax\n"); 
	fprintf(file, "\tcmp rax, 1\n"); 
	fprintf(file, "\tjge label%d\n", nb_label);	
}

/* affiche les instructions pour un while */
void print_while(FILE *file, Node * node, Table * table, int *nb_label, Table * global){
	int nb_temp = (*nb_label);
	(*nb_label) += 3;
	fprintf(file, "label%d:\n", nb_temp + 1);									/* le label +1 pour revenir au debut de la boucle */
	print_comp(file, node->firstChild, table, nb_temp + 3, global);			/* la condition et si la condition est vrai on va au label +3 */
	fprintf(file, "\tjmp label%d\n", nb_temp + 2);								/* sinon on va au label +2 pour sortir de la boucle */
	fprintf(file, "label%d:\n", nb_temp + 3);									/* label +3 du corps du while */
	print_body(file, node->firstChild->nextSibling, table, nb_label, global);	/* le corps de la boucle */
	fprintf(file, "\tjmp label%d\n", nb_temp + 1);	  							/* on on va au label +1 pour boucler */
	fprintf(file, "label%d:\n", nb_temp + 2);	 								/* label +2 pour sortir de la boucle */
	if (node->nextSibling != NULL) 												
    	print_body(file, node->nextSibling, table, nb_label, global);		/* les instructions après la boucle */
}

/* affiche les instructions pour un if */
void print_if(FILE *file, Node * node, Table * table, int *nb_label, Table * global){
	int nb_temp = (*nb_label);
	(*nb_label) += 3;
	print_comp(file, node->firstChild, table, nb_temp + 1, global);							/* la condition et si la condition est vrai on va au label +1 */
	fprintf(file, "\tjmp label%d\n", nb_temp + 2);                                             /* sinon on va au label +2 */
    fprintf(file, "label%d:\n", nb_temp + 1);	                                                /* label +1 du corps du if */			
	if (node->nextSibling != NULL){
		if (node->nextSibling->label == Else){													/* si on a un else */
	    	if (node->firstChild->nextSibling != NULL) 
	        	print_body(file, node->firstChild->nextSibling, table, nb_label, global);    /* le corps du if */
	        fprintf(file, "\tjmp label%d\n", nb_temp + 3);	                                    /* on va au label +3 pour sortir du if et évité le else */
	        fprintf(file, "label%d:\n", nb_temp + 2); 										    /* label +2 du corps du else */
	    	if (node->nextSibling != NULL) 
	        	print_body(file, node->nextSibling, table, nb_label, global);				/* le corps du else */
	        fprintf(file, "label%d:\n", nb_temp + 3)	;	        		        	 		/* label 3 pour sortir */						
		}
		else {																							/* si on a pas de else */
			if (node->firstChild->nextSibling != NULL) 
		    	print_body(file, node->firstChild->nextSibling, table, nb_label, global);  			/* le corps du if */
		    fprintf(file, "label%d:\n", nb_temp + 2);       											/* label 2 pour sortir */
		}
	    if (node->nextSibling != NULL) 
	        print_body(file, node->nextSibling, table, nb_label, global);						/* les instructions après le if */
	}
}

/* affiche les instructions pour un return */
void print_return(FILE *file, Node * node, Table * table, int *nb_label, Table * global){
	if  (node->firstChild != NULL){									/* si il y a une valeur de retour */
		evaluate(file, node->firstChild, table, *nb_label, global);	/* évalue l'expression */
		fprintf(file, "\tpop rax\n");								/* et on retourne le résultat de l'évaluation dans rax */
	}
	print_end_function(file, table);
}

/* affiche les instructions pour un '=' */
void print_equal(FILE *file, Node * node, Table * table, int *nb_label, Table * global){
	Table *lvalue = NULL;
	evaluate(file, node->firstChild->nextSibling, table, *nb_label, global);	/* on évalue l'expression apres le égale */
	lvalue = find_in_2_tables(table, global, node->firstChild->ident);          /* on recupere la ligne de la lvalue */
	fprintf(file, "\tpop rax\n");                                            	/* et on retourne le résultat de l'évaluation dans rax */
	if (lvalue->classe == VAR) {
		if (lvalue->pos < 0)fprintf(file, "\tmov dword [rbp%d], eax\n", lvalue->pos);	  			/* on ecrit le mov pour mettre la valeur a la position de la lvalue */
		else fprintf(file, "\tmov dword [rbp+%d], eax\n", lvalue->pos);
	}
	if (lvalue->classe == GLB) fprintf(file, "\tmov dword [glb+%d], eax\n", lvalue->pos);	  			/* on ecrit le mov pour mettre la valeur a la position de la lvalue */
	if (node->nextSibling != NULL) 
        print_body(file, node->nextSibling, table, nb_label, global);
}

/* utilisé avant l'appel des fonctions afin de donner les arguments */
void print_arg(FILE * file, Node * node, Table * table, int nb_arg,  int nb_label, Table * global){
	Table *tmp = NULL;
	char * reg[6] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
	tmp = find_in_2_tables(table, global, node->ident);
	if (tmp == NULL) return;
	if (tmp->classe == FUN){								/* si c'est une fonction en argument */
		evaluate(file, node, table, nb_label, global);		/* on evalue */
		fprintf(file, "\tmov %s, eax\n", reg[nb_arg]);
	}
	/* si il y a moins de 7 argument (de 0 à 5) */
	else if (node->firstChild == NULL && nb_arg < 6){
		/* si c'est une variable */
		if (tmp->classe == VAR) {
			if (tmp->pos < 0)fprintf(file, "\tmov %s, dword [rbp%d]\n", reg[nb_arg], tmp->pos);
			else fprintf(file, "\tmov %s, dword [rbp+%d]\n", reg[nb_arg], tmp->pos);
		}
		/* si c'est une globale */
		if (tmp->classe == GLB) fprintf(file, "\tmov %s, dword [glb+%d]\n", reg[nb_arg], tmp->pos);
	}
	/* si il y a plus de 6 arguments on les push */
	else {
		if (tmp->classe == VAR){
			if (tmp->pos < 0)fprintf(file, "\tmov eax, dword [rbp%d]\n", tmp->pos);
			else fprintf(file, "\tmov eax, dword [rbp+%d]\n", tmp->pos);
		} 
		if (tmp->classe == GLB) fprintf(file, "\tmov eax, dword [glb+%d]\n", tmp->pos);	
		fprintf(file, "\tpush rax\n");
	}	
	nb_arg++;
    if (node->firstChild != NULL) 
        print_arg(file, node->firstChild, table, nb_arg,nb_label, global);
    if (node->nextSibling != NULL) {
        print_arg(file, node->nextSibling, table, nb_arg, nb_label, global);
    }
}

/* utilisé dans les fonctions afin de récupéres les paramètres */
void print_param(FILE * file, Node * node, Table * table, int nb_arg, Table * global){
	Table *tmp = NULL;
	char * reg[6] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};	
	/* si il y a moins de 7 argument (de 0 à 5) */
	if (node->firstChild != NULL && nb_arg < 6){
		/* on affiche simplement */
		tmp = find_in_2_tables(table, global, node->firstChild->ident);
		if (tmp == NULL) return;
		if (tmp->pos < 0)fprintf(file, "\tmov dword [rbp%d], %s\n", tmp->pos, reg[nb_arg]);
		else fprintf(file, "\tmov dword [rbp+%d], %s\n", tmp->pos, reg[nb_arg]);
		if (tmp->classe == VAR) {
			if (tmp->pos < 0)fprintf(file, "\tmov dword [rbp%d], %s\n", tmp->pos, reg[nb_arg]);
			else fprintf(file, "\tmov dword [rbp+%d], %s\n", tmp->pos, reg[nb_arg]);
		}
		if (tmp->classe == GLB) fprintf(file, "\tmov dword [glb+%d], %s\n", tmp->pos, reg[nb_arg]);
		nb_arg++;
	}
    if (node->firstChild != NULL) 
        print_param(file, node->firstChild, table, nb_arg, global);
    if (node->nextSibling != NULL) 
        print_param(file, node->nextSibling, table, nb_arg, global);
}

/* affiche les differentes instructions de corps d'une fonction */
void print_body(FILE * file, Node *node, Table* table, int *nb_label, Table * global){
	Table *tmp = NULL;
    if (node->label == equal)
		return print_equal(file, node, table, nb_label, global);

	if (node->label == While)
		return print_while(file, node, table, nb_label, global);									
	
	if (node->label == If)
		return print_if(file, node, table, nb_label, global);		        	 	

	if (node->label == ident){
		tmp = find(global, node->ident);
		if (tmp != NULL){
			if (tmp->classe == FUN){
				evaluate(file, node, table, *nb_label, global);
				
			}
		}
	}
	if (node->label == Return)
		print_return(file, node, table, nb_label, global);
    if (node->firstChild != NULL) 
        print_body(file, node->firstChild, table, nb_label, global);
    if (node->nextSibling != NULL) 
        print_body(file, node->nextSibling, table, nb_label, global);	
}

/* affiche l'ensemble des instructions du programme */
void print_all_asm(FILE * file, Node *node, Table** table, int nb_table, int * nb_label){
    if (node->label == DeclVars){
    	fprintf(file, "section .data\n");
    	fprintf(file, "\tglb : dd ");
    	print_debcl_glb(file, node->firstChild);
    	fprintf(file, "\n");
    	if (node->nextSibling != NULL) 
        	print_all_asm(file, node->nextSibling, table, nb_table, nb_label);    	
        return;
    }
    else if (node->label == DeclFonct){
    	fprintf(file, "%s:\n", node->firstChild->nextSibling->ident);
        print_start_function(file, table[(nb_table)]);
		print_param(file, node->firstChild->nextSibling->nextSibling->firstChild, table[(nb_table)], 0, table[0]);
        print_body(file, node->firstChild->nextSibling->nextSibling->nextSibling, table[(nb_table)], nb_label, table[0]);
        print_end_function(file, table[(nb_table)]);    
        (nb_table)++;
        if (node->nextSibling != NULL)
            print_all_asm(file, node->nextSibling, table, nb_table, nb_label);
        return;
    } 
    if (node->firstChild != NULL) 
        return print_all_asm(file, node->firstChild, table, nb_table, nb_label);
    if (node->nextSibling != NULL) 
        return print_all_asm(file, node->nextSibling, table, nb_table, nb_label);

}

/* FONCTIONS PUTCHAR / PUTINT / GETCHAR / GETINT */

void my_putchar(FILE * file){
    fprintf(file, "putchar:\n");                              
    fprintf(file, "\tmov sil, byte [rbp]\n");  
    fprintf(file, "\tmov rdi, stringchar\n");                     /* stringchar : db "%c", 1, 0  dans section .data*/
    fprintf(file, "\tmov al, 0\n");
    fprintf(file, "\tcall printf\n");
    fprintf(file, "\tret\n");                                     
}

void my_getchar(FILE * file){
    fprintf(file, "getchar:\n");
    fprintf(file, "\tmov sil, byte [rbp]\n");
    fprintf(file, "\tmov rdi, scanstringchar\n");
    fprintf(file, "\tmov al, 0\n");
    fprintf(file, "\tcall scanf\n");
    fprintf(file, "\tmov eax, dword [rbp]\n");    
    fprintf(file, "\tret\n");                
}

void my_putint(FILE * file){
    fprintf(file, "putint:\n");                                  /* Nom de la fonction */
    fprintf(file, "\tmov esi, dword [rbp]\n");     /* L'entier à afficher */
    fprintf(file, "\tmov rdi, stringint\n");                     /* stringint : db "%d", 10, 0  dans section .data*/
    fprintf(file, "\tmov al, 0\n");
    fprintf(file, "\tcall printf\n");
    fprintf(file, "\tret\n");                                    /* La fonction ne retourne rien */
}

void my_getint(FILE * file){
    fprintf(file, "getint:\n");
    fprintf(file, "\tmov esi, dword [rbp]\n");
    fprintf(file, "\tmov rdi, scanstringint\n");
    fprintf(file, "\tmov al, 0\n");
    fprintf(file, "\tcall scanf\n");
    fprintf(file, "\tmov eax, dword [rbp]\n");       
    fprintf(file, "\tret\n");
}