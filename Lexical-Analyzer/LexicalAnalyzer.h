#ifndef LEX_H
#define LEX_H

#include <iostream>
#include <fstream>

using namespace std;

enum token_type {NONE = -1, EOF_T, IDENT_T, NUMLIT_T, STRLIT_T,  
	LISTOP_T, CONS_T, IF_T, COND_T, ELSE_T, DISPLAY_T, NEWLINE_T,
	AND_T, OR_T, NOT_T, DEFINE_T, LET_T, NUMBERP_T, LISTP_T, ZEROP_T,
	NULLP_T, STRINGP_T, PLUS_T, MINUS_T, DIV_T, MULT_T, MODULO_T, ROUND_T,
	EQUALTO_T, GT_T, LT_T, GTE_T, LTE_T, LPAREN_T, RPAREN_T, LBRACK_T,
	RBRACK_T, SQUOTE_T, ERROR_T, IDKEY_T,PREDICATE_T, NUM_TOKENS};

class LexicalAnalyzer 
{
    public:
	LexicalAnalyzer (char * filename);
	~LexicalAnalyzer ();
	token_type GetToken ();
	string GetTokenName (token_type t) const;
	string GetLexeme () const;
	void ReportError (const string & msg);
	ofstream debugFile;	// .dbg
    private:
	ifstream input; 	// .ss 
	ofstream listingFile;	// .lst
	ofstream tokenFile;	// .p1
	token_type token;
	string line;
	int linenum;
	int pos;
	string lexeme;
	int errors;
};
	
#endif
