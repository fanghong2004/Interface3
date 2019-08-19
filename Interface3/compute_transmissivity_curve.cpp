/*--------------------------------------------------------------*/
/* 								*/
/*		compute_transmissivity_curve			*/
/*								*/
/*	NAME							*/
/*	compute_transmissivity_curve - estimates transmissivity	*/
/*		assuming an exponential decay of transmivivity	*/
/*		with depth - note returned value is 		*/
/*		relative transmissivity (i.e per unit		*/
/*		Ksat at the surface)				*/
/*								*/
/*								*/
/*	SYNOPSIS						*/
/*	compute_transmissivity_curve(				*/
/*				double	,			*/
/*				double	,			*/
/*				double	)			*/
/*								*/
/*	returns:						*/
/*	transmissivity - (unitless) multiplier for Ksat0 	*/
/*		to calculate transmissivity over range of	*/
/*		depths specified				*/
/*								*/
/*	OPTIONS							*/
/*	double	m - Ksat decay parameter			*/
/*	double	z - (m) depth to the water table		*/
/*	double  D - maximum depth				*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	computes transmissivity multiplier over range of 	*/
/*	depths between z and D; assumes an exponential decay	*/
/*	of Ksat with depth (decay given by m) until z_layer1	*/
/*	and then a constant conductivity below that layer	*/
/*	given by Ksat at the surface				*/
/*	Note that if m is 0, we assume that Ksat is constant    */
/*	with depth						*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					                        */
/*								                                */
/*--------------------------------------------------------------*/
#include "stdafx.h"
#include <stdio.h>

#include <math.h>
#include "CHESS.h"
#include "Constants.h"

double	compute_transmissivity_curve( 
					double	m,
					double  z_layer1,
					double	s1,
					double	smax
					)
{
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	double	transmissivity;
	//transmissivity = 0.0;
	 
	/*--------------------------------------------------------------*/
	/*	for do not include surface overland flow or detention   */
	/*	storage here						*/
	/*--------------------------------------------------------------*/
	if(s1 <= ZERO) s1=0.;
	if (s1 <= smax)  {
		if (m > ZERO){
			transmissivity = (exp ( -1.0 * (s1/ m)) - exp ( -1.0 * (smax/m))); 
		}
		else
			transmissivity = (smax - s1);
		}
	else  
		transmissivity = 0.0;
	
	return(transmissivity);
} /*compute_transmissivity_curve*/
