#include <cstdlib>
#include <iostream>
#include <iomanip>
#include "SetLimits.h"
#include "SyntacticalAnalyzer.h"
#include "LexicalAnalyzer.h"
using namespace std;

int main (int argc, char * argv[])
{
	if (argc < 2)
	{
		printf ("format: proj1 <filename>\n");
		exit (1);
	}
	
	SetLimits ();
//	SyntacticalAnalyzer parser (argv[1]);
	LexicalAnalyzer model (argv[1]);
	
	token_type current;
	do
	{
		current = model.GetToken();	

	}
	while(current != EOF_T);










	return 0;
}
