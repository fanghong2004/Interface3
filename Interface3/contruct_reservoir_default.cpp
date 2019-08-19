//xu.
#include "stdafx.h"
#include <stdio.h>

#include "CHESS.h"
#include "Functions.h"
#include "Constants.h"
#include<iostream>

using namespace std;

struct reservoir_default *construct_reservoir_defaults(
	int	nytpes,
	FILE	*default_file,
	struct command_line_object *command_line,
	struct reservoir_default * reservoir_default_object_list)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.									*/
	/*--------------------------------------------------------------*/
	void	*alloc(size_t, char *, char *);

	/*--------------------------------------------------------------*/
	/*	Local variable definition.									*/
	/*--------------------------------------------------------------*/
	char	record[MAXSTR];

	read_record(default_file, record);


	for (int i = 0; i < nytpes; i++) {
		fscanf(default_file, "%d", &(reservoir_default_object_list[i].ID));
	}
	read_record(default_file, record);

	for (int i = 0; i < nytpes; i++) {
		fscanf(default_file, "%d", &(reservoir_default_object_list[i].StartYear));
	}
	read_record(default_file, record);

	for (int i = 0; i < nytpes; i++) {
		fscanf(default_file, "%d", &(reservoir_default_object_list[i].EndYear));
	}
	read_record(default_file, record);


	for (int i = 0; i < nytpes; i++) {
		fscanf(default_file, "%lf", &(reservoir_default_object_list[i].Ve));
	}
	read_record(default_file, record);

	for (int i = 0; i < nytpes; i++) {
		fscanf(default_file, "%lf", &(reservoir_default_object_list[i].Vd));
	}
	read_record(default_file, record);

	for (int mon_inx = 0; mon_inx < 12; mon_inx++) {
		for (int i = 0; i < nytpes; i++) {

		fscanf(default_file, "%lf", &(reservoir_default_object_list[i].Vp[mon_inx]));
		}
	read_record(default_file, record);
	}

	for (int mon_inx = 0; mon_inx < 12; mon_inx++) {
		for (int i = 0; i < nytpes; i++) {

			fscanf(default_file, "%lf", &(reservoir_default_object_list[i].Vc[mon_inx]));
		}
		read_record(default_file, record);
	}

	for (int mon_inx = 0; mon_inx < 12; mon_inx++) {
		for (int i = 0; i < nytpes; i++) {

			fscanf(default_file, "%lf", &(reservoir_default_object_list[i].kmon[mon_inx]));
		}
		read_record(default_file, record);
	}

	fclose(default_file);
	return(reservoir_default_object_list);
} /*end construct_zone_defaults*/
