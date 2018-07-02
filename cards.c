#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

enum suit {CLUBS, SPADES, HEARTS, DIAMONDS};
enum comp {HIGHER, LOWER, SAME};

void setupCards(){
	srand(time(NULL));
}

typedef struct {
	int suit;
	int value;
 } card;
 
 typedef struct {
  	card *cards;
  	int len;
	int cap;
  } deck;
 
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


void printCard(card c){
 	char suitName[9],
 		 ch[3];
 	
 	if (c.suit == HEARTS){
 		strcpy(suitName, "HEARTS");
 	} else if (c.suit == DIAMONDS){
 		strcpy(suitName, "DIAMONDS");
 	} else if (c.suit == CLUBS){
 	 	strcpy(suitName, "CLUBS");
 	} else {
 	  	strcpy(suitName, "SPADES");
 	}
 	
 	if (c.value > 1 && c.value < 10){
		ch[0] = ( c.value) + '0';
		ch[1] = '\0';
	} else if (c.value == 1){
		strcpy(ch, "A");
	} else if (c.value == 10){
		strcpy(ch, "10");
	} else if (c.value == 11){
		strcpy(ch, "J");
	} else if (c.value == 12){
		strcpy(ch, "Q");
	} else if (c.value == 13){
		strcpy(ch, "K");
	} 
 	
 	printf("%s of %s\n", ch, suitName);
 }
 
 
 void printDeck(deck d){
 	
 	int i, length;
	card c;
	length = d.len;
 	for (i = 0; i < length; i++){
		c = d.cards[i];
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
 		bufferIndex,
 		i;
 	
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
 
void sortDeck(deck *d){
 	card buffCard;
 	int i, 
 		j;
 		
 	for (i = 1; i < d->len; i++){
 		buffCard = d->cards[i];
 		for (j = i - 1; j >= 0; j--) {
 			if (compCards(d->cards[j], buffCard) == HIGHER){
 				d->cards[j + 1] = d->cards[j];
 				if (j == 0) {
 					d->cards[j] = buffCard;
 				}
 			} else if (compCards(d->cards[j], buffCard) == LOWER){
 				d->cards[j + 1] = buffCard;
 				break;
 			}
		}
 	}
 } 

void moveCard(deck *deckFrom, deck *deckTo, int cardIndex){
	
	if (deckTo->cap == deckTo->len){
		deckTo->cards = realloc(deckTo->cards, (deckTo->len + 1) * sizeof(card));
		deckTo->cap++;
	}

	deckTo->cards[deckTo->len++] = deckFrom->cards[cardIndex];
	shiftCards(*deckFrom, cardIndex);
	deckFrom->len--;

} 

deck joinDecks(deck d1, deck d2){
	int i, j;
	deck d;
	
	d.cap = d.len = d1.len + d2.len;
	d.cards = malloc(d.cap * sizeof(card));
	
	j = 0;
	
	for (i = 0; i < d1.len; i++, j++){
		d.cards[j] = d1.cards[i];
	}
	
	for (i = 0; i < d2.len; i++, j++){
		d.cards[j] = d2.cards[i];
	}
	
	return d;
}