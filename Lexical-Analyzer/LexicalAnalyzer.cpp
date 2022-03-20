/* Author: Andrew McKernan
 * LexicalAnalyzer.cpp: This program reads in a sample file and categorizes the input into token.
 * This program is table driven from a large State Transition Diagram.
 * Date: 10/11/2020
 * I would guess that I spent 15-20 hours on this project.
 */
#include <iomanip>
#include <string>
#include <cstdlib>
#include "LexicalAnalyzer.h"
#include <iostream>


using namespace std;

static string token_names[] = { "EOF_T", "IDENT_T", "NUMLIT_T", "STRLIT_T",
	"LISTOP_T", "CONS_T", "IF_T", "COND_T", "ELSE_T", "DISPLAY_T", "NEWLINE_T",
	"AND_T", "OR_T", "NOT_T", "DEFINE_T", "LET_T", "NUMBERP_T", "LISTP_T",
	"ZEROP_T", "NULLP_T", "STRINGP_T", "PLUS_T", "MINUS_T", "DIV_T", "MULT_T",
	"MODULO_T", "ROUND_T", "EQUALTO_T", "GT_T", "LT_T", "GTE_T", "LTE_T",
	"LPAREN_T", "RPAREN_T", "LBRACK_T", "RBRACK_T", "SQUOTE_T", "ERROR_T", "IDKEY_T", "PREDICATE_T" };

static int backup_table[] = {0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1,
	1, 0,0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0};

		//input decoding
static string input_table[] = { "ws", "/", "*", "=", "(", ")", "'", "[", "]",
	">", "<", "+", "-",	".", "digit", "c", "a",	"d", "r", "alpha",	"?", "_", "\"",	"other"};

static int state_table[][24] = {
	
	//state 1
	{1, -23, -24, -27, -32, -33, -36, -34, -35, 2, 3, 4, 5, 6, 7, 9, 13,
	13, 13, 13, -37, -37, 14, -37},

	//state 2
	{-28, -28, -28, -30, -28,-28, -28, -28, -28, -28, -28, -28,-28,	-28,
	-28, -28,-28, -28, -28, -28, -28,-28, -28, -37},

	//state 3
	{-29, -29, -29, -31, -29,-29, -29, -29, -29, -29, -29, -29,-29,	-29,
	-29, -29,-29, -29, -29, -29, -29,-29, -29, -37},

	//state 4
	{-21, -21,-21, -21, -21, -21, -21, -21, -21,-21,-21, -21, -21, 6, 7,
	-21, -21, -21, -21, -21, -21,-21, -21, -37},

	//state 5
	{-22, -22,-22, -22, -22, -22, -22, -22, -22, -22,-22, -22, -22, 6, 7,
	-22, -22, -22, -22, -22, -22, -22, -22, -37},

	//state 6
	{-37, -37, -37, -37, -37, -37, -37, -37, -37, -37, -37, -37, -37, -37,
	8, -37, -37, -37, -37, -37, -37, -37, -37, -37},

	//state 7
	{-2, -2, -2, -2, -2, -2, -2, -2,-2, -2, -2, -2, -2, 8, 7, -2, -2,
	-2, -2, -2, -2, -2, -2, -37},

	//state 8
	{-2, -2, -2, -2, -2, -2, -2, -2,-2, -2, -2, -2, -2, -2, 8, -2, -2, -2,
	-2, -2, -2, -2, -2, -2},

	//state 9
	{-38,	-38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38,
	13, 13, 10, 11, 13, 13, -38, 13, -38, -37},

	//state 10
	{-38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38,
	13, 13, 13, 10, 15, 13, -38, 13, -38, -37},

	//state 11
	{-38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38,
	13, 13, 12, 11, 15, 13, -37, 13, -38, -37},

	//state 12
	{-38,	-38, -38, -38, -38,	-38, -38, -38, -38, -38, -38, -38, -38, -38,
	13, 13, 13, 13, 15, 13, -38, 13, -38, -37},

	//state 13
	{-38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38, -38,
	13, 13, 13, 13, 13, 13, -39, 13, -38, -38},

	//state 14
	{14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 
	14, 14, 14, 14, -3, 14},
	
	//state 15
	{-4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, 13, 13, 13,
	13, 13, 13, -4, 13, -4, -37 }
	
};

//open files
// This function will initialize the lexical analyzer class
LexicalAnalyzer::LexicalAnalyzer(char* filename)
{
	lexeme = "";
	input.open(filename);
	string lName = "";
	string tName = "";

	for(int i = 0; filename[i] != 's'; i++)
	{
		lName += filename[i];
		tName += filename[i];
	}
	tName += 'p';
	tName += '1';
	tokenFile.open(tName);

	lName += 'l';
	lName += 's';
	lName += 't';
	listingFile.open(lName);

	line = " ";
	pos = 0;
	linenum = 0;
	listingFile << "Input file: " << filename << "\n";

}

//close files
// This function will complete the execution of the lexical analyzer class
LexicalAnalyzer::~LexicalAnalyzer()
{
	listingFile << errors << " errors found in input file\n";
	tokenFile << "EOF_T\t\n";
	input.close();
	tokenFile.close();
	listingFile.close();
}

// This function will find the next lexeme int the input file and return
//
// the token_type value associated with that lexeme
//hard one, similar to nextone
token_type LexicalAnalyzer::GetToken()
{
	int state = 1;
	int loc = 23;
	string symbol = "";
	lexeme = "";
	while( isspace(line[pos]) )
	{
		pos++;
		
		if(pos >= (int)line.length() )
		{
			linenum++;
			getline(input, line);
			if( input.fail())
			{
				return EOF_T; //returns none
			}
			listingFile << "\t" << linenum << ": " << line << "\n";
			line += ' '; //guarentees a space at the end of the line
			pos = 0; //resets to traverse whole line
		}
	}       

		while( state > 0 )
		{
		
			symbol += line[pos];
			//if(line[pos] != ' ')
				lexeme += line[pos];
			
			//formats to comply with input_table
			if (isspace(line[pos])) 
				symbol = "ws"; //if whitespace
			else if (isalpha(line[pos]) && line[pos] != 'c' && line[pos] != 'a' && line[pos] != 'd' && line[pos] != 'r') 
				symbol = "alpha"; //if alpha
			else if (isdigit(line[pos])) 
				symbol = "digit";
			loc = 23;
			//calculates what symbol was read
			for (int i = 0; i < 24; i++)
			{	
				if (symbol == input_table[i])
				{
					loc = i;
					symbol = ""; //reset symboli
			
				}
			}
			
			state = state_table[state - 1][loc];
		
			pos++;
		}
		
		token = token_type(-state);
	
			
		if( token == ERROR_T)
		{
		
			ReportError("Error at " + to_string(linenum) + "," + to_string(pos)
				+ ": Invalid character found: " + line[pos - 1] + "\n");
		}
		
		if(backup_table[token]) //backup for the value we just read
		{
			pos--;
			lexeme.pop_back();
		}		
	
		if(token == PREDICATE_T) //has to be before IDKEY
		{
			if( lexeme == "number?")
				token = NUMBERP_T;
			else if( lexeme == "list?")
				token = LISTP_T;
			else if (lexeme == "zero?")
				token = ZEROP_T;
			else if(lexeme == "null?")
				token = NULLP_T;
			else if (lexeme == "string?")
				token = STRINGP_T;
			else
			{
				token = IDKEY_T;
				pos--;
				lexeme.pop_back();
			}
		}
		if(token == IDKEY_T) //if it is IDKEY_T
		{	
	
			if(lexeme == "cons")
				token = CONS_T;
			else if(lexeme == "if")
				token = IF_T;
			else if(lexeme == "cond")
				token = COND_T;
			else if(lexeme == "else")
				token = ELSE_T;
			else if(lexeme == "display")
				token = DISPLAY_T;
			else if(lexeme == "newline")
				token = NEWLINE_T;
			else if(lexeme == "and")
				token = AND_T;
			else if(lexeme == "or")
				token = OR_T;
			else if(lexeme == "not")
				token = NOT_T;
			else if(lexeme == "define")
				token = DEFINE_T;
			else if(lexeme == "let")
				token = LET_T;
			else if(lexeme == "modulo")
				token = MODULO_T;
			else if(lexeme == "round")
				token = ROUND_T;
			else if( token == IDKEY_T)
				token = IDENT_T;
		}

		
		
		line += ' ';
		
		if( token == ERROR_T && lexeme.length() > 1)//if the erorr is at the end of a valid imput
		{
			lexeme.pop_back();
			pos--;
			if(lexeme.length() == 1) //could probably call gettoken but couldnt get to work
			{
				if(isdigit(lexeme[0]))
					token = NUMLIT_T;
				else if(isalpha(lexeme[0]))
					token = IDENT_T;
				else if(lexeme[0] == '>')
					token = GT_T;
				else if(lexeme[0] == '<')
					token - LT_T;
				else if(lexeme[0] == '=')
					token = EQUALTO_T;
				else if(lexeme[0] == '(')
					token = LPAREN_T;
				else if(lexeme[0] == ')')
					token = RPAREN_T;
				else if(lexeme[0] == '[')
					token = RBRACK_T;
				else if(lexeme[0] == '\'')
					token = SQUOTE_T;
				else if(lexeme[0] == '+')
					token = PLUS_T;
				else if(lexeme[0] == '-')
					token = MINUS_T;
				else if(lexeme[0] == '/')
					token = DIV_T;
				else if(lexeme[0] == '*')
					token = MULT_T;
			}

			

		}
	

		tokenFile << GetTokenName(token) << "\t" << lexeme << "\n";
			
	return token;
}

// The GetTokenName function returns a string containing the name of the
// token passed to it. 
string LexicalAnalyzer::GetTokenName(token_type t) const
{
	return token_names[t];
}

//should be simple
// This function will return the lexeme found by the most recent call to 
// the get_token function
string LexicalAnalyzer::GetLexeme() const
{
	return lexeme;
}

// This function will be called to write an error message to a file
void LexicalAnalyzer::ReportError(const string& msg)
{
	errors++;
	
	listingFile << msg;
}
