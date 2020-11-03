#include<stdio.h>
#include "ml_lib.h"

/*---------------------------------SEZIONE FUNZIONI GESTIONE MEMORIA---------------------------------*/

/*Restituisce un puntatore alla matrice contenente le pedine*/
pedina **createPedine(){
    int i,j;
    pedina **p= (pedina**) malloc(11*2*sizeof(pedina));

    for(i =0; i < 2; i++){
        for(j = 0; j < 11; j++){
            p[i][j] = (pedina*) malloc(sizeof(pedina));
            p[i][j]->id_pedina = j;
            p[i][j]->id_giocatore = i;
            p[i][j]->grado = 0;
        }
    }

    return p;
}

/*Distrugge la matrice contenente le pedine*/
void destroyPedine(pedina **p){
    int i,j;

    for(i = 0; i < 2; i++){
        for(j = 0; j < 11; j++)
            free(p[i][j]);
    }
    free(p);
}

/* Funzione che crea la matrice della scacchiera */
pedina **createMatrix(const pedina r, const pedina c){

	pedina **m = (pedina**) malloc(r*c*sizeof(pedina));
	
	return m;
}

void destroyMatrix(pedina **board){
    free(board);
}

/*
* Numero 1 usato per identificare il player 1,
* Numero 2 usato per identificare il player 2,
* Numero 0 usato per identificare le celle su cui non sono ammessi posizionamenti delle pedine,
* Simbolo # usato per identificare la linea di gioco tra i due player.
*
* Funzione che inizializza la matrice con le pedine dei player
*/
void setValuesMatrix(pedina **board, pedina **players){
	
	/*int r1,c1;*/
	int i,j, p = 0;
	
	for(i = 0; i < ROW; i++){
	    for(j = 0; j < COL; j++){
	        if((i % 2 && !(j % 2)) || (!(i % 2) && (j % 2)))
	            board[i][j] = 0;
	        else if(i < 3){
	            board[i][j] = players[0][p];
	            p++;
	        }
	        else if(i == 3)
	            p = 0;
	        else{
                board[i][j] = players[1][p];
                p++;
	        }
	    }
	} /*Dovrebbe funzionare, in caso lascio la vecchio versione a commento qui sotto*/
	
	/*for(r1=0;r1<r;r1++){
		for(c1=0;c1<c;c1++){
			if(r1<3){
				if(r1%2==0){
					if(c1%2==0)
						m[r1][c1]=1;
					else 
						m[r1][c1]=0;
				} else {
					if(c1%2==0)
						m[r1][c1]=0;
					else 
						m[r1][c1]=1;
				}
			} else {
				if(r1==3){
					if(c1%2==0)
						m[r1][c1]=0;
					else 
						m[r1][c1]=5;
				} else {
					if(r1%2==0){
						if(c1%2==0)
							m[r1][c1]=2;
						else 
							m[r1][c1]=0;
					} else {
						if(c1%2==0)
							m[r1][c1]=0;
						else 
							m[r1][c1]=2;
					}
				}
			}
			
			
		}
	}*/
}

/*---------------------------------SEZIONE FUNZIONI OUTPUT---------------------------------*/

void printPedina(pedina *p){
    if(p->id_player && p->grado)
        printf("N");
    else if(p->id_player && !(p->grado))
        printf("n");
    else if(!(p->id_player) && p->grado)
        printf("B");
    else
        printf("b");
}

/* Funzione che stampa la scacchiera in base al posizionamento delle pedine. */
void printMatrix(pedina **m){

    int i,j;

    for(i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            if ((i % 2 && !(j % 2)) || (!(i % 2) && (j % 2))) /*Caselle oscurate*/
                printf("# ");
            else if(!m[i][j]){
                printf("  ");                
            }
            else{
                printPedina(m[i][j]);
                printPedina(m[i][j]->middle);
                printPedina(m[i][j]->down);
            }
        }
        printf("\n");
    }
    
/*Lascio la vecchia versione qui sotto a commento, non si sa mai*/
/*	int r1,c1;
	
	for(r1=0;r1<r;r1++){
		for(c1=0;c1<c;c1++){
			if(m[r1][c1]==0){
				printf(" ");
			} else if(m[r1][c1]==1){
					printf("N ");
				} else if(m[r1][c1]==2) {
						printf("B ");
					} else if(m[r1][c1]==5){
						printf("# ");
					}
			
		}
		printf("\n");
	}*/
}

/*---------------------------------SEZIONE FUNZIONI MOVE---------------------------------*/

/*
 * Restituisce 1 se la mossa è stata fatta, 0 se non è stato possibile
*/
int move(pedina **p, int from_x, int from_y, int to_x, int to_y){
    int success = 1, d = distance(from_x,from_y,to_x,to_y), grade_control = gradeCheck(p,from_y,to_y);
    if(to_x < 0 || to_y < 0 || to_x >= ROW || to_y >= COL || p[to_x][to_y] || d == -1 || !grade_control ) /*validazione input utente*/
        success = 0;
    else{
        if(d == 1){
            p[to_x][to_y] = p[from_x][from_y];
            p[from_x][from_y] = 0;
        }
        else if(d == 2){
            int middle_x = (from_x + to_x)/2,middle_y = (from_y+to_y)/2];
            if(p[middle_x][middle_y]){ /*verifica esistenza pedina in mezzo*/
                if(p[middle_x][middle_y]->id_player == p[from_x][from_y]->id_player)
                    success = 0; /*se amica, annulla mossa*/
                else
                    capture(p,from_x,from_y,to_x,to_y); /*se nemica cattura*/
            }
            else
                success = 0;
        }
        else
            success = 0;
    }
    return success;
}

/*
 * Restituisce la distanza in modulo tra due punti nella matrice:
 * Se è maggiore di 2, uguale a 0, o la destinazione è in una casella non accessibile restituisce il codice errore -1
*/
int distance(int from_x, int from_y, int to_x, int to_y){
    int result, dx = abs(to_x - from_x), dy = abs(to_y - from_y);
    if((to_x % 2 && !(to_y % 2)) || (!(to_x % 2) && (to_y % 2))) /*DA MODIFICARE AL MOMENTO DELL'INPUT UTENTE*/
        result = -1;
    else if(dx == dy && dx)
        result = dx;
    else
        result = -1; /* TODO: OTTIMIZZARE */

    return result;
}

/*
 * TODO: Implementa funzione cattura:
 * Questa funzione si occupa catturare le pedine indicate
*/
void capture(pedina **p, int from_x, int from_y, int to_x, int to_y){

}

/*
 * Verifica il grado della pedina mossa:
 * restituisce 1 se la mossa è consentita, 0 se non è consentita*/
int gradeCheck(pedina **p, int from_y, int to_y){
    int success = 1;

    if(p[from_x][from_y]->grado == 0 && (to_y - from_y) > 0)
        success = 0;

    return success;
}
