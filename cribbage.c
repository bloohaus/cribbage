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
	
	scoreHand.cards[0].value = 6;
	scoreHand.cards[1].value = 7;
	scoreHand.cards[2].value = 7;
	scoreHand.cards[3].value = 8;
	scoreHand.cards[4].value = 9;

    pPoints = pairPoints(scoreHand);
    printf("there were %d pair points in the first hand.\n", pPoints);

    fPoints = fifteenPoints(scoreHand, 0, 0);
    printf("there were %d fifteen points in the first hand.\n", fPoints);
    
    rPoints = runPoints(scoreHand);
    printf("there were %d run points in the first hand\n", rPoints);
}

int cardValue(card c){
    if (c.value <= 10){
        return c.value;
    } else {
        return 10;
    }
}

int fifteenPoints(deck hand, int sum, int position){
	int i, j;
	int points, workingSum;

	points = 0;

	for (i = position; i < hand.len; i++){
/*		if (hand.cards[i].value > (15 - sum)/2){
			break;
		} */
		workingSum = sum + cardValue(hand.cards[i]);
		if (workingSum > 15){
			break;
		} else if (workingSum == 15){
			points += 2;
		} else if (workingSum < 15){
		points += fifteenPoints(hand, workingSum, i + 1);
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

int runPoints(deck hand){
	int len,
		multiplier,
		i;
	
	len = 0;
	multiplier = 1;
	
	for (i = 1; i < hand.len; i++){
		if (hand.cards[i - 1].value == hand.cards[i].value){
			multiplier++;
		} else if (hand.cards[i - 1].value == hand.cards[i].value - 1){
			len++;
		} else if (len < 3){
			len = 0;
			multiplier = 1;
		} else {
			break;
		}
	}
	if (len >= 3){
		return len * multiplier;
	} else {
		return 0;
	}
}