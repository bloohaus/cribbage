#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

enum suit {CLUBS, SPADES, HEARTS, DIAMONDS};
enum comp {HIGHER, LOWER, SAME};

void setupCards(){
	srand(time(NULL));
}

typedef struct 
 {
 	int suit;
 	int value;
 } card;
 
 typedef struct 
  {
  	card *cards;
  	int len;
  } deck;

 typedef card* cardptr;
 
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
 	
 	return d;
 }
 
 int randRange(int min, int max){
 	return min + rand() / (RAND_MAX / (max - min + 1) + 1);
 }
 
 
 void printCard(card* c){
 	char suitName[9];
 	if (c->suit == HEARTS) 
 	{
 		strcpy(suitName, "HEARTS");
 	} else if (c->suit == DIAMONDS)
 	 {
 		strcpy(suitName, "DIAMONDS");
 	} else if(c->suit == CLUBS)
 	 {
 	 	strcpy(suitName, "CLUBS");
 	 } else
 	  {
 	  	strcpy(suitName, "SPADES");
 	  }
 	printf("%d of %s\n", c->value, suitName);
 }
 
 
 void printDeck(deck d){
 	
 	int i, length;
	card c;
	length = d.len;
 	for (i = 0; i < length; i++){
		c = d.cards[i];
 		printCard(&c);
 	}
 }
 
 
 void shuffleDeck(deck *d){
 	card *buffDeck;
 	int deckLength,
 		selection,
 		bufferIndex,
 		i;
 	
 	buffDeck = malloc(d->len * sizeof(card));
 	
 	deckLength = d->len;
 	bufferIndex = 0;
 	
 	while (deckLength > 0){
 		
 		selection = randRange(0, deckLength - 1);
 		buffDeck[bufferIndex] = d->cards[selection];
 		for (i = selection; i < deckLength - 2; i++){
 			d->cards[i] = d->cards[i + 1];
 		}
 		deckLength--;
 		bufferIndex++;
 	}
 	
 	free(d->cards);
	d->cards = buffDeck;

 }
 
deck *deal(deck *d, int hands, int cards){
 	int i, j;
 	
	deck *handptr;
	handptr = malloc(hands * sizeof(deck));

	for (j = 0; j < hands; j++){
		
		handptr[j].len = cards;
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
	 } else if (c1.value > c2.value) {
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
