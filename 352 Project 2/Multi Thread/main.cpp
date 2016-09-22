/*This file contains the entirety of my project.  It contains the main method, which in turn correlates to the main thread, and creates the other five threads.  The main thread then waits for the other threads to complete then outputs to standard out the letter count of input and output.  The other threads are the reader thread which reads in from a file specified in the command line, to a buffer of a size entered by the user during execution.  The writer thread takes the output buffer and writes it to a file.  The Encryption thread takes the input buffer, encrypts the letters, and then stores them in the output buffer.  The inputCounter thread counts the number of letters in the input buffer.  And lastly the output counter thread counts the number of letters in the output buffer.
 */

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fstream>
using namespace std;

//Function Prototypes:
void countLetters(int (&countArray)[26], char* toCount, int toCountSize);
void printLetterArray(int countArray[26]);
void encryptBuffer(char* ibuff, char* obuff, int b_Size);
char encryptLetter(char toEncrypt, bool& alpha);

//Threads:
void *reader(void*);
void *writer(void*);
void *encryption(void*);
void *inputCounter(void*);
void *outputCounter(void*);

//Global Variables:

//File:
ifstream input;
ofstream output;

/*
 * read1_sem:  A semaphore for the reader thread.  There are two, because
 *           counting and encryption need to happen before the input buffer
 *           can be cleared.
 * read2_sem: see above
 * write_sem:  A semaphore for the writer thread.
 * encrypt1_sem: A semaphore for the encryption thread.  There are several
 *             other threads that need the input and output buffers before
 *            it is overwritten, so there are 3 encrypt sems
 * encrypt2_sem: see above
 * encrypt3_sem: see above
 * inCount_sem: A semaphore for the inputCounter thread
 * outCount_sem: A semaphore for the outputCounter thread
 *            
 */
//Semaphores:
sem_t read1_sem, read2_sem, write_sem, encrypt1_sem, encrypt2_sem, encrypt3_sem, inCount_sem, outCount_sem;

/*
 * inputMutex: A mutex for keeping mutual exclusion on the input buffer
 * outputMutex: A mutex for keeping mutual exclusion on the output buffer
 */
//Mutexes
pthread_mutex_t inputMutex, outputMutex;

/*
 * ibuff: the input buffer
 * obuff: the output buffer
 */
//String:
char *ibuff;
char *obuff;

/*
 * b_Size: the size of the buffer chosen by the user
 * writeSize: the actual number of characters that will be written. If
 *        the end of the file is reached then this will be different than b_Size
 */
//Int:
int b_Size;
int writeSize;

//Count:
int inputCountArray[26];
int outputCountArray[26];
int (&inputCountArray_R)[26] = inputCountArray;
int (&outputCountArray_R)[26] = outputCountArray;

//Encryption: Keeps track of which direction the encryption is going
int encryptKey = 1;

//The main logic and main thread of the application
int main(int argc, char *argv[]){
  
  //Instantiate Ints:
  b_Size = 0;
  writeSize = 0;

  //Threads:
  pthread_t readerThread;
  pthread_t writerThread;
  pthread_t encryptionThread;
  pthread_t inputCounterThread;
  pthread_t outputCounterThread;

  //Instantiate Mutexes
  pthread_mutex_init(&inputMutex, NULL);
  pthread_mutex_init(&outputMutex, NULL);

  //Instantiate Semaphores
  sem_init(&read1_sem, 0, 1);//Read must happen first so its sems are at 1
  sem_init(&read2_sem, 0, 1);
  sem_init(&write_sem, 0, 0);
  sem_init(&encrypt1_sem, 0, 0);
  sem_init(&encrypt2_sem, 0, 1);//Encrypt must be able to run the initial time without write and count
  sem_init(&encrypt3_sem, 0, 1);
  sem_init(&inCount_sem, 0, 0);
  sem_init(&outCount_sem, 0, 0);

  //check to make sure there are the correct number of arguments
  if (argc != 3){
    cout << "Improper number of arguments, you must specify exactly the input and output files" << endl;
    return 0;
  }
  
  //Initialize Count Arrays
  for(int i = 0; i < 26; i++){
    inputCountArray[i] = 0;
    outputCountArray[i] = 0;
  }

  //Open the files for input and output and check for errors
  input.open(argv[1]);
  if(!input){
       input.close();
       cout << "Input file does not exist" << endl;
       return 1;
  }
  output.open(argv[2]);
  if(!output){
       output.close();
       cout << "Output file is not valid" << endl;
       return 1;
  }
  
    //Get buffer size input from user
  cout << "Please enter a buffer size" << endl;
  cin >> b_Size;

  
  //Instantiate the buffers
  ibuff = new char[b_Size];
  obuff = new char[b_Size];
  for (int i = 0; i < b_Size; i++){
    ibuff[i] = '\0';
    obuff[i] = '\0';
  }
  
  //Start creating the Threads
  if(pthread_create(&readerThread, NULL, &reader, NULL) != 0){
    cout << "Reader Thread failed to create!" << endl;
  }
  
  if(pthread_create(&writerThread, NULL, &writer, NULL) != 0){
    cout << "Writer Thread failed to create!" << endl;
  }

  if(pthread_create(&encryptionThread, NULL, &encryption, NULL) != 0){
    cout << "Encryption Thread failed to create!" << endl;
  }

  if(pthread_create(&inputCounterThread, NULL, &inputCounter, NULL) != 0){
    cout << "Input Counter Thread failed to create!" << endl;
  }

  if(pthread_create(&outputCounterThread, NULL, &outputCounter, NULL) != 0){
    cout << "Output Counter Thread failed to create!" << endl;
  }

  //Wait for all threads to complete
  pthread_join(readerThread, NULL);
  pthread_join(writerThread, NULL);
  pthread_join(encryptionThread, NULL);
  pthread_join(inputCounterThread, NULL);
  pthread_join(outputCounterThread, NULL);

  //Print out Relevant Counter Information
  cout << "Input File Contains:" << endl;
  printLetterArray(inputCountArray);
  cout << "Output File Contains:" << endl;
  printLetterArray(outputCountArray);

  //Destroy Things
  pthread_mutex_destroy(&inputMutex);
  pthread_mutex_destroy(&outputMutex);
  sem_destroy(&read1_sem);
  sem_destroy(&read2_sem);
  sem_destroy(&inCount_sem);
  sem_destroy(&outCount_sem);
  sem_destroy(&encrypt1_sem);
  sem_destroy(&encrypt2_sem);
  sem_destroy(&encrypt3_sem);
  sem_destroy(&write_sem);
  delete[] ibuff;
  delete[] obuff;

  return 0;
}

//The reader thread
void *reader(void*){
  bool readFlag = true;

  //Keep doing going until the eof
  while(readFlag){

       //Wait on Semaphores
    sem_wait(&read1_sem);
    sem_wait(&read2_sem);

    //Lock the input buffer
    pthread_mutex_lock(&inputMutex);

    //Reset buffer
    for (int i = 0; i < b_Size; i++){
	 ibuff[i] = '\0';
    }

    //Read into buffer
    input.readsome(ibuff, b_Size);

    //Unlock the input buffer
    pthread_mutex_unlock(&inputMutex);

    //signal semaphores
    sem_post(&encrypt1_sem);
    sem_post(&inCount_sem);
    if (ibuff[b_Size - 1] == input.eof()) readFlag = false;
    
  }
  
  //exit the thread
  pthread_exit(0);
 }


//The startup method for the writer thread
void *writer(void*){
    bool writeFlag = true;

    //Continue until eof is reached
    while(writeFlag){

	 //Wait on semaphores
    sem_wait(&write_sem);

    //Lock output buffer
    pthread_mutex_lock(&outputMutex);

    //determine writeSize so that null characters are not added to file
    for (int i = 0; i < b_Size; i++){
      if (obuff[i] != input.eof()) writeSize++;
    }

    //Write to file
    output.write(obuff, writeSize);

    //Reset writeSize
    writeSize = 0;

    //Unlock output buffer
    pthread_mutex_unlock(&outputMutex);

    //Signal
    sem_post(&encrypt2_sem);
    if(obuff[b_Size - 1] == input.eof()) writeFlag = false;
  }

    //Exit thread
  pthread_exit(0);
}

//The startup method for the encryption thread
void *encryption(void*){
  bool encryptFlag = true;

  //Continue until eof is reached
  while(encryptFlag){

       //Wait on semaphores
    sem_wait(&encrypt1_sem);
    sem_wait(&encrypt2_sem);
    sem_wait(&encrypt3_sem);

    //Lock input and output buffers
    pthread_mutex_lock(&inputMutex);
    pthread_mutex_lock(&outputMutex);
    
    //Encrypt
    encryptBuffer(ibuff, obuff, b_Size);

    //Unlock input and output buffers
    pthread_mutex_unlock(&inputMutex);
    pthread_mutex_unlock(&outputMutex);

    //Signal
    sem_post(&outCount_sem);
    sem_post(&write_sem);
    sem_post(&read2_sem);
    if(ibuff[b_Size - 1] == input.eof() && obuff[b_Size - 1] == input.eof()) encryptFlag = false;
  }

  //Exit the thread
  pthread_exit(0);
}

//The startup method for the input counter thread
void *inputCounter(void*){
  bool inputCountFlag = true;

  //Continue until eof is reached
  while(inputCountFlag){

       //Wait on semaphores
    sem_wait(&inCount_sem);

    //Lock input buffer
    pthread_mutex_lock(&inputMutex);
    
    //count
    countLetters(inputCountArray_R, ibuff, b_Size);

    //Unlock input buffer
    pthread_mutex_unlock(&inputMutex);

    //Signal
    sem_post(&read1_sem);
    if(ibuff[b_Size -1] == input.eof()) inputCountFlag = false;
  }

  //exit thread
  pthread_exit(0);
}

//The startup method for the output counter thread
void *outputCounter(void*){
  bool outputCountFlag = true;

  //Continue until reached eof
  while(outputCountFlag){

       //wait on semaphore
    sem_wait(&outCount_sem);

    //Lock output buffer
    pthread_mutex_lock(&outputMutex);
    
    //Count
    countLetters(outputCountArray_R, obuff, b_Size);

    //Unlock output buffer
    pthread_mutex_unlock(&outputMutex);

    //Signal
    sem_post(&encrypt3_sem);
    if(obuff[b_Size -1] == input.eof()) outputCountFlag = false;
  }

  //exit thread
  pthread_exit(0);
}

//Counts the letters from a character array and stores them in a letter Array
void countLetters(int (&countArray)[26], char* toCount, int toCountSize){

     //go through each index of the array and increment the appropriate index in the count array.
     //these are indexed in alphabetical order a = 0, b = 1, c = 2... etc.
	for (int i = 0; i < toCountSize; i++){
		switch(tolower(toCount[i])){
			case 'a':
				countArray[0]++;
				break;
			case 'b':
				countArray[1]++;
				break;
			case 'c':
				countArray[2]++;
				break;
			case 'd':
				countArray[3]++;
				break;
			case 'e':
				countArray[4]++;
				break;
			case 'f':
				countArray[5]++;
				break;
			case 'g':
				countArray[6]++;
				break;
			case 'h':
				countArray[7]++;
				break;
			case 'i':
				countArray[8]++;
				break;
			case 'j':
				countArray[9]++;
				break;
			case 'k':
				countArray[10]++;
				break;
			case 'l':
				countArray[11]++;
				break;
			case 'm':
				countArray[12]++;
				break;
			case 'n':
				countArray[13]++;
				break;
			case 'o':
				countArray[14]++;
				break;
			case 'p':
				countArray[15]++;
				break;
			case 'q':
				countArray[16]++;
				break;
			case 'r':
				countArray[17]++;
				break;
			case 's':
				countArray[18]++;
				break;
			case 't':
				countArray[19]++;
				break;
			case 'u':
				countArray[20]++;
				break;
			case 'v':
				countArray[21]++;
				break;
			case 'w':
				countArray[22]++;
				break;
			case 'x':
				countArray[23]++;
				break;
			case 'y':
				countArray[24]++;
				break;
			case 'z':
				countArray[25]++;
				break;
			default:
			     //Default case for things such as spaches non alpha num etc.
				break;
		}
	}
}

//Prints the contents of the letter array to the screen
void printLetterArray(int countArray[26]){
	if (countArray[0] != 0) cout << "A: " << countArray[0] << endl;
	if (countArray[1] != 0) cout << "B: " << countArray[1] << endl;
	if (countArray[2] != 0) cout << "C: " << countArray[2] << endl;
	if (countArray[3] != 0) cout << "D: " << countArray[3] << endl;
	if (countArray[4] != 0) cout << "E: " << countArray[4] << endl;
	if (countArray[5] != 0) cout << "F: " << countArray[5] << endl;
	if (countArray[6] != 0) cout << "G: " << countArray[6] << endl;
	if (countArray[7] != 0) cout << "H: " << countArray[7] << endl;
	if (countArray[8] != 0) cout << "I: " << countArray[8] << endl;
	if (countArray[9] != 0) cout << "J: " << countArray[9] << endl;
	if (countArray[10] != 0) cout << "K: " << countArray[10] << endl;
	if (countArray[11] != 0) cout << "L: " << countArray[11] << endl;
	if (countArray[12] != 0) cout << "M: " << countArray[12] << endl;
	if (countArray[13] != 0) cout << "N: " << countArray[13] << endl;
	if (countArray[14] != 0) cout << "O: " << countArray[14] << endl;
	if (countArray[15] != 0) cout << "P: " << countArray[15] << endl;
	if (countArray[16] != 0) cout << "Q: " << countArray[16] << endl;
	if (countArray[17] != 0) cout << "R: " << countArray[17] << endl;
	if (countArray[18] != 0) cout << "S: " << countArray[18] << endl;
	if (countArray[19] != 0) cout << "T: " << countArray[19] << endl;
	if (countArray[20] != 0) cout << "U: " << countArray[20] << endl;
	if (countArray[21] != 0) cout << "V: " << countArray[21] << endl;
	if (countArray[22] != 0) cout << "W: " << countArray[22] << endl;
	if (countArray[23] != 0) cout << "X: " << countArray[23] << endl;
	if (countArray[24] != 0) cout << "Y: " << countArray[24] << endl;
	if (countArray[25] != 0) cout << "Z: " << countArray[25] << endl;
}

//Takes in a buffer and encrypts each letter
void encryptBuffer(char* ibuff, char* obuff, int b_Size){
     bool alpha = false; //keeps track of whether or not the letter to encrypt was actually changed
     bool& alpha_R = alpha;

     //Loops through every index in buffer, and sends it to encryptLetter which will transform that
     //single index, assuming it is an alphabet character
	for (int i = 0; i < b_Size; i++){
		obuff[i] = encryptLetter(ibuff[i], alpha_R);
		
		//Update Encrypt Key
		if(alpha){
			switch(encryptKey){
				case 0:
					encryptKey = 1;
					break;
				case 1:
					encryptKey = -1;
					break;
				case -1:
					encryptKey = 0;
					break;
			}
			alpha = false;
		}
	}
}

//Encrypts a letter given the rules
char encryptLetter(char toEncrypt, bool& alpha){

     //If the character is not from the alphabet don't do anything with it.
     if (isalpha(toEncrypt) == 0) return toEncrypt;
	
	char toReturn;
	
	//Calculate encyrpted character
	if (toEncrypt == 'a' && encryptKey == -1) toReturn = 'z';
	else if(toEncrypt == 'A' && encryptKey == -1) toReturn = 'Z';
	else if (toEncrypt == 'z' && encryptKey == 1) toReturn = 'a';
	else if (toEncrypt == 'Z' && encryptKey == 1) toReturn = 'A';
	else toReturn = toEncrypt + encryptKey;
	
	//Set flag so that Encrypt key gets changed
	alpha = true;
	
	return toReturn;
}
