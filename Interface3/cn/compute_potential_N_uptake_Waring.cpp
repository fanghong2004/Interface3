/*--------------------------------------------------------------*/
/* 								*/
/*		compute_potential_N_uptake_Waring					*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_potential_N_uptake_Waring -				*/
/*		computes potential N uptake from soil		*/
/*		for this strata without mineralization		*/
/*		limitation					*/
/*								*/
/*	SYNOPSIS						*/
/*	int compute_potential_N_uptake_Waring(					*/
/*                          struct epconst_struct,              */
/*			    struct epvar_struct *epv,		*/
/*                          struct cstate_struct *,             */
/*                          struct nstate_struct *,             */
/*                          struct cdayflux_struct *)           */
/*								*/
/*								*/
/*	returns int:						*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*              modified from Peter Thornton (1998)             */
/*                      dynamic - 1d-bgc ver4.0                 */
/*--------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>

#include <math.h>
#include "CHESS.h"
#include "Constants.h"
double compute_potential_N_uptake_Waring(
								  struct	epconst_struct epc,
								  struct	epvar_struct *epv,
								  struct cstate_struct *cs,
								  struct nstate_struct *ns,
								  struct cdayflux_struct *cdf)
{
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	double froot, fleaf;          /* RATIO   new fine root C : new leaf C     */
	double f2;          /* RATIO   fraction to leaf and fraction to root*/
	double f4;          /* RATIO   new live wood C : new wood C     */
	double f3;          /* RATIO   new leaf C : new wood C     */
	double g1;          /* RATIO   C respired for growth : C grown  */
	double cnl;         /* RATIO   leaf C:N      */
	double cnfr;        /* RATIO   fine root C:N */
	double cnlw;        /* RATIO   live wood C:N */
	double cndw;        /* RATIO   dead wood C:N */
	//double cnmax;       /* RATIO   max of root and leaf C:N      */
	double c_allometry, n_allometry;
	double plant_calloc, plant_ndemand;
	double  a, b, c; /* working variables */ //k2,
	/*---------------------------------------------------------------
	Assess the carbon availability on the basis of this day's gross production and maintenance respiration costs
	----------------------------------------------------------------*/
	cs->availc = cdf->psn_to_cpool-cdf->total_mr; //avaliabl carbon equals GPP minus maintanence respiration

	/* no allocation when the daily C balance is negative */
	if (cs->availc < 0.0) cs->availc = 0.0;

	/* test for cpool deficit */
	if (cs->cpool < 0.0){
	/*--------------------------------------------------------------
	running a deficit in cpool, so the first priority is to let today's available C accumulate in cpool.  The actual
	accumulation in the cpool is resolved in day_carbon_state().
		--------------------------------------------------------------*/
		if (-cs->cpool < cs->availc){
		/*------------------------------------------------
		cpool deficit is less than the available
		carbon for the day, so aleviate cpool deficit
		and use the rest of the available carbon for
		new growth and storage. Remember that fluxes in
		and out of the cpool are reconciled at the end
		of the daily loop, so for now, just keep track
		of the amount of daily GPP-MR that is not needed
		to restore a negative cpool.
			-----------------------------------------------*/
			cs->availc += cs->cpool;
		}
		else{
		/*---------------------------------------------------------
		cpool deficit is >= available C, so all of the
		daily GPP, if any, is used to alleviate negative cpool
			------------------------------------------------------------*/
			cs->availc = 0.0;
		}
	} /* end if negative cpool */
	else {
		cs->availc += cs->cpool;
	}


	/* assign local values for the allocation control parameters */
	f2   = epc.alloc_crootc_stemc;
	f3   = epc.alloc_stemc_leafc;
	f4   = epc.alloc_livewoodc_woodc;
	g1   = epc.gr_perc;
	cnl  = epc.leaf_cn;
	cnfr = epc.froot_cn;
	cnlw = epc.livewood_cn;
	cndw = epc.deadwood_cn;
	/*--------------------------------------------------------------- */
	/*	given the available C, use Waring allometric relationships to */
	/*	estimate N requirements -					*/ 
	/*	constants a and b are taken from Landsberg and Waring, 1997 */
	/*--------------------------------------------------------------*/
	
	if (((cdf->potential_psn_to_cpool) > ZERO) && (cdf->psn_to_cpool > ZERO)) {
		a = 0.8;
		b = 2.5;
		c = cdf->potential_psn_to_cpool;
		plant_calloc = cs->availc;
		if (plant_calloc > ZERO){
			froot = a / (1.0 + b * (cdf->psn_to_cpool) / c);
			if (epc.veg_type == TREE)
				fleaf = (1.0 - froot) / (1 + (1+f2)*f3);
			else
				fleaf = 1.0 - froot;
		}
		else {
			froot = 0.0;
			fleaf = 0.0;
		}

		if (epc.veg_type == TREE){
			c_allometry = ((1.0+g1)*(fleaf + froot + f3*(1.0+f2)));
			n_allometry = (fleaf/cnl + froot/cnfr + (f3*f4*(1.0+f2))/cnlw
				+ (f3*(1.0-f4)*(1.0+f2))/cndw);
		}
		else{
			c_allometry = (1.0+g1)*(fleaf + froot );
			n_allometry = (fleaf/cnl + froot/cnfr);
		}

		plant_ndemand = cs->availc * (n_allometry / c_allometry);
	}
	else
		plant_ndemand = 0.0;
	
	return(plant_ndemand);
} /* 	end compute_potential_N_uptake_Waring */
