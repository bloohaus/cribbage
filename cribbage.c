#include "cards.c"
#include <stdio.h>
#include <stdlib.h> 


#define HANDNUMBER 2
#define HANDLENGTH 6
#define UPINDEX 22

int fifteenPoints(deck hand, int sum, int position, card *set);
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
    
	card emptySet[6];
	emptySet[0].value = 0;

    pPoints = pairPoints(scoreHand);
    printf("there were %d pair points in the first hand.\n", pPoints);
    for (i = 0; i < scoreHand.len; i++){

		if (cardValue(scoreHand.cards[i]) > 7){
			break;
		}
    	fPoints += fifteenPoints(scoreHand, 0, i, emptySet);
    }
    printf("there were %d fifteen points in the first hand.\n", fPoints);
}

int cardValue(card c){
    if (c.value <= 10){
        return c.value;
    } else {
        return 10;
    }
}

int setLen(card *set){
	int i;
	i = 0;

	while (set[i].value != 0){
		i++;
	}
	return i;
}

void addCardtoSet(card c, card *set){
	
	int i;
	i = setLen(set);

	set[i] = c;
	set[i + 1].value = 0;
}

card cardPop(card *set){
	int i;
	card c;
	i = setLen(set);
	c = set[i - 1];
	set[i - 1].value = 0;

	return c;
}

void printSet(card *set){
	int i, j;
	j = setLen(set);
	for (i = 0; i < j; i++){
		printCard(set[i]);
	}
}

int fifteenPoints(deck hand, int sum, int position, card* set){
	int i, j;
	int points, workingSum;
	card homeSet[6];

	/* copy cards from caller to function frame */
	j = setLen(set);
	for (i = 0; i < j; i++){
		homeSet[i] = set[i];
	}

	homeSet[i].value = 0;

	points = 0;

	for (i = position; i < hand.len; i++){
		workingSum = sum + cardValue(hand.cards[i]);
		addCardtoSet(hand.cards[i], homeSet);

		if (workingSum > 15){
			printSet(homeSet);
			printf("adds up to %d\n", workingSum);
			printf("is higher than 15.\n");
			break;
		} else if (workingSum == 15){
			printSet(homeSet);
			printf("adds up to %d\n", workingSum);
			printf("is 15. 2 points.\n");
			points += 2;
		} else if (workingSum < 15){
					printSet(homeSet);
		printf("adds up to %d\n", workingSum);
			for (j = i + 1; j < hand.len; j++){
				printSet(homeSet);
				printf("adds up to %d\n", workingSum);
				printf("is less than 15. let's call fifteenPoints(hand, workingSum: %d, position: %d, set)\n", workingSum, j);
				points += fifteenPoints(hand, workingSum, j, homeSet);
			}
			
		}
		cardPop(homeSet);
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