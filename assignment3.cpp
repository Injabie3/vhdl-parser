// ENSC 251 Assignment 3
// Title:			assignment3.cpp
// Related Files:	parserClasses.h, parserClasses.cpp
// Author(s):		Lesley Shannon, Ryan Lui, Lior Bragilevsky
// Student Number:	301251951, 301248920
// Last Modified:	2015-10-26
// Special Thanks:	Ashton Tito (301266791) for help with understanding
// Version:			2.0
// Revision Hist.:	1.0 - File creation.
//					2.0 - Final product.

//Use only the following three libraries:
#include "parserClasses.h"
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

//Example Test code for interacting with your Token, TokenList, and Tokenizer classes
//Add your own code to further test the operation of your Token, TokenList, and Tokenizer classes
int main() {
	ifstream sourceFile;
	ofstream outFile;
	TokenList tokens;
	TokenList *conditionalTokens;
	Tokenizer tokenizer;
	string fileName;
	int lines = 0; //number of lines
	int commentsRemoved = 0;

	//Read in a file line-by-line and tokenize each line
	cout << "Enter VHDL file name with extension >>";
	cin >> fileName;
	sourceFile.open(fileName);
	outFile.open("output.txt");
	if (!sourceFile.is_open()) {
		cout << "Failed to open file" << endl;
		return 1;
	}

	while (!sourceFile.eof()) {
		string lineA, lineB;

		getline(sourceFile, lineA);

		//while the current line ends with a line-continuation \ append the next line to the current line
		while (lineA.length() > 0 && lineA[lineA.length() - 1] == '\\') {
			lineA.erase(lineA.length() - 1, 1);
			getline(sourceFile, lineB);
			lineA += lineB;
		}

		tokenizer.setString(&lineA);
		while (!tokenizer.isComplete()) {
			tokens.append(tokenizer.getNextToken());
		}
		//Re-insert newline that was removed by the getline function
		//tokens.append("\n");
	}

	/*Test your tokenization of the file by traversing the tokens list and printing out the tokens*/
	Token *t = tokens.getFirst();
	while (t) {
		cout << "[" << t->getStringRep() << "] ";
		outFile << "[" << t->getStringRep() << "] ";
		if (t->getStringRep() == "\n")
		{
			lines++;
		}
		t = t->getNext();
	}
	cout << endl << "Number of lines: " << lines << endl;
	lines = 0;
	//commentsRemoved = removeComments(tokens);

	cout << "Comments removed: " << commentsRemoved << endl;
	cout << "\Categorized Tokens:\n";
	/*Test your tokenization of the file by traversing the tokens list and printing out the tokens*/
	t = tokens.getFirst();
	string details;
	int width;
	cout << setw(20) << "Token" << "|" << setw(5) << "Type" << "|" << setw(5) << "KW?" << "|" << setw(20) << "Token Type (if any)" << "|" << setw(10) << "Width (if any)" << endl;
	while(t) {
		tokens.findAndSetTokenDetails(t);
		details = "N/A";
		width = 0;
		if (t->getTokenDetails() != NULL)
		{
			details = t->getTokenDetails()->type;
			width = t->getTokenDetails()->width;
		}
		cout << setw(20) << t->getStringRep() << "|" << setw(5) << t->getTokenType() << "|" << setw(5) << t->isKeyword() << "|" << setw(20) << details << "|" << setw(10) << width << endl;
		if (t->getStringRep() == "\n")
		{
			lines++;
		}
		t = t->getNext();
	}
	cout << endl << "Number of lines: " << lines << endl;

	conditionalTokens = findAllConditionalExpressions(tokens);
	if (conditionalTokens != NULL)
		t = conditionalTokens->getFirst();
	while (t) {
		cout << "[" << t->getStringRep() << "] " << endl;
		outFile << "[" << t->getStringRep() << "] " << endl;
		if (t->getStringRep() == "\n")
		{
			lines++;
		}
		t = t->getNext();
	}
	sourceFile.close();
	outFile.close();
	return 0;
}
