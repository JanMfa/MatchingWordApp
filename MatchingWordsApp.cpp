// MatchingWordsApp is an application to create an output of substring matching words.
// Condition: commonwords.txt and allwords.txt has to be in the same folder/path.
// allwords.txt consist of single word on each line in alphabetical order
// commonwords.txt consist of substrings  on each line
// Output: matchingwords.txt will be in the same folder/path
// matchingwords.txt consist of substring matches presented in alphabetical order, e.g.:
// that: at, hat

#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <errno.h>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <algorithm>
#include <pthread.h>
#include <cmath>

// --VECTORS --
std::vector<std::string> allwords;
std::vector<std::vector<std::string>> allwordsArr;
std::vector<std::string> commonwords;
std::vector<std::string> matchwords;
std::vector<std::vector<std::string>> matchwordsArr;
std::vector<std::string> finalWords;
std::vector<std::vector<std::string>> finalWordsArr;

// -- LOCKS --
pthread_mutex_t mutex;

// -- VARIABLES --
static int divide = 0;

//========================== readAllWords() =============================
//Purpose:	Open the file allwords.txt and push the line into allwords vector.
//			Create empty vector of matchwords for the pairing of vector that is use later.
//PRE:
//PARAM:
//POST: Create an initialize value of vector of allwords
//========================================================================
void readAllWords()
{
	std::string line;
	// Open file allwords.txt.
	std::ifstream file("allwords.txt");
	int counter = 0;
	// When the file is open.
	if (file.is_open()) {
		// Get the line in the file.
		while (std::getline(file, line)) {
			// Push the line into wholewords vector.
			allwords.push_back(line);
			// Push empty string into matchwords vector.
			matchwords.push_back("");
		}
		// Close the file.
		file.close();
	}
}

//========================== readAllWords() =============================
//Purpose:	Divide the allwords into allwordsArr 
//PRE:
//PARAM:
//POST: Create an array of vector of allwordsArr from allwords
//========================================================================
void divideAllWords() {
	//get the total number of allwords vector
	int allWordsSize = allwords.size();
	// splitting the square root number of size of allwords 
	divide = round(sqrt(allWordsSize));
	int counter = 0;
	int arr_counter = 0;
	// allocating empty vectors
	// the number of vectors will be the square root of number of allwords
	for (int i = 0; i < divide;i++) {
		std::vector<std::string> allwordsv;
		std::vector<std::string> matchwordsv;
		allwordsArr.push_back(allwordsv);
		matchwordsArr.push_back(matchwordsv);
	}
	// allocate filled-in vectors  into the arrays
	// each array will consist of vector with the size of sqaure root of number of allwords
	while (counter < allWordsSize) {
		allwordsArr[arr_counter].push_back(allwords[counter]);
		matchwordsArr[arr_counter].push_back(matchwords[counter]);
		// increment the array when the counter is reach the number of divide and its multiplication
		if (counter!= 0 && counter % divide == 0) {
			arr_counter++;
		}
		counter++;
	}
}

//========================== readCommonWords() =============================
//Purpose:	Open the file commonwords.txt and push the line into commonwords vector.
//PRE:
//PARAM:
//POST: Create a sorted value of vector commonwords.
//========================================================================
void readCommonWords()
{
	std::string line;
	// Open file commonwords.txt.
	std::ifstream file1("commonwords.txt");
	// When the file is open.
	if (file1.is_open()) {
		// Get the line in the file.
		while (std::getline(file1, line)) {
			// Push the line into commonwords vector.
			commonwords.push_back(line);
		}
		// Close the file.
		file1.close();
	}
	// Sort the commonwords vector.
	std::sort(commonwords.begin(), commonwords.end());
}

//========================== readCommonWords() =============================
//Purpose:	Create 30 array of matchwords vector
//PRE: allwordsArr and commonwords has to be initialized
//PARAM: index: index of where the matching start
//POST: Create 30 array of matchwords vector by appending the string of the allwordsArr and commonwords
//========================================================================
void matchCommonWords(int index)
{
	// Initialize the lock.
	pthread_mutex_init(&mutex, NULL);
	// Loop through the wholewords vector.
	for (int i = 0; i < allwordsArr[index].size(); i++) {
		// Initialized the string to be pushed in as empty string matchwords vector index i.
		std::string push_in_string = matchwordsArr[index][i];
		for (int j = 0; j < commonwords.size(); j++) {
			// Find commonwords of index j inside the wholewords of index i.
			if (std::string::npos != allwordsArr[index][i].find(commonwords[j])) {
				// Push in the commonwords if the commonwords is find inside the wholewords vector of index i.
				push_in_string.append(" ");
				push_in_string.append(commonwords[j]);
				push_in_string.append(",");
			}
		}
		// Push in the string into the matchwords after all of the commonwords have been appended.
		matchwordsArr[index][i] = push_in_string;
	}
	// Release the lock.
	pthread_mutex_unlock(&mutex);
}


//========================== main() =============================
//Purpose:	Where the program starts
//PRE: commonwords.txt and allwords.txt exist
//PARAM: 
//POST: Create a text file output called matchingwords.txt
//========================================================================
int main()
{
	// Open the file matchingwords.txt as output of the whole project.
	std::ofstream ofs;
	ofs.open("matchingwords.txt", std::ofstream::out);

	// Create thread of thread1 to execute readAllWords function.
	std::thread thread1{ readAllWords };
	// Create thread of thread2 to execute readCommonWords function.
	std::thread thread2{ readCommonWords };

	// Block the threads until the two functions are done executing.
	thread1.join();
	thread2.join();

	divideAllWords();

	// Create array of thread.
	std::vector<std::thread> thread_arr;
	for (int i = 0; i < divide; i++) {
		thread_arr.push_back(std::thread(matchCommonWords, i));
	}
	
	// Make sure that the thread wait until the rest of the thread completes its execution
	for (int i = 0; i < divide; i++) {
		thread_arr[i].join();
	}

	// Outputing to into the file matchingwords.txt.
	for (int i = 0; i < divide; i++) {
		for (int j = 0; j < allwordsArr[i].size(); j++) {
			// Remove the last ',' if the matchwords has a trailing ','.
			if (!matchwordsArr[i][j].empty() && matchwordsArr[i][j].back() == ',') {
				matchwordsArr[i][j].pop_back();
			}
			// Writing into file.
			ofs << allwordsArr[i][j] << ":" << matchwordsArr[i][j] << "\n";
		}
	}

	// Close mathingwords.txt file
	ofs.close();

	std::cout << "Execution is done. Please check matchingwords.txt file for output.";

	// To hold the console, so it does not close upon execution
	std::getchar();

	return 0;
}


