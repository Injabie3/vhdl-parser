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
#define _CRT_SECURE_NO_WARNINGS

//Use only the following libraries:
#include "parserClasses.h"
#include <string>

//****Token class function definition******

//Default Constructor
Token::Token()
{
	next = NULL;
	prev = NULL;
	stringRep = "";
	details = NULL;
}

//Constructor with string initialization
//Not allocating tokenDetails here.
Token::Token(const string &stringRep)
{
	next = NULL;
	prev = NULL;
	this->stringRep = stringRep;
	details = NULL;	
}

//Copy constructor for Token class.
//Allocate token details accordingly by checking if details member of object is NULL or not.
Token::Token(const Token &token)
{
	next = token.getNext();
	prev = token.getPrev();
	stringRep = token.getStringRep();
	if (token.getTokenDetails() != NULL)
	{
		details = new tokenDetails;
		details->type = token.getTokenDetails()->type;
		details->width = token.getTokenDetails()->width;
	}

	//Check if tokenType should be initialized here.
}

//Overloaded assignment operator for Token class
//Self-note: Everything here should be the same as copy constructor, because it's supposed to do the same thing.
//Allocate token details accordingly by checking if details member of object is NULL or not.
void Token::operator =(const Token& token)
{
	next = token.getNext();
	prev = token.getPrev();
	stringRep = token.getStringRep();
	if (token.getTokenDetails() != NULL)
	{
		details = new tokenDetails;
		details->type = token.getTokenDetails()->type;
		details->width = token.getTokenDetails()->width;
	}

	//Check if tokenType should be initialized here.
}

//Destructor for Token class
Token::~Token()
{
	//Free dynamic memory, if any has been allocated.
	if (details != NULL)
	{
		delete details;
		details = NULL;
	}
}


//New Token class member function: setTokenDetails
void Token::setTokenDetails(const string &type, int width)
{
	return;
}

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

//New TokenList class member function
void TokenList::findAndSetTokenDetails(Token *token)
{
	int checkInvalidKeyword = 0;	//Store index for checking if token contains invalid keyword characters
	int checkDelimiters = 0;		//Store index for checking if token contains delimiter characters
	int checkAlpha = 0;				//Store index for checking the first alpha character.
	int checkNonNumeral = 0;		//Store index of first non-numeral character
	int checkQuote1 = 0;			//Store index for checking first single/double quote.
	int checkQuote2 = 0;			//Store index for checking second single/double quote.

	int bitVectorType = 0;			//Used to tell if bit vector is binary (2), octal (3), or hex (4)

	string stringRepLower = "";		//The string in the token in all lowercase.
	char *buffer = NULL;
	

	//Stage -1: Ignore \n and \r and do not set details.
	if (token->getStringRep() == "\n" || token->getStringRep() == "\r")
		return;

	//Stage 0: Preparing lowercase string
	//Convert stringRep contents to lowercase
	buffer = new char[token->stringRep.length() + 1];
	strcpy(buffer, token->stringRep.c_str());
	for (int ii = 0; ii < (int)token->stringRep.length(); ii++)
	{
		stringRepLower.push_back((char)tolower(buffer[ii])); //"Pushes" the current lowercased character into the converted lowercase string.
	}

	delete buffer;	//No need for this character array anymore, so delete and set to NULL
	buffer = NULL;

	checkInvalidKeyword = stringRepLower.find_first_of(invalidKeyword, 0);		//Check for characters that would eliminate token to be keyword

	checkQuote1 = stringRepLower.find_first_of(singleQuotes+doubleQuotes, 0);	//Check location of first single/double quote.
	checkQuote2 = stringRepLower.find_first_of(singleQuotes + doubleQuotes, checkQuote1+1);	//Check location of second single/double quote.
	checkNonNumeral = stringRepLower.find_first_not_of(numeral, 0);	//Check location of first non-numeral character


	//Stage 1: Keyword Checker
	//If we don't have invalid keyword characters, continue the check for keyword, else move on.
	if (checkInvalidKeyword == -1)
	{
		for (int ii = 0; ii < 97; ii++)	//Loop to check array of keywords all in lowercase. Note to self to change 97 to a defined constant later.
		{
			if (stringRepLower == keywords[ii])
			{
				token->_isKeyword = true;
				break;
			}
		}
	}


	//Stage 2: Categorizing the token
	//#1 - Comments
	if (token->prev != NULL)
	{
		//Current token is a comment if the previous token is -- and this token is not a new line
		//NOTE: Check if tolower affects \n and \r and if it doesn't change below to the local version.
		if (token->prev->getStringRep() == "--" && token->stringRep != "\n" && token->stringRep != "\r")
		{
			token->type = T_CommentBody;
			return;
		}
	}

	//#2 - Identifiers
	//Check if first character is alpha, and there are no illegal characters for identifiers
	if (stringRepLower.find_first_of(alpha, 0) == 0 && stringRepLower.find_first_of(delimiters, 0) == -1 )
	{
		token->type = T_Identifier;

		//Still a WIP
		return;
	}
	//#3 - Literals
	//Check for two instances of '' or "", or if token contains numerals only, or true false
	else if ((checkQuote1 != -1 && checkQuote2 != -1) || checkNonNumeral == -1 || stringRepLower == "false" || stringRepLower == "true")
	{
		token->type = T_Literal;
		if (stringRepLower == "false" || stringRepLower == "true") //Type is boolean
		{
			//Set token details using the class public function
			token->setTokenDetails("boolean", 0);
		}
		else if (checkQuote1 == 0)	//First character is " or '
		{
			if (stringRepLower[0] == '\'') //std_logic
				token->setTokenDetails("std_logic", 0);
			else //std_logic_vector, with the width being the difference between the two locations of the quotes
				token->setTokenDetails("std_logic_vector", checkQuote2 - checkQuote1-1);
		}
		else if (checkNonNumeral == -1) //Integer
		{
			token->setTokenDetails("integer", 0);
		}
		else //Bit vectors - std_logic_vector
		{
			
			if (stringRepLower[0] == 'b') //Binary
				bitVectorType = 2;
			else if (stringRepLower[0] == 'o') //Octal
				bitVectorType = 3;
			else bitVectorType = 4;
			token->setTokenDetails("std_logic_vector", bitVectorType*(checkQuote2 - checkQuote1-1));
		}
		return;
	}
	//#4 - Operators
	//Loop to check array of operators all in lowercase. Note to self to change 28to a defined constant later.
	for (int ii = 0; ii < 28; ii++)	
	{
		if (stringRepLower == operators[ii])
		{
			token->type = T_Operator;
			token->setTokenDetails("operator", 0);
			return;
		}
	}

	//#5 - Everything else: We can't find anything matching by this point, so set type to other.
	token->type = T_Other;
	return;

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


