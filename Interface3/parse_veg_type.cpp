//--------------------------------------------------------------
// identify the vegetation type
//--------------------------------------------------------------
#include "stdafx.h"
#include <stdio.h>

#include <string.h>
#include "CHESS.h"
#include "Constants.h"

int	parse_veg_type( char *input_string)
{
	//------------------------------------------------------
	//	Local Variable Definition. 							
	//------------------------------------------------------
	int veg_type;

	if ((strncmp(input_string,"TREE",4) == 0) || (strncmp(input_string, "tree",4) == 0) )
		veg_type = TREE;
	else if ((strncmp(input_string,"GRASS",5) == 0) || 	(strncmp(input_string, "grass",5) == 0) )
		veg_type = GRASS;
	else if ((strncmp(input_string,"C4GRASS",7) == 0) || (strncmp(input_string, "c4grass",7) == 0) )
		veg_type = C4GRASS;
	else if ((strncmp(input_string,"C3GRASS",7) == 0) || (strncmp(input_string, "c3grass",7) == 0) )
		veg_type = GRASS;
	else if ((strncmp(input_string,"NON_VEG",5) == 0) || (strncmp(input_string, "non_veg",5) == 0) )
		veg_type = NON_VEG;
	else {
		fprintf(stderr,"\n FATAL ERROR - parse_veg_type");
		fprintf(stderr,"\n %s Unknown veg type in stratum default file", input_string);
		fprintf(stderr,"\n must be either TREE or GRASS");
		exit(0);
	}
	
	return(veg_type);
}
