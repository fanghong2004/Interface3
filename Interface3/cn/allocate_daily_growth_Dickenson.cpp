/*--------------------------------------------------------------*/
/* 								*/
/*								*/
/*	allocate_daily_growth				*/
/*								*/
/*	NAME							*/
/*		allocate_daily_growth			*/
/*								*/
/*	SYNOPSIS						*/
/*	int allocate_daily_growth_Dickenson( int , double, double,double,	*/
/*			    struct cdayflux_struct *,  		*/
/*			    struct cstate_struct *,		*/
/*			    struct ndayflux_struct *,	 	*/
/*			    struct nstate_struct *, 		*/
/*			    struct ndayflux_patch_struct *, 	*/
/*			    struct epconst_struct)		*/
/*								*/
/*	returns:						*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*		calculates daily C and N allocations		*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*								*/
/*              modified from Peter Thornton (1998)             */
/*                      dynamic - 1d-bgc ver4.0                 */
/*--------------------------------------------------------------*/
#include <stdio.h>

#include <stdlib.h>
#include <math.h>
#include "CHESS.h"
#include "Constants.h"

int allocate_daily_growth_Dickenson(int nlimit,
						  double sen,
						  double pnow,
						  double total_soil_frootc,
						  double cover_fraction,
						  struct cdayflux_struct *cdf,
						  struct cstate_struct *cs,
						  struct ndayflux_struct *ndf,
						  struct nstate_struct *ns,
						  struct ndayflux_patch_struct *ndf_patch,
						  struct epvar_struct *epv,
						  struct epconst_struct epc,
						  struct date current_date)
{
	/*------------------------------------------------------*/
	/*	Local function declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	int ok=1;
	double fleaf;          /* RATIO   new leaf C: new total C     */
	double froot;          /* RATIO   new fine root C : new total C     */
	double flive, fdead;	/* RATIO  live/dead C : new total C */
	double fwood;          /* RATIO   wood          */
	double fcroot;          /* RATIO   new stem C : new croot C */   
	double g1;          /* RATIO   C respired for growth : C grown  */
	double cnl;         /* RATIO   leaf C:N      */
	double cnfr;        /* RATIO   fine root C:N */
	double cnlw;        /* RATIO   live wood C:N */
	double cndw;        /* RATIO   dead wood C:N */
	double nlc;         /* actual new leaf C, minimum of C and N limits   */
	//double nloss, closs;
	double gresp_store, total_wood;
	double plant_ndemand, mean_cn;
	double sum_plant_nsupply, soil_nsupply;
	double plant_nalloc, plant_calloc;
	double plant_remaining_ndemand;
	double excess_allocation_to_leaf, excess_c, excess_lai;
	double sminn_to_npool;
	double B,C, totalc_used,total_used; /* working variables */
	double preday_npool, preday_cpool;

	/* assign local values for the allocation control parameters */
	B = epc.alloc_stemc_leafc;
	fcroot = epc.alloc_crootc_stemc;
	flive = epc.alloc_livewoodc_woodc;
	fdead = (1-flive);
	g1 = epc.gr_perc;
	cnl = epc.leaf_cn;
	cnfr = epc.froot_cn;
	cnlw = epc.livewood_cn;
	cndw = epc.deadwood_cn;
	excess_c = 0.0;
	sminn_to_npool = 0.0;
	plant_ndemand = ndf->potential_N_uptake;
	preday_npool = ns->npool;
	preday_cpool = cs->cpool;

	/*--------------------------------------------------------------*/
	/*	Determine allocation partitioning			*/
	/*      from Dickenson et al, 1998, Journal of Climate          */
	/*	ratio to leaves in an exponential function of LAI	*/
	/*--------------------------------------------------------------*/

	/*---------------------------------------------------------------*/
	/* constant B and C are currently set for forests from Dickenson et al. */	
	/*	we will now use the alloc_leafc_stemc to partition carbon between coarse roots and stem */
	/*	similarly with dead stem and coarse root allocation 		*/
	/*	the only impact of these pools is change in height for competition	*/
	/*----------------------------------------------------------------*/
	C = 30;

	fleaf = exp(-0.25 *sen * epv->proj_lai);
	fleaf = min(fleaf, 1.0);
	total_wood = (cs->live_crootc + cs->dead_crootc + cs->live_stemc + cs->dead_stemc);
	/*
	if (total_wood < ZERO)
		froot = 0.5*(1-fleaf);
	else
		froot = (1-fleaf) * 1/B * exp(-C*B*(cs->frootc)/total_wood);
	fwood = (1-froot-fleaf);
	*/


	if (epc.veg_type==TREE) {
		froot = 0.5*(1-fleaf);
		fwood = 0.5*(1-fleaf);
		}
	else {
		fwood = 0;
		froot = (1-fleaf);
		}
	

	if (epc.veg_type == TREE){
	   mean_cn = 1.0 / (fleaf / cnl + froot / cnfr + flive * fwood / cnlw + fwood * fdead / cndw);
	}
	else{
	   mean_cn = (fleaf * cnl + froot * cnfr);
	}


	/*--------------------------------------------------------------*/
	/*	the amount available from the soil is potential_N_uptake adjusted */
	/*	by fract_potential_uptake which is calculated in resolve_N_competition */
	/*	based on available soil mineralized nitrogen				*/
	/*--------------------------------------------------------------*/
	if (nlimit == 1)
		if (total_soil_frootc > ZERO)
			soil_nsupply = min(ndf->potential_N_uptake,
			(ndf_patch->plant_avail_uptake *
			cs->frootc / total_soil_frootc));
		else
			soil_nsupply = ndf->potential_N_uptake;
	else
		soil_nsupply = ndf->potential_N_uptake;
		
	soil_nsupply = max(soil_nsupply, 0.0);
		/*----------------------------------------------------------------
		now compare the combined decomposition immobilization and plant
		growth N demands against the available soil mineral N pool.
	--------------------------------------------------------------------*/
	if (nlimit == 0){
	/* N availability is not limiting so plant
		uptake, and both can proceed at  potential rates */
		/* Determine the split between retranslocation N and soil mineral
		N to meet the plant demand */
		sum_plant_nsupply = ns->retransn + soil_nsupply;
		if (sum_plant_nsupply > 0.0){
			ndf->retransn_to_npool = max(ns->retransn,ndf->potential_N_uptake
				* (ns->retransn/sum_plant_nsupply));
		}
		else{
			ndf->retransn_to_npool = 0.0;
		}
		sminn_to_npool = ndf->potential_N_uptake - ndf->retransn_to_npool;
		plant_nalloc = ndf->retransn_to_npool + sminn_to_npool;
		plant_calloc = cs->availc*(1-epc.gr_perc);
	}
	else{
	/* N availability can not satisfy the sum of immobiliation and
	plant growth demands, so these two demands compete for available
		soil mineral N */
		sminn_to_npool = soil_nsupply;
		plant_remaining_ndemand = plant_ndemand - sminn_to_npool;
		/* the demand not satisfied by uptake from soil mineral N is
		now sought from the retranslocated N pool */
		if (plant_remaining_ndemand <= ns->retransn){
		/* there is enough N available in retranslocation pool to
			satisfy the remaining plant N demand */
			ndf->retransn_to_npool = plant_remaining_ndemand;
			plant_nalloc = ndf->retransn_to_npool + sminn_to_npool;
			plant_calloc = cs->availc*(1-epc.gr_perc);
		}
		else{
		/* there is not enough retranslocation N left to satisfy the
		entire demand. In this case, all remaing retranslocation N is
		used, and the remaining unsatisfied N demand is translated
		back to a C excess, which is deducted from
			photosynthesis source */
			ndf->retransn_to_npool = ns->retransn;
			plant_nalloc = ndf->retransn_to_npool + sminn_to_npool;
			plant_calloc = plant_nalloc / (1-epc.gr_perc) * mean_cn;
			excess_c = max(cs->availc - plant_calloc,0.0);
			cdf->psn_to_cpool -= excess_c;
		}
	}
	/* calculate the amount of new leaf C dictated by these allocation
	decisions, and figure the daily fluxes of C and N to current
	growth and storage pools */


	plant_nalloc = max(plant_nalloc, 0.0);
	plant_calloc = max(plant_calloc, 0.0);
		
	/* pnow is the proportion of this day's growth that is displayed now,
	the remainder going into storage for display next year through the
	transfer pools */
	nlc = plant_calloc * fleaf;


	/* daily C fluxes out of cpool and into new growth or storage */
	cdf->cpool_to_leafc              = nlc * pnow;
	cdf->cpool_to_leafc_store      = nlc * (1.0-pnow);
	cdf->cpool_to_frootc             = froot * plant_calloc * pnow;
	cdf->cpool_to_frootc_store     = froot * plant_calloc * (1.0-pnow);
	if (epc.veg_type == TREE){
		cdf->cpool_to_livestemc        = plant_calloc * flive * fwood * (1-fcroot) * pnow;
		cdf->cpool_to_livestemc_store  = plant_calloc * flive * fwood * (1-fcroot) * (1.0-pnow);
		cdf->cpool_to_deadstemc          = plant_calloc * fdead * fwood * (1-fcroot)  * pnow;
		cdf->cpool_to_deadstemc_store  = plant_calloc * fdead * fwood * (1-fcroot) * (1.0-pnow);
		cdf->cpool_to_livecrootc         = plant_calloc * fwood * fcroot * flive  * pnow;
		cdf->cpool_to_livecrootc_store = plant_calloc * fwood * fcroot * flive  * (1.0-pnow);
		cdf->cpool_to_deadcrootc         = plant_calloc * fwood * fcroot * fdead *  pnow;
		cdf->cpool_to_deadcrootc_store = plant_calloc * fwood  * fcroot * fdead *  (1.0-pnow);
	}

	/* daily N fluxes out of npool and into new growth or storage */
	ndf->sminn_to_npool = sminn_to_npool;
	ndf_patch->sminn_to_npool += sminn_to_npool * cover_fraction;



	ndf->npool_to_leafn              = cdf->cpool_to_leafc / cnl;
	ndf->npool_to_leafn_store      = cdf->cpool_to_leafc_store / cnl;
	ndf->npool_to_frootn              = cdf->cpool_to_frootc / cnfr;
	ndf->npool_to_frootn_store      = cdf->cpool_to_frootc_store / cnfr;
	if (epc.veg_type == TREE){
		ndf->npool_to_livestemn        = cdf->cpool_to_livestemc / cnlw; 
		ndf->npool_to_livestemn_store  = cdf->cpool_to_livestemc_store / cnlw; 
		ndf->npool_to_deadstemn        = cdf->cpool_to_deadstemc / cndw; 
		ndf->npool_to_deadstemn_store  = cdf->cpool_to_deadstemc_store / cndw; 
		ndf->npool_to_livecrootn        = cdf->cpool_to_livecrootc / cnlw; 
		ndf->npool_to_livecrootn_store  = cdf->cpool_to_livecrootc_store / cnlw; 
		ndf->npool_to_deadcrootn        = cdf->cpool_to_deadcrootc / cndw; 
		ndf->npool_to_deadcrootn_store  = cdf->cpool_to_deadcrootc_store / cndw; 

	}



	total_used = 
		ns->npool - preday_npool +
		ndf->npool_to_leafn +
		ndf->npool_to_leafn_store+
		ndf->npool_to_frootn +
		ndf->npool_to_frootn_store +
		ndf->npool_to_livestemn  +
		ndf->npool_to_livestemn_store  +
		ndf->npool_to_deadstemn         +
		ndf->npool_to_deadstemn_store  +
		ndf->npool_to_livecrootn         +
		ndf->npool_to_livecrootn_store +
		ndf->npool_to_deadcrootn         +
		ndf->npool_to_deadcrootn_store;


	totalc_used = 
	    cs->cpool - preday_cpool +
		cdf->cpool_to_leafc +
		cdf->cpool_to_leafc_store+
		cdf->cpool_to_frootc +
		cdf->cpool_to_frootc_store +
		cdf->cpool_to_livestemc  +
		cdf->cpool_to_livestemc_store  +
		cdf->cpool_to_deadstemc         +
		cdf->cpool_to_deadstemc_store  +
		cdf->cpool_to_livecrootc         +
		cdf->cpool_to_livecrootc_store +
		cdf->cpool_to_deadcrootc         +
		cdf->cpool_to_deadcrootc_store;

	if (fabs(total_used - ndf->sminn_to_npool - ndf->retransn_to_npool) > ZERO)
	printf("\n\n nlimt %d new sminn %lf retrans %lf plant_nalloc %lf total %lf, potential_uptake %lf total used %lf cused %lf calloc %lf cavail %lf psn_to_cpool %lf mean_cn %lf actual mean cn %lf %lf", nlimit,
		ndf->sminn_to_npool, ndf->retransn_to_npool, plant_nalloc, ndf->sminn_to_npool+ndf->retransn_to_npool, 
		ndf->potential_N_uptake, total_used, totalc_used, plant_calloc, cs->availc, cdf->psn_to_cpool, mean_cn, totalc_used/total_used, plant_calloc/plant_nalloc);



		 
	/* calculate the amount of carbon that needs to go into growth
	respiration storage to satisfy all of the storage growth demands */
	if (epc.veg_type == TREE){
		gresp_store = (cdf->cpool_to_leafc_store + cdf->cpool_to_frootc_store
			+ cdf->cpool_to_livestemc_store + cdf->cpool_to_deadstemc_store
			+ cdf->cpool_to_livecrootc_store + cdf->cpool_to_deadcrootc_store)
			* g1;
	}
	else{
		gresp_store = (cdf->cpool_to_leafc_store+cdf->cpool_to_frootc_store) * g1;
	}
	cdf->cpool_to_gresp_store = gresp_store;


	/*---------------------------------------------------------------------------	*/
	/*	create a maximum lai							*/
	/*---------------------------------------------------------------------------	*/

	excess_lai = (cs->leafc + cs->leafc_transfer + cs->leafc_store + cdf->cpool_to_leafc) * epc.proj_sla - epc.max_lai; 
	if ( excess_lai > ZERO) 
	{
		excess_c = excess_lai / epc.proj_sla;
		if (epc.veg_type == TREE) {
	/*---------------------------------------------------------------------------	*/
	/*	shift allocation to stemwood if leafc max has been reached		*/
	/*---------------------------------------------------------------------------	*/
			excess_allocation_to_leaf = min(cdf->cpool_to_leafc,  excess_c);
			cdf->cpool_to_leafc -= excess_allocation_to_leaf;
			cdf->cpool_to_deadstemc += fdead*excess_allocation_to_leaf;
			cdf->cpool_to_livestemc += flive*excess_allocation_to_leaf;
			ndf->npool_to_leafn = max(ndf->npool_to_leafn - excess_allocation_to_leaf / cnl, 0.0);
			ndf->npool_to_deadstemn += fdead*excess_allocation_to_leaf / cndw ;
			ndf->npool_to_livestemn += flive*excess_allocation_to_leaf / cnlw;
		}
		else {
		     excess_allocation_to_leaf = min(cdf->cpool_to_leafc,  excess_c);
		     cdf->cpool_to_leafc -= excess_allocation_to_leaf;
		     ndf->npool_to_leafn = max(ndf->npool_to_leafn - excess_allocation_to_leaf / cnl, 0.0);

		     cs->cpool += excess_allocation_to_leaf;
		     ns->npool += excess_allocation_to_leaf / cnl;
		}
	}
	
	/*---------------------------------------------------------------------------
	if ((current_date.month == 5) && (current_date.day < 10))
	printf(" \n %lf %lf %lf %lf %lf %lf ",
	gresp_store, cdf->cpool_to_leafc, cdf->cpool_to_leafc_store, cs->leafc,
	cs->leafc_store, cdf->psn_to_cpool);
	---------------------------------------------------------------------------*/
	return(!ok);
} /* end daily_allocation.c */

