#include "cards.c"
#include <stdio.h>
#include <stdlib.h> 


#define HANDNUMBER 2
#define HANDLENGTH 6

int main(int argc, char * argv[]){
 	
 	deck d;
    card **hands;
 	d = makeDeck();
    int i;

    setupCards();

 	shuffleDeck(&d);
 	printDeck(d.cards, 52);
 	srand(time(NULL));
 	
    hands = deal(&d, HANDNUMBER, HANDLENGTH);
 	card crib[4];

 	for (i = 0; i < HANDNUMBER; i++){
 	printf("Hand %d:\n", i);
 	printDeck(hands[i], HANDLENGTH);
    }
 }
 
 