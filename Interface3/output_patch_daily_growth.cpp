/*--------------------------------------------------------------*/
/* 																*/
/*					output_growth_patch						*/
/*																*/
/*	output_growth_patch - creates output_growth files objects.		*/
/*																*/
/*	NAME														*/
/*	output_growth_patch - output_growths current contents of a patch.			*/
/*																*/
/*	SYNOPSIS													*/
/*	void	output_growth_patch(										*/
/*					struct	patch_object	*patch,				*/
/*					struct	date	date,  						*/
/*					FILE 	*outfile)							*/
/*																*/
/*	OPTIONS														*/
/*																*/
/*	DESCRIPTION													*/
/*																*/
/*	output_growths spatial structure according to commandline			*/
/*	specifications to specific files							*/
/*																*/
/*	PROGRAMMER NOTES											*/
/*																*/
/*	We only permit one fileset per spatial modelling level.     */
/*	Each fileset has one file for each timestep.  				*/
/*																*/
/*--------------------------------------------------------------*/
#include "stdafx.h"
#include <stdio.h>

#include "CHESS.h"

void	output_patch_daily_growth(struct	patch_object *patch,
	int pch,
	double(*PlantMon)[PLANT_NUM],
	struct	date current_date,
	FILE *outfile,
	struct command_line_object *command_line)
{

	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int   layer; //c,check,
	double alai, apai, aresp, asoilhr;
	double aleafc, afrootc, awoodc;
	//double atotalN, apredaytN;

	struct	canopy_strata_object 	*strata;
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/

	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	//int  layer;
	double agpp, anpp;

	int last_day_of_month[13]{ 0,31,28,31,30,31,30,31,31,30,31,30,31 };
	// rectified for leap year
	if ((current_date.year % 4 == 0 && current_date.year % 100 != 0) || current_date.year % 400 == 0) {
		last_day_of_month[2] += 1;//add one day for Feb
	}

	double aleafn, afrootn, awoodn;
	double alitrc;

	double asoilc, asminn, anitrate, asurfaceN;
	double alitrn, asoiln;
	double astemc, astemn;
	double adstemc, adstemn;
	double atotaln, atotalc, adenitrif, astreamflow_N;
	double anitrif, aDOC, aDON, arootdepth;
	double acrootc, adcrootc, acrootn, adcrootn;
	double arespm, arespg, asoilar;
	//struct	patch_object  *patch;
	//struct	zone_object	*zone;
	//struct hillslope_object *hillslope;

	/*--------------------------------------------------------------*/
	/*	Initialize Accumlating variables.							*/
	/*--------------------------------------------------------------*/
	alai = 0.0;
	apai = 0.0;
	agpp = 0.0;
	anpp = 0.0;
	aresp = 0.0;
	arespm = 0.0;
	arespg = 0.0;
	asoilar = 0.0;

	aleafc = 0.0;
	astemc = 0.0;
	afrootc = 0.0;
	awoodc = 0.0;
	adstemc = 0.0;
	acrootc = 0.0;
	adcrootc = 0.0;

	aleafn = 0.0;
	afrootn = 0.0;
	astemn = 0.0;
	awoodn = 0.0;
	adstemn = 0.0;
	acrootn = 0.0;
	adcrootn = 0.0;
	arootdepth = 0.0;


	alitrn = (patch->litter_ns.litr1n + patch->litter_ns.litr2n + patch->litter_ns.litr3n + patch->litter_ns.litr4n);
	asoiln = (patch->soil_ns.soil1n + patch->soil_ns.soil2n + patch->soil_ns.soil3n + patch->soil_ns.soil4n);
	alitrc = (patch->litter_cs.litr1c + patch->litter_cs.litr2c + patch->litter_cs.litr3c + patch->litter_cs.litr4c);
	asoilc = (patch->soil_cs.soil1c + patch->soil_cs.soil2c + patch->soil_cs.soil3c + patch->soil_cs.soil4c);

	asurfaceN = patch->surface_NO3 + patch->surface_NH4;
	anitrate = patch->soil_ns.nitrate;
	asminn = patch->soil_ns.sminn;
	astreamflow_N = patch->surface_leach_NO3_out + patch->subsurface_leach_NO3_out +
		patch->surface_leach_NH4_out + patch->subsurface_leach_NH4_out;
	adenitrif = patch->ndf.denitrif;
	anitrif = patch->ndf.sminn_to_nitrate;
	aDON = patch->DON_loss;
	aDOC = patch->DOC_loss;

	asoilhr = (patch->cdf.litr1c_hr + patch->cdf.litr2c_hr + patch->cdf.litr4c_hr +
		patch->cdf.soil1c_hr + patch->cdf.soil2c_hr + patch->cdf.soil3c_hr + patch->cdf.soil4c_hr);

	//Note:: all strata_cover_fraction* were deleted. need double check against original source code
	for (layer = 0; layer < patch->num_layers; layer++) {
		//for ( c=0 ; c<patch->layers[layer].count; c++ ){
		strata = patch->canopy_strata;
		agpp += strata->cdf.psn_to_cpool;
		anpp += strata->cdf.net_psn;
		aresp += (strata->cdf.total_gr + strata->cdf.total_mr);
		arespm += strata->cdf.total_mr;
		//arespm  += (strata->cdf.leaf_day_mr + strata->cdf.leaf_night_mr
		//	      + strata->cdf.livestem_mr + strata->cdf.livecroot_mr  + strata->cdf.froot_mr);

		arespg += strata->cdf.total_gr;
		//arespg  += (cdf->cpool_leaf_gr         + cdf->cpool_froot_gr       + cdf->cpool_livecroot_gr
		//          + cdf->cpool_deadcroot_gr    + cdf->cpool_livestem_gr    + cdf->cpool_deadstem_gr
		//			+ cdf->transfer_leaf_gr      + cdf->transfer_froot_gr    + cdf->transfer_livecroot_gr
		//          + cdf->transfer_deadcroot_gr + cdf->transfer_livestem_gr + cdf->transfer_deadstem_gr);


		asoilar += (strata->cdf.cpool_livecroot_gr + strata->cdf.cpool_deadcroot_gr
			+ strata->cdf.cpool_froot_gr + strata->cdf.transfer_froot_gr
			+ strata->cdf.transfer_livecroot_gr + strata->cdf.transfer_deadcroot_gr
			+ strata->cdf.livecroot_mr + strata->cdf.froot_mr); //need check for definition

		aleafc += (strata->cs.leafc + strata->cs.leafc_store + strata->cs.leafc_transfer);
		astemc += (strata->cs.live_stemc + strata->cs.livestemc_store + strata->cs.livestemc_transfer);
		adstemc += (strata->cs.dead_stemc + strata->cs.deadstemc_store + strata->cs.deadstemc_transfer);


		afrootc += (strata->cs.frootc + strata->cs.frootc_store + strata->cs.frootc_transfer);
		acrootc += (strata->cs.live_crootc + strata->cs.livecrootc_store + strata->cs.livecrootc_transfer);
		adcrootc += (strata->cs.dead_crootc + strata->cs.deadcrootc_store + strata->cs.deadcrootc_transfer);


		aleafn += (strata->ns.leafn + strata->ns.leafn_store + strata->ns.leafn_transfer);
		astemn += (strata->ns.live_stemn + strata->ns.livestemn_store + strata->ns.livestemn_transfer);
		adstemn += (strata->ns.dead_stemn + strata->ns.deadstemn_store + strata->ns.deadstemn_transfer);
		afrootn += (strata->ns.frootn + strata->ns.frootn_store + strata->ns.frootn_transfer);
		acrootn += (strata->ns.live_crootn + strata->ns.livecrootn_store + strata->ns.livecrootn_transfer);
		adcrootn += (strata->ns.dead_crootn + strata->ns.deadcrootn_store + strata->ns.deadcrootn_transfer);

		arootdepth += strata->rootzone.depth;
		alai += strata->epv.proj_lai;
		apai += strata->epv.proj_pai;
		//}
	}
	atotaln = aleafn + astemn + adstemn + afrootn + acrootn + adcrootn + alitrn + asoiln;
	atotalc = aleafc + astemc + adstemc + afrootc + acrootc + adcrootc + alitrc + asoilc;

	if (command_line->pday == true) {
		fprintf(outfile, "%4.0d", current_date.year);
		fprintf(outfile, "%4.0d", current_date.month);
		fprintf(outfile, "%4.0d", current_date.day);
		fprintf(outfile, "%9.0d", patch->ID);
		fprintf(outfile, "%11.1f", patch->x);
		fprintf(outfile, "%12.1f", patch->y);
		fprintf(outfile, "%12.2f", alai);                        // average LAI
		fprintf(outfile, "%12.2f", apai);                        // average PAI
		fprintf(outfile, "%12.2f", agpp * 1000);                   // GPP converted from kgC/m2 to gc/m2
		fprintf(outfile, "%12.2f", anpp * 1000);                   // NPP converted from kgC/m2 to gc/m2
		fprintf(outfile, "%12.2f", aresp * 1000);                  // maintance respiration + growth respiration
		fprintf(outfile, "%12.2f", arespm * 1000);                 // maintance respiration
		fprintf(outfile, "%12.2f", arespg * 1000);                 // growth respiration
		fprintf(outfile, "%12.2f", asoilhr * 1000);                // soil heterotrophic respiration
		fprintf(outfile, "%12.2f", asoilar * 1000);                // soil autotrophic respiration
		fprintf(outfile, "%12.1f", aleafc * 1000);                 // leaf carbon
		fprintf(outfile, "%12.1f", astemc * 1000);                 // stem carbon
		fprintf(outfile, "%12.0f", adstemc * 1000);                // dead stem carbon
		fprintf(outfile, "%12.1f", afrootc * 1000);                // fine root carbon
		fprintf(outfile, "%12.1f", acrootc * 1000);                // coarse root carbon
		fprintf(outfile, "%12.0f", adcrootc * 1000);               // dead coarse root carbon
		fprintf(outfile, "%12.1f", alitrc * 1000);                 // liter carbon
		fprintf(outfile, "%12.0f", asoilc * 1000);                 // soil carbon
		fprintf(outfile, "%12.1f", atotalc * 1000);                 // total carbon
		fprintf(outfile, "%12.2f", aleafn * 1000);                 // leaf nitrogen
		fprintf(outfile, "%12.2f", astemn * 1000);                 // wood stem nitrogen
		fprintf(outfile, "%12.2f", adstemn * 1000);                // dead stem nitrogen
		fprintf(outfile, "%12.2f", afrootn * 1000);                // fine root nitrogen
		fprintf(outfile, "%12.2f", acrootn * 1000);                // coarse root nitrogen
		fprintf(outfile, "%12.2f", adcrootn * 1000);               // dead coarse root nitrogen
		fprintf(outfile, "%12.1f", alitrn * 1000);                 // liter nitrogen
		fprintf(outfile, "%12.1f", asoiln * 1000);                 // soil nitrogen
		fprintf(outfile, "%12.1f", atotaln * 1000);                 // total nitrogen??
		fprintf(outfile, "%12.0f", anitrate * 1000);               // (gN/m2) soil mineral N in nitrate form */
		fprintf(outfile, "%12.2f", asminn * 1000);                 // soil mineral nitrogen (gN/m2)
		fprintf(outfile, "%12.2f", asurfaceN * 1000);              // surface no3 + surface nh4 (Unit: g/m2)
		fprintf(outfile, "%12.2f", astreamflow_N * 1000);          // stream flow nitrate????? (Unit: g/m2/day)
		fprintf(outfile, "%12.2f", adenitrif * 1000);              // annual denitrification
		fprintf(outfile, "%12.2f", anitrif * 1000);                // annual nitrification
		fprintf(outfile, "%12.2f", aDOC * 1000);                   // dessolved organic carbon (unit: gC/m2/day)
		fprintf(outfile, "%12.2f", aDON * 1000);                   // dessolved organic nitrogen(unit:gN/m2/day)
		fprintf(outfile, "%12.2f", arootdepth);                  // annual root zone depth
		fprintf(outfile, "\n");
	}
	else if (command_line->pmon == true) {


		PlantMon[pch][0] = current_date.year;
		PlantMon[pch][1] = current_date.month;
		PlantMon[pch][2] = current_date.day;//it can be used as mutiplyer for rain accumulated value
		PlantMon[pch][3] = patch->ID;
		PlantMon[pch][4] = patch->x;
		PlantMon[pch][5] = patch->y;

		PlantMon[pch][6] += alai;                        // average LAI
		PlantMon[pch][7] += apai;                        // average PAI
		PlantMon[pch][8] += agpp * 1000;                   // GPP converted from kgC/m2 to gc/m2
		PlantMon[pch][9] += anpp * 1000;                   // NPP converted from kgC/m2 to gc/m2
		PlantMon[pch][10] += aresp * 1000;                  // maintance respiration + growth respiration
		PlantMon[pch][11] += arespm * 1000;                 // maintance respiration
		PlantMon[pch][12] += arespg * 1000;                 // growth respiration
		PlantMon[pch][13] += asoilhr * 1000;                // soil heterotrophic respiration
		PlantMon[pch][14] += asoilar * 1000;                // soil autotrophic respiration
		PlantMon[pch][15] += aleafc * 1000;                 // leaf carbon
		PlantMon[pch][16] += astemc * 1000;                 // stem carbon
		PlantMon[pch][17] += adstemc * 1000;                // dead stem carbon
		PlantMon[pch][18] += afrootc * 1000;                // fine root carbon
		PlantMon[pch][19] += acrootc * 1000;                // coarse root carbon
		PlantMon[pch][20] += adcrootc * 1000;               // dead coarse root carbon
		PlantMon[pch][21] += alitrc * 1000;                 // liter carbon
		PlantMon[pch][22] += asoilc * 1000;                 // soil carbon
		PlantMon[pch][23] += atotalc * 1000;                 // total carbon
		PlantMon[pch][24] += aleafn * 1000;                 // leaf nitrogen
		PlantMon[pch][25] += astemn * 1000;                 // wood stem nitrogen
		PlantMon[pch][26] += adstemn * 1000;                // dead stem nitrogen
		PlantMon[pch][27] += afrootn * 1000;                // fine root nitrogen
		PlantMon[pch][28] += acrootn * 1000;                // coarse root nitrogen
		PlantMon[pch][29] += adcrootn * 1000;               // dead coarse root nitrogen
		PlantMon[pch][30] += alitrn * 1000;                 // liter nitrogen
		PlantMon[pch][31] += asoiln * 1000;                 // soil nitrogen
		PlantMon[pch][32] += atotaln * 1000;                 // total nitrogen??
		PlantMon[pch][33] += anitrate * 1000;               // (gN/m2 soil mineral N in nitrate form */
		PlantMon[pch][34] += asminn * 1000;                 // soil mineral nitrogen (gN/m2
		PlantMon[pch][35] += asurfaceN * 1000;              // surface no3 + surface nh4 (Unit: g/m2
		PlantMon[pch][36] += astreamflow_N * 1000;          // stream flow nitrate????? (Unit: g/m2/day
		PlantMon[pch][37] += adenitrif * 1000;              // annual denitrification
		PlantMon[pch][38] += anitrif * 1000;                // annual nitrification
		PlantMon[pch][39] += aDOC * 1000;                   // dessolved organic carbon (unit: gC/m2/day
		PlantMon[pch][40] += aDON * 1000;                   // dessolved organic nitrogen(unit:gN/m2/day
		PlantMon[pch][41] += arootdepth;

		//PRINT VALUE IN THE LAST DAY OF MONTH
		if (current_date.day == last_day_of_month[current_date.month]) {

			//we chose enum all values since it's better efficiency
			fprintf(outfile, "%4.0d", current_date.year);
			fprintf(outfile, "%4.0d", current_date.month);
			fprintf(outfile, "%4.0d", current_date.day);
			fprintf(outfile, "%9.0d", patch->ID);
			fprintf(outfile, "%11.1f", patch->x);
			fprintf(outfile, "%12.1f", patch->y);


			fprintf(outfile, "%12.3f", PlantMon[pch][6] / PlantMon[pch][2]);                       // average LAI
			fprintf(outfile, "%12.3f", PlantMon[pch][7] / PlantMon[pch][2]);                      // average PAI
			fprintf(outfile, "%12.3f", PlantMon[pch][8] / PlantMon[pch][2]);                  // GPP converted from kgC/m2 to gc/m2
			fprintf(outfile, "%12.3f", PlantMon[pch][9] / PlantMon[pch][2]);                  // NPP converted from kgC/m2 to gc/m2
			fprintf(outfile, "%12.3f", PlantMon[pch][10] / PlantMon[pch][2]);                // maintance respiration + growth respiration
			fprintf(outfile, "%12.3f", PlantMon[pch][11] / PlantMon[pch][2]);                 // maintance respiration
			fprintf(outfile, "%12.3f", PlantMon[pch][12] / PlantMon[pch][2]);               // growth respiration
			fprintf(outfile, "%12.3f", PlantMon[pch][13] / PlantMon[pch][2]);                // soil heterotrophic respiration
			fprintf(outfile, "%12.3f", PlantMon[pch][14] / PlantMon[pch][2]);                // soil autotrophic respiration
			fprintf(outfile, "%12.3f", PlantMon[pch][15] / PlantMon[pch][2]);               // leaf carbon
			fprintf(outfile, "%12.3f", PlantMon[pch][16] / PlantMon[pch][2]);               // stem carbon
			fprintf(outfile, "%12.3f", PlantMon[pch][17] / PlantMon[pch][2]);              // dead stem carbon
			fprintf(outfile, "%12.3f", PlantMon[pch][18] / PlantMon[pch][2]);             // fine root carbon
			fprintf(outfile, "%12.3f", PlantMon[pch][19] / PlantMon[pch][2]);              // coarse root carbon
			fprintf(outfile, "%12.3f", PlantMon[pch][20] / PlantMon[pch][2]);              // dead coarse root carbon
			fprintf(outfile, "%12.3f", PlantMon[pch][21] / PlantMon[pch][2]);                 // liter carbon
			fprintf(outfile, "%12.3f", PlantMon[pch][22] / PlantMon[pch][2]);                 // soil carbon
			fprintf(outfile, "%12.3f", PlantMon[pch][23] / PlantMon[pch][2]);                 // total carbon
			fprintf(outfile, "%12.3f", PlantMon[pch][24] / PlantMon[pch][2]);                // leaf nitrogen
			fprintf(outfile, "%12.3f", PlantMon[pch][25] / PlantMon[pch][2]);                // wood stem nitrogen
			fprintf(outfile, "%12.3f", PlantMon[pch][26] / PlantMon[pch][2]);            // dead stem nitrogen
			fprintf(outfile, "%12.3f", PlantMon[pch][27] / PlantMon[pch][2]);            // fine root nitrogen
			fprintf(outfile, "%12.3f", PlantMon[pch][28] / PlantMon[pch][2]);            // coarse root nitrogen
			fprintf(outfile, "%12.3f", PlantMon[pch][29] / PlantMon[pch][2]);              // dead coarse root nitrogen
			fprintf(outfile, "%12.3f", PlantMon[pch][30] / PlantMon[pch][2]);                // liter nitrogen
			fprintf(outfile, "%12.3f", PlantMon[pch][31] / PlantMon[pch][2]);                // soil nitrogen
			fprintf(outfile, "%12.3f", PlantMon[pch][32] / PlantMon[pch][2]);                // total nitrogen??
			fprintf(outfile, "%12.3f", PlantMon[pch][33] / PlantMon[pch][2]);              // (gN/m2) soil mineral N in nitrate form */
			fprintf(outfile, "%12.3f", PlantMon[pch][34] / PlantMon[pch][2]);               // soil mineral nitrogen (gN/m2)
			fprintf(outfile, "%12.3f", PlantMon[pch][35] / PlantMon[pch][2]);              // surface no3 + surface nh4 (Unit: g/m2)
			fprintf(outfile, "%12.3f", PlantMon[pch][36] / PlantMon[pch][2]);         // stream flow nitrate????? (Unit: g/m2/day)
			fprintf(outfile, "%12.3f", PlantMon[pch][37] / PlantMon[pch][2]);             // annual denitrification
			fprintf(outfile, "%12.3f", PlantMon[pch][38] / PlantMon[pch][2]);              // annual nitrification
			fprintf(outfile, "%12.3f", PlantMon[pch][39] / PlantMon[pch][2]);              // dessolved organic carbon (unit: gC/m2/day)
			fprintf(outfile, "%12.3f", PlantMon[pch][40] / PlantMon[pch][2]);                  // dessolved organic nitrogen(unit:gN/m2/day)
			fprintf(outfile, "%12.3f", PlantMon[pch][41] / PlantMon[pch][2]);               // annual root zone depth
			fprintf(outfile, "\n");

			//REINIT AS 0
			for (int inx = 0; inx != PLANT_NUM; inx++) {
				PlantMon[pch][inx] = 0;
			}

		}
	}

	return;
} /*end output_growth_patch*/
