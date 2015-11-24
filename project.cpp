//Use only the following three libraries:
#include "parserClasses.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

//Print out the list in a nice format.
void printList(TokenList *theList);

//Function executes error checking on the token list, and outputs the error to the screen, depending on the mode set.
void statistics(TokenList &theList, bool verbose, ostream& outputStream);

//Function checks a conditional statement list for width/type mismatches, and outputs the error to the screen depending on the mode set.
//Array should be in the following format:
// Element 1: Count of unknown type and width.
// Element 2: Count of type mismatch.
// Element 3: Count of width mismatch.
void checkConditionalMismatch(TokenList *conditionalList, bool verbose, ostream &outputStream, int(&errors)[3]);

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

	//Read in a file line-by-line and tokenize each line
	sourceFile.open("test.txt");
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
	while(t) {
		cout << t->getStringRep() << " ";
		t = t->getNext();
	}

	t = tokens.getFirst();
	while (t)
	{
		tokens.findAndSetTokenDetails(t);
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
	printList(&tokens);
	printList(&identifierTokens);
	printList(&operatorTokens);
	printList(&literalTokens);
	printList(&commentBodyTokens);
	printList(&otherTokens);
	
  /* Create operator,identifier,literal, etc. tokenLists from the master list of tokens */
	
	conditionalStatements = findAllConditionalExpressions(tokens);

	printList(conditionalStatements);
	statistics(tokens, true, cout);

	return 0;
}

//Print out the list in a nice format.
void printList(TokenList *theList)
{
	int tokenTypeInt;
	string tokenType;
	string details;
	string stringRep;
	int error = 0;
	int width;

	Token *t = theList->getFirst();
	cout << endl;
	cout << setw(20) << "Token" << "|" << setw(15) << "Type" << "|" << setw(5) << "KW?" << "|" << setw(20) << "Token Type (if any)" << "|" << setw(10) << "Width (if any)" << "|" << setw(6) << "CS Error?" << endl;
	while (t) {
		details = "N/A";
		width = 0;
		error = 0;
		if (t->getConditionalError())
			error = 1;

		tokenTypeInt = t->getTokenType();
		if (tokenTypeInt == 0)
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

		if (t->getStringRep() == "\n")
			stringRep = "(new line)";
		else
			stringRep = t->getStringRep();

		cout << setw(20) << stringRep << "|" << setw(15) << tokenType << "|" << setw(5) << t->isKeyword() << "|" << setw(20) << details << "|" << setw(10) << width << "|" << setw(10) << error << endl;
		t = t->getNext();
	}
	cout << endl << endl;
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

	//Check for missing "then"s and "end if"s
	checkErrorConditionalStatements(&theList, verbose, outputStream, missingThen, missingEndIf);

	//Find conditional statements
	conditionalStatements = findAllConditionalExpressions(theList,true);

	checkConditionalMismatch(conditionalStatements, verbose, outputStream, mismatchErrors);

	outputStream << "# of tokens: " << numberTokens << endl;
	outputStream << "# of lines: " << numberLines << endl;
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
void checkConditionalMismatch(TokenList *conditionalList, bool verbose, ostream &outputStream, int (&errors)[3])
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
					if (verbose)
						printErrorLine(current, "Unknown type and width", outputStream);
				}
				else if (before->getTokenDetails()->type != after->getTokenDetails()->type)	//Type mismatch
				{
					errors[1]++;
					if (verbose)
						printErrorLine(current, "Type mismatch", outputStream);
				}
				else if (before->getTokenDetails()->width != after->getTokenDetails()->width) //Width mismatch
				{
					errors[2]++;
					if (verbose)
						printErrorLine(current, "Width mismatch", outputStream);
				}
			}
		}
		current = current->getNext();
	}
}