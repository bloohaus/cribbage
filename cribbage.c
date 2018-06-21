#include "cards.c"
#include <stdio.h>
#include <stdlib.h> 


#define HANDNUMBER 2
#define HANDLENGTH 6

int fifteenPoints(deck hand);
int cardValue(card c);

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

    moveCard(&d, hands, d.len - 1);
    sortDeck(hands);
    printf("\nNew Hand 1:\n");
    printDeck(hands[0]);
 }
 
int cardValue(card c){
    if (c.value <= 10){
        return c.value;
    } else {
        return 10;
    }
}

int fifteenPoints(deck hand){
    ;;;;
}