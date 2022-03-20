/*******************************************************************************
* File name: SyntacticalAnalyzer.cpp                                           *
* Project: CS 460 Project 2 Fall 2020                                          *
* Author: Andrew McKernan                                                      *
* Date: 11/30/20                                                               *
* Description: This file contains an assortment of functions for syntacticaly  *
*				proccesing a scheme file,                                      *
*******************************************************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include "SyntacticalAnalyzer.h"
#include <string>


using namespace std;

SyntacticalAnalyzer::SyntacticalAnalyzer (char * filename)
{
	lex = new LexicalAnalyzer (filename);
	t = lex->GetToken();
	cg = new CodeGenerator(filename, lex);
	string name = filename;
	name = name.substr(0, name.length() - 2);
	name += "p2";
	p2file.open(name);
	numTabs = 0;
	int totalErrors = program ();
}


SyntacticalAnalyzer::~SyntacticalAnalyzer ()
{
	delete lex;
	delete cg;
}

int SyntacticalAnalyzer::program()
{
	int errors = 0;
	stringstream msg;
	
	if(t != LPAREN_T && t != EOF_T)
	{
		errors++;
		msg << "'" << lex->GetLexeme() << "' unexpected";
		lex->ReportError(msg.str());
	}
	while(t != LPAREN_T && t != EOF_T) //advances till we get a valid first
	{
		t = lex->GetToken();
	}
	
	p2file << "Using Rule 1\n";
	if(t == LPAREN_T)
	{
		t = lex->GetToken();
		errors += define();
		if(t == LPAREN_T)
			t = lex->GetToken();
		else
		{
			lex->ReportError("'(' expected");
			errors++;
		}
		errors += more_defines();
		if(t == EOF_T)
			t = lex->GetToken();
		else
		{
			lex->ReportError("End of file expected");
			errors++;
		}
	}
	else
	{
		lex->ReportError("'(' expected");
		errors++;
	}
	
	
	return errors;
}

int SyntacticalAnalyzer::more_defines()
{
	int errors = 0;
	stringstream msg;
	
	if(t != DEFINE_T && t != IDENT_T && t != EOF_T)
	{
		errors++;
		msg << "'" << lex->GetLexeme() << "' unexpected";
		lex->ReportError(msg.str());
	}
	while(t != DEFINE_T && t != IDENT_T && t != EOF_T) //advances till we get a valid first
	{
		t = lex->GetToken();
	}
	
	if( t == IDENT_T )
	{
		p2file << "Using Rule 3\n";
		t = lex->GetToken();
		errors += stmt_list();
		
		if(t == RPAREN_T)
			t = lex->GetToken();
		else
		{
			errors++;
			lex->ReportError("')' expected");
		}
	}
	
	else if (t == DEFINE_T) 
	{
		p2file << "Using Rule 2\n";
		errors += define();
		
		if( t == LPAREN_T )
			t = lex->GetToken();
		else
		{
			errors++;
			lex->ReportError("'(' expected");
		}
		
		errors += more_defines();
	}
	else
	{
		errors++;
		msg << "'" << lex->GetLexeme() << "' unexpected";
		lex->ReportError(msg.str());
	}
	while( t != EOF_T) //advances till we get a vaild follow
	{
		t = lex->GetToken();
	}
	return errors;
}

int SyntacticalAnalyzer::define()
{
	int errors = 0;
	bool main;
	stringstream msg;
	
	if(t != DEFINE_T && t != EOF_T)
	{
		errors++;
		msg << "'" << lex->GetLexeme() << "' unexpected";
		lex->ReportError(msg.str());
	}
	while(t != DEFINE_T && t != EOF_T) //advances till we get a valid first
	{
		t = lex->GetToken();
	}
	
	p2file << "Using Rule 4\n";
	if(t == DEFINE_T)
	{
		t = lex->GetToken();
			
		if(t == LPAREN_T)
		{
			t = lex->GetToken();
	
			if(t == IDENT_T)
			{
				if( lex->GetLexeme().compare("main") == 0 )
				{
					cg->WriteCode(numTabs, "int main(");
					main = true;
				}
				else
				{
					main = false;
					cg->WriteCode(numTabs, "Object " + lex->GetLexeme() + "(");
				}
					
				t = lex->GetToken();
			}
			else
			{
				lex->ReportError("Identity expected");
				errors++;
			}
		}
		else
		{
			lex->ReportError("'(' expected");
			errors++;
		}
	}
	else
	{
		errors++;
		lex->ReportError("Keyword 'Define' expected");
	}	

	errors += param_list();
	cg->WriteCode(numTabs, ")\n{\n");
	numTabs++;
	
	cg->WriteCode(numTabs, "Object __RetVal;\n");
		
	if(t == RPAREN_T)
		t = lex->GetToken();
	else
	{
		lex->ReportError("')' expected");
		errors++;
	}
	
	if(t == NUMLIT_T || t == STRLIT_T || t == SQUOTE_T)
	{ 		
		errors += stmt_assign();	
	}
	else if( t == LPAREN_T)
	{		
			errors += stmt_assign();	
	}
	else
	{
		errors += stmt();	
		errors += stmt_list();
	}
		
		errors += stmt_list();
	
	
	if(main)
		cg->WriteCode(numTabs, "return 0;\n");
	else
		cg->WriteCode(numTabs, "return __RetVal;\n");
	
	numTabs--;
	cg->WriteCode(numTabs,"}\n");
	
	if(t == RPAREN_T)
		t = lex->GetToken();
	else
	{
		lex->ReportError("')' expected");
		errors++;
	}
	
	if( t != LPAREN_T && t != EOF_T)
	{
		errors++;
		msg << "'" << lex->GetLexeme() << "' unexpected";
		lex->ReportError(msg.str());
	}	
	while( t != LPAREN_T && t != EOF_T) //advances till we get a vaild follow
	{
		t = lex->GetToken();
	}
	
	
	return errors;
}

int SyntacticalAnalyzer::stmt_list() //could posibly cause errors if lamdba is a stmt
{
	int errors = 0;
	
	//if  t is the start of a stmt
	if( t == IDENT_T || t == LPAREN_T || t == NUMLIT_T || t == STRLIT_T || t == SQUOTE_T)
	{
		p2file << "Using Rule 5\n";
		errors += stmt();
		errors += stmt_list();
	}
	else //lamdba rule
		p2file << "Using Rule 6\n";
	return errors;
}

int SyntacticalAnalyzer::stmt_list(string op) //could posibly cause errors if lamdba is a stmt
{
	int errors = 0;
	
	//if  t is the start of a stmt
	if( t == IDENT_T || t == LPAREN_T || t == NUMLIT_T || t == STRLIT_T || t == SQUOTE_T)
	{
		p2file << "Using Rule 5\n";
		
		cg->WriteCode(0, op);
		
		
		errors += stmt();
		errors += stmt_list(op);
	}
	else //lamdba rule
		p2file << "Using Rule 6\n";
	return errors;
}

int SyntacticalAnalyzer::stmt()
{
	int errors = 0;
	stringstream msg;
	
	if( t == IDENT_T)
	{	
		cg->WriteCode(0, "Object(" + lex->GetLexeme() + ")");
		p2file << "Using Rule 8\n";		
		t = lex->GetToken();		
	}
	else if( t == LPAREN_T)
	{
		
		p2file << "Using Rule 9\n";
		t = lex->GetToken();		
		errors += action();
		//cg->WriteCode(0,";\n");
		
		
		if(t == RPAREN_T)
			t = lex->GetToken();
		else
		{
			errors++;
			lex->ReportError("')' expected");
		}
		
	}
	else if(t == NUMLIT_T || t == STRLIT_T || t == SQUOTE_T)
	{
		
		p2file << "Using Rule 7\n";
		errors += literal();
	}
	else
	{
		errors++;
		msg << "'" << lex->GetLexeme() << "' unexpected";
		lex->ReportError(msg.str());
	}
	while( t != RBRACK_T && t != RPAREN_T && t != IDENT_T && t != LPAREN_T && t != NUMLIT_T && t != STRLIT_T && t != SQUOTE_T && t != EOF_T)
	{
		t = lex->GetToken();
	}
	
	return errors;
}

int SyntacticalAnalyzer::stmt_assign()
{
	int errors = 0;
	stringstream msg;
	
	if( t == IDENT_T)
	{	
		cg->WriteCode(0, "Object(" + lex->GetLexeme() + ")");
		p2file << "Using Rule 8\n";		
		t = lex->GetToken();		
	}
	else if( t == LPAREN_T)
	{
		
		p2file << "Using Rule 9\n";
		t = lex->GetToken();
		
		
		
		if(t != DISPLAY_T && t != NEWLINE_T && t != IF_T && t != COND_T && t != LET_T )
		{
			cg->WriteCode(numTabs, "__RetVal = ");
			errors += action_assign();
			cg->WriteCode(0, ";\n");
		}
		else
		{
			errors += action();
			//cg->WriteCode(0,";\n");
		}
		
		
		if(t == RPAREN_T)
			t = lex->GetToken();
		else
		{
			errors++;
			lex->ReportError("')' expected");
		}
	}
	else if(t == NUMLIT_T || t == STRLIT_T || t == SQUOTE_T)
	{
		
		p2file << "Using Rule 7\n";
		cg->WriteCode(numTabs, "__RetVal = ");	
		errors += literal();
		cg->WriteCode(0, ";\n");
	}
	else
	{
		errors++;
		msg << "'" << lex->GetLexeme() << "' unexpected";
		lex->ReportError(msg.str());
	}
	

	while( t != RBRACK_T && t != RPAREN_T && t != IDENT_T && t != LPAREN_T && t != NUMLIT_T && t != STRLIT_T && t != SQUOTE_T && t != EOF_T)
	{
		t = lex->GetToken();
	}
		
	return errors;
}

int SyntacticalAnalyzer::stmt_assign(string var)
{
	int errors = 0;
	stringstream msg;
	
	if( t == IDENT_T)
	{	
		cg->WriteCode(0, "Object(" + lex->GetLexeme() + ")");
		p2file << "Using Rule 8\n";		
		t = lex->GetToken();		
	}
	else if( t == LPAREN_T)
	{
		
		p2file << "Using Rule 9\n";
		t = lex->GetToken();
		if(t != DISPLAY_T && t != NEWLINE_T && t != IF_T && t != COND_T && t != LET_T)
		{
			cg->WriteCode(numTabs, var);
			errors += action();
			cg->WriteCode(0, ";\n");
		}
		else if(t == IDENT_T)
		{
			errors += action();
			cg->WriteCode(0, ");\n");
		}
		
		else
		{
			errors += action();
			//cg->WriteCode(0,";\n");
		}
		
		
		if(t == RPAREN_T)
			t = lex->GetToken();
		else
		{
			errors++;
			lex->ReportError("')' expected");
		}
	}
	else if(t == NUMLIT_T || t == STRLIT_T || t == SQUOTE_T)
	{
		
		p2file << "Using Rule 7\n";
		cg->WriteCode(numTabs, var);	
		errors += literal();
		cg->WriteCode(0, ";\n");
	}
	else
	{
		errors++;
		msg << "'" << lex->GetLexeme() << "' unexpected";
		lex->ReportError(msg.str());
	}
	while( t != RBRACK_T && t != RPAREN_T && t != IDENT_T && t != LPAREN_T && t != NUMLIT_T && t != STRLIT_T && t != SQUOTE_T && t != EOF_T)
	{
		t = lex->GetToken();
	}
		
	return errors;
}

int SyntacticalAnalyzer::literal()
{
	int errors = 0;
	stringstream msg;
	
	if(t == NUMLIT_T)
	{
		p2file << "Using Rule 10\n";
		cg->WriteCode(0, "Object(" + lex->GetLexeme() + ")");
		t = lex->GetToken();
	}
	else if( t == STRLIT_T )
	{
		p2file << "Using Rule 11\n";
		cg->WriteCode(0, "Object(" + lex->GetLexeme() + ")");
		t = lex->GetToken();
	}
	else if( t == SQUOTE_T )
	{
		p2file << "Using Rule 12\n";
		
		t = lex->GetToken();
		if(t == LPAREN_T)
			cg->WriteCode(0, "Object(\"");
		else
			cg->WriteCode(0, "Object(\"");
		errors += quoted_lit();
	}
	else
	{
		errors++;
		msg << "'" << lex->GetLexeme() << "' unexpected";
		lex->ReportError(msg.str());
	}
	while( t != RBRACK_T && t != RPAREN_T && t != IDENT_T && t != LPAREN_T && t != NUMLIT_T && t != STRLIT_T && t != SQUOTE_T && t != EOF_T)
	{
		t = lex->GetToken();
	}
	return errors;
}

int SyntacticalAnalyzer::quoted_lit()
{
	int errors = 0;
	p2file << "Using Rule 13\n";
	
	errors += any_other_token();
	cg->WriteCode(0, "\")");
	return errors;
}

int SyntacticalAnalyzer::more_tokens()
{
	int errors = 0;
	
	//if t starts with any_other_t
	if( t == LPAREN_T || t ==  IDENT_T || t == NUMLIT_T || t == STRLIT_T
		|| t == IF_T || t == DISPLAY_T || t == NEWLINE_T || t == LISTOP1_T
		|| t == LISTOP2_T || t == AND_T || t == OR_T || t == NOT_T
		|| t == DEFINE_T || t == LET_T || t == NUMBERP_T || t == LISTP_T
		|| t == ZEROP_T || t == NULLP_T || t == STRINGP_T || t == PLUS_T
		|| t == MINUS_T || t == DIV_T || t == MULT_T || t == MODULO_T
		|| t == ROUND_T || t == EQUALTO_T || t == GT_T || t == LT_T
		|| t == GTE_T || t == LTE_T || t == SQUOTE_T || t == COND_T || t == ELSE_T)
	{
		p2file << "Using Rule 14\n";
		errors += any_other_token();
		errors += more_tokens();
	}
	else //lamdba rule	
		p2file << "Using Rule 15\n";
	
	return errors;
}

int SyntacticalAnalyzer::param_list() //may never return errors
{
	int errors = 0;
	
	if(t == IDENT_T)
	{
	
		p2file << "Using Rule 16\n";
		cg->WriteCode(0, "Object " + lex->GetLexeme());
		t = lex->GetToken();	
		if( t == IDENT_T)
			cg->WriteCode(0, ", ");
		
		errors += param_list();
	}
	else //lamdba rule
	{
	
		p2file << "Using Rule 17\n";
	}
		
	return errors;
}

int SyntacticalAnalyzer::else_part()
{
	int errors = 0;
	//if  t is the start of a stmt
	if( t == IDENT_T || t == LPAREN_T || t == NUMLIT_T || t == STRLIT_T || t == SQUOTE_T)
	{
		p2file << "Using Rule 18\n";
		cg->WriteCode(numTabs, "else\n");
		cg->WriteCode(numTabs, "{\n");
		numTabs++;	
		errors += stmt_assign();		
		numTabs--;
		cg->WriteCode(numTabs, "}\n");
	}
	else //lamdba rule
		p2file << "Using Rule 19\n";
	return errors;
}

int SyntacticalAnalyzer::stmt_pair()
{
	int errors = 0;
	
	if(t == LPAREN_T)
	{
		t = lex->GetToken();
		p2file << "Using Rule 20\n";
		errors += stmt_pair_body();
	}
	else
		p2file << "Using Rule 21\n";
	
	return errors;
}

int SyntacticalAnalyzer:: stmt_pair_body()
{
	int errors = 0;
	stringstream msg;
	if(t == ELSE_T)
	{
		p2file << "Using Rule 23\n";
		t = lex->GetToken();
		cg->WriteCode(numTabs, "else\n");
		cg->WriteCode(numTabs, "{\n");
		numTabs++;
		errors += stmt();
		numTabs--;
		cg->WriteCode(numTabs, "}\n");
		if(t == RPAREN_T)
			t = lex->GetToken();
		else
		{
			errors++;
			lex->ReportError("')' expected");
		}
	}
	// else if its the start of a statement
	else if( t == IDENT_T || t == LPAREN_T || t == NUMLIT_T || t == STRLIT_T || t == SQUOTE_T)
	{
		p2file << "Using Rule 22\n";
		
		cg->WriteCode(numTabs, "else if( ");
		errors += stmt();
		cg->WriteCode(0, ")\n");
		cg->WriteCode(numTabs, "{\n");
		numTabs++;
		errors += stmt_assign();
		numTabs--;
		cg->WriteCode(numTabs, "}\n");
		
		
		if(t == RPAREN_T)
		{
			t = lex->GetToken();
			errors += stmt_pair();
		}
		else
		{
			errors++;
			lex->ReportError("')' expected");
		}
		
	}
	else //if invalid token
	{
		errors++;
		msg << "'" << lex->GetLexeme() << "' unexpected";
		lex->ReportError(msg.str());
	}
		
		

	return errors;
}

int SyntacticalAnalyzer:: stmt_pair_body_first()
{
	int errors = 0;
	stringstream msg;
	if(t == ELSE_T)
	{
		p2file << "Using Rule 23\n";
		t = lex->GetToken();
		cg->WriteCode(numTabs, "else\n");
		numTabs++;
		errors += stmt();
		numTabs--;
		if(t == RPAREN_T)
			t = lex->GetToken();
		else
		{
			errors++;
			lex->ReportError("')' expected");
		}
	}
	// else if its the start of a statement
	else if( t == IDENT_T || t == LPAREN_T || t == NUMLIT_T || t == STRLIT_T || t == SQUOTE_T)
	{
		p2file << "Using Rule 22\n";
		
		cg->WriteCode(numTabs, "if( ");
		errors += stmt();
		cg->WriteCode(0, ")\n");
		cg->WriteCode(numTabs, "{\n");
		numTabs++;
		errors += stmt_assign();
		numTabs--;
		cg->WriteCode(numTabs, "}\n");
		
		if(t == RPAREN_T)
		{
			t = lex->GetToken();
			errors += stmt_pair();
		}
		else
		{
			errors++;
			lex->ReportError("')' expected");
		}
		
	}
	else //if invalid token
	{
		errors++;
		msg << "'" << lex->GetLexeme() << "' unexpected";
		lex->ReportError(msg.str());
	}
		
		

	return errors;
}

int SyntacticalAnalyzer::assign_pair()
{
	int errors = 0;
	p2file << "Using Rule 24\n";
	
	if( t == LBRACK_T)
	{
		t = lex->GetToken();
		if(t == IDENT_T)
		{
			cg->WriteCode(numTabs, "Object " + lex->GetLexeme() + " = ");
			t = lex->GetToken();			
			errors += stmt();
			cg->WriteCode(0, ";\n");
			if(t == RBRACK_T)
			{
				t = lex->GetToken();
			}
			else
			{
				errors++;
				lex->ReportError("']' expected");
			}
		}
		else
		{
			errors++;
			lex->ReportError("Identity expected");
		}
	}
	else
	{
		errors++;
		lex->ReportError("'[' expected");
	}
	
	return errors;
}

int SyntacticalAnalyzer::more_assigns()
{
	int errors = 0;
	stringstream msg;
	//if token starts with assign_pair
	if(t == LBRACK_T)
	{
		p2file << "Using Rule 25\n";
		errors += assign_pair();
		errors += more_assigns();
	}
	else //lambda rule
	{
		p2file << "Using Rule 26\n";
	}
	
	return errors;
}

int SyntacticalAnalyzer::action()
{
	int errors = 0;
	stringstream msg;

	if( t == IF_T)
	{
		p2file << "Using Rule 27\n";
		t = lex->GetToken();
		cg->WriteCode(numTabs, "if(");
		errors += stmt();
		cg->WriteCode(0, ")\n");
		cg->WriteCode(numTabs, "{\n");
		numTabs++;	
		errors += stmt_assign();
		numTabs--;
		cg->WriteCode(numTabs, "\n}\n");
		
		errors += else_part();
		
	}
	else if(t == COND_T)
	{
		p2file << "Using Rule 28\n";
		t = lex->GetToken();
		if(t == LPAREN_T)
		{
			t = lex->GetToken();
			errors += stmt_pair_body_first();
		}
		else
		{
			errors++;
			lex->ReportError("'(' expected");
		}
		
	}
	else if(t == LET_T)
	{
		p2file << "Using Rule 29\n";
		t = lex->GetToken();
		if(t == LPAREN_T)
		{
			t = lex->GetToken();
			errors += more_assigns();
			if(t == RPAREN_T)
			{
				t = lex->GetToken();
				errors += stmt_assign();
				errors += stmt_list();
			}
			else
			{
				errors++;
				lex->ReportError("')' expected");
			}
		}
		else
		{
			errors++;
			lex->ReportError("'(' expected");
		}
	}
	else if(t == LISTOP1_T)
	{
		p2file << "Using Rule 30\n";
		cg->WriteCode(0, "listop(\"" + lex->GetLexeme() + "\", ");
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, ")");
	}
	else if(t == LISTOP2_T)
	{
		p2file << "Using Rule 31\n";
		cg->WriteCode(0, "listop(\"" + lex->GetLexeme() + "\", ");
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, ", ");
		errors += stmt();
		cg->WriteCode(0, ")");
	}
	else if(t == AND_T)
	{
		p2file << "Using Rule 32\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, " && ");
		errors += stmt();
	}
	else if(t == OR_T)
	{
		p2file << "Using Rule 33\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, " || ");
		errors += stmt();
	}
	else if(t == NOT_T)
	{
		p2file << "Using Rule 34\n";
		t = lex->GetToken();
		cg->WriteCode(0, "! ");
		errors += stmt();
	}
	else if(t == NUMBERP_T)
	{
		p2file << "Using Rule 35\n";
		t = lex->GetToken();
		cg->WriteCode(0, "numberp(");
		errors += stmt();
		cg->WriteCode(0,")");
	}
	else if(t == LISTP_T)
	{
		p2file << "Using Rule 36\n";
		t = lex->GetToken();
		cg->WriteCode(0, "listp(");
		errors += stmt();
		cg->WriteCode(0,")");
	}
	else if(t == ZEROP_T)
	{
		p2file << "Using Rule 37\n";
		t = lex->GetToken();
		cg->WriteCode(0, "zerop(");
		errors += stmt();
		cg->WriteCode(0,")");
	}
	else if(t == NULLP_T)
	{
		p2file << "Using Rule 38\n";
		t = lex->GetToken();
		cg->WriteCode(0, "nullp(");
		errors += stmt();
		cg->WriteCode(0,")");
	}
	else if(t == STRINGP_T)
	{
		p2file << "Using Rule 39\n";
		t = lex->GetToken();
		cg->WriteCode(0, "stringp(");
		errors += stmt();
		cg->WriteCode(0,")");
	}
	else if(t == PLUS_T)
	{	
		
		p2file << "Using Rule 40\n";
		t = lex->GetToken();	
		cg->WriteCode(0, "(");
		errors += stmt();
		errors += stmt_list(" + ");
		cg->WriteCode(0, ")");
	}
	else if(t == MINUS_T)
	{
		p2file << "Using Rule 41\n";
		t = lex->GetToken();
		cg->WriteCode(0, "(");
		errors += stmt();
		errors += stmt_list(" - ");
		cg->WriteCode(0, ")");
	}
	else if(t == DIV_T)
	{
		p2file << "Using Rule 42\n";
		t = lex->GetToken();
		cg->WriteCode(0, "(");
		errors += stmt();
		errors += stmt_list(" / ");
		cg->WriteCode(0, ")");
	}
	else if(t == MULT_T)
	{
		p2file << "Using Rule 43\n";
		t = lex->GetToken();	
		cg->WriteCode(0, "(");
		errors += stmt();
		errors += stmt_list(" * ");
		cg->WriteCode(0, ")");
	}
	else if(t == MODULO_T)
	{
		p2file << "Using Rule 44\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, " % ");
		errors += stmt();
	}
	else if(t == ROUND_T)
	{
		p2file << "Using Rule 45\n";
		t = lex->GetToken();
		cg->WriteCode(0, "round(");
		errors += stmt();
		cg->WriteCode(0, ")");
	}
	else if(t == EQUALTO_T)
	{
		p2file << "Using Rule 46\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, " == ");
		errors += stmt();
	}
	else if(t == GT_T)
	{
		p2file << "Using Rule 47\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, " > ");
		errors += stmt();
	}
	else if(t == LT_T)
	{
		p2file << "Using Rule 48\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, " < ");
		errors += stmt();
	}
	else if(t == GTE_T)
	{
		p2file << "Using Rule 49\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, " >= ");
		errors += stmt();
	}
	else if(t == LTE_T)
	{
		p2file << "Using Rule 50\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, " <= ");
		errors += stmt();
	}
	else if(t == IDENT_T)
	{
		p2file << "Using Rule 51\n";
		cg->WriteCode(numTabs, lex->GetLexeme() + "(");
		t = lex->GetToken();
		errors += stmt();
		errors += stmt_list(", ");
		cg->WriteCode(0, ");\n");//here
	}
	else if(t == DISPLAY_T)
	{
		cg->WriteCode(numTabs, "cout << ");
		p2file << "Using Rule 52\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0,";\n");//here
	}
	else if(t == NEWLINE_T)
	{
		cg->WriteCode(numTabs, "cout << endl;\n");
		p2file << "Using Rule 53\n";
		t = lex->GetToken();
	}
	else //not sure which rule this should return since its invalid
	{
		errors++;
		msg << "'" << lex->GetLexeme() << "' unexpected";
		lex->ReportError(msg.str());
	}
	
	
	return errors;
}

int SyntacticalAnalyzer::action_assign()
{
	int errors = 0;
	stringstream msg;

	if( t == IF_T)
	{
		p2file << "Using Rule 27\n";
		t = lex->GetToken();
		cg->WriteCode(numTabs, "if(");
		errors += stmt();
		cg->WriteCode(0, ")\n");
		cg->WriteCode(numTabs, "{\n");
		numTabs++;	
		errors += stmt_assign();
		numTabs--;
		cg->WriteCode(numTabs, "\n}\n");
		
		errors += else_part();
		
	}
	else if(t == COND_T)
	{
		p2file << "Using Rule 28\n";
		t = lex->GetToken();
		if(t == LPAREN_T)
		{
			t = lex->GetToken();
			errors += stmt_pair_body_first();
		}
		else
		{
			errors++;
			lex->ReportError("'(' expected");
		}
		
	}
	else if(t == LET_T)
	{
		p2file << "Using Rule 29\n";
		t = lex->GetToken();
		if(t == LPAREN_T)
		{
			t = lex->GetToken();
			errors += more_assigns();
			if(t == RPAREN_T)
			{
				t = lex->GetToken();
				errors += stmt_assign();
				errors += stmt_list();
			}
			else
			{
				errors++;
				lex->ReportError("')' expected");
			}
		}
		else
		{
			errors++;
			lex->ReportError("'(' expected");
		}
	}
	else if(t == LISTOP1_T)
	{
		p2file << "Using Rule 30\n";
		cg->WriteCode(0, "listop(\"" + lex->GetLexeme() + "\", ");
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, ")");
	}
	else if(t == LISTOP2_T)
	{
		p2file << "Using Rule 31\n";
		cg->WriteCode(0, "listop(\"" + lex->GetLexeme() + "\", ");
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, ", ");
		errors += stmt();
		cg->WriteCode(0, ")");
	}
	else if(t == AND_T)
	{
		p2file << "Using Rule 32\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, " && ");
		errors += stmt();
	}
	else if(t == OR_T)
	{
		p2file << "Using Rule 33\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, " || ");
		errors += stmt();
	}
	else if(t == NOT_T)
	{
		p2file << "Using Rule 34\n";
		t = lex->GetToken();
		cg->WriteCode(0, "! ");
		errors += stmt();
	}
	else if(t == NUMBERP_T)
	{
		p2file << "Using Rule 35\n";
		t = lex->GetToken();
		cg->WriteCode(0, "numberp(");
		errors += stmt();
		cg->WriteCode(0,")");
	}
	else if(t == LISTP_T)
	{
		p2file << "Using Rule 36\n";
		t = lex->GetToken();
		cg->WriteCode(0, "listp(");
		errors += stmt();
		cg->WriteCode(0,")");
	}
	else if(t == ZEROP_T)
	{
		p2file << "Using Rule 37\n";
		t = lex->GetToken();
		cg->WriteCode(0, "zerop(");
		errors += stmt();
		cg->WriteCode(0,")");
	}
	else if(t == NULLP_T)
	{
		p2file << "Using Rule 38\n";
		t = lex->GetToken();
		cg->WriteCode(0, "nullp(");
		errors += stmt();
		cg->WriteCode(0,")");
	}
	else if(t == STRINGP_T)
	{
		p2file << "Using Rule 39\n";
		t = lex->GetToken();
		cg->WriteCode(0, "stringp(");
		errors += stmt();
		cg->WriteCode(0,")");
	}
	else if(t == PLUS_T)
	{	
		
		p2file << "Using Rule 40\n";
		t = lex->GetToken();	
		cg->WriteCode(0, "(");
		errors += stmt();
		errors += stmt_list(" + ");
		cg->WriteCode(0, ")");
	}
	else if(t == MINUS_T)
	{
		p2file << "Using Rule 41\n";
		t = lex->GetToken();
		cg->WriteCode(0, "(");
		errors += stmt();
		errors += stmt_list(" - ");
		cg->WriteCode(0, ")");
	}
	else if(t == DIV_T)
	{
		p2file << "Using Rule 42\n";
		t = lex->GetToken();
		cg->WriteCode(0, "(");
		errors += stmt();
		errors += stmt_list(" / ");
		cg->WriteCode(0, ")");
	}
	else if(t == MULT_T)
	{
		p2file << "Using Rule 43\n";
		t = lex->GetToken();	
		cg->WriteCode(0, "(");
		errors += stmt();
		errors += stmt_list(" * ");
		cg->WriteCode(0, ")");
	}
	else if(t == MODULO_T)
	{
		p2file << "Using Rule 44\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, " % ");
		errors += stmt();
	}
	else if(t == ROUND_T)
	{
		p2file << "Using Rule 45\n";
		t = lex->GetToken();
		cg->WriteCode(0, "round(");
		errors += stmt();
		cg->WriteCode(0, ")");
	}
	else if(t == EQUALTO_T)
	{
		p2file << "Using Rule 46\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, " == ");
		errors += stmt();
	}
	else if(t == GT_T)
	{
		p2file << "Using Rule 47\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, " > ");
		errors += stmt();
	}
	else if(t == LT_T)
	{
		p2file << "Using Rule 48\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, " < ");
		errors += stmt();
	}
	else if(t == GTE_T)
	{
		p2file << "Using Rule 49\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, " >= ");
		errors += stmt();
	}
	else if(t == LTE_T)
	{
		p2file << "Using Rule 50\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0, " <= ");
		errors += stmt();
	}
	else if(t == IDENT_T)
	{
		p2file << "Using Rule 51\n";
		cg->WriteCode(numTabs, lex->GetLexeme() + "(");
		t = lex->GetToken();
		errors += stmt();
		errors += stmt_list(", ");
		cg->WriteCode(0, ")");//here
	}
	else if(t == DISPLAY_T)
	{
		cg->WriteCode(numTabs, "cout << ");
		p2file << "Using Rule 52\n";
		t = lex->GetToken();
		errors += stmt();
		cg->WriteCode(0,";\n");//here
	}
	else if(t == NEWLINE_T)
	{
		cg->WriteCode(numTabs, "cout << endl;\n");
		p2file << "Using Rule 53\n";
		t = lex->GetToken();
	}
	else //not sure which rule this should return since its invalid
	{
		errors++;
		msg << "'" << lex->GetLexeme() << "' unexpected";
		lex->ReportError(msg.str());
	}
	
	
	return errors;
}

int SyntacticalAnalyzer::any_other_token()
{
	int errors = 0;
	stringstream msg;
	
	if(t == LPAREN_T)
	{
		p2file << "Using Rule 54\n";
		cg->WriteCode(0,lex->GetLexeme());
		t = lex->GetToken();
		
		errors += more_tokens();
		if(t == RPAREN_T)
		{
			cg->WriteCode(0,lex->GetLexeme());
			t = lex->GetToken();
		}
		else
		{
			errors++;
			lex->ReportError("')' expected");
		}
	}
	else if (t == IDENT_T)
	{
		p2file << "Using Rule 55\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == NUMLIT_T	)
	{
		p2file << "Using Rule 56\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == STRLIT_T)
	{
		p2file << "Using Rule 57\n";
		cg->WriteCode(0,  lex->GetLexeme().substr(1,lex->GetLexeme().length() - 2) + " ");
		t = lex->GetToken();
	}
	else if (t == IF_T)
	{
		p2file << "Using Rule 58\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == DISPLAY_T)
	{
		p2file << "Using Rule 59\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == NEWLINE_T)
	{
		p2file << "Using Rule 60\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == LISTOP1_T)
	{
		p2file << "Using Rule 61\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == LISTOP2_T)
	{
		p2file << "Using Rule 62\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == AND_T)
	{
		p2file << "Using Rule 63\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == OR_T)
	{
		p2file << "Using Rule 64\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == NOT_T)
	{
		p2file << "Using Rule 65\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == DEFINE_T)
	{
		p2file << "Using Rule 66\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == LET_T)
	{
		p2file << "Using Rule 67\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == NUMBERP_T)
	{
		p2file << "Using Rule 68\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == LISTP_T)
	{
		p2file << "Using Rule 69\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == ZEROP_T)
	{
		p2file << "Using Rule 70\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == NULLP_T)
	{
		p2file << "Using Rule 71\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == STRINGP_T)
	{
		p2file << "Using Rule 72\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == PLUS_T)
	{
		p2file << "Using Rule 73\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == MINUS_T)
	{
		p2file << "Using Rule 74\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == DIV_T)
	{
		p2file << "Using Rule 75\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == MULT_T)
	{
		p2file << "Using Rule 76\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == MODULO_T)
	{
		p2file << "Using Rule 77\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == ROUND_T)
	{
		p2file << "Using Rule 78\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == EQUALTO_T)
	{
		p2file << "Using Rule 79\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == GT_T)
	{
		p2file << "Using Rule 80\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == LT_T)
	{
		p2file << "Using Rule 81\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == GTE_T)
	{
		p2file << "Using Rule 82\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == LTE_T)
	{
		p2file << "Using Rule 83\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == SQUOTE_T)
	{
		p2file << "Using Rule 84\n";
		t = lex->GetToken();
		errors += any_other_token();
	}
	else if (t == COND_T)
	{
		p2file << "Using Rule 85\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else if (t == ELSE_T)
	{
		p2file << "Using Rule 86\n";
		cg->WriteCode(0, lex->GetLexeme() + " ");
		t = lex->GetToken();
	}
	else
	{
		errors++;
		msg << "'" << lex->GetLexeme() << "' unexpected";
		lex->ReportError(msg.str());
	}
	
	return errors;
}


