/* getting weird behavior from cards (diff 
results from identical function call). check
to see whether calling free() on hands, even though hands were allocated as single block 
of memory */

#include "cards.c"
#include <stdio.h>
#include <stdlib.h> 


#define HANDNUMBER 2
#define HANDLENGTH 6
#define WIN_VALUE 126
#define SKUNK_VALUE 75

enum {NOT_CRIB, CRIB};
enum {QUIET, LOUD};

int fifteenPoints(deck hand, int sum, int position);
int runPoints(deck hand);
int pairPoints(deck hand);
int flushPoints(deck hand, deck upCard, int crib);
int nobPoints(deck hand, deck upCard);
int cardValue(card c);
int cutForDeal();
void pickUpCard(deck *d, deck *upCard, int dealer);
int addPoints(int player, int *playerPoints, int points);
void win(int *playerPoints);
void cribSelectProgram(deck *hand, deck *crib);
void cribSelectPlayer(deck *hand, deck *crib, int dealer);
int scoreHand(deck hand, deck upCard, int crib);
int peg(deck *hands, int *playerPoints, int dealer);
int pegSelectionScore(card c, deck peggingDeck, int sum, int index, int loud);

int main(int argc, char **argv){

 	deck d,
 		 upCard,
 		 crib,
    	 *hands;
    int i, 
        dealer,
        playerPoints[2];

    setupCards();
	
	playerPoints[0] = playerPoints[1] = 0;
	
	dealer = cutForDeal();
	
	
	while (playerPoints[0] < WIN_VALUE && playerPoints[1] < WIN_VALUE){

    	d = makeDeck();
 		shuffleDeck(&d);
 		
 		upCard = emptyDeck(1);
    	crib = emptyDeck(4);
    	
    	hands = deal(&d, HANDNUMBER, HANDLENGTH);

 		for (i = 0; i < HANDNUMBER; i++){
 			sortDeck(hands[i]);
    	}
    	
    	cribSelectProgram(&hands[0], &crib);
    	cribSelectPlayer(&hands[1], &crib, dealer);
    	
    	printf("crib selected. your hand:\n");
    	printDeck(hands[1]);
    	
    	sortDeck(crib);
    	
    	pickUpCard(&d, &upCard, dealer);
    
    	printf("Cut Card: ");
    	printCard(upCard.cards[0]);
    	
    	if (upCard.cards[0].value == 11){
    		printf("Nibs (2 pts) to the dealer!\n");
    		if (addPoints(dealer, playerPoints, 2)){
    			win(playerPoints);
    			break;
    		}
    	}
    	
    	printf("=============PEGGING==============\n\n");
    	
    	peg(hands, playerPoints, dealer);
    	
    	printf("=============SCORING==============\n\n");
    	printf("Your hand:\n");    	
    	
    	if (dealer){
    		printf("Scoring the computer hand.\n");
    		if (addPoints(!dealer, playerPoints, scoreHand(hands[!dealer], upCard, NOT_CRIB))){
    			win(playerPoints);
    			break;
    		}
    			
    		printf("Scoring your hand.\n");
    		if (addPoints(dealer, playerPoints, scoreHand(hands[dealer], upCard, NOT_CRIB))){
    			win(playerPoints);
    			break;
    		}
    			
    		printf("Scoring your crib.\n");
    		if (addPoints(dealer, playerPoints, scoreHand(crib, upCard, CRIB))){
    			win(playerPoints);
    		}
    			
    	} else {
    		printf("Scoring your hand.\n");
    		if (addPoints(!dealer, playerPoints, scoreHand(hands[!dealer], upCard, NOT_CRIB))){
    			win(playerPoints);
    			break;
    		}
    		
    		printf("Scoring Computer's hand.\n");
    		if (addPoints(dealer, playerPoints, scoreHand(hands[dealer], upCard, NOT_CRIB))){
    			win(playerPoints);
    			break;
    		}
    			
    		printf("Scoring Computer's crib.\n");
    		if (addPoints(dealer, playerPoints, scoreHand(crib, upCard, CRIB))){
    			win(playerPoints);
    			break;
    		}
    		
    	}

		printf("The Computer has %d points.\n", playerPoints[0]);
    	printf("You have %d points.\n", playerPoints[1]);
		
    	freeDeck(d);
    	freeDeck(upCard);
    	freeDeck(crib);
    	for (i = 0; i < 2; i++){
    		freeDeck(hands[i]);
    	}
    	
    	free(hands);
    	
    	dealer = !dealer;
    	
    	playerPoints[0] += WIN_VALUE;
    }
    
    return 10;
}

//Game Logic Functions

int cardValue(card c){
    if (c.value <= 10){
        return c.value;
    } else {
        return 10;
    }
}

int fifteenPoints(deck hand, int sum, int position){
	int i;
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
	
	len = 1;
	multiplier = 1;
	
	for (i = 1; i < hand.len; i++){
		if (hand.cards[i - 1].value == hand.cards[i].value){
			multiplier *= 2;
		} else if (hand.cards[i - 1].value == hand.cards[i].value - 1){
			len++;
		} else if (len < 3){
			len = 1;
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

int flushPoints(deck hand, deck upCard, int crib){
	int flush,
		suit,
		points,
		i;
		
	points = 0;
	flush = 1;
	suit = hand.cards[0].suit;
	
	for (i = 1; i < hand.len; i++){
		if (hand.cards[i].suit != suit){
			flush = 0;
			break;
		}
	}
	
	if (flush && !crib){
		points += 4;
	}
	
	if (upCard.cards[0].suit == suit && flush){
		if (!crib){
			points++;
		} else {
			points = 5;
		}
	}
	return points;
}

int nobPoints(deck hand, deck upCard){
	int i;
	for (i = 0; i < hand.len; i++){
		if (hand.cards[i].value == 11 && hand.cards[i].suit == upCard.cards[0].suit){
			return 1;
		}
	}
	return 0;
}


//Game Play Functions

int cutForDeal(){
	deck d,
		 p0Cut,
		 p1Cut;
	int p1Select, 
		dealer;
	
	d = makeDeck();
	shuffleDeck(&d);
	p0Cut = emptyDeck(1);
	p1Cut = emptyDeck(1);
	
	p1Select = 1000;
	
	while (p1Select <= 0 || p1Select > d.len){
		printf("Cut for Deal. Select a card between 1 and %d.\n", d.len);
		scanf("%d", &p1Select);
	}
	moveCard(&d, &p1Cut, p1Select - 1);
	
	moveCard(&d, &p0Cut, randRange(0, d.len - 1));
	
	printf("You selected:      ");
	printCard(p1Cut.cards[0]);
	printf("Player 1 selected: ");
	printCard(p0Cut.cards[0]);
	
	if (p1Cut.cards[0].value > p0Cut.cards[0].value){
		printf("You lost the deal.\n");
		dealer = 0;
	} else if (p1Cut.cards[0].value < p0Cut.cards[0].value){
		printf("You won the deal.\n");
		dealer = 1;
	} else {
		printf("You tied.");
		dealer = cutForDeal();
	}
	
	freeDeck(d);
	freeDeck(p0Cut);
	freeDeck(p1Cut);
	
	return dealer;
}

void pickUpCard(deck *d, deck *upCard, int dealer){
	int selection;
		
	selection = 1000;
	
	if (dealer){
		moveCard(d, upCard, randRange(0, d->len - 1));
	} else {
		while (selection < 0 || selection > d->len - 1){
			printf("Pick the cut card by entering a number between 1 and %d\n", d->len);
			scanf("%d", &selection);
			selection -= 1;
		}
		moveCard(d, upCard, selection);
	}
}

int addPoints(int player, int *playerPoints, int points){
	playerPoints[player] += points;
	if (*playerPoints >= WIN_VALUE){
		return 1;
	}
	return 0;
}

void win(int *playerPoints){
	if (playerPoints[0] > playerPoints[1]){
		printf("Player 1 wins with %d points!\n", playerPoints[0]);
		printf("Player 2 (you) finished with %d points.\n", playerPoints[1]);
		if (playerPoints[1] <= SKUNK_VALUE){
			printf("You were SKUNKED!\n");
		}
	} else {
		printf("Player 2 (you) wins with %d points!\n", playerPoints[1]);
		printf("Player 1 finished with %d points.\n", playerPoints[0]);
		if (playerPoints[0] <= SKUNK_VALUE){
			printf("You SKUNKED the opposition!\n");
		}
	}
}


int scoreHand(deck hand, deck upCard, int crib){
	deck sHand;
	int pPoints,
        fPoints,
        flPoints,
        rPoints,
        nPoints,
        points;
	sHand = joinDecks(hand, upCard);
	sortDeck(sHand);
	printDeck(sHand);

    pPoints = pairPoints(sHand);
    printf("pair points: %d\n", pPoints);

    fPoints = fifteenPoints(sHand, 0, 0);
    printf("fifteen points: %d\n", fPoints);
    
    rPoints = runPoints(sHand);
	printf("run points: %d\n", rPoints);
    
    flPoints = flushPoints(hand, upCard, crib);
    printf("flush points: %d\n", flPoints);
    
    nPoints = nobPoints(hand, upCard);
    printf("nob points: %d\n", nPoints);
    
    points = pPoints + fPoints + rPoints + flPoints + nPoints;
    
    printf("Total points: %d\n", points);
    
    freeDeck(sHand);
    
    return points;
}

void cribSelectProgram(deck *hand, deck *crib){
	moveCard(hand,crib, hand->len - 1);
	moveCard(hand,crib, hand->len - 1);
}


void cribSelectPlayer(deck *hand, deck *crib, int dealer){
	int selection,
		i;
	
	for (i = 0; i < 2; i++){
		selection = 1000;
		while (selection < 0 || selection > hand->len - 1){
			printDeck(*hand);
			printf("Enter the card number to transfer it to %s crib.\n", dealer? "your": "the dealer's");
			scanf("%d", &selection);
			selection--;
		}
		moveCard(hand, crib, selection);
	}
}


int computerPeg(deck hand, deck peggingDeck, int index, int sum){

	int i,
		scoreBuff,
		maxScore, 
		selection;
	
	scoreBuff = 0;
	maxScore = 0;
	selection = 0;

	if (hand.len == 1) {
		return 0;
	}

	for (i = 0; i < hand.len; i++) {
		if (cardValue(hand.cards[i]) + sum > 31) {
			i--;
			break;
		}
		scoreBuff = pegSelectionScore(hand.cards[i], peggingDeck, sum, index, QUIET);
		if (scoreBuff >= maxScore) {
			maxScore = scoreBuff;
			selection = i;
		}
	}

	if (maxScore == 0) {
		return i - 1; 
	} else {
		return selection;
	}
}

int humanPeg(deck hand, int sum){
	int selection,
		buffSum;

	buffSum = 0; 

	printDeck(hand);

	do {
		printf("Sum is %d\n", sum);
		printf("Select a card to play.\n");
		scanf("%i", &selection);
		
		if (selection > hand.len || selection < 1){
			printf("Selection is out of range. Try again.\n");
			buffSum = 3100;
			continue;
		}

		selection--;
		buffSum = sum + cardValue(hand.cards[selection]);
		if (buffSum > 31) {
			printf("Invalid selection. Cannot go over 31.\n");
		}
	} while (buffSum > 31);
	
	return selection;
}

int checkRun(deck d) {
	int i,
		runLen;

	if (d.len < 3){
		return 0;
	}

	runLen = 1; 

	for (i = 1; i < d.len - 1; i++){
		if (d.cards[i].value == d.cards[i-1].value + 1){
			runLen++;
		} else {
			break;
		}
	}

	if (runLen == d.len){
		return runLen;
	} else {
		return 0;
	}

}

int peggingRuns(card c, deck peggingDeck, int index){
	int i,
		runLength,
		maxRun;
	deck localDeck, 
		 testDeck;

	if (peggingDeck.len < 2){
		return 0;
	}

	runLength = 0;
	maxRun = 0;

	localDeck = emptyDeck(peggingDeck.len - index + 1); //plus one to accomodate c
	copyDeck(peggingDeck, &localDeck);
	localDeck.cards[localDeck.len - 1] = c;

	testDeck = emptyDeck(localDeck.len);

	for (i = localDeck.len - 1; i >= 0; i--){
		moveCard(&localDeck, &testDeck, i);
		sortDeck(testDeck);
		runLength = checkRun(testDeck);
		if (runLength > maxRun){
			maxRun = runLength;
		}
	}

	freeDeck(localDeck);
	freeDeck(testDeck);
	
	if (runLength >= 3){
		return runLength;
	} else {
		return 0;
	}
	
}

int peggingPairs(card c, deck peggingDeck, int index) {
	int i,
		pairs;
	pairs = 0;
	for (i = peggingDeck.len - 1; i >= index; i--){
		if (peggingDeck.cards[i].value != c.value){
			break;
		} else {
			pairs++;
		}
	}
	return pairs;
}

int peggingFifteens(card c, deck peggingDeck, int sum){
	if (cardValue(c) + sum == 15) {
		return 1;
	} else {
		return 0;
	}
}

int peggingThirtyOnes(card c, deck peggingDeck, int sum) {
	if (cardValue(c) + sum == 31) {
		return 1;
	} else {
		return 0;
	}
}

int pegSelectionScore(card c, deck peggingDeck, int sum, int index, int loud){
	int score,
		buffInt;
		
	buffInt = 0;
	score = 0;
	
	if (buffInt = peggingPairs(c, peggingDeck, index)){
		switch (buffInt) {
			case 2:
				if (loud){
					printf("A pair for 2.\n");
				}
				score += 2;
				break;
			case 3:
				if (loud){
					printf("Three of a kind for 6.\n");
				}
				score += 6;
				break;
			case 4:
				if (loud){
					printf("Four of a kind (!!!) for 12.\n");
				}
				score += 12;
				break;
			default:
				if (loud){
					printf("peggingPairs() returned neither 2, 3, nor 4.\n");
					printf("peggingPairs(): %d\n", buffInt);
				}
		}
	}
	if (buffInt = peggingRuns(c, peggingDeck, index)){
		if (loud){
			printf("Run of %d for %d.\n", buffInt, buffInt);
		}
		
		score += buffInt;
	}
	if (peggingFifteens(c, peggingDeck, sum)){
		if (loud){
			printf("Fifteen for 2.\n");
		}
		score += 2;
	}
	
	if (peggingThirtyOnes(c, peggingDeck, sum)){
		if (loud){
			printf("Thirty-one for 2.\n");
		}
		score += 2;
	}
	
	return score;
}

int go(int player, deck *peggingDeck, int index, int *sum, int *playerPoints, deck *localHands){
	int selection,
		rVal, 
		score;
	
	rVal = 0;
	score = 0;

	while (cardValue(localHands[player].cards[0]) + *sum <= 31 && localHands[player].len > 0) {
		if (player) {
			selection = humanPeg(localHands[player], *sum);
    	} else {
    		selection = computerPeg(localHands[player], *peggingDeck, index, *sum);
		}

		*sum += cardValue(localHands[player].cards[selection]);
		score = pegSelectionScore(localHands[player].cards[selection], *peggingDeck, *sum, index, LOUD);

		printf("%s played: ", player? "You": "Computer");
		printCard(localHands[player].cards[selection]);

		moveCard(localHands + player, peggingDeck, selection);

		if (*sum < 31){
			printf("%s %d\n", player? "You say": "Computer says", *sum);
		}

		if (addPoints(player, playerPoints, score)){
    		rVal = 1;
    		break;
    	} 
	}

	return rVal;

}

int peg(deck *hands, int *playerPoints, int dealer){

	int player, 
		sum,
		i,
		index,
		selection,
		rVal,
		score;
	deck localHands[2],
		 peggingDeck;
		 
	player = !dealer;
	sum = 0;
	rVal = 0;
	index = 0;
	selection = 0,
	score = 0;
	
	peggingDeck = emptyDeck(8);
	
	for (i = 0; i < 2; i++){
		localHands[i] = emptyDeck(4);
		copyDeck(hands[i], localHands + i);
	}

	while (localHands[0].len > 0 || localHands[1].len > 0) {
    	if (localHands[player].len == 0){
    		player = !player;
    		continue;
    	}
    	if (cardValue(localHands[player].cards[0]) + sum <= 31){

			printDeck(peggingDeck);
			printf("Index: %d\n", index);
    		if (player){
    			selection = humanPeg(localHands[player], sum);
    		} else {
    			selection = computerPeg(localHands[player], peggingDeck, index, sum);
    		}

    		sum += cardValue(localHands[player].cards[selection]);   

			printf("%s played: ", player? "You": "Computer");
			printCard(localHands[player].cards[selection]);

    		score = pegSelectionScore(localHands[player].cards[selection], peggingDeck, sum, index, LOUD);
    		
    		if (addPoints(player, playerPoints, score)){
    			rVal = 1;
    			break;
    		} 

    		if (sum >= 31){
    			sum = 0;
    			index = peggingDeck.len;    
			
    		} else {
    			printf("%s %d\n", player? "You say": "Computer says", sum);
    		}
    		
    		moveCard(localHands + player, &peggingDeck, selection);

    	} else {
    		
			
			printf("%s go.\n", player? "You say": "Computer says");
			
			if (go(!player, &peggingDeck, index, &sum, playerPoints, localHands)) {
				rVal = 1;
    			break;
			}
    		
			printf("Go for one.\n");
			if (addPoints(!player, playerPoints, 1)){
    			rVal = 1;
    			break;
    		}
    		sum = 0;
    		index = peggingDeck.len;
//     		continue bc turn stays w player who said go
    		continue; 
    	}
    	player = !player;
	}
	

	for (i = 0; i < 2; i++){
		freeDeck(localHands[i]);
	}

	freeDeck(peggingDeck);

	if (rVal){
		return 1;
	} else {
		return 0; 
	}

}