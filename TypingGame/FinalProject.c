//SE 185: Final Project Template//
/////////////////////////

//Compile: gcc FinalProject.c -o FinalProject -lncurses
//Run: ./FinalProject wordList.txt

////////////////////
#include<stdio.h>//
//add more here////
#include <math.h>
#include <ncurses/ncurses.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAXWORDS 2000
#define WORDLENGTH 64
#define WIDTH 70
#define HEIGHT 20
//////////////////////
//Struct Definition//
////////////////////
typedef struct listItem {
	int x;
	int y;
	struct listItem* nextPtr; //struct with node to create linked list
	char word[WORDLENGTH];
} listItem;
/////////////////////////////////////
//User Defined Functions Prototype//

//List prototypes here and define//

//them below the main function////

/////////////////////////////////
int find_and_delete_word(listItem* first, char word[WORDLENGTH]);
void delete_word_after();
void add_word();
void initWord();
listItem* get_next_word();
listItem* create_list();

int new_word();
int instructions();
void main_screen();

void trimws();
void printBorder();
int read_words();
FILE* myFile = NULL;

void printWords();
void spawnWords();
void moveWords();
void clearWords();
int how_many_words(int difficulty, int time);
int get_input(char input[WORDLENGTH]);

int isGameOver(listItem* first);
void gameOver(int time, int numWords);

int main(int argc, char* argv[]){
	int i;
	listItem* first = create_list(); //linked list node
	srand((int)time(0));
	
	printf("This is the typing game!\n\nWords will appear at the top of the given box, and you will have to type them before they reach the bottom of the same box. Good Luck!\n\n");
	main_screen(); //allows user to either play game or add words
	
	char* wordlist[MAXWORDS];
	int wordcount = read_words(wordlist, argv[1]);
	int wordPosition[100][100]; //reads in array
	
	initscr(); //erases screen before game
	printBorder(); //prints game border
	
	int time;
	char input[WORDLENGTH];
	
	int totalTime = 0;
	int wordsTyped = 0;
	int difficulty = 1;
	
	int numWords = 3;
	spawnWords(first, numWords, wordlist, 0);
	
	while (1) {
		time = get_input(input);
		if (time < 1) {
			time = 1;
		}
		totalTime += time;
		
		if (find_and_delete_word(first, input)) { //conditional will delete word if it is typed correctly
			wordsTyped++;
			mvprintw(HEIGHT + 3, 1, "   "); //prints correct if user types the right word
			mvprintw(HEIGHT + 3, 1, "Correct!");
		}
		else {
			mvprintw(HEIGHT + 3, 1, "   "); //prints incorrect if user types the wrong word
			mvprintw(HEIGHT + 3, 1, "Incorrect!");
		}
		
		numWords = how_many_words(difficulty, time); //spawns words based on difficulty/time
		for (i = 0; i < numWords; i++) {
			moveWords(first);
		}
		
		if (isGameOver(first)) {
			gameOver(totalTime, wordsTyped); //if word reaches bottom, game ends
			break;
		}
		spawnWords(first, numWords, wordlist, 0);
		
		if (wordsTyped % 25 == 0) {
			difficulty++; //increases difficulty as more words are typed
		}
	}
	
	return 0;
}

///////////////////////////////////////
//User Defined Functions' Definition//
/////////////////////////////////////
void gameOver(int time, int numWords) { //when game ends, prints time played and words typed
	endwin();
	if (numWords > 50) {
		printf("Game Over! You successfully typed more than 50 words!\n");
	}
	else {
		printf("Game Over! You successfully typed less than 50 words!\n");		
	}
	printf("You made it %d seconds and typed %d words!\n", time, numWords);
}

int isGameOver(listItem* first) { //ends game when words reach bottom
	listItem* curr = get_next_word(first);
	while (curr != NULL) {
		if (curr->y >= HEIGHT) {
			return 1;
		}
		refresh();
		curr = get_next_word(curr);
	}
	return 0;
}

int how_many_words(int difficulty, int time) { //increases difficulty based on words typed
	int numWords = 0;
	if (time < 5) {
		numWords = difficulty;
	}
	else if (time < 10) {
		numWords = difficulty * 2;
	}
	else {
		numWords = difficulty * 3;
	}
	return numWords;
}

int get_input(char input[WORDLENGTH]) { //returns time it took the user to type a word
	time_t before = time(NULL);
	scanf(" %s", input);
	time_t after = time(NULL);
	int time = after - before;
	
	mvprintw(HEIGHT + 2, 8, "\t\t\t\t\t");
	mvprintw(HEIGHT + 2, 8, "%s", input);
	refresh();
	
	return time;
}

void moveWords(listItem* first) { //moves words down a line each time a word is typed
	listItem* curr = get_next_word(first);
	
	clearWords(first);
	
	while (curr != NULL) {
		curr->y++;
		curr = get_next_word(curr);
	}
	printWords(first);
}

void clearWords(listItem* first) { //erases words from screen
	int i;
	listItem* curr = get_next_word(first);
	while (curr != NULL) {
		for (i = 0; i < strlen(curr->word); i++) {
			mvprintw(curr->y, curr->x + i, "%s", " "); //prints whitespace as long as the word being deleted
			refresh();
		}
		curr = get_next_word(curr);
	}
}

void printWords(listItem* first) { //prints words based on values from function spawnWords
	listItem* curr = get_next_word(first);
	while (curr != NULL) {
		mvprintw(curr->y, curr->x, "%s", curr->word);
		refresh();
		curr = get_next_word(curr);
	}
}

void spawnWords(listItem* firstWord, int numWords, char* wordlist[MAXWORDS]) { //puts words at the top of the box
	int i;
	int wordCounter = 0;
	int xVal;
	int yVal = 1;
	int arrayVal;
	int firstLine[500];
	int occupied = 0;
	
	for (i = 0; i < WIDTH; i++) {
		firstLine[i] = 0;
	}
	while (wordCounter < numWords) {
		occupied = 0;
		xVal = rand() % WIDTH + 1;
		arrayVal = rand() % 1000;
		
		for (i = xVal; i < xVal + strlen(wordlist[arrayVal]) + 1; i++) {
			if (firstLine[i] == 1) {
				occupied = 1;
			}
		}
		//fills in word
		if (occupied == 0) {
			for (i = xVal; i < xVal + strlen(wordlist[arrayVal]) + 1; i++) {
				firstLine[i] = 1;
			}
			add_word(firstWord, wordlist[arrayVal], xVal, yVal);
			mvprintw(yVal, xVal, "%s", wordlist[arrayVal]);
			refresh();
			wordCounter++;
		}
	}
}

void trimws(char* str) { //erases extra whitespace
	int length = strlen(str);
	int x;
	if (length == 0) {
		return;
	}
	x = length - 1;
	while (isspace(str[x]) && (x >= 0)) {
		str[x] = '\0';
		x -= 1; //when it reaches the null character, it will erase it and leave only the word
	}
}

int read_words(char* WL[MAXWORDS], char* file_name) { //uses file to read words
	int numread = 0;
	char line[WORDLENGTH];
	char* p;
	FILE* fp = fopen(file_name, "r");
	while (!feof(fp)) {
		p = fgets(line, WORDLENGTH, fp);
		if (p != NULL) {
			trimws(line);
			WL[numread] = (char*)malloc(strlen(line) + 1);
			strcpy(WL[numread], line);
			numread++;
		}
	}
	fclose(fp);
	return numread; //creates an array with the words
}

void printBorder() { //prints game border
	int i;
	for (i = 0; i <= HEIGHT; i++) {
		mvprintw(i, 0, "*");
	}
	for (i = 0; i <= HEIGHT; i++) {
		mvprintw(i, WIDTH + 8, "*");
	}
	for (i = 1; i <= WIDTH + 7; i++) {
		mvprintw(0, i, "-");
	}
	for (i = 1; i <= WIDTH + 7; i++) {
		mvprintw(HEIGHT, i, "-");
	}
	mvprintw(HEIGHT + 2, 1, "Input: ");
}

int new_word() { //add word function
	char newWord[20] = "uh";
	int i;
	int wordLength;
	int repeatFlag = 1;
	int printFlag = 0; //if 1, new entry is printed
	printf("\n\nYou can now add your own words!\nTo go back to the main menu, type 'n'.\n\nEnter your word here and click enter.\n-> ");
	scanf("%s", newWord);
	wordLength = strlen(newWord);
	
	for (i = 0; i < wordLength; i++) { //checks that word is alphabetical
		if (isalpha(newWord[i]) == 0) {
			printf("\n%d is invalid, please enter alphabetical characters\n", i + 1);
			printFlag = 0;
			repeatFlag = 1;
		}
		else {
			printFlag = 1;
		}
	}
	
	if (newWord[0] == 'n' && newWord[1] == NULL) {
		printFlag = 0;
		repeatFlag = 0;
		main_screen();
	}
	
	if (printFlag != 0) {
		fprintf(myFile, "\n%s", newWord);
	}
	
	if (repeatFlag != 0) {
		new_word();
	}
}

void main_screen() { //asks user to play game or add new words
	myFile = fopen("wordlist.txt", "a");
	int choice = 0;
	printf("\nEnter 1 to begin or 2 to add your own words!\n-> ");
	scanf(" %d", &choice);
	while (choice != 1 && choice != 2) {
		printf("\n\nInvalid choice, please try again!\n-> ");
		scanf("%d", &choice);
	}
	
	if (choice == 1) {
		fclose(myFile);
		return;
	}
	if (choice == 2) {
		new_word();
	}
}

int find_and_delete_word(listItem* first, char word[WORDLENGTH]) {
	listItem* curr = first; //looks for typed word, then calls function to delete it
	listItem* next;
	int check;
	while (curr != NULL) { //checks until end of linked list
		if (curr->nextPtr == NULL) {
			return 0;
		}
		next = get_next_word(curr);
		char nextWord[WORDLENGTH];
		strcpy(nextWord, next->word);
		
		check = strcmp(nextWord, word);
		if (check == 0) { //clears word from list
			clearWords(first);
			delete_word_after(curr);
			return 1;
		}
		curr = get_next_word(curr);
	}
	return 0;
}

void delete_word_after(listItem* thisWord) { //deletes word after the given word
	listItem* deleteThis = thisWord->nextPtr;
	
	if (deleteThis->nextPtr == NULL) {
		thisWord->nextPtr = NULL;
		free(deleteThis);
		return;
	}
	
	listItem* wordAfter = deleteThis->nextPtr;
	thisWord->nextPtr = wordAfter;
	free(deleteThis);
}

void add_word(listItem* firstWord, char* word, int x, int y) { //adds word to linked link, as long as user chose option 2
	listItem* newWord = (listItem*)malloc(sizeof(listItem));
	initWord(newWord, word, x, y, firstWord->nextPtr);
	
	firstWord->nextPtr = newWord;
}

void initWord(listItem* this, char* word, int x, int y, listItem* nextPtr) {
	strcpy(this->word, word); //creates new item in the linked list
	this->x = x;
	this->y = y;
	this->nextPtr = nextPtr;
}

listItem* get_next_word(listItem* thisWord) { //next item in linked list is returned
	return thisWord->nextPtr;
}

listItem* create_list(void) { //first item/node in the list is returned
	listItem* firstWord = (listItem*)malloc(sizeof(listItem));
	
	initWord(firstWord, "NULL", -1, -1, NULL);
	
	return firstWord;
}