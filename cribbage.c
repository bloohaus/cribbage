/* getting weird behavior from cards (diff 
results from identical function call). check
to see whether calling free() on hands, even though hands were allocated as single block 
of memory */

#include "cards.c"
#include <stdio.h>
#include <stdlib.h> 
#include <ncurses.h>
#include <menu.h>
#include <errno.h>

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

typedef struct windowBox {
	WINDOW *inner;
	WINDOW *outer;
} windowBox;

void printScore(int *points);
int wcutForDeal();
void wprintDeck(deck d);
void wcribSelectHuman(deck *hand, deck *crib, int dealer);
void wwin(int *playerPoints);
ITEM **buildItemsFromDeck(deck *d);
void printCutCard(card upCard);
void winnerPrint(char *str);
windowBox windowBoxInit(int h, int w, int y, int x);
void printBoxStr(windowBox wb, char *str);
void killBox(windowBox w);
int whumanPeg(deck d, int sum);
void wprintSum(int sum);

int fifteenPoints(deck hand, int sum, int position);
int runPoints(deck hand);
int pairPoints(deck hand);
int flushPoints(deck hand, deck upCard, int crib);
int nobPoints(deck hand, deck upCard);
int cardValue(card c);
void pickUpCard(deck *d, deck *upCard, int dealer);
int addPoints(int player, int *playerPoints, int points);
void win(int *playerPoints);
void cribSelectComputer(deck *hand, deck *crib, int player);
void cribSelectHuman(deck *hand, deck *crib, int dealer);
int scoreHand(deck hand, deck upCard, int crib, int loud);
int peg(deck *hands, int *playerPoints, int dealer);
int pegSelectionScore(card c, deck peggingDeck, int sum, int index, int loud);
void cleanup(decks ds);

WINDOW *gameWindow;

windowBox scoreBox,
		  logBox,
		  peggingBox,
		  handBox,
		  sumBox,
		  cutBox;

MENU *handMenu;

int 	scoreWindow_w, 
		scoreWindow_h,
		logWindow_w,
		logWindow_h,
		peggingWindow_w,
		peggingWindow_h,
		handWindow_w,
		handWindow_h,
		gameWindow_w,
		gameWindow_h,
		sumBox_h,
		sumBox_w;

int main(int argc, char *argv[]){

	deck d,
 		 upCard,
 		 crib,
    	 *hands;

	decks ds;

    int i, 
        dealer,
        playerPoints[2],
		startx, 
		starty;

    setupCards();
	playerPoints[0] = playerPoints[1] = 0;


	initscr();			/* Start curses mode 		*/
	cbreak();			/* Line buffering disabled, Pass on
					 * everty thing to me 		*/
	keypad(stdscr, TRUE);		/* I need that nifty F1 	*/
	start_color();			// Start color 			
	init_pair(1, COLOR_RED, COLOR_BLACK);

	scoreWindow_h = 5;
	scoreWindow_w = 35;
	starty = 0;	
	startx = (COLS - scoreWindow_w);	
	refresh();
	scoreBox = windowBoxInit(scoreWindow_h, scoreWindow_w, starty, startx);
	printScore(playerPoints);

	logWindow_h = 20;
	logWindow_w = scoreWindow_w;
	starty += scoreWindow_h;
	logBox = windowBoxInit(logWindow_h, logWindow_w, starty, startx);
	
	peggingWindow_h = (logWindow_h + scoreWindow_h) - 13;
	peggingWindow_w = COLS - scoreWindow_w;
	peggingBox = windowBoxInit(peggingWindow_h, peggingWindow_w, 0, 0);
	
	sumBox_h = 4;
	sumBox_w = (COLS - scoreWindow_w) / 2;
	sumBox = windowBoxInit(sumBox_h, sumBox_w, peggingWindow_h, 0);
	wprintw(sumBox.inner, "Sum:\n");
	wrefresh(sumBox.inner);
	
	cutBox = windowBoxInit(sumBox_h, sumBox_w, peggingWindow_h, sumBox_w);
	wprintw(cutBox.inner, "Cut Card:\n");
	wrefresh(cutBox.inner);

	handWindow_h = (scoreWindow_h + logWindow_h) - (peggingWindow_h + sumBox_h);
	handWindow_w = COLS - scoreWindow_w;
	handBox = windowBoxInit(handWindow_h, handWindow_w, peggingWindow_h + sumBox_h, 0);

	gameWindow_h = LINES - (peggingWindow_h + handWindow_h);
	gameWindow_w = COLS;
	gameWindow = newwin(gameWindow_h, gameWindow_w, (peggingWindow_h + handWindow_h + sumBox_h), 0);
	scrollok(gameWindow, TRUE);
	dealer = wcutForDeal();
	
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
    	wcribSelectHuman(&hands[1], &crib, dealer);
    	
    	wclear(gameWindow);
    	wrefresh(gameWindow);
    	
    	sortDeck(crib);
    	
    	pickUpCard(&d, &upCard, dealer);
    	printCutCard(upCard.cards[0]);
    	
    	if (upCard.cards[0].value == 11){
    		wprintw(logBox.inner, "Nibs (2 pts) to %s!\n", dealer? "you" : "the Computer");
    		wrefresh(logBox.inner);
    		if (addPoints(dealer, playerPoints, 2)){
    			wwin(playerPoints);
    			break;
    		}
    	}
    	
    	wprintw(logBox.inner, "======PEGGING======\n");
		wrefresh(logBox.inner);
    	
    	if (peg(hands, playerPoints, dealer)) {
			wwin(playerPoints);
			cleanup(ds);
    		break;
		}
    	
    	wprintw(logBox.inner, "======SCORING======\n"); 	
    	wrefresh(logBox.inner);
    	
    	if (dealer){
    		wprintw(logBox.inner, "Scoring the computer hand.\n");
    		if (addPoints(!dealer, playerPoints, scoreHand(hands[!dealer], upCard, NOT_CRIB, LOUD))){
    			wwin(playerPoints);
				cleanup(ds);
    			break;
    		}
			wprintw(logBox.inner, "\n");
    			
    		wprintw(logBox.inner, "Scoring your hand.\n");
    		if (addPoints(dealer, playerPoints, scoreHand(hands[dealer], upCard, NOT_CRIB, LOUD))){
    			wwin(playerPoints);
				cleanup(ds);
    			break;
    		}
			wprintw(logBox.inner, "\n");
    			
    		wprintw(logBox.inner, "Scoring your crib.\n");
    		if (addPoints(dealer, playerPoints, scoreHand(crib, upCard, CRIB, LOUD))){
    			wwin(playerPoints);
				cleanup(ds);
				break;
    		}
			wprintw(logBox.inner, "\n");
    			
    	} else {
    		wprintw(logBox.inner, "Scoring your hand.\n");
    		if (addPoints(!dealer, playerPoints, scoreHand(hands[!dealer], upCard, NOT_CRIB, LOUD))){
    			wwin(playerPoints);
				cleanup(ds);
    			break;
    		}
			wprintw(logBox.inner, "\n");
    		
    		wprintw(logBox.inner, "Scoring Computer's hand.\n");
    		if (addPoints(dealer, playerPoints, scoreHand(hands[dealer], upCard, NOT_CRIB, LOUD))){
    			wwin(playerPoints);
				cleanup(ds);
    			break;
    		}
			wprintw(logBox.inner, "\n");
    			
    		wprintw(logBox.inner, "Scoring Computer's crib.\n");
    		if (addPoints(dealer, playerPoints, scoreHand(crib, upCard, CRIB, LOUD))){
    			wwin(playerPoints);
				cleanup(ds);
    			break;
    		}
			wprintw(logBox.inner, "\n");
			wrefresh(logBox.inner);
			
			wclear(sumBox.inner);
			wclear(cutBox.inner);
			wrefresh(sumBox.inner);
			wrefresh(cutBox.inner);
    		
    	}
		
    	freeDeck(d);
    	freeDeck(upCard);
    	freeDeck(crib);
    	for (i = 0; i < 2; i++){
    		freeDeck(hands[i]);
    	}
    	
    	free(hands);
    	
    	dealer = !dealer;

    }

	killBox(scoreBox);
	killBox(logBox);
	killBox(handBox);
	killBox(peggingBox);
	killBox(cutBox);
	killBox(sumBox);
	delwin(gameWindow);
	endwin();			/* End curses mode		  */
	return 0;

}

void printCutCard(card upCard) {
	wclear(cutBox.inner);
	wprintw(cutBox.inner, "Cut Card:\n");
	wprintw(cutBox.inner, upCard.string);
	wrefresh(cutBox.inner);
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

ITEM **buildItemsFromDeck(deck *d) {
	int i;
	ITEM **items;

	items = calloc(d->len + 1, sizeof(ITEM *));

	for (i = 0; i < d->len; i++) {
		items[i] = new_item(d->cards[i].string, "");
	}

	items[d->len] = (ITEM *) NULL;

	return items;
}

int wcutForDeal(){
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

	keypad(gameWindow, TRUE);
	
	while (p1Select <= 0 || p1Select > d.len){
		wprintw(gameWindow, "Cut for Deal. Select a card between 1 and %d.\n", d.len);
		wscanw(gameWindow, "%d", &p1Select);
		wrefresh(logBox.inner);
	}
	
	moveCard(&d, &p1Cut, p1Select - 1);
	
	moveCard(&d, &p0Cut, randRange(0, d.len - 1));
	
	wprintw(logBox.inner, "You selected:      %s\n", p1Cut.cards[0].string);

	wprintw(logBox.inner, "Computer selected: %s\n", p0Cut.cards[0].string);
	
	if (p1Cut.cards[0].value > p0Cut.cards[0].value){
		wprintw(logBox.inner, "You lost the deal.\n");
		dealer = 0;
	} else if (p1Cut.cards[0].value < p0Cut.cards[0].value){
		wprintw(logBox.inner, "You won the deal.\n");
		dealer = 1;
	} else {
		wprintw(logBox.inner, "You tied.\n");
		dealer = wcutForDeal();
	}

	wclear(gameWindow);
	wrefresh(gameWindow);
	wrefresh(logBox.inner);
	wclear(logBox.inner);
	
	freeDeck(d);
	freeDeck(p0Cut);
	freeDeck(p1Cut);
	
	return dealer;
}


void pickUpCard(deck *d, deck *upCard, int dealer){
	int selection;
	char cardStr[16];
		
	selection = 1000;
	
	if (dealer){
		moveCard(d, upCard, randRange(0, d->len - 1));
	} else {
		while (selection < 0 || selection > d->len - 1){
			wclear(gameWindow);
			wrefresh(gameWindow);
			wprintw(gameWindow, "Pick the cut card by entering a number between 1 and %d\n", d->len);
			wscanw(gameWindow, "%d", &selection);
			selection -= 1;
		}
		mvwprintw(handBox.inner, 0, COLS - 15, "Cut Card:\n");
		sprintCard(d->cards[selection], cardStr);
		mvwprintw(handBox.inner, 1, COLS - 15, cardStr);
		wrefresh(handBox.inner);
		moveCard(d, upCard, selection);
	}
}

int addPoints(int player, int *playerPoints, int points){

	playerPoints[player] += points;

	if (*playerPoints >= WIN_VALUE){
		return 1;
	}

	printScore(playerPoints);

	return 0;
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

    pPoints = pairPoints(localHand);
    fPoints = fifteenPoints(localHand, 0, 0);
    rPoints = runPoints(localHand);   
    flPoints = flushPoints(hand, upCard, crib);
    nPoints = nobPoints(hand, upCard);
    points = pPoints + fPoints + rPoints + flPoints + nPoints;

	if (loud) {
/*		wprintw(logBox.inner, "\n");
		wprintw(logBox.inner, "pPoints: %d\n", pPoints);
		wprintw(logBox.inner, "fPoints: %d\n", fPoints);
		wprintw(logBox.inner, "rPoints: %d\n", rPoints);
		wprintw(logBox.inner, "flPoints: %d\n", flPoints);
		wprintw(logBox.inner, "nPoints: %d\n", nPoints);   */
		wprintw(logBox.inner, "Total points: %d\n", points);
	}

	wrefresh(logBox.inner);
    
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

	wprintDeck(hand);
	wprintw(logBox.inner, "Sum is %d\n", sum);

	do {
		wprintw(gameWindow, "Select a card to play.\n");
		scanf("%d", &selection);
		
		if (selection > hand.len || selection < 1){
			wprintw(gameWindow, "Selection is out of range. Try again.\n");
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

	testDeck = emptyDeck(localDeck.len);
	
	for (i = 0; i < localDeck.len && localDeck.len - i > 2; i++){
		for (j = i; j < localDeck.len; j++){
			copyCard(localDeck, &testDeck, j);
		}
		sortDeck(testDeck);
		if (newCheckRun(testDeck)){
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
					wprintw(logBox.inner, "A pair for 2.\n");
				}
				score += 2;
				break;
			case 2:
				if (loud){
					wprintw(logBox.inner, "Three of a kind for 6.\n");
				}
				score += 6;
				break;
			case 3:
				if (loud){
					wprintw(logBox.inner, "Four of a kind (!!!) for 12.\n");
				}
				score += 12;
				break;
			default:
				if (loud){
					wprintw(logBox.inner, "peggingPairs() returned neither 1, 2, nor 3.\n");
					wprintw(logBox.inner, "peggingPairs(): %d\n", buffInt);
				}
		}
	}

	if ((buffInt = peggingRuns(c, peggingDeck, index))){
		if (loud){
			wprintw(logBox.inner, "Run of %d for %d.\n", buffInt, buffInt);
		}
		score += buffInt;
	}

	if (peggingFifteens(c, peggingDeck, sum)){
		if (loud){
			wprintw(logBox.inner, "Fifteen for 2.\n");
		}
		score += 2;
	}
	
	if (peggingThirtyOnes(c, peggingDeck, sum)){
		if (loud){
			wprintw(logBox.inner, "Thirty-one for 2.\n");
		}
		score += 2;
	}
	wrefresh(logBox.inner);
	return score;
}

void printPeggingDeck(deck peggingDeck, int index) {
	int i;

	wclear(peggingBox.inner);
	for (i = 0; i < peggingDeck.len; i++) {
		if (i == index){
			wprintw(peggingBox.inner, "*************\n");
		}
		wprintw(peggingBox.inner, "%s\n", peggingDeck.cards[i].string);
	}
	wrefresh(peggingBox.inner);
}

int go(int player, deck *peggingDeck, int index, int *sum, int *playerPoints, deck *localHands){
	int selection,
		rVal, 
		score;
	
	rVal = 0;
	score = 0;

	while (cardValue(localHands[player].cards[0]) + *sum <= 31 && localHands[player].len > 0) {
		if (player) {
			selection = whumanPeg(localHands[player], *sum);
    	} else {
    		selection = computerPeg(localHands[player], *peggingDeck, index, *sum);
		}
		
		score = pegSelectionScore(localHands[player].cards[selection], *peggingDeck, *sum, index, LOUD);
		*sum += cardValue(localHands[player].cards[selection]);
		moveCard(localHands + player, peggingDeck, selection);

		if (*sum < 31 && *sum != 15){
			wprintw(logBox.inner, "%s %d\n", player? "You say": "Computer says", *sum);
		}

		if (addPoints(player, playerPoints, score)){
    		rVal = 1;
    		break;
    	} 
	}

	return rVal;

}

int whumanPeg(deck d, int sum){
	ITEM **items;
	MENU *menu;
	int i,
		selection,
		ch,
		whileBreak;

	items = buildItemsFromDeck(&d);

	whileBreak = 0;

	wclear(handBox.inner);
	
	menu = new_menu(items);
	set_menu_win(menu, handBox.inner);
 	set_menu_sub(menu, derwin(handBox.inner, 13, 35, 1, 1));
 	set_menu_mark(menu, ">");
	keypad(handBox.inner, TRUE);
	post_menu(menu);
 	wrefresh(handBox.inner);
	
	
	while(!whileBreak && (ch = wgetch(handBox.inner)) != KEY_F(1))
	{       switch(ch)
	        {	case KEY_DOWN:
				menu_driver(menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(menu, REQ_UP_ITEM);
				break;

			case 10:
			case ' ':	/* Enter */
			{	
				selection = item_index(current_item(menu));
				if (cardValue(d.cards[selection]) + sum <= 31){
					whileBreak = !whileBreak;
				} else {
					wprintw(logBox.inner, "Selection too high.\n");
					wrefresh(logBox.inner);
				}
				
			}
			break;
		}
	}	
	
	for (i = 0; i < d.len; i++) {
		free_item(items[i]);
	}
	wclear(handBox.inner);

	free_menu(menu);
	
	return selection;
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
    			selection = whumanPeg(localHands[player], sum);
    		} else {
    			selection = computerPeg(localHands[player], peggingDeck, index, sum);
    		}

    		score = pegSelectionScore(localHands[player].cards[selection], peggingDeck, sum, index, LOUD);
    		sum += cardValue(localHands[player].cards[selection]);   
    		wprintSum(sum);
			moveCard(localHands + player, &peggingDeck, selection);

    		if (addPoints(player, playerPoints, score)){
    			rVal = 1;
    			break;
    		} 

    		if (sum >= 31){
    			sum = 0;
    			index = peggingDeck.len;
    			wprintSum(sum);
    		} 
    		
    		

    	} else {
    		
			wprintw(logBox.inner, "%s go.\n", player? "You say": "Computer says");
			
			if (go(!player, &peggingDeck, index, &sum, playerPoints, localHands)) {
				rVal = 1;
    			break;
			}
    		
			wprintw(logBox.inner, "Go for one.\n");
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

	wprintw(logBox.inner, "Last card for one.\n");
	if (addPoints(!player, playerPoints, 1)){
		rVal = 1;
	}
	
	wclear(peggingBox.inner);
	wrefresh(peggingBox.inner);

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

void printScore(int *points) {
	
	mvwprintw(scoreBox.inner, 0, 0, "Computer Score: %03d", points[0]);
	wattron(scoreBox.inner, COLOR_PAIR(1));
	wprintw(scoreBox.inner, "  (%03d)\n", WIN_VALUE - points[0]);
	wattroff(scoreBox.inner, COLOR_PAIR(1));

	mvwprintw(scoreBox.inner, 1, 0, "Your Score:     %03d", points[1]);
	wattron(scoreBox.inner, COLOR_PAIR(1));
	wprintw(scoreBox.inner, "  (%03d)\n", WIN_VALUE - points[1]);
	wattroff(scoreBox.inner, COLOR_PAIR(1));
	
	touchwin(scoreBox.outer);
	wrefresh(scoreBox.inner);
}

void wprintSum(int sum){
	wclear(sumBox.inner);
	mvwprintw(sumBox.inner, 0, 0, "Sum:\n");
	wprintw(sumBox.inner, "%d", sum);
	wrefresh(sumBox.inner);
}

void wprintDeck(deck d){
	char lStr[25];
	int i;
	
	for (i = 0; i < d.len; i++){
		sprintf(lStr, "%d. %s\n", i + 1, d.cards[i].string);
		wprintw(gameWindow, lStr);
	}
}

int selectedItemsCount(ITEM **items, int l) {
	int i,
		count;

	count = 0;

	for (i = 0; i < l; i++){
		if (item_value(items[i])) {
			count++;
		}
	}
	return count;
}


void wcribSelectHuman(deck *d, deck *crib, int dealer){
	int selections[2],
		selectionsIndex,
		i,
		c,
		whileBreak;
	ITEM **items;
	MENU *menu;

	items = buildItemsFromDeck(d);


	whileBreak = 0;
	selectionsIndex = 0;

	wclear(handBox.inner);

	menu = new_menu(items);
	set_menu_win(menu, handBox.inner);
 	set_menu_sub(menu, derwin(handBox.inner, 13, 35, 1, 1));
 	set_menu_mark(menu, ">");
	keypad(handBox.inner, TRUE);
	menu_opts_off(menu, O_ONEVALUE);
	post_menu(menu);
 	wrefresh(handBox.inner);

	wclear(gameWindow);
	wprintw(gameWindow, "Select two cards for %s crib.\n", dealer? "your" : "the computer's");
	wprintw(gameWindow, "Press <SPACE> to choose a card and <ENTER> to submit your choices.\n");
	wrefresh(gameWindow);

	while (!whileBreak && (c = wgetch(handBox.inner)) != KEY_F(1)) {
		switch (c){
			case 10:
				if (selectedItemsCount(items, d->len) != 2){
					wprintw(gameWindow, "You must select exactly two cards.\n");
					wrefresh(gameWindow);
				} else {
					wclear(handBox.inner);
					wrefresh(handBox.inner);
					for (i = 0; i < d->len; i++){
						if (item_value(items[i])){
							selections[selectionsIndex++] = i;
						}
						if (selectionsIndex == 2) break;
					}
					whileBreak = 1;
				}
				break;
			case ' ':
				menu_driver(menu, REQ_TOGGLE_ITEM);
				break;
			case KEY_DOWN:
				menu_driver(menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(menu, REQ_UP_ITEM);
				break;
		}
	}

	for (i = 0; i < d->len; i++){
		free_item(items[i]);
	}

	free_menu(menu);

	wclear(gameWindow);
	wrefresh(gameWindow);

	for (i = 0; i < 2; i++){
		moveCard(d, crib, selections[i]);
	}

}

void wwin(int *playerPoints){
	WINDOW *w;
	w = logBox.inner;
	if (playerPoints[0] > playerPoints[1]){
		wprintw(w, "Computer wins with %d points!\n", playerPoints[0]);
		if (playerPoints[1] <= SKUNK_VALUE){
			wprintw(w, "You were SKUNKED!\n");
		}
	} else {
		wprintw(w, "You win with %d points!\n", playerPoints[1]);
		if (playerPoints[0] <= SKUNK_VALUE){
			printf("You SKUNKED the opposition!\n");
		}
	}
}

windowBox windowBoxInit(int h, int w, int y, int x){
	windowBox wb;
	wb.outer = newwin(h, w, y, x);
	box(wb.outer, 0, 0);
	wrefresh(wb.outer);
	wb.inner = derwin(wb.outer, h - 2, w - 2, 1, 1);
	scrollok(wb.inner, TRUE);
	return wb;
}

void printBoxStr(windowBox wb, char *str){
	touchwin(wb.outer);
	wprintw(wb.inner, "%s", str);
	wrefresh(wb.inner);
}

void killBox(windowBox w){
	delwin(w.inner);
	delwin(w.outer);
}