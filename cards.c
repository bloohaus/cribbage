//functions and structs to model playing cards. Must call setupCards() in program to initiate random numbers.

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

enum suit {CLUBS, SPADES, HEARTS, DIAMONDS};
enum comp {HIGHER, LOWER, SAME};


typedef struct {
	int suit;
	int value;
 } card;
 
 typedef struct {
  	card *cards;
  	int len;
	int cap;
  } deck;

//**********************Funciton Declarations  

/*
void setupCards();
deck makeDeck();
int randRange(int min, int max);
void printCard(card c);
void printDeck(deck d)
void shiftCards(deck d, int holeIndex);
void shuffleDeck(deck *d);
deck *deal(deck *d, int hands, int cards);
int compCards(card c1, card c2);
void sortDeck(deck d);
void moveCard(deck *deckFrom, deck *deckTo, int cardIndex);
void copyCard(deck deckFrom, deck deckTo, int cardIndex);
void copyDeck(deck deckFrom, deck deckTo);
void copyDeckFromIndex(deck deckFrom, deck deckTo, int index);
deck joinDecks(deck d1, deck d2);
deck emptyDeck(int size);
void freeDeck(deck d);
*/


//********************Function Definitions 
void setupCards(){
	srand(time(NULL));
}

deck makeDeck(){
 	deck d;
 	int suit,
 		val,
 		cardIndex;
 	d.cards = malloc(52 * sizeof(card));
 	cardIndex = 0;
 	for(suit = 0;suit < 4;suit++)
 	{
 		for(val = 1;val <= 13; val++)
 		{
 			d.cards[cardIndex].value = val;
 			d.cards[cardIndex].suit = suit;
 			cardIndex++;
 		}
 	}

	d.len = 52;
	d.cap = 52;
 	
 	return d;
 }
 
int randRange(int min, int max){
 	return min + rand() / (RAND_MAX / (max - min + 1) + 1);
 }

void sprintCard(card c, char *str) {

	char suitName[9],
 		 ch[3];


	switch (c.suit) {
		case HEARTS: 
			strcpy(suitName, "HEARTS");
			break;
		case DIAMONDS:
			strcpy(suitName, "DIAMONDS");
			break;
		case CLUBS:
			strcpy(suitName, "CLUBS");
			break;
		case SPADES:
			strcpy(suitName, "SPADES");
			break;
	}
	
	switch (c.value) {
		case 1:
			strcpy(ch, "A");
			break;
		case 10:
			strcpy(ch, "10");
			break;
		case 11:
			strcpy(ch, "J");
			break;
		case 12:
			strcpy(ch, "Q");
			break;
		case 13: 
			strcpy(ch, "K");
			break;
		default: 
			ch[0] = (c.value) + '0';
			ch[1] = '\0';
	}
 	sprintf(str, "%s of %s\n", ch, suitName);
}

void printCard(card c){
 	char suitName[9],
 		 ch[3];


	switch (c.suit) {
		case HEARTS: 
			strcpy(suitName, "HEARTS");
			break;
		case DIAMONDS:
			strcpy(suitName, "DIAMONDS");
			break;
		case CLUBS:
			strcpy(suitName, "CLUBS");
			break;
		case SPADES:
			strcpy(suitName, "SPADES");
			break;
	}
	
	switch (c.value) {
		case 1:
			strcpy(ch, "A");
			break;
		case 10:
			strcpy(ch, "10");
			break;
		case 11:
			strcpy(ch, "J");
			break;
		case 12:
			strcpy(ch, "Q");
			break;
		case 13: 
			strcpy(ch, "K");
			break;
		default: 
			ch[0] = (c.value) + '0';
			ch[1] = '\0';
	}
	
 	
 	printf("%s of %s\n", ch, suitName);
 }
 
 
void printDeck(deck d){
 	
 	int i;
	card c;

 	for (i = 0; i < d.len; i++){
		c = d.cards[i];
		printf("%d: ", i + 1);
 		printCard(c);
 	}
 }
 
void shiftCards(deck d, int holeIndex){
	for (; holeIndex < d.len - 1; holeIndex++){
		d.cards[holeIndex] = d.cards[holeIndex + 1];
	}
}
 
void shuffleDeck(deck *d){
 	card *buffCards;
 	int deckLength,
 		selection,
 		bufferIndex;
 	
 	buffCards = malloc(d->len * sizeof(card));
 	
 	deckLength = d->len;
 	bufferIndex = 0;
 	
 	while (deckLength > 0){
 		
 		selection = randRange(0, deckLength - 1);
 		buffCards[bufferIndex] = d->cards[selection];
 		shiftCards(*d, selection);
 		deckLength--;
 		bufferIndex++;
 	}
 	
 	free(d->cards);
	d->cards = buffCards;

 }
 
deck *deal(deck *d, int hands, int cards){
 	int i, j;
 	
	deck *handptr;
	handptr = malloc(hands * sizeof(deck));

	for (j = 0; j < hands; j++){
		
		handptr[j].len = cards;
		handptr[j].cap = cards;
		handptr[j].cards = malloc(cards * sizeof(card));
	}
	 
	
 	for(i = 0; i < cards; i++)
 	{
		for (j = 0; j < hands; j++) {
			handptr[j].cards[i] = d->cards[d->len - 1];
			d->len--;
		}
 	}

	return handptr;

 }


int compCards(card c1, card c2){
	 if (c1.value < c2.value){
		 return LOWER;
	 } else if (c1.value > c2.value){
		 return HIGHER;
	 } else {
		 if (c1.suit < c2.suit){
			 return LOWER;
		 } else if (c1.suit > c2.suit){
			 return HIGHER;
		 } else {
			 return SAME;
		 }
	 }
 }
 
void sortDeck(deck d){
 	card buffCard;
 	int i, 
 		j;
 		
 	for (i = 1; i < d.len; i++){
 		buffCard = d.cards[i];
 		for (j = i - 1; j >= 0; j--) {
 			if (compCards(d.cards[j], buffCard) == HIGHER){
 				d.cards[j + 1] = d.cards[j];
 				if (j == 0) {
 					d.cards[j] = buffCard;
 				}
 			} else if (compCards(d.cards[j], buffCard) == LOWER){
 				d.cards[j + 1] = buffCard;
 				break;
 			}
		}
 	}
 } 

void moveCard(deck *deckFrom, deck *deckTo, int cardIndex){
	
	if (deckTo->cap == deckTo->len){
		deckTo->cards = realloc(deckTo->cards, (deckTo->cap + 1) * sizeof(card));
		deckTo->cap++;
	}

	deckTo->cards[deckTo->len] = deckFrom->cards[cardIndex];
	deckTo->len++;
	shiftCards(*deckFrom, cardIndex);
	deckFrom->len--;

} 

void copyCard(deck deckFrom, deck *deckTo, int cardIndex){
	if (deckTo->cap < deckTo->len + 1){
		deckTo->cards = realloc(deckTo->cards, (deckTo->len + 1) * sizeof(card));
		deckTo->cap++;
	}
	
	deckTo->cards[deckTo->len] = deckFrom.cards[cardIndex];
	deckTo->len++;
}

void copyDeck(deck deckFrom, deck *deckTo){
	int i;
	
	if (deckTo->cap != deckFrom.len) {
		deckTo->cards = realloc(deckTo->cards, deckFrom.len * sizeof(card));
	}
	
	deckTo->len = deckTo->cap = deckFrom.len;
	
	for (i = 0; i < deckFrom.len; i++){
		deckTo->cards[i] = deckFrom.cards[i];
	} 
}

void copyDeckFromIndex(deck deckFrom, deck *deckTo, int index){
	int i;
	
	if (deckTo->cap != deckFrom.len - index) {
		deckTo->cards = realloc(deckTo->cards, (deckFrom.len - index) * sizeof(card));
	}
	
	deckTo->len = deckTo->cap = deckFrom.len - index;
	
	for (i = 0; index < deckFrom.len; i++, index++) {
		deckTo->cards[i] = deckFrom.cards[index];
	}
}

deck joinDecks(deck d1, deck d2){
	int i, j;
	deck d;
	
	d.cap = d.len = d1.len + d2.len;
	d.cards = malloc(d.cap * sizeof(card));
	
	for (i = 0, j = 0; i < d1.len; i++, j++){
		d.cards[j] = d1.cards[i];
	}
	
	for (i = 0; i < d2.len; i++, j++){
		d.cards[j] = d2.cards[i];
	}
	
	return d;
}

void freeDeck(deck d){
	free(d.cards);
}

deck emptyDeck(int size){
	deck d;
	d.len = 0;
	d.cap = size;
	d.cards = malloc(size * sizeof(card));
	return d;
}
