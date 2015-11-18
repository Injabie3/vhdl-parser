//Use only the following three libraries:
#include "parserClasses.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
void printList(const TokenList theList);

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

	Tokenizer tokenizer;

	//Read in a file line-by-line and tokenize each line
	sourceFile.open("test.txt");
	if (!sourceFile.is_open()) {
		cout << "Failed to open file" << endl;
		return 1;
	}

	while(!sourceFile.eof()) {
		string line;
		getline(sourceFile, line);

		tokenizer.setString(&line);
		while(!tokenizer.isComplete()) {
			tokens.append(tokenizer.getNextToken());
		}
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

	printList(identifierTokens);
	printList(operatorTokens);
	printList(literalTokens);
	printList(commentBodyTokens);
	printList(otherTokens);

  /* Create operator,identifier,literal, etc. tokenLists from the master list of tokens */


	return 0;
}

void printList(const TokenList theList)
{
	int tokenTypeInt;
	string tokenType;
	string details;
	string stringRep;
	int width;

	Token *t = theList.getFirst();
	cout << endl;
	cout << setw(20) << "Token" << "|" << setw(15) << "Type" << "|" << setw(5) << "KW?" << "|" << setw(20) << "Token Type (if any)" << "|" << setw(10) << "Width (if any)" << endl;
	while (t) {
		details = "N/A";
		width = 0;

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

		cout << setw(20) << stringRep << "|" << setw(15) << tokenType << "|" << setw(5) << t->isKeyword() << "|" << setw(20) << details << "|" << setw(10) << width << endl;
		t = t->getNext();
	}
	cout << endl << endl;
}