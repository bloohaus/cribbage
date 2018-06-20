#include "cards.c"
#include <stdio.h>
#include <stdlib.h> 


#define HANDNUMBER 4
#define HANDLENGTH 6

int main(int argc, char * argv[]){

 	deck d;
    deck *hands;
    int i, 
        j;

    setupCards();

    d = makeDeck();

 	shuffleDeck(&d);
 	
    hands = deal(&d, HANDNUMBER, HANDLENGTH);
 	card crib[4];

 	for (i = 0; i < HANDNUMBER; i++){
 		printf("Hand %d:\n", i + 1);
 		printDeck(hands[i]);
 		sortDeck(hands + i);
 		printf("Sorted Hand %d:\n", i + 1);
 		printDeck(hands[i]);
 		printf("\n");
    }
 }
 