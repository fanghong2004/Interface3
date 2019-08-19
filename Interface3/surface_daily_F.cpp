//-------------------------------------------------------------------------------------------------------------------------------
//			surface_daily_F			
//								
//	NAME							
//	canopy_statrum_daily_F 					
//								
//								
//	SYNOPSIS						
//	void surface_daily_F 				
//								
//	OPTIONS							
//								
//	DESCRIPTION						
//								
//	PROGRAMMER NOTES					
//-------------------------------------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include <stdio.h>

#include <iostream>
#include "CHESS.h"
#include "Constants.h"
using namespace std;

void		surface_daily_F(
							struct	patch_object		*patch,
							struct 	command_line_object	*command_line,
							struct 	date 			current_date)
{
	//--------------------------------------------------------------
	//	Local function declaration				
	//--------------------------------------------------------------
	double	compute_diffuse_radiative_fluxes(
		int,
		double *,
		double,
		double,
		double,
		double,
		double,
		double);
	
	double	compute_direct_radiative_fluxes(
		int,
		double *,
		double,
		double,
		double,
		double,
		double,
		double);
	
	
	double	compute_direct_radiative_PAR_fluxes(
		int,
		double *,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double);

	
	double	compute_nonvascular_stratum_conductance(
		int	,
		double	,
		double	,
		double	,
		double	,
		double	);

	double	 compute_litter_rain_stored(
		int,
		struct	patch_object *);
	
	double	penman_monteith(
		int,
		double,
		double,
		double,
		double,
		double,
		double,
		int);
	
//-------------------------------------------------------------------------------------------------------------------------------
//  Local variable definition.                                  
//-------------------------------------------------------------------------------------------------------------------------------
	double  albedo;
	double	dry_evaporation;
	double	Kstar_direct;
	double	Kstar_diffuse;
	double	APAR_diffuse;
	double	APAR_direct;
	double	potential_evaporation_rate;
	double	potential_rainy_evaporation_rate;
	double	rainy_evaporation;
	double	rnet_evap, rnet_evap_litter, rnet_evap_soil;
	double	PE_rate, PE_rainy_rate;
	double	soil_potential_evaporation;
	double	soil_potential_dry_evaporation_rate;
	double	soil_potential_rainy_evaporation_rate;
	double	exfiltration;
	struct	litter_object	*litter;
	double balance;
	
	litter = &(patch->litter);
    //---------------------------------------------------------------------------------------------------------------------------
	//	Initialize litter variables.				
    //---------------------------------------------------------------------------------------------------------------------------
	Kstar_diffuse     = 0.0;
	APAR_diffuse      = 0.0;
	Kstar_direct      = 0.0;
	APAR_direct       = 0.0;
	exfiltration      = 0.0;
	rainy_evaporation = 0.0;
	dry_evaporation   = 0.0;
    //---------------------------------------------------------------------------------------------------------------------------
	//	calculate surface albedo as a function of amount of	litter vs soil					
    //---------------------------------------------------------------------------------------------------------------------------
	if (litter->proj_pai >= 1.0)
		albedo = LITTER_ALBEDO;
	else
		albedo = LITTER_ALBEDO * litter->proj_pai + patch->soil_defaults->albedo * (1-litter->proj_pai);

    //---------------------------------------------------------------------------------------------------------------------------
	//  Intercept diffuse radiation.                                
	//  We assume that the zone slope == patch slope.               
	//  We also assume that radiation reflected into the upper hemisphere is lost.                                     
	//  We do not make adjustements for chaanging gap fraction over the diurnal cycle - using a suitable mean gap fraction instead.                                                
	//  We do take into account the patch level horizon which will allow simulation of clear-cuts/clearings with low sza's 
	//  Note that for zone level temperature and radiation computation we took into account only zone level horizon
	//      since we figured that climate above the zone was well mixed.                                                  
    //---------------------------------------------------------------------------------------------------------------------------
	Kstar_diffuse = compute_diffuse_radiative_fluxes(
		command_line->verbose_flag,
		&(patch->Kdown_diffuse),
		(patch->Kdown_direct),
		-10000.0,
		0.0,
		1.0,
		patch->theta_noon,
		albedo);
	
	APAR_diffuse = compute_diffuse_radiative_fluxes(
		command_line->verbose_flag,
		&(patch->PAR_diffuse),
		patch->PAR_direct,
		-10000.0,
		0.0,
		1.0,
		patch->theta_noon,
		albedo);

    //---------------------------------------------------------------------------------------------------------------------------
	//  Intercept direct radiation.                                 
	//      hard to measure for each strata.  We could use top      
	//      of canopy albedo but this integrates the effect of the  
	//      entire canopy.  Furthermore, it requires in general     
	//      knowledge of the canopy BRDF - which we want to avoid.  
	//      Instead we assume a certain reflectance and             
	//      transmittance for each strata's canopy elements.        
	//  We assume that the zone slope == patch slope.               
	//  We also assume that radiation reflected into the upper      
	//      hemisphere is lost.                                     
	//  We do not make adjustements for chaanging gap fraction over 
	//      the diurnal cycle - using a suitable mean gap fraction  
	//      instead.                                                
	//  We do take into account the patch level horizon which will  
	//      allow simulation of clear-cuts/clearings with low sza's 
	//  Note that for zone level temperature and radiation          
	//      computation we took into account only zone level horizon
	//      since we figured that climate above the zone was well   
	//      mixed.                                                  
    //---------------------------------------------------------------------------------------------------------------------------
	Kstar_direct = compute_direct_radiative_fluxes(
		command_line->verbose_flag,
		&(patch->Kdown_direct),
		-10000,
		0.0,
		1.0,
		patch->theta_noon,
		albedo,
		albedo);
	APAR_direct = compute_direct_radiative_fluxes(
		command_line->verbose_flag,
		&(patch->PAR_direct),
		-10000,
		0.0,
		1.0,
		patch->theta_noon,
		albedo,
		albedo);

    //---------------------------------------------------------------------------------------------------------------------------
	//	Determine non-vascular condductance to evaporation.		
	//	This conductance represnets the inverse of an additional resistance to vapour flux from the stratum rian storage		
	//	surface over and above aerodynamic resistances that also affect turbulent heat transfer.  						 	
	//	                            								
	//	A linear relationship is currently assumed with the amount of relative rain stored - or with the patch unsat zone storage  
	//	relative to water equiv depth top water table if roots are present Parameters for the relationship	
	//	are supplied via the stratum default file.					
    //---------------------------------------------------------------------------------------------------------------------------
	litter->gsurf = compute_nonvascular_stratum_conductance(
			command_line->verbose_flag,
			litter->rain_stored + patch->rain_throughfall,
			litter->rain_capacity,
			litter->gl_c,	
			litter->gsurf_slope,
			litter->gsurf_intercept);
	
	if (patch->rootzone.depth > ZERO)
		patch->gsurf = compute_nonvascular_stratum_conductance(
			command_line->verbose_flag,
			patch->rz_storage + patch->rain_throughfall,
			patch->sat_deficit,
			patch->soil_defaults->gl_c,	
			patch->soil_defaults->gsurf_slope,
			patch->soil_defaults->gsurf_intercept);

	else
		patch->gsurf = compute_nonvascular_stratum_conductance(
			command_line->verbose_flag,
			patch->unsat_storage + patch->rain_throughfall,
			patch->sat_deficit,
			patch->soil_defaults->gl_c,	
			patch->soil_defaults->gsurf_slope,
			patch->soil_defaults->gsurf_intercept);
    //---------------------------------------------------------------------------------------------------------------------------
	//	Compute evaporation and exfiltration RATES (m/s) for daylight period. The rainy rate assumes a vpd of 10Pa.			
	//	Note if surface heat flux makes evap negative we have condensation.  					
    //---------------------------------------------------------------------------------------------------------------------------
	if(patch->metv.dayl > ZERO)
		rnet_evap = 1000 * (Kstar_direct + Kstar_diffuse + patch->Lstar_soil + patch->surface_heat_flux) / patch->metv.dayl;
	else 
		rnet_evap = 0.0;

	rnet_evap_litter = min(litter->proj_pai,1)*rnet_evap;
	rnet_evap_soil   = max(0.0, rnet_evap-rnet_evap_litter);

	if (rnet_evap < 0.0) rnet_evap = 0.0;
    //---------------------------------------------------------------------------------------------------------------------------
	//	Make sure ga and gsurf are non-zero.			
    //---------------------------------------------------------------------------------------------------------------------------
	patch->ga = max((patch->ga * patch->stability_correction),0.0001);
  
	//---------------------------------------------------------------------------------------------------------------------------
	//	Estimate potential evap rates.				
    //---------------------------------------------------------------------------------------------------------------------------
	potential_evaporation_rate = penman_monteith(
		command_line->verbose_flag,
		patch->metv.tday,
		patch->metv.pa,
		patch->metv.vpd,
		rnet_evap_litter,
		1/patch->litter.gsurf,
		1/(patch->ga),
		2) ;

 	potential_rainy_evaporation_rate = penman_monteith(
		command_line->verbose_flag,
		patch->metv.tday,
		patch->metv.pa,
		10,
		rnet_evap_litter,
		1/patch->litter.gsurf,
		1/(patch->ga),
		2) ;

	PE_rate = penman_monteith(
		command_line->verbose_flag,
		patch->metv.tday,
		patch->metv.pa,
		patch->metv.vpd,
		rnet_evap_litter,
		0.0,
		1/(patch->ga),
		2) ;

	PE_rainy_rate = penman_monteith(
		command_line->verbose_flag,
		patch->metv.tday,
		patch->metv.pa,
		10,
		rnet_evap_litter,
		0.0,
		1/(patch->ga),
		2) ;

	PE_rainy_rate = max(0, PE_rainy_rate);
	PE_rate       = max(0, PE_rate);


  	potential_evaporation_rate       = max(0,potential_evaporation_rate);
	potential_rainy_evaporation_rate = max(0,potential_rainy_evaporation_rate);
    //---------------------------------------------------------------------------------------------------------------------------
	//	Do not allow negative potential evap if it raining since condensation/dew dep is the same as rain		
    //---------------------------------------------------------------------------------------------------------------------------
	if ( patch->rain > 0 ){
		potential_evaporation_rate       = max(0,potential_evaporation_rate);
		potential_rainy_evaporation_rate = max(0,potential_rainy_evaporation_rate);
	}
    
	//---------------------------------------------------------------------------------------------------------------------------
	//	Compute potential evaporation of litter. 		
	//	Weighted by rain and non rain periods of the daytime	
	//	m/day = m/s * (sec/day)					
	//								
	//	Note that Kstar is converted from Kj/m2*day to W/m2	
    //---------------------------------------------------------------------------------------------------------------------------
	patch->potential_evapotranspiration = potential_evaporation_rate	* (patch->metv.dayl - patch->daytime_rain_duration)
									+ potential_rainy_evaporation_rate * patch->daytime_rain_duration;
	patch->PET = PE_rate * (patch->metv.dayl - patch->daytime_rain_duration)+ PE_rainy_rate * patch->daytime_rain_duration;

	//if (patch->potential_evapotranspiration < 0.  && patch->ID==3842){ //
	//	cout << patch->evaporation_surf_litter << " " << potential_evaporation_rate << " " << potential_rainy_evaporation_rate << endl;
	//	cout << patch->potential_evapotranspiration << endl;
	//	cout << patch->metv.dayl << " " << patch->daytime_rain_duration << endl;
	//	cout << patch->ID << " surface daily f " << endl;
	//	getchar();
	//}	
    //---------------------------------------------------------------------------------------------------------------------------
	//	Update rain storage ( this also updates the patch level rain_throughfall and potential_evaporation	
    //---------------------------------------------------------------------------------------------------------------------------
	balance = patch->litter.preday_rain_stored + patch->rain_throughfall;
	litter->rain_stored  = compute_litter_rain_stored(command_line->verbose_flag, patch);



    //---------------------------------------------------------------------------------------------------------------------------
	//	if litter is empty then we have bare soil		
	//								
	//	We assume that the exfiltration rate is limited to the minimum of the potential evap rate for the 		
	//	soil and the max exfiltration rate.			
	//	The potential evap rate is estimated using P-M with the same aero cond as the litter but with a 		
	//	surface cond estimated based on vapour conductance in soil column.						
	//								
	//	The surface heat flux of the soil column is estimated aasuming no litter covering the surface (0 m height).	
    //---------------------------------------------------------------------------------------------------------------------------
	soil_potential_rainy_evaporation_rate = penman_monteith(
		command_line->verbose_flag,
		patch->metv.tday,
		patch->metv.pa,
		10.0,
		rnet_evap_soil,
		1.0/patch->gsurf,
		1.0/patch->ga,
		2);
	
	soil_potential_dry_evaporation_rate = penman_monteith(
		command_line->verbose_flag,
		patch->metv.tday,
		patch->metv.pa,
		patch->metv.vpd,
		rnet_evap_soil,
		1.0/patch->gsurf,
		1.0/patch->ga,
		2);
	
	soil_potential_evaporation  = soil_potential_dry_evaporation_rate
		* (patch->metv.dayl - patch->daytime_rain_duration )
		+ soil_potential_rainy_evaporation_rate
		* patch->daytime_rain_duration;
	
	patch->PET                          += soil_potential_evaporation; //added by guoping litter + soil potential ET
	patch->potential_evapotranspiration += soil_potential_evaporation; //added by guoping litter + soil potential ET


    //---------------------------------------------------------------------------------------------------------------------------
	//	base soil evapotration/ exfiltration will only occur on exposed soil layers					
    //---------------------------------------------------------------------------------------------------------------------------
	exfiltration =	min(soil_potential_evaporation,	patch->potential_exfiltration);

	if ( patch->sat_deficit_z > 0 ){
		patch->exfiltration_unsat_zone = exfiltration;
		patch->exfiltration_sat_zone   = 0;
	}
	else{
		patch->exfiltration_unsat_zone = 0;
		patch->exfiltration_sat_zone   = exfiltration;
	}

	balance=balance -(patch->litter.rain_stored+patch->evaporation_surf_litter+patch->rain_throughfall);
	/*
	if(balance> 0.0000001 || balance < -0.00000001){
		printf(" surface daily balance is not zero patch_daily.f %12.8f \n",balance);
		printf(" year month day patchid %d %d %d %d \n",current_date.year, current_date.month,current_date.day,patch->ID);
		getchar();
	}
	*/
	return;
} //end surface_daily_F.c
