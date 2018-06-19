#include "cards.c"
#include <stdio.h>
#include <stdlib.h> 


#define HANDNUMBER 2
#define HANDLENGTH 6


void sameCount(deck d);

int main(int argc, char * argv[]){
 	
 	deck d;
    deck *hands;
    int i, 
        j;

    setupCards();

    d = makeDeck();

    sameCount(d);
 	shuffleDeck(&d);
    sameCount(d);
 //	printDeck(d);


 	
    hands = deal(&d, HANDNUMBER, HANDLENGTH);
 	card crib[4];

 	for (i = 0; i < HANDNUMBER; i++){
 	printf("Hand %d:\n", i + 1);
 	printDeck(hands[i]);
    }


 }
 

void sameCount(deck d){
    int sameCounter, i, j;
    sameCounter = 0;
    for (i = 0; i < d.len; i++){
        for (j = i + 1; j < d.len; j++){
            if (compCards(d.cards[i], d.cards[j]) == SAME){
                printCard(d.cards + i);
                sameCounter++;
            }
        }
    }
    printf("We found %d duplicates.\n", sameCounter);
 }