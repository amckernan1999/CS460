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

using namespace std;

class SyntacticalAnalyzer 
{
    public:
	SyntacticalAnalyzer (char * filename);
	~SyntacticalAnalyzer ();
    private:
	LexicalAnalyzer * lex;
	token_type t;
	ofstream p2file;
	int program ();
	int more_defines();
	int define();
	int stmt_list();
	int stmt();
	int literal();
	int quoted_lit();
	int more_tokens();
	int param_list();
	int else_part();
	int stmt_pair();
	int stmt_pair_body();
	int assign_pair();
	int more_assigns();
	int action();
	int any_other_token();
};
	
#endif
