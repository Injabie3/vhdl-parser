// ENSC 251 Project
// Title:			project.cpp
// Related Files:	parserClasses.h, parserClasses.cpp
// Author(s):		Lesley Shannon, Ryan Lui, Lior Bragilevsky
// Student Number:	301251951, 301248920
// Last Modified:	2015-11-30
// Version:			2.0
// Revision Hist.:	1.0 - File creation.
//					2.0 - Final product for project.

#include "parserClasses.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

//Print out the list in a nice format.
void printList(TokenList *theList, ostream &outputStream);

//Function executes error checking on the token list, and outputs the error to the screen, depending on the mode set.
void statistics(TokenList &theList, bool verbose, ostream& outputStream);

//Function checks a conditional statement list for width/type mismatches, and outputs the error to the screen depending on the mode set.
//Array should be in the following format:
// Element 1: Count of unknown type and width.
// Element 2: Count of type mismatch.
// Element 3: Count of width mismatch.
void checkConditionalMismatch(TokenList *originalList, TokenList *conditionalList, bool verbose, ostream &outputStream, int(&errors)[3]);

//Function finds the first conditional statement belonging to the conditional statement specified via an integer in the original list, and returns a pointer to that token.  This function can only be called after findAllConditionalStatements() has been called.
Token* findConditionalToken(TokenList *originalList, int conditionalStatement);

//Example Test code for interacting with your Token, TokenList, and Tokenizer classes
//Add your own code to further test the operation of your Token, TokenList, and Tokenizer classes
int main() {
	ifstream sourceFile;
	TokenList tokens;

	//Lists for types of tokens
	TokenList operatorTokens;
	TokenList identifierTokens;
	TokenList literalTokens;
	TokenList commentBodyTokens;
	TokenList otherTokens;
	TokenList *conditionalStatements = NULL;

	int lines = 0; //number of lines
	int tokenNumber = 0; //number of tokens
	int missingThen = 0;
	int missingEndIf = 0;

	Tokenizer tokenizer;
	string fileName;
	ofstream outFile;
	//Read in a file line-by-line and tokenize each line
	cout << "Enter VHDL file name with extension >>";
	cin >> fileName;
	sourceFile.open(fileName);
	outFile.open("output.txt");
	if (!sourceFile.is_open()) {
		cout << "Failed to open file" << endl;
		return 1;
	}

	tokens.append("\n");
	while(!sourceFile.eof()) {
		string line;
		getline(sourceFile, line);

		tokenizer.setString(&line);
		while(!tokenizer.isComplete()) {
			tokens.append(tokenizer.getNextToken());
			tokenNumber++;
		}

		//Re-insert newline that was removed by the getline function to keep track of the number of lines in the code.
		tokens.append("\n");
		lines++; //number of lines
	}


	/*Test your tokenization of the file by traversing the tokens list and printing out the tokens*/
	Token *t = tokens.getFirst();

	cout <<		"Before setting token details, the list looks like:\n"
		<<		"--------------------------------------------------\n";
	outFile <<	"Before setting token details, the list looks like:\n"
		<<		"--------------------------------------------------\n";

	while(t) {
		cout << t->getStringRep() << " ";
		outFile << t->getStringRep() << " ";
		t = t->getNext();
	}

	t = tokens.getFirst();
	while (t)
	{
		tokens.findAndSetTokenDetails(t);
		t = t->getNext();
	}

	t = tokens.getFirst();
	cout <<		"\nAfter setting token details, the list looks like:\n"
		<<		"-------------------------------------------------\n";
	outFile <<	"\nAfter setting token details, the list looks like:\n"
		<<		"-------------------------------------------------\n";

	while (t) {
		cout << t->getStringRep() << " ";
		outFile << t->getStringRep() << " ";
		t = t->getNext();
	}


	Token *copy;
  /* For your testing purposes only */
  /* Ensure that tokens have all type information set*/
	checkErrorConditionalStatements(&tokens, false, cout, missingThen, missingEndIf);
	t = tokens.getFirst();
	while (t)
	{
		copy = new Token;
		*copy = *t;
		if (t->isIdentifier())
			identifierTokens.append(copy);
		else if (t->isOperator())
			operatorTokens.append(copy);
		else if (t->isLiteral())
			literalTokens.append(copy);
		else if (t->isComment())
			commentBodyTokens.append(copy);
		else
			otherTokens.append(copy);
		t = t->getNext();
	}

	cout << endl << endl;
	outFile << endl << endl;

	//Print out the sorted lists to both the terminal and output file.
	cout << "The entire list:\n";
	printList(&tokens, cout);
	cout << "Identifier tokens:\n";
	printList(&identifierTokens, cout);
	cout << "Operator tokens:\n";
	printList(&operatorTokens, cout);
	cout << "Literal tokens:\n";
	printList(&literalTokens, cout);
	cout << "Comment body tokens:\n";
	printList(&commentBodyTokens, cout);
	cout << "Other tokens:\n";
	printList(&otherTokens, cout);

	outFile << "The entire list:\n";
	printList(&tokens, outFile);
	outFile << "Identifier tokens:\n";
	printList(&identifierTokens, outFile);
	outFile << "Operator tokens:\n";
	printList(&operatorTokens, outFile);
	outFile << "Literal tokens:\n";
	printList(&literalTokens, outFile);
	outFile << "Comment body tokens:\n";
	printList(&commentBodyTokens, outFile);
	outFile << "Other tokens:\n";
	printList(&otherTokens, outFile);
	
	//Find conditional statements.
	conditionalStatements = findAllConditionalExpressions(tokens);

	//Print out conditional statements to both terminal and output file.
	cout << "Conditional statement tokens:\n";
	printList(conditionalStatements, cout);
	outFile << "Conditional statement tokens:\n";
	printList(conditionalStatements, outFile);

	cout << "Below is verbose output:\n";
	outFile << "Below is verbose output:\n";
	//Run statistics, and print to both terminal and output file.
	statistics(tokens, true, cout);
	statistics(tokens, true, outFile);

	cout << "------------------------\n";
	outFile << "------------------------\n";
	cout << "Below is non-verbose output:\n";
	outFile << "Below is non-verbose output:\n";
	//Run statistics, and print to both terminal and output file.
	statistics(tokens, false, cout);
	statistics(tokens, false, outFile);


	return 0;
}

//Print out the list in a nice format to the output stream.
void printList(TokenList *theList, ostream &outputStream)
{
	int tokenTypeInt;
	string tokenType;	//Temporary string to hold the token type of the token.
	string details;		//Temporary string to hold the token type of a literal/identifier (when applicable)
	string stringRep;	//Temporary string to hold the string in the token.
	int error = 0;		//Temporary int to display 1 or 0 for true/false conditional error in token (when applicable)
	int width;			//Temporary int to hold the width of a literal/identifier (when applicable)

	Token *t = theList->getFirst();
	outputStream << endl;
	outputStream << setw(20) << "Token" << "|" << setw(15) << "Type" << "|" << setw(5) << "KW?" << "|" << setw(20) << "Token Type (if any)" << "|" << setw(14) << "Width (if any)" << "|" << setw(6) << "CS Error?" << endl;
	//Traverse and print out list to the output stream.
	while (t) {
		details = "N/A";	//Initialize to N/A.
		width = 0;			//Initialize to no width.
		error = 0;			//Initialize to no error.
		if (t->getConditionalError())
			error = 1;

		tokenTypeInt = t->getTokenType();

		//Change tokenType to the correct token type. Could've used the get functions (bool) for each, but this works as well.
		if (t->getStringRep() == "\n")
			tokenType = "n/a";
		else if (tokenTypeInt == 0)
			tokenType = "T_Operator";
		else if (tokenTypeInt == 1)
			tokenType = "T_Identifier";
		else if (tokenTypeInt == 2)
			tokenType = "T_Literal";
		else if (tokenTypeInt == 3)
			tokenType = "T_CommentBody";
		else tokenType = "T_Other";

		if (t->getTokenDetails() != NULL)
		{
			details = t->getTokenDetails()->type;
			width = t->getTokenDetails()->width;
		}

		if (t->getStringRep() == "\n") //Indicate new line token.
			stringRep = "(new line)";
		else
			stringRep = t->getStringRep();	//Store the string into the temp string.

		//Output the result on the output stream
		outputStream << setw(20) << stringRep << "|" << setw(15) << tokenType << "|" << setw(5) << t->isKeyword() << "|" << setw(20) << details << "|" << setw(14) << width << "|" << setw(10) << error << endl;
		t = t->getNext();
	}
	outputStream << endl << endl;
}

//Function executes error checking on the token list, and outputs the error to the screen, depending on the mode set.
//List must have \n appended after each line of tokens!
void statistics(TokenList &theList, bool verbose, ostream& outputStream)
{
	Token *move = theList.getFirst();
	int numberLines = 0;	//Number of lines
	int numberTokens = 0;	//Number of tokens
	int numberConditionalStatements = 0; //Number of conditional statements
	int missingThen = 0;	//Number of missing thens
	int missingEndIf = 0;	//Number of missing end ifs
	int mismatchErrors[3];	//Array for conditional mismatches
	TokenList *conditionalStatements = NULL;

	//Get number of lines and tokens
	while (move)
	{
		if (move->getStringRep() == "\n")
		{
			numberLines++;
			numberTokens--;
		}
		numberTokens++;
		move = move->getNext();
	}

	if (verbose)
	{
		outputStream << "Errors in the token list:\n";
		outputStream << "=========================\n";
	}

	//Check for missing "then"s and "end if"s
	checkErrorConditionalStatements(&theList, verbose, outputStream, missingThen, missingEndIf);

	//Find conditional statements
	conditionalStatements = findAllConditionalExpressions(theList,verbose);

	checkConditionalMismatch(&theList,conditionalStatements, verbose, outputStream, mismatchErrors);
	
	if (verbose)
		outputStream << "\n\n";

	outputStream << "Statistics on the token list:" << endl;
	outputStream << "=============================" << endl;
	outputStream << "# of tokens: " << numberTokens << endl;
	outputStream << "# of lines: " << numberLines - 1 << endl;
	outputStream << "# of missing \"then\"s: " << missingThen << endl;
	outputStream << "# of missing \"end if\"s: " << missingEndIf << endl;
	outputStream << "# of unknown types/widths: " << mismatchErrors[0] << endl;
	outputStream << "# of type mismatches: " << mismatchErrors[1] << endl;
	outputStream << "# of width mismatches: " << mismatchErrors[2] << endl;

}

//Function checks a conditional statement list for width/type mismatches, and outputs the error to the screen depending on the mode set.
//Array should be in the following format:
// Element 1: Count of unknown type and width.
// Element 2: Count of type mismatch.
// Element 3: Count of width mismatch.
void checkConditionalMismatch(TokenList *originalList, TokenList *conditionalList, bool verbose, ostream &outputStream, int (&errors)[3])
{
	errors[0] = 0;	//Reset count of unknown type/width (no tokenDetails struct)
	errors[1] = 0;	//Reset count of type mismatch
	errors[2] = 0;	//Reset count of width mismatch.
	Token *current = conditionalList->getFirst();
	Token *before = NULL;
	Token *after = NULL;

	while (current)
	{
		for (int ii = 0; ii < 6; ii++)
		{
			if (current->getStringRep() == COMPARISON_OPERATORS[ii]) //Found a comparison operator. Commence checks.
			{
				before = current->getPrev();
				after = current->getNext();

				if (before->getTokenDetails() == NULL || after->getTokenDetails() == NULL) //No details structure, so can't do comparison check.
				{
					errors[0]++;
					if (verbose) //Print out the entire line from the original list.
						printErrorLine(findConditionalToken(originalList,current->getConditionalStatement()), "Unknown type and width", outputStream);
				}
				else if (before->getTokenDetails()->type != after->getTokenDetails()->type)	//Type mismatch
				{
					errors[1]++;
					if (verbose) //Print out the entire line from the original list.
						printErrorLine(findConditionalToken(originalList, current->getConditionalStatement()), "Type mismatch", outputStream);
				}
				else if (before->getTokenDetails()->width != after->getTokenDetails()->width) //Width mismatch
				{
					errors[2]++;
					if (verbose) //Print out the entire line from the original list.
						printErrorLine(findConditionalToken(originalList, current->getConditionalStatement()), "Width mismatch", outputStream);
				}
			}
		}
		current = current->getNext();
	}
}

//Function finds the first conditional statement belonging to the conditional statement specified via an integer in the original list, and returns a pointer to that token.  This function can only be called after findAllConditionalStatements() has been called.
Token* findConditionalToken(TokenList *originalList, int conditionalStatement)
{
	Token* current = originalList->getFirst();

	while (current)
	{
		if (current->getConditionalStatement() == conditionalStatement)
			return current;
		current = current->getNext();
	}

	return 0; //Just for safety, but this should not be needed because precondition guarantees that this will not happen.
}