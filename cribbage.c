#include "cards.c"
#include <stdio.h>
#include <stdlib.h> 


#define HANDNUMBER 2
#define HANDLENGTH 6
#define UPINDEX 22

int fifteenPoints(deck hand, int sum, int position);
int runPoints(deck hand);
int pairPoints(deck hand);
int cardValue(card c);

int main(int argc, char * argv[]){

 	deck d,
 		 upCard,
 		 crib,
    	 *hands,
    	 scoreHand;
    int i, 
        j,
        pPoints,
        fPoints,
        rPoints;

    setupCards();

    d = makeDeck();
 	shuffleDeck(&d);
 	
    hands = deal(&d, HANDNUMBER, HANDLENGTH);

 	for (i = 0; i < HANDNUMBER; i++){
 		sortDeck(hands + i);
    }
    
    upCard.len = upCard.cap = crib.len = crib.cap = 0;
    
    upCard.cards = crib.cards = NULL;
    
    moveCard(&d, &upCard, UPINDEX);
    
    moveCard(hands, &crib, 1);
    moveCard(hands, &crib, 1);

	scoreHand = joinDecks(hands[0], upCard);
	sortDeck(&scoreHand);
	printDeck(scoreHand);
    
    pPoints = pairPoints(scoreHand);
    printf("there were %d pair points in the first hand.\n", pPoints);
    
    fPoints = fifteenPoints(scoreHand, 0, 0);
    printf("there were %d fifteen points in the first hand.\n", fPoints);
}
 
int cardValue(card c){
    if (c.value <= 10){
        return c.value;
    } else {
        return 10;
    }
}

int fifteenPoints(deck hand, int sum, int position){
    int points, i, j;
    points = 0;
	sum = sum + cardValue(hand.cards[position]);
	printf("Calling fifteenPoints\nsum: %d\nposition: %d\ncard:", sum, position);
	printCard(hand.cards[position]);
/*	if (sum > 15){
		return 0;
	} */
    for (j = position + 1; j < hand.len; j++){
		printf("Looking at card: ");
		printCard(hand.cards[j]);
    	if (sum + cardValue(hand.cards[j]) == 15){
    		points += 2;
			printf("found a 15!\n points: %d\n", points);
    	} else if (sum + cardValue(hand.cards[j]) < 15) {
			printf("Under 15->looping through rest.\n");
    		for (i = j + 1; i < hand.len; i++){
    			points += fifteenPoints(hand, sum+ cardValue(hand.cards[j]), i);
    		}
		}
    }
    return points;
}

int pairPoints(deck hand){
	int points,
		i,
		j;
		
	points = 0;
	
	for (i = 0; i < hand.len - 1; i++){
		for (j = i + 1; j < hand.len; j++){
			if (hand.cards[i].value == hand.cards[j].value){
				points += 2;
			}
		}
	}
	return points;
}