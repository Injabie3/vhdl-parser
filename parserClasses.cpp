// ENSC 251 Project
// Title:			parserClasses.cpp
// Related Files:	parserClasses.h, Project.cpp
// Author(s):		Lesley Shannon, Ryan Lui, Lior Bragilevsky
// Student Number:	301251951, 301248920
// Last Modified:	2015-11-03
// Special Thanks:	Ashton Tito (301266791) for help with understanding
// Version:			2.0
// Revision Hist.:	1.0 - File creation.
//					2.0 - Final product.
//					3.0 - Modifications for Project.


//Use only the following libraries:
#include "parserClasses.h"
#include <string>



//****TokenList class function definitions******
//           function implementations for append have been provided and do not need to be modified

//Creates a new token for the string input, str
//Appends this new token to the TokenList
//On return from the function, it will be the last token in the list
void TokenList::append(const string &str) {
	Token *token = new Token(str);
	append(token);
}

//Appends the token to the TokenList if not null
//On return from the function, it will be the last token in the list
void TokenList::append(Token *token) {
	if (!head) {
		head = token;
		tail = token;
	}
	else {
		tail->setNext(token);
		token->setPrev(tail);
		tail = token;
	}
}


//Complete the implementation of the following member functions:
//****Tokenizer class function definitions******

//Computes a new tokenLength for the next token
//Modifies: size_t tokenLength, and bool complete
//(Optionally): may modify offset
//Does NOT modify any other member variable of Tokenizer
void Tokenizer::prepareNextToken()
{
	/*Fill in implementation */
	string token = "";	//The token that we will be appending at the end.
	int firstChar = 0;	//Index of first character
	int lastChar = 0;	//Index of character right after the last character of token

	int checkAlphaNumeral = 0;	//Index of first alphanumeral character, used to check if first character is alphanumeral.

	int checkBitVectorChar = 0; //Index of boxBOX character, to check if we are dealing with bit vector
	int checkBitVectorSymbol = 0; //Index of " character, to check if we are dealing with bit vector

	int checkDoubleQuotes = 0;	//index of first " character, used to check if first character is ".
	int checkSingleQuotes = 0;	//index of first ' character, used to check if first character is '.

	int checkComments = 0; //Index of first - character, used to check if we detect a comment or a normal - operator
	int checkComments2 = 0; //Index of second - character, used to check if we detect a comment

	int checkDelimiter = 0;		//Index of first delimiter character, used to check if first character is delimiter;
	int checkSecondDelimiter = 0;	//Index of second delimiter character, used to check if second character is also part of the delimiter;

	//Run the checks
	firstChar = str->find_first_not_of(" \t\r\n", offset);

	checkAlphaNumeral = str->find_first_of(alphaNumerals, firstChar); //Find first location of alphanumeral character

	checkBitVectorChar = str->find_first_of(bitVectorChar, firstChar); //Find first location of boxBOX characters
	checkBitVectorSymbol = str->find_first_of(bitVectorSymbol, firstChar); //Find first location of " character

	checkDoubleQuotes = str->find_first_of(doubleQuotes, firstChar); //Find first location of "

	checkSingleQuotes = str->find_first_of(singleQuotes, firstChar); //Find first location of '

	checkComments = str->find_first_of(commentDelimiter, firstChar);
	checkComments2 = str->find_first_of(commentDelimiter, firstChar + 1);

	checkDelimiter = str->find_first_of(delimiters, firstChar); //Find first location of delimiter character
	checkSecondDelimiter = str->find_first_of(secondDelimiter, firstChar + 1); //Find second location of delimiter character, if any
	
	if ((offset == -1) || (offset >= (int)str->length()) || firstChar == -1)	//If the offset is str::npos (aka -1), or exceeds the boundaries of the string's length, or we can't find a non-whitespace character, set complete to true.
	{
		complete = true;
	}
	else if (commentFlag)
	{
		lastChar = -1;	//Take the rest of the line as a comment. See @296 comment: http://puu.sh/kWCCw/c9955679d7.png
	}
	else if (firstChar == checkAlphaNumeral) //First character is alphanumeral
	{
		if (firstChar == checkBitVectorChar && firstChar + 1 == checkBitVectorSymbol) //We are dealing with bit vector. Handle appropriately
		{
			lastChar = str->find_first_of(bitVectorSymbol, firstChar + 2)+1;
		}
		else //Normal alphanumeral string
		{
			lastChar = str->find_first_not_of(alphaNumerals, firstChar);
		}
	}
	else if (firstChar == checkSingleQuotes) //First character is ' We check this first to handle ' properly
	{
		checkSingleQuotes = str->find_first_of(singleQuotes, firstChar + 1);			//Find next '
		if (firstChar + 2 == checkSingleQuotes) //If true, we have 'x', where x is a bit literal
		{
			lastChar = checkSingleQuotes + 1;
		}
		else //We have detected that this is not a bit literal, and as such, treat ' as just an attribute and its own token
		{
			lastChar = firstChar + 1;
		}
	}
	else if (firstChar == checkDoubleQuotes) //First character is ". We check this first to handle " properly
	{
		lastChar = str->find_first_of(doubleQuotes, firstChar + 1) + 1;	//+1 because we want to include that last character
	}
	else if (firstChar == checkComments)
	{
		if (firstChar + 1 == checkComments2)	//We have a comment!
		{
			lastChar = checkComments2 + 1;
			commentFlag = true;
		}
		else //Normal - operator, handle like regular delimiter.
		{
			lastChar = firstChar + 1;
		}
	}
	else if (firstChar == checkDelimiter) //First character is delimiter (all other cases)
	{
		if (firstChar + 1 == checkSecondDelimiter) //Check if the character following the first token character is a two character delimiter
		{
			lastChar = checkSecondDelimiter + 1;
		}
		else //We're dealing with single character delimiter.
		{
			lastChar = firstChar + 1; //+1 because we want to include the first character
		}
	}

	if ((lastChar == -1) || (lastChar >= (int)str->length()))	//If lastChar is str::npos (aka -1), or exceeds the boundaries of the string's length
	{
		lastChar = str->length();
	}
	offset = firstChar;
	tokenLength = lastChar - firstChar;


}

//Sets the current string to be tokenized
//Resets all Tokenizer state variables
//Calls Tokenizer::prepareNextToken() as the last statement before returning.
void Tokenizer::setString(string *theStr)
{
	str = theStr; 		//Point to the string to be processed.
	offset = 0;			//Reset offset
	tokenLength = 0; 	//Reset tokenLength
	complete = false;	//Reset complete flag.
	commentFlag = false;	//Reset comment flag.
	
	prepareNextToken();
}

//Returns the next token. Hint: consider the substr function
//Updates the tokenizer state
//Updates offset, resets tokenLength, updates processingABC member variables
//Calls Tokenizer::prepareNextToken() as the last statement before returning.
string Tokenizer::getNextToken()
{
	/*Fill in implementation */ 
	string token = "";
	token = str->substr(offset, tokenLength);
	offset += tokenLength;
	prepareNextToken();
	
	return token;

}



//****Challenge Task Functions******

//Removes the token from the linked list if it is not null
//Deletes the token
//On return from function, head, tail and the prev and next Tokens (in relation to the provided token) may be modified.
void TokenList::deleteToken(Token *token)
{
	/// include case where token points to null one case is made might be able to remove tempNext and tempPrev != NULL in following
	/*Fill in implementation */
	Token *tempNext = NULL;
	Token *tempPrevious = NULL;

	if (token == NULL) //No token 
		return;
	
	tempNext = token->next;
	tempPrevious = token->prev;

	delete token;

	///At this point, we have determined that the list will be empty when we remove this token
	if (tempPrevious != NULL && tempNext == NULL)	//Deleting last node
	{
		tail = tempPrevious; /// set tail to point at remaining node
		tempPrevious->next = NULL;
	}
	else if (tempPrevious == NULL && tempNext != NULL) ///Deleting first node
	{
		head = tempNext; /// set head to point at remaining node
		tempNext->prev = NULL; /// set tail to point at remaining node
	}
	else if (tempPrevious == NULL && tempNext == NULL) //The deleted node was the only node, so set head and tail to NULL
	{
		head = NULL;
		tail = NULL;
	}
	else ///Middle node, with nodes on left and right
	{
		tempPrevious->next = tempNext;
		tempNext->prev = tempPrevious;
	}
	
}

//Removes all comments from the tokenList including the -- marker
//Returns the number of comments removed
int removeComments(TokenList &tokenList)
{
	/*Fill in implementation */
	Token *move = tokenList.getFirst();
	Token *temp = NULL;
	int removed = 0;	//Pairs of -- and comments removed.

	while (move != NULL)
	{
		temp = move;
		if (temp->getStringRep() == "--")
		{
			move = temp->getNext();
			tokenList.deleteToken(temp);	//Delete -- token
			temp = move;
			move = move->getNext();
			if (temp->getStringRep() != "\n") 
			{
				tokenList.deleteToken(temp);	//Delete comment token if not new line. \n is guaranteed and endorsed by TA on @402: http://puu.sh/kZ6KX/96a00c445b.png
			}
			
			removed++;
		}
		else
		{
			move = move->getNext();
		}
	}
	return removed;
}


