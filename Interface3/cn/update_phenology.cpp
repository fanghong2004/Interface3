/*--------------------------------------------------------------*/
/*                                                              */
/*		update_phenology				*/
/*                                                              */
/*  NAME                                                        */
/*		update_phenology				*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/* 	void update_phenology(										*/
/*			struct zone *										*/
/*			struct epv_struct *,		                        */
/*			struct epconst_struct,   		*/
/*                      struct phenology_struct *,              */
/*                      struct cstate_struct *,                 */
/*                      struct cdayflux_struct *,               */
/*                      struct cdayflux_patch_struct *,         */
/*                      struct nstate_struct *,                 */
/*                      struct ndayflux_struct *,               */
/*                      struct ndayflux_patch_struct *,         */
/*                      struct litter_c_object *,               */
/*                      struct litter_n_object *,               */
/*                      struct soil_c_object *,                 */
/*                      struct soil_n_object *,                 */
/*                      double,                                 */
/*			struct date current_date,		*/
/*		 	int	grow_flag);			*/
/*                                  				*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*	performs seasonal leaf drop and budding   		*/
/*	updates annnual allocates during leaf out		*/
/*	computes leaf and fine root litfall			*/
/*                                                              */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*	modifed from phenology and prephenology in		*/
/*	P.Thornton (1997) version of 1d_bgc			*/
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>

#include "Chess.h"
#include "constants.h"
#include <iostream>
using namespace std;

void update_phenology(struct patch_object  *patch,
struct epvar_struct	*epv,
struct epconst_struct	epc,
struct phenology_struct *phen,
struct cstate_struct *cs,
struct cdayflux_struct	*cdf,
struct cdayflux_patch_struct *cdf_patch,
struct nstate_struct *ns,
struct ndayflux_struct	*ndf,
struct ndayflux_patch_struct *ndf_patch,
struct litter_c_object *cs_litr,
struct litter_n_object *ns_litr,
struct soil_c_object *cs_soil,
struct soil_n_object *ns_soil,
struct rooting_zone_object *rootzone,
	double	effective_soil_depth,
	double	cover_fraction,
	double	gap_fraction,
	double	theta_noon,
struct date current_date,
	int	grow_flag)
{
	/*--------------------------------------------------------------*/
	/*  Local function declaration                                  */
	/*--------------------------------------------------------------*/
	long	yearday(struct date);
	int	update_rooting_depth(
	struct rooting_zone_object *,
		double,
		double,
		double,
		double);
	int	compute_annual_litfall(
	struct epconst_struct,
	struct phenology_struct *,
	struct cstate_struct *,
		int);
	int     compute_cwd_decay(struct epconst_struct *,
		double,
	struct cstate_struct *,
	struct nstate_struct *,
	struct litter_c_object *,
	struct litter_n_object *,
	struct cdayflux_patch_struct *,
	struct ndayflux_patch_struct *,
	struct ndayflux_struct *);
	int	compute_deadleaf_turnover(
	struct epconst_struct,
	struct epvar_struct *,
		double,
	struct cstate_struct *,
	struct nstate_struct *,
	struct litter_c_object *,
	struct litter_n_object *,
	struct cdayflux_patch_struct *,
	struct ndayflux_patch_struct *,
		int);
	int	compute_leaf_litfall(
	struct epconst_struct,
		double,
		double,
	struct cstate_struct *,
	struct nstate_struct *,
	struct litter_c_object *,
	struct litter_n_object *,
	struct cdayflux_patch_struct *,
	struct ndayflux_patch_struct *,
	struct cdayflux_struct *,
	struct ndayflux_struct *,
		int);
	int	compute_froot_litfall(
	struct epconst_struct,
		double,
		double,
	struct cstate_struct *,
	struct nstate_struct *,
	struct litter_c_object *,
	struct litter_n_object *,
	struct cdayflux_patch_struct *,
	struct ndayflux_patch_struct *);

	double compute_growingseason_index(
	struct patch_object *,
	struct epconst_struct
		);

	/*--------------------------------------------------------------*/
	/*  Local variable definition.                                  */
	/*--------------------------------------------------------------*/
	int ok = 1;
	long day;
	double perc_sunlit, leaflitfallc, frootlitfallc;
	double	rootc, sai, new_proj_lai_sunlit;
	double excess_n;
	double remdays_transfer;
	int expand_flag, litfall_flag, DaysOfYear;


	// determine the number of days in a year
	if ((current_date.year % 4 == 0 && current_date.year % 100 != 0) || current_date.year % 400 == 0)
		DaysOfYear = 366;
	else
		DaysOfYear = 365;
	day = yearday(current_date);

	/*-------------------------------------------------------------------------------------------*/
	/* dynamic phenology											                             */
	/*-------------------------------------------------------------------------------------------*/
	phen->gsi = compute_growingseason_index(patch, epc);

	/*
    if (phen->gsi > 0.5 && current_date.month<=6 && patch->leaf_expand_flag==-999) {
		phen->expand_startday = day;
		patch->leaf_expand_flag = 1;
	}
	if (phen->gsi < 0.5 && current_date.month >= 8 && patch->litfall_stop_flag ==-999) {
		phen->litfall_stopday = day;
		patch->litfall_stop_flag = 1;
	}*/
	

	phen->expand_startday = 100;
	phen->litfall_stopday = 300;
	if(fabs(phen->expand_startday-day)<0.01) patch->leaf_expand_flag = 1;
	if (fabs(phen->litfall_stopday - day) < 0.01) patch->litfall_stop_flag = 1;


	
	//===============================================================================================
	// leaf on flag  for compute_maintain respiration
	//===============================================================================================
	patch->leaf_on_flag = -999;
	if (epc.phenology_type == DECID){
		if (day >= phen->expand_startday && day <= patch->preyear_litfall_stopday)  //phen->litfall_stopday{
			patch->leaf_on_flag = 1;

		else
			patch->leaf_on_flag = -999;

	}
	else if (epc.phenology_type == EVERGREEN){
		patch->leaf_on_flag = 1;
	}
	
	phen->daily_allocation = epc.alloc_prop_day_growth;

	/*-------------------------------------------------------------------------------------------*/
	/*	Leaf expansion - spring leaf out			                                             */
	/*-------------------------------------------------------------------------------------------*/
	if (epc.phenology_type == DECID && day >= phen->expand_startday && day <= patch->preyear_litfall_stopday) { //phen->litfall_stopday
		remdays_transfer = max(patch->preyear_litfall_stopday - phen->expand_startday, 1); //phen->litfall_stopday

		cdf->leafc_transfer_to_leafc = 2.0 * cs->leafc_transfer / remdays_transfer;
		ndf->leafn_transfer_to_leafn = 2.0 * ns->leafn_transfer / remdays_transfer;
		cdf->frootc_transfer_to_frootc = 2.0 * cs->frootc_transfer / remdays_transfer;
		ndf->frootn_transfer_to_frootn = 2.0 * ns->frootn_transfer / remdays_transfer;

		if (epc.veg_type == TREE) {
			cdf->livestemc_transfer_to_livestemc = 2.0 * cs->livestemc_transfer / remdays_transfer;
			ndf->livestemn_transfer_to_livestemn = 2.0 * ns->livestemn_transfer / remdays_transfer;
			cdf->deadstemc_transfer_to_deadstemc = 2.0 * cs->deadstemc_transfer / remdays_transfer;
			ndf->deadstemn_transfer_to_deadstemn = 2.0 * ns->deadstemn_transfer / remdays_transfer;
			cdf->livecrootc_transfer_to_livecrootc = 2.0 * cs->livecrootc_transfer / remdays_transfer;
			ndf->livecrootn_transfer_to_livecrootn = 2.0 * ns->livecrootn_transfer / remdays_transfer;
			cdf->deadcrootc_transfer_to_deadcrootc = 2.0 * cs->deadcrootc_transfer / remdays_transfer;
			ndf->deadcrootn_transfer_to_deadcrootn = 2.0 * ns->deadcrootn_transfer / remdays_transfer;
		}
	}
	else if (epc.phenology_type == EVERGREEN){
		/* calculate rates required to empty each transfer compartment by the end of transfer period, at approximately a
		constant rate of transfer */
		remdays_transfer = DaysOfYear;

		cdf->leafc_transfer_to_leafc = cs->leafc_transfer / remdays_transfer;
		ndf->leafn_transfer_to_leafn = ns->leafn_transfer / remdays_transfer;
		cdf->frootc_transfer_to_frootc = cs->frootc_transfer / remdays_transfer;
		ndf->frootn_transfer_to_frootn = ns->frootn_transfer / remdays_transfer;
		if (epc.veg_type == TREE)
		{
			cdf->livestemc_transfer_to_livestemc = cs->livestemc_transfer / remdays_transfer;
			ndf->livestemn_transfer_to_livestemn = ns->livestemn_transfer / remdays_transfer;
			cdf->deadstemc_transfer_to_deadstemc = cs->deadstemc_transfer / remdays_transfer;
			ndf->deadstemn_transfer_to_deadstemn = ns->deadstemn_transfer / remdays_transfer;
			cdf->livecrootc_transfer_to_livecrootc = cs->livecrootc_transfer / remdays_transfer;
			ndf->livecrootn_transfer_to_livecrootn = ns->livecrootn_transfer / remdays_transfer;
			cdf->deadcrootc_transfer_to_deadcrootc = cs->deadcrootc_transfer / remdays_transfer;
			ndf->deadcrootn_transfer_to_deadcrootn = ns->deadcrootn_transfer / remdays_transfer;
		}
	}


	/*-------------------------------------------------------------------------------------------*/
	/*	Leaf drop - fall litter fall				                                             */
	/*-------------------------------------------------------------------------------------------*/
	/*-------------------------------------------------------------------------------------------*/
	/* at beginning of litter fall figure out how much to drop                                   */
	/*-------------------------------------------------------------------------------------------*/
	/*if (phen->expand_startday == -999)
		phen->litfall_startday = int(DaysOfYear / 2);
	else*/
	phen->litfall_startday = int((patch->preyear_litfall_stopday + phen->expand_startday) / 2);

	litfall_flag = 0.0;
	if (epc.phenology_type == DECID && day >= phen->litfall_startday && day <= patch->preyear_litfall_stopday)  {//phen->litfall_stopday
		litfall_flag = 1;
		ok = compute_annual_litfall(epc, phen, cs, grow_flag);
		ok = 1;
	}
	else if (epc.phenology_type == EVERGREEN && day == DaysOfYear)  {
		ok = compute_annual_litfall(epc, phen, cs, grow_flag);
		ok = 1;
	}

	/*-------------------------------------------------------------------------------------------*/
	/*	compute daily litter fall				                                                 */
	/*-------------------------------------------------------------------------------------------*/
	leaflitfallc = 0.0;
	frootlitfallc = 0.0;
	if (epc.phenology_type == DECID && litfall_flag == 1) {
		if (day <= patch->preyear_litfall_stopday)
			remdays_transfer = max(1, (patch->preyear_litfall_stopday - day)); 
		else
			remdays_transfer = max(1, patch->preyear_litfall_stopday + DaysOfYear - day); 

		leaflitfallc =phen->leaflitfallc / remdays_transfer;
		if (leaflitfallc > cs->leafc)
			leaflitfallc = cs->leafc;

		frootlitfallc = phen->frootlitfallc / remdays_transfer;
		if (frootlitfallc > cs->frootc)
			frootlitfallc = cs->frootc;
	}
	else if (epc.phenology_type == EVERGREEN ) {

		leaflitfallc = phen->leaflitfallc;
		if (leaflitfallc > cs->leafc)
			leaflitfallc = cs->leafc;

		frootlitfallc = phen->frootlitfallc;
		if (frootlitfallc > cs->frootc)
			frootlitfallc = cs->frootc;
	}

	/*-------------------------------------------------------------------------------------------*/
	/*	on the last day of litterfall make sure that deciduous no longer have any leaves left	 */
	/*	this is also considered to be the end of the growing season so do annual allocation		 */
	/*-------------------------------------------------------------------------------------------*/
	phen->annual_allocation = 0;
	if (day == patch->preyear_litfall_stopday && epc.phenology_type == DECID){  //phen->litfall_stopday
		leaflitfallc = cs->leafc;
		phen->daily_allocation  = 0;
		phen->annual_allocation = 1;
	}
	else if (day == DaysOfYear && epc.phenology_type == EVERGREEN){
		phen->annual_allocation = 1;
	}

	/*-------------------------------------------------------------------------------------------*/
	/* update growth variables					                                                 */
	/* this is expression of allocation from last seasons stored photosynthesis. note all cdf    */
	/* and ndf variables are zero'd at the start of the day, so only values set above are used	 */
	/*-------------------------------------------------------------------------------------------*/
	/* Leaf carbon transfer growth */
	cs->leafc += cdf->leafc_transfer_to_leafc;
	cs->leafc_transfer -= cdf->leafc_transfer_to_leafc;

	/* Leaf nitrogen transfer growth */
	ns->leafn += ndf->leafn_transfer_to_leafn;
	ns->leafn_transfer -= ndf->leafn_transfer_to_leafn;

	if (grow_flag > 0) {
		cs->frootc += cdf->frootc_transfer_to_frootc;
		cs->frootc_transfer -= cdf->frootc_transfer_to_frootc;
		ns->frootn += ndf->frootn_transfer_to_frootn;
		ns->frootn_transfer -= ndf->frootn_transfer_to_frootn;
		if (epc.veg_type == TREE){
			/* Stem and coarse root transfer growth */
			cs->live_stemc += cdf->livestemc_transfer_to_livestemc;
			cs->livestemc_transfer -= cdf->livestemc_transfer_to_livestemc;
			cs->dead_stemc += cdf->deadstemc_transfer_to_deadstemc;
			cs->deadstemc_transfer -= cdf->deadstemc_transfer_to_deadstemc;
			cs->live_crootc += cdf->livecrootc_transfer_to_livecrootc;
			cs->livecrootc_transfer -= cdf->livecrootc_transfer_to_livecrootc;
			cs->dead_crootc += cdf->deadcrootc_transfer_to_deadcrootc;
			cs->deadcrootc_transfer -= cdf->deadcrootc_transfer_to_deadcrootc;

			/* nitrogen transfer */
			ns->live_stemn += ndf->livestemn_transfer_to_livestemn;
			ns->livestemn_transfer -= ndf->livestemn_transfer_to_livestemn;
			ns->dead_stemn += ndf->deadstemn_transfer_to_deadstemn;
			ns->deadstemn_transfer -= ndf->deadstemn_transfer_to_deadstemn;
			ns->live_crootn += ndf->livecrootn_transfer_to_livecrootn;
			ns->livecrootn_transfer -= ndf->livecrootn_transfer_to_livecrootn;
			ns->dead_crootn += ndf->deadcrootn_transfer_to_deadcrootn;
			ns->deadcrootn_transfer -= ndf->deadcrootn_transfer_to_deadcrootn;

		}
	}	/* end of grow processing */



	/*-------------------------------------------------------------------------------------------*/
	/* check for leaf and fine root litfall for this day                                         */
	/*-------------------------------------------------------------------------------------------*/
	if ((leaflitfallc > ZERO) && (cs->leafc > ZERO) && (ns->leafn > ZERO)){
		/*---------------------------------------------------------------------------------------*/
		/* set daily flux variables                                                              */
		/*---------------------------------------------------------------------------------------*/
		/*	compute leaf litter fall				                                             */
		/*---------------------------------------------------------------------------------------*/

		if (ok && compute_leaf_litfall(epc,
			leaflitfallc, cover_fraction,
			cs, ns, cs_litr, ns_litr, cdf_patch, ndf_patch, cdf, ndf, grow_flag)){
			fprintf(stderr,
				"FATAL ERROR: in leaf_litfall() from update_phenology()\n");
			exit(0);
		}
		//phen->leaflitfallc -= leaflitfallc;
		if (phen->leaflitfallc < 0)
			phen->leaflitfallc = 0;
	}
	

	/*-------------------------------------------------------------------------------------------*/
	/*	add additional leaf litterfall if water stress conditions occur. only drop when          */
	/*  accumulated litterfall due to water stress is greater than usual litterfall			     */
	/*-------------------------------------------------------------------------------------------*/
	/*
	if (epv->psi < epc.psi_open)	{
	perc_leaflitfall = (1.0 / (epc.psi_close - epc.psi_open) *
	epv->psi + (-epc.psi_open) /
	(epc.psi_close - epc.psi_open)) / 100.0;
	leaflitfallc = (perc_leaflitfall * cs->leafc);
	phen->leaflitfallc_wstress += leaflitfallc;
	if ((phen->leaflitfallc_wstress > phen->leaflitfallc) &&
	(phen->leaflitfallc_wstress < 1.5 * phen->leaflitfallc)) {
	if (ok && compute_leaf_litfall(epc,
	leaflitfallc,cover_fraction,
	cs,ns, cs_litr,ns_litr,cdf_patch,ndf_patch, cdf,ndf, grow_flag)){
	fprintf(stderr,
	"FATAL ERROR: in leaf_litfall() from update_phenology()\n");
	exit(0);
	}
	}
	}
	*/

	if ((frootlitfallc > 0.0) && (grow_flag > 0) && (cs->frootc > ZERO) && (ns->frootn > ZERO)){
		/*----------------------------------------------------------------------------------------*/
		/*	compute fine root turnover				                                              */
		/*----------------------------------------------------------------------------------------*/
		if (ok && compute_froot_litfall(epc, frootlitfallc,
			cover_fraction, cs, ns, cs_litr,
			ns_litr, cdf_patch, ndf_patch)){
			fprintf(stderr,
				"FATAL ERROR: in froot_litfall() from update_phenology()\n");
			exit(0);
		}
		//phen->frootlitfallc -= frootlitfallc;
		if (phen->frootlitfallc < 0)
			phen->frootlitfallc = 0;
	}
	
	/*-------------------------------------------------------------------------------------------*/
	/*	tree wood turnovers and dead standing grass turnovers	                                 */
	/*	- note this is not mortality but turnover rates		                                     */
	/*-------------------------------------------------------------------------------------------*/
	if (((epc.veg_type == GRASS) || (epc.veg_type == C4GRASS))
		&& (grow_flag > 0) && (cs->dead_leafc > ZERO)) {
		if (ok && compute_deadleaf_turnover(epc, epv, cover_fraction, cs, ns,
			cs_litr, ns_litr, cdf_patch, ndf_patch, grow_flag)){
			fprintf(stderr, "FATAL ERROR: in compute_deadleaf_turnover() from update_phenology()\n");
			exit(0);
		}
	}

	if ((epc.veg_type == TREE) && (grow_flag > 0)) {
		/*---------------------------------------------------------------------------------------*/
		/*	compute coarse woody debris fragmentation		                                     */
		/*---------------------------------------------------------------------------------------*/
		if ((cs->cwdc > ZERO) && (cover_fraction > ZERO)) {
			if (ok && compute_cwd_decay(&(epc), cover_fraction, cs, ns, cs_litr,
				ns_litr, cdf_patch, ndf_patch, ndf)){
				fprintf(stderr,
					"FATAL ERROR: in cwd_decay() from update_phenology()\n");
				exit(0);
			}
		}
		/*---------------------------------------------------------------------------------------*/
		/*	compute live steam and coarse root turnover		                                     */
		/*---------------------------------------------------------------------------------------*/
		if (cs->live_stemc > ZERO) {
			cdf->livestemc_to_deadstemc = min(epv->day_livestem_turnover, cs->live_stemc);
			ndf->livestemn_to_deadstemn = min(cdf->livestemc_to_deadstemc
				/ epc.livewood_cn, ns->live_stemn);
			excess_n = max(0.0, ndf->livestemn_to_deadstemn -
				(cdf->livestemc_to_deadstemc / epc.deadwood_cn));
			ns->npool += excess_n;
			cs->live_stemc -= cdf->livestemc_to_deadstemc;
			cs->dead_stemc += cdf->livestemc_to_deadstemc;
			ns->live_stemn -= ndf->livestemn_to_deadstemn;
			ns->dead_stemn += (ndf->livestemn_to_deadstemn - excess_n);
		}
		if (cs->live_crootc > ZERO) {
			cdf->livecrootc_to_deadcrootc = min(epv->day_livecroot_turnover, cs->live_crootc);
			ndf->livecrootn_to_deadcrootn = min(cdf->livecrootc_to_deadcrootc
				/ epc.livewood_cn, ns->live_crootn);

			excess_n = max(0.0, ndf->livecrootn_to_deadcrootn -
				(cdf->livecrootc_to_deadcrootc / epc.deadwood_cn));
			ns->npool += excess_n;
			cs->live_crootc -= cdf->livecrootc_to_deadcrootc;
			cs->dead_crootc += cdf->livecrootc_to_deadcrootc;
			ns->live_crootn -= ndf->livecrootn_to_deadcrootn;
			ns->dead_crootn += (ndf->livecrootn_to_deadcrootn - excess_n);
		}
	} /* end tree processing */

	if (grow_flag == 0) { /* non-grow processing */
		/*---------------------------------------------------------------------------------------*/
		/* update state variables assumes no retranslocation                                     */
		/*	this is done in literfall routine			                                         */
		/*---------------------------------------------------------------------------------------*/
		/*
		cs->leafc        -= leaflitfallc;
		cs->leafc_store  += leaflitfallc;
		ns->leafn        -= leaflitfallc / epc.leaf_cn;
		ns->leafn_store  += leaflitfallc / epc.leaf_cn;
		*/
	}

	/*-------------------------------------------------------------------------------------------*/
	/* check for rounding errors on end of litfall season                                        */
	/*-------------------------------------------------------------------------------------------*/
	if (fabs(cs->leafc) <= 1e-13){
		cs->leafc = 0.0;
		ns->leafn = 0.0;
	}
	if (fabs(cs->frootc) <= 1e-13){
		cs->frootc = 0.0;
		ns->frootn = 0.0;
	}

	/*-------------------------------------------------------------------------------------------*/
	/*	compute new rooting depth based on current root carbon                                   */
	/*-------------------------------------------------------------------------------------------*/
	rootc = cs->frootc + cs->live_crootc + cs->dead_crootc;

	if ((grow_flag > 0) && (rootc > ZERO)){
		if (ok && update_rooting_depth(
			rootzone, rootc, epc.root_growth_direction, epc.root_distrib_parm,
			effective_soil_depth)){
			fprintf(stderr,
				"FATAL ERROR: in compute_rooting_depth() from update_phenology()\n");
			exit(0);
		}
	}

	/*-------------------------------------------------------------------------------------------*/
	/* now figure out a sunlit and shaded flux density use Chen et al. 1999 Ecological Modelling */
	/* 124: 99-119 to estimate shaded and sunlit fractions then update lai based on sunlit/shaded*/
	/* sla. We need to do a predictor-corrector type convergence here since sunlit/shaded        */
	/* fraction depend on total proj_lai	                                                     */
	/*-------------------------------------------------------------------------------------------*/
	perc_sunlit = 0;
	if ((cs->leafc > ZERO) && (epc.veg_type != NON_VEG)) {
		epv->proj_lai = max((cs->leafc * (epv->proj_sla_sunlit * perc_sunlit +
			epv->proj_sla_shade * (1 - perc_sunlit))), 0.0);

		new_proj_lai_sunlit = 2.0 * cos(theta_noon) *
			(1.0 - exp(-0.5*(1 - gap_fraction)*epv->proj_lai / cos(theta_noon)));

		while (fabs(epv->proj_lai_sunlit - new_proj_lai_sunlit) > 0.00001*new_proj_lai_sunlit)  {
			epv->proj_lai_sunlit = new_proj_lai_sunlit;
			epv->proj_lai_shade = epv->proj_lai - epv->proj_lai_sunlit;

			if ((epv->proj_lai_sunlit + epv->proj_lai_shade) > ZERO)
				perc_sunlit = (epv->proj_lai_sunlit) / (epv->proj_lai_sunlit + epv->proj_lai_shade);
			else
				perc_sunlit = 1.0;

			epv->proj_lai = max((cs->leafc * (epv->proj_sla_sunlit * perc_sunlit +
				epv->proj_sla_shade * (1 - perc_sunlit))), 0.0);
			new_proj_lai_sunlit = 2.0 * cos(theta_noon) *
				(1.0 - exp(-0.5*(1 - gap_fraction)*
				epv->proj_lai / cos(theta_noon)));
		}
	}
	else {
		epv->proj_lai = 0.0;
		epv->proj_lai_sunlit = 0.0;
		epv->proj_lai_shade = 0.0;
	}

	/*-------------------------------------------------------------------------------------------*/
	/* update lai based on sla use sunlit sla for lai up to 1 and shaded sla for lai above that  */
	/*-------------------------------------------------------------------------------------------*/
	if ((epv->proj_lai_sunlit + epv->proj_lai_shade) > ZERO)
		perc_sunlit = (epv->proj_lai_sunlit) / (epv->proj_lai_sunlit + epv->proj_lai_shade);
	else
		perc_sunlit = 1.0;

	epv->all_lai = epv->proj_lai * epc.lai_ratio;

	//if (current_date.year == 1962 && current_date.month == 6 && current_date.day >= 5 && patch->ID == 4419){
	//	printf("update phenology 1.cpp %12.8f %12.8f \n", epv->proj_pai, epv->proj_lai);
	//	printf("current_date.year is %d %d %d \n", current_date.year, current_date.month, current_date.day);
	//	printf(" \n");
	//}

	if (epc.veg_type == TREE)  {
		sai = epc.proj_swa*(1.0 - exp(-0.175*(cs->live_stemc + cs->dead_stemc)));
		epv->proj_pai = max(epv->proj_lai + sai, 0.0);
		epv->all_pai  = max(epv->all_lai + sai, 0.0);
	}
	else {
		epv->proj_pai = epv->proj_lai;
		epv->all_pai  = epv->all_lai;
	}

	//if (current_date.year == 1962 && current_date.month == 6 && current_date.day>= 5 && patch->ID == 4419){
	//	printf("update phenology.cpp %12.8f %12.8f %d \n", epv->proj_pai, epv->proj_lai, epc.veg_type);
	//	printf("update phenology.cpp %12.8f %12.8f  %12.8f \n", sai, cs->live_stemc, cs->dead_stemc);
	//	printf("current_date.year is %d %d %d \n", current_date.year, current_date.month, current_date.day);
	//	printf(" \n");
	//}

	/*-------------------------------------------------------------------------------------------*/
	/*	update height						                                                     */
	/*-------------------------------------------------------------------------------------------*/
	if (epc.veg_type == TREE)
	if ((cs->leafc + cs->live_stemc + cs->dead_stemc) > ZERO)
		epv->height = epc.height_to_stem_coef
		* pow((cs->leafc + cs->live_stemc + cs->dead_stemc), epc.height_to_stem_exp); //originally, no cs->leafc
	else
		epv->height = 0.0;
	else
	if (epc.veg_type == NON_VEG) {
		epv->height = 0.0;
	}
	else {
		if (cs->leafc > ZERO)
			epv->height = epc.height_to_stem_coef * pow((cs->leafc), epc.height_to_stem_exp);
		else
			epv->height = 0.0;
	}

	/*-------------------------------------------------------------------------------------------*/
	/*	keep a seasonal max_lai for outputing purposes		                                     */
	/*-------------------------------------------------------------------------------------------*/
	if (day == phen->expand_startday)
		epv->max_proj_lai = 0.0;
	if ((day >= phen->expand_startday) && (day < phen->litfall_startday))
	if (epv->proj_lai > epv->max_proj_lai)
		epv->max_proj_lai = epv->proj_lai;

	return;

}/*end update phenology*/
