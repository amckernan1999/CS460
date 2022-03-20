/*******************************************************************************
* File name: SyntacticalAnalyzer.h                                             *
* Project: CS 460 Project 2 Fall 2020                                          *
* Author:                                                                      *
* Date:                                                                        *
* Description: This file contains                                              *
*******************************************************************************/

#ifndef SYN_H
#define SYN_H

#include <iostream>
#include <fstream>
#include "LexicalAnalyzer.h"
#include "CodeGenerator.h"
using namespace std;

class SyntacticalAnalyzer 
{
    public:
	SyntacticalAnalyzer (char * filename);
	~SyntacticalAnalyzer ();
    private:
	CodeGenerator * cg;
	LexicalAnalyzer * lex;
	token_type t;
	ofstream p2file;
	int numTabs;
	int program ();
	int more_defines();
	int define();
	int stmt_list();
	int stmt_list(string op);
	int stmt();
	int stmt_assign();
	int stmt_assign(string var);
	int literal();
	int quoted_lit();
	int more_tokens();
	int param_list();
	int else_part();
	int stmt_pair();
	int stmt_pair_body();
	int stmt_pair_body_first();
	int assign_pair();
	int more_assigns();
	int action();
	int action_assign();
	int any_other_token();
};
	
#endif
