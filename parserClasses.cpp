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
#include <cstring>

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
	if (details == NULL) //No memory allocated for tokenDetails struct
	{
		details = new tokenDetails;
		details->type = type;
		details->width = width;
	}
	else //Memory allocated for tokenDetails struct
	{
		details->type = type;
		details->width = width;
	}
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
		//To account for conditional statements list, setting next/prev to NULL to dissociate with previous list
		//Got the OK to modify append from @481: http://puu.sh/lkwKu/717331a7f7.png
		token->setPrev(NULL);
		token->setNext(NULL);
	}
	else {
		tail->setNext(token);
		token->setPrev(tail);
		tail = token;
		//To account for conditional statements list, setting next/prev to NULL to dissociate with previous list
		//Got the OK to modify append from @481: http://puu.sh/lkwKu/717331a7f7.png
		token->setNext(NULL);
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

	int bound1 = 0;					//First bound of a vector, used to determine length of identifier.
	int bound2 = 0;					//Second bound of a vector, used to determine length of identifier.

	string stringRepLower = "";		//The string in the token in all lowercase.
	string prevStringRepLower = "";	//The previous token's string in all lowercase, if it exists.
	string nextStringRepLower = ""; //The next token's (or next-next token's) string in all lowercase, if it exists.
	Token *type = NULL;				//Pointer to the token holding the type, if it exists. Used when checking if it is an identifier or not.
	Token *here = NULL;				//Pointer to a token, used to loop through and check previous tokens to see if there is another identifier with the same name.
	

	//Stage -1: Ignore \n and \r and do not set details.
	if (token->getStringRep() == "\n" || token->getStringRep() == "\r")
		return;

	//Stage 0: Preparing lowercase string
	//Convert stringRep contents to lowercase
	stringRepLower = stringLower(token);

	checkInvalidKeyword = stringRepLower.find_first_of(INVALID_KEYWORD, 0);		//Check for characters that would eliminate token to be keyword

	checkQuote1 = stringRepLower.find_first_of(SINGLE_QUOTES + DOUBLE_QUOTES, 0);	//Check location of first single/double quote.
	checkQuote2 = stringRepLower.find_first_of(SINGLE_QUOTES + DOUBLE_QUOTES, checkQuote1+1);	//Check location of second single/double quote.
	checkNonNumeral = stringRepLower.find_first_not_of(NUMERAL, 0);	//Check location of first non-numeral character


	//Stage 1: Keyword Checker
	//If we don't have invalid keyword characters, continue the check for keyword, else move on.
	if (checkInvalidKeyword == -1)
	{
		for (int ii = 0; ii < 97; ii++)	//Loop to check array of KEYWORDS all in lowercase. Note to self to change 97 to a defined constant later.
		{
			if (stringRepLower == KEYWORDS[ii])
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
	//#2 - Operators
	//Loop to check array of operators all in lowercase. Note to self to change 28to a defined constant later.
	for (int ii = 0; ii < 28; ii++)
	{
		if (stringRepLower == OPERATORS[ii])
		{
			token->type = T_Operator;
			return;
		}
	}

	//#3 - Identifiers
	//Check if first character is alpha, and there are no illegal characters for identifiers
	if (stringRepLower.find_first_of(ALPHA, 0) == 0 && stringRepLower.find_first_of(DELIMITERS, 0) == -1 )
	{
		token->type = T_Identifier;

		//Still a WIP
		if (token->getPrev() != NULL) //Check if previous token exists
		{
			prevStringRepLower = stringLower(token->getPrev());
			//Check previous token for "signal" or "variable"
			//If true, then check the token after the next(aka the one after : )
			if (prevStringRepLower == "signal" || prevStringRepLower == "variable")
			{
				//Assuming valid VHDL at this point, so next token and token after that exists.
				//Take the token after the next (aka the one after :) as type
				nextStringRepLower = stringLower(token->getNext()->getNext());
				//If the type is "std_logic_vector", check the next few tokens to determine width
				if (nextStringRepLower == "std_logic_vector")
				{
					//<type> ( # downto/to # )
					type = token->getNext()->getNext();
					bound1 = stoi(type->getNext()->getNext()->getStringRep());
					bound2 = stoi(type->getNext()->getNext()->getNext()->getNext()->getStringRep());
					token->setTokenDetails(nextStringRepLower, abs(bound1 - bound2) + 1);
					return;
				}
				else
				{
					//<type>
					token->setTokenDetails(nextStringRepLower, 0);
					return;
				}
			}
		}

		//If we haven't returned at this point in this if statement:
		//Check all previous nodes for same identifier name, and copy its contents if we find it.
		here = token->getPrev();
		while (here != NULL)
		{
			prevStringRepLower = stringLower(here);
			if (prevStringRepLower == stringRepLower && here->getTokenDetails() != NULL)
			{
				token->setTokenDetails(here->getTokenDetails()->type, here->getTokenDetails()->width);
				return;
			}
			here = here->getPrev();
		}

		return;
	}
	//#4 - Literals
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

	checkAlphaNumeral = str->find_first_of(ALPHA_NUMERALS, firstChar); //Find first location of alphanumeral character

	checkBitVectorChar = str->find_first_of(BITVECTOR_CHAR, firstChar); //Find first location of boxBOX characters
	checkBitVectorSymbol = str->find_first_of(BITVECTOR_SYMBOL, firstChar); //Find first location of " character

	checkDoubleQuotes = str->find_first_of(DOUBLE_QUOTES, firstChar); //Find first location of "

	checkSingleQuotes = str->find_first_of(SINGLE_QUOTES, firstChar); //Find first location of '

	checkComments = str->find_first_of(COMMENT_DELIMITER, firstChar);
	checkComments2 = str->find_first_of(COMMENT_DELIMITER, firstChar + 1);

	checkDelimiter = str->find_first_of(DELIMITERS, firstChar); //Find first location of delimiter character
	checkSecondDelimiter = str->find_first_of(SECOND_DELIMITER, firstChar + 1); //Find second location of delimiter character, if any
	
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
			lastChar = str->find_first_of(BITVECTOR_SYMBOL, firstChar + 2)+1;
		}
		else //Normal alphanumeral string
		{
			lastChar = str->find_first_not_of(ALPHA_NUMERALS, firstChar);
		}
	}
	else if (firstChar == checkSingleQuotes) //First character is ' We check this first to handle ' properly
	{
		checkSingleQuotes = str->find_first_of(SINGLE_QUOTES, firstChar + 1);			//Find next '
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
		lastChar = str->find_first_of(DOUBLE_QUOTES, firstChar + 1) + 1;	//+1 because we want to include that last character
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
			if (temp != NULL && temp->getTokenType() == T_CommentBody) 
			{
				tokenList.deleteToken(temp);	//Delete comment token token is classified as comment body. \n is no longer guaranteed as mentioned by Eric (TA) in @471
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

//Removes all tokens of the given tokenType
//Returns the number of tokens removed
int removeTokensOfType(TokenList &tokenList, tokenType type)
{
	Token *move = tokenList.getFirst();		//A pointer to iterate through the tokens
	Token *temp = NULL; //Pointer used to delete the token.
	int removed = 0; //Counter for the number of tokens removed
	while (move != NULL)
	{
		temp = move;
		if (temp->getTokenType() == type) //Found matching type
		{
			move = move->getNext();
			tokenList.deleteToken(temp); //Delete the token of matching type
			removed++;
		}
		else
		{
			move = move->getNext();
		}
	}

	return removed;
}

//Creates a new TokenList, and returns a pointer to this list
//Searches for all conditional expressions in tokenList and appends them to the new list
//Format is as follows:
//Each token that is part of a condtional expression is appended sequentially
//At the end of a conditional expression a newline character is appened
//Example: if (a = true) then
//Your list should include "(", "a", "=", "true", ")" and "\n" 
//tokenList is NOT modified
TokenList* findAllConditionalExpressions(const TokenList &tokenList)
{
	TokenList *newList = NULL;	//The tokenlist that will be returned at the end.
	Token *temp = NULL;		//Temporary pointer to a token.
	Token *move = NULL;
	Token *copiedToken = NULL; //New copy of the token, using copy constructor/overloaded assignment operator
	bool conditionalStatement = false;	//bool to keep track of if else, elseif, and then was found before.
	string lowerStringRep = ""; //The lowered string.

	move = tokenList.getFirst(); 
	
	while (move != NULL)
	{
		lowerStringRep = stringLower(move);
		//If we run into if, elsif, or then, toggle conditionalStatement
		if (lowerStringRep == "if" || lowerStringRep == "elsif")
			conditionalStatement = true;
		else if (lowerStringRep == "then")
		{
			if (conditionalStatement == true)
			{
				conditionalStatement = false;
				newList->append("\n");
			}
		}
		else if (lowerStringRep == "end") //Detect end if to avoid conflict with the above if.
		{
			temp = move->getNext();
			lowerStringRep = stringLower(temp);
			if (lowerStringRep == "if") //Make move point to the "if" token so that when we iterate, we skip over it and don't cause undesired results.
			{
				move = temp;
			}
		}
		else if (conditionalStatement)
		{
			if (newList == NULL)
			{
				newList = new TokenList;
			}
			copiedToken = new Token;
			*copiedToken = *move; //Make a copy of the contents of the token via overloaded assignment operator.
			newList->append(copiedToken); //Append the copy of the token to the new list.
		}

		move = move->getNext();
	}
	
	return newList;
}



string stringLower(Token *token)
//Custom helper function: Makes all alpha characters in stringRep of token lowercase, and returns the lowered string. If token is NULL, returns empty string.
{
	string stringRepLower = "";		//The string in the token in all lowercase.
	char *buffer = NULL;			//The string held in a character array.
	
	if (token == NULL) //If NULL token, return empty string.
		return "";

	buffer = new char[token->getStringRep().length() + 1];
	strcpy(buffer, token->getStringRep().c_str());
	for (int ii = 0; ii < (int)token->getStringRep().length(); ii++)
	{
		stringRepLower.push_back((char)tolower(buffer[ii])); //"Pushes" the current lowercased character into the converted lowercase string.
	}

	delete buffer;	//No need for this character array anymore, so delete and set to NULL
	buffer = NULL;
	return stringRepLower;
}
