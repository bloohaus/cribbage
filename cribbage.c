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

typedef struct {
	deck d;
	deck upCard;
	deck crib;
	deck *hands;
} decks;

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
void cribSelectComputer(deck *hand, deck *crib, int player);
void cribSelectHuman(deck *hand, deck *crib, int dealer);
int scoreHand(deck hand, deck upCard, int crib, int loud);
int peg(deck *hands, int *playerPoints, int dealer);
int pegSelectionScore(card c, deck peggingDeck, int sum, int index, int loud);
void cleanup(decks ds);

int main(int argc, char **argv){

 	deck d,
 		 upCard,
 		 crib,
    	 *hands;
	decks ds;
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

		ds.d = d;
		ds.crib = crib;
		ds.hands = hands;
		ds.upCard = upCard;
    	
    	cribSelectComputer(&hands[0], &crib, dealer);
    	cribSelectHuman(&hands[1], &crib, dealer);
    	
    	printf("Crib selected. Your hand:\n");
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
    	
    	printf("=============PEGGING==============\n");
    	
    	if (peg(hands, playerPoints, dealer)) {
			win(playerPoints);
			cleanup(ds);
    		break;
		}
    	
    	printf("=============SCORING==============\n"); 	
    	
    	if (dealer){
    		printf("Scoring the computer hand.\n");
    		if (addPoints(!dealer, playerPoints, scoreHand(hands[!dealer], upCard, NOT_CRIB, LOUD))){
    			win(playerPoints);
				cleanup(ds);
    			break;
    		}
			printf("\n");
    			
    		printf("Scoring your hand.\n");
    		if (addPoints(dealer, playerPoints, scoreHand(hands[dealer], upCard, NOT_CRIB, LOUD))){
    			win(playerPoints);
				cleanup(ds);
    			break;
    		}
			printf("\n");
    			
    		printf("Scoring your crib.\n");
    		if (addPoints(dealer, playerPoints, scoreHand(crib, upCard, CRIB, LOUD))){
    			win(playerPoints);
				cleanup(ds);
				break;
    		}
			printf("\n");
    			
    	} else {
    		printf("Scoring your hand.\n");
    		if (addPoints(!dealer, playerPoints, scoreHand(hands[!dealer], upCard, NOT_CRIB, LOUD))){
    			win(playerPoints);
				cleanup(ds);
    			break;
    		}
			printf("\n");
    		
    		printf("Scoring Computer's hand.\n");
    		if (addPoints(dealer, playerPoints, scoreHand(hands[dealer], upCard, NOT_CRIB, LOUD))){
    			win(playerPoints);
				cleanup(ds);
    			break;
    		}
			printf("\n");
    			
    		printf("Scoring Computer's crib.\n");
    		if (addPoints(dealer, playerPoints, scoreHand(crib, upCard, CRIB, LOUD))){
    			win(playerPoints);
				cleanup(ds);
    			break;
    		}
			printf("\n");
    		
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

    }
    
    return 10;
}

void cleanup(decks ds) {
	int i;

	freeDeck(ds.d);
	freeDeck(ds.upCard);
	freeDeck(ds.crib);
	for (i = 0; i < 2; i++){
		freeDeck(ds.hands[i]);
	}
	free(ds.hands);
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

/* 
AA234 = 2 x 4
AAA23 = 3 x 3
AA223 = (2 x 3) + (2 x 3)
multiplier * multiplier * len
 */

int runPoints(deck hand){
	int arrayLen;
	arrayLen = (hand.len / 2) + 1;
	
	int len,
		multiplier[arrayLen],
		multiplierIndex,
		multiplierValue,
		i,
		delta,
		mVal;
		
	mVal = 1;
	len = 1;
	multiplierIndex = 0;
	multiplierValue = 500;
	
	for (i = 0; i < arrayLen; i++){
		multiplier[i] = 1;
	}
	

	for (i = 1; i < hand.len; i++){
		delta = hand.cards[i].value - hand.cards[i - 1].value;
		if (delta > 1){
			if (len < 3){
				len = 1;
				continue;
			} else {
				break;
			}
			
		} else if (delta == 0){
			if (hand.cards[i].value == multiplierValue){
				multiplier[multiplierIndex]++;
			} else {
				multiplierValue = hand.cards[i].value;
				multiplier[++multiplierIndex]++;
			}
		} else if (delta == 1){
			len++;
		} else {
			fprintf(stderr, "something is wrong. deck is not sorted./n");
		}
	}
	
	for (i = 0; i < arrayLen; i++){
		mVal *= multiplier[i];
	}

	if (len >= 3){
		return len * mVal;
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
		points = 4;
	}
	
	if (upCard.cards[0].suit == suit && flush){
		points = 5;
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
	printf("Computer selected: ");
	printCard(p0Cut.cards[0]);
	
	if (p1Cut.cards[0].value > p0Cut.cards[0].value){
		printf("You lost the deal.\n");
		dealer = 0;
	} else if (p1Cut.cards[0].value < p0Cut.cards[0].value){
		printf("You won the deal.\n");
		dealer = 1;
	} else {
		printf("You tied. ");
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
		printf("Computer wins with %d points!\n", playerPoints[0]);
		printf("You finished with %d points.\n", playerPoints[1]);
		if (playerPoints[1] <= SKUNK_VALUE){
			printf("You were SKUNKED!\n");
		}
	} else {
		printf("You win with %d points!\n", playerPoints[1]);
		printf("Computer finished with %d points.\n", playerPoints[0]);
		if (playerPoints[0] <= SKUNK_VALUE){
			printf("You SKUNKED the opposition!\n");
		}
	}
}

int flushPointsJustHand(deck hand) {
	int suit,
		i;
	suit = hand.cards[0].suit;

	for (i = 1; i < hand.len; i++) {
		if (hand.cards[i].suit != suit) {
			return 0;
		}
	}

	return 4;
}

int scoreHandWithoutUpcard(deck hand){
	int pPoints,
        fPoints,
        flPoints,
        rPoints,
        points;
	
	
	pPoints = pairPoints(hand);
    fPoints = fifteenPoints(hand, 0, 0);
    rPoints = runPoints(hand);   
    flPoints = flushPointsJustHand(hand);
    points = pPoints + fPoints + rPoints + flPoints;

	return points;
}

int scoreHand(deck hand, deck upCard, int crib, int loud){
	deck localHand;
	int pPoints,
        fPoints,
        flPoints,
        rPoints,
        nPoints,
        points;
	localHand = joinDecks(hand, upCard);
	sortDeck(localHand);
	if (loud) {
		printDeck(localHand);
	}
	

    pPoints = pairPoints(localHand);
    fPoints = fifteenPoints(localHand, 0, 0);
    rPoints = runPoints(localHand);   
    flPoints = flushPoints(hand, upCard, crib);
    nPoints = nobPoints(hand, upCard);
    points = pPoints + fPoints + rPoints + flPoints + nPoints;

	if (loud) {
		printf("\n");
		printf("pPoints: %d\n", pPoints);
		printf("fPoints: %d\n", fPoints);
		printf("rPoints: %d\n", rPoints);
		printf("flPoints: %d\n", flPoints);
		printf("nPoints: %d\n", nPoints);
		printf("Total points: %d\n", points);
	}


    
    freeDeck(localHand);
    
    return points;
}

void cribSelectComputer(deck *hand, deck *crib, int player){
	
	int i,
		j,
		maxScore,
		scoreBuff,
		runner,
		buffI,
		buffJ;
	deck localDeck;
	localDeck = emptyDeck(4);	
	
	maxScore = 0;
	scoreBuff = 0;
	
	buffI = hand->len - 2;
	buffJ = hand->len - 1;

	for (i = 0; i < hand->len - 1; i++){
		for (j = i + 1; j < hand->len; j++){
			localDeck.cap = 0;
			localDeck.len = 0;
			for (runner = 0; runner < hand->len; runner++) {
				if (runner != i && runner != j) {
					copyCard(*hand, &localDeck, runner);
				}
			}
			sortDeck(localDeck);
			scoreBuff = scoreHandWithoutUpcard(localDeck);

			if (cardValue(hand->cards[i]) + cardValue(hand->cards[j]) == 15 ||
				hand->cards[i].value == hand->cards[j].value){
				if (!player){
					scoreBuff += 2;
				} else {
					scoreBuff -= 2;
				}
			}

			if (scoreBuff > maxScore) {
				maxScore = scoreBuff;
				buffI = i;
				buffJ = j;
			}
		}
	}

	moveCard(hand,crib, buffJ);
	moveCard(hand,crib, buffI);

	freeDeck(localDeck);
}


void cribSelectHuman(deck *hand, deck *crib, int dealer){
	int selection,
		i;

	for (i = 0; i < 2; i++){
		printDeck(*hand);
		selection = 1000;
		while (selection < 0 || selection > hand->len - 1){
			printf("Enter the card number to transfer it to %s crib.\n", dealer? "your": "the computer's");
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

	if (maxScore == 0 && i > 0) {
		return i - 1; 
	} else {
		return selection;
	}
}

int humanPeg(deck hand, int sum){
	int selection,
		buffSum, 
		g;

	buffSum = 0; 
	selection = 0;

	printDeck(hand);
	printf("Sum is %d\n", sum);

	do {
		printf("Select a card to play.\n");
		scanf("%d", &selection);
		
		if (selection > hand.len || selection < 1){
			printf("Selection is out of range. Try again.\n");
			buffSum = 3100;
			while ((g = getc(stdin)) != EOF && g != '\n'){
				;
			}
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

int newCheckRun(deck d){
	int run,
		i;
	run = 1;
	
	if (d.len < 3) return 0;
	
	for (i = 1; i < d.len; i++){
		if (d.cards[i].value - d.cards[i - 1].value != 1){
			run = 0;
			break;
		}
	}
	return run;
}

int peggingRuns(card c, deck peggingDeck, int index){
	int i, j,
		runLength,
		localDeckLen;
	deck localDeck, 
		 testDeck;

	if (peggingDeck.len < 2){
		return 0;
	}

	localDeckLen = peggingDeck.len - index + 1; //plus one to accomodate c

	runLength = 0;

	localDeck = emptyDeck(localDeckLen); 
	for (i = index; i < peggingDeck.len; i++){
		copyCard(peggingDeck, &localDeck, i);
	}

	localDeck.cards[localDeck.len] = c;
	localDeck.len++;

	printf("localDeck:\n");
	printDeck(localDeck);

	testDeck = emptyDeck(localDeck.len);
	
	for (i = 0; i < localDeck.len && localDeck.len - i > 2; i++){
		for (j = i; j < localDeck.len; j++){
			copyCard(localDeck, &testDeck, j);
		}
		sortDeck(testDeck);
		if (newCheckRun(testDeck)){
			printf("We have a run.\n");
			printDeck(testDeck);
			printf("testDeck.len: %d\n", testDeck.len);
			runLength = testDeck.len;
			break;
		}
		
		testDeck.len = 0;
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
	
	if ((buffInt = peggingPairs(c, peggingDeck, index))){
		// offset by one because peggingPairs returns the number of cards in the 
		// peggingDeck that are identical to the card selected by the player.
		switch (buffInt) {
			case 1:
				if (loud){
					printf("A pair for 2.\n");
				}
				score += 2;
				break;
			case 2:
				if (loud){
					printf("Three of a kind for 6.\n");
				}
				score += 6;
				break;
			case 3:
				if (loud){
					printf("Four of a kind (!!!) for 12.\n");
				}
				score += 12;
				break;
			default:
				if (loud){
					printf("peggingPairs() returned neither 1, 2, nor 3.\n");
					printf("peggingPairs(): %d\n", buffInt);
				}
		}
	}

	if ((buffInt = peggingRuns(c, peggingDeck, index))){
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

void printPeggingDeck(deck peggingDeck, int index) {
	int i;
	printf("\n");
	for (i = 0; i < peggingDeck.len; i++) {
		if (i == index){
			printf("*************\n");
		}
		printCard(peggingDeck.cards[i]);
	}
	printf("\n");
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

		printf("%s played: ", player? "You": "Computer");
		printCard(localHands[player].cards[selection]);

		score = pegSelectionScore(localHands[player].cards[selection], *peggingDeck, *sum, index, LOUD);
		*sum += cardValue(localHands[player].cards[selection]);
		moveCard(localHands + player, peggingDeck, selection);

		if (*sum < 31 && *sum != 15){
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

    		if (player){
				printPeggingDeck(peggingDeck, index);
    			selection = humanPeg(localHands[player], sum);
    		} else {
    			selection = computerPeg(localHands[player], peggingDeck, index, sum);
    		}

			printf("%s played: ", player? "You": "Computer");
			printCard(localHands[player].cards[selection]);

    		score = pegSelectionScore(localHands[player].cards[selection], peggingDeck, sum, index, LOUD);
    		sum += cardValue(localHands[player].cards[selection]);   
			moveCard(localHands + player, &peggingDeck, selection);

    		if (addPoints(player, playerPoints, score)){
    			rVal = 1;
    			break;
    		} 

    		if (sum >= 31){
    			sum = 0;
    			printf("line 859: index is %d\n", index);
    			index = peggingDeck.len;    
    			printf("line 861: index is %d\n", index);
			
    		} else {
    			printf("%s %d\n", player? "You say": "Computer says", sum);
    		}
    		
    		

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
			if (localHands[player].len > 0){
    			continue; 
			}
    	}
    	player = !player;
	}

	printf("Last card for one.\n");
	if (addPoints(!player, playerPoints, 1)){
		rVal = 1;
	}
	
	for (i = 0; i < 2; i++) {
		printf("%s: %d points.\n", i? "You" : "Computer", playerPoints[i]);
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