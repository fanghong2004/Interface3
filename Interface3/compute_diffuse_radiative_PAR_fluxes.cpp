/*								*/
/*		compute_diffuse_radiative_PAR_fluxes			*/
/*								*/
/*								*/
/*	NAME							*/
/*	compute_diffuse_radiative_PAR_fluxes 	-		*/
/*		updates downwelling diffuse radiative flux	*/
/*								*/
/*	SYNOPSIS						*/
/*	double	compute_radiative_PAR_fluxes(			*/
/*					int			*/
/*					double*,		*/
/*					double*,		*/
/*					double,			*/
/*					double,			*/
/*					double,			*/
/*					double,			*/
/*					double)			*/
/*								*/
/*	OPTIONS							*/
/*	*flux_down_ptr (Kj/m2*day) - downwelling flux.		*/
/*	extinction_coef (DIM) - extinction coef for nonopaquenes*/
/*	gap_fraction (DIM)- canopy gap fraction from  above	*/
/*	proj_lai (DIM)- projected area of layer element		*/
/*	theta_noon (rad) - solar zenith angle at noon		*/
/*	reflectance_canopy (DIM) - reflectance of this layer	*/
/*	reflectance_bkgd (DIM) - reflectance of lower layer	*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	Using equation 8.18 of 					*/
/*	Jarvis, P.G. and J.W. Leverenz,				*/
/*	"Productivity of Temperate, Deciduous, and 		*/
/*	Evergreen forests" ??					*/
/*	which in turn took it from Norman, J. (1981)		*/
/*	Simulation of Microclimates, in Hatfield, J,		*/
/*	Application of biometeorology to integrated pest 	*/
/*	management" Adademic Press.				*/
/*								*/
/*	Updates the radiative flux via a pointer		*/
/*	and returns the flux absorbed by the medium.		*/
/*								*/
/*	Note that the reflectance and absorptance should be	*/
/*	specified for the typical region not part of a gap.	*/
/*	Ideally, this corresponds to a big leaf's reflectance 	*/
/*	and absorptance required to match the hemispherical	*/
/*	albedo of the canopy areas that are intercepting the	*/
/*	radiation.  The Beer's law formulation actually applies	*/
/*	if we assume that the light intercepting elements are	*/
/*	opaque - the extinction coefficent can correct for this.*/
/*	The equation relies on the current ratio of direct to	*/
/*	diffuse irradiance and the solar zenith angle.		*/
/*	We only have daily values for these things so we make	*/
/*	approximations as noted.				*/
/*								*/
/*	We ignor between layer interactions for diffuse.	*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*								*/
/*	We keep the terminology general here since both Kdown	*/
/*	and PAR can be attenuated using this routine.		*/
/* 	 							*/
/*	July 28, 1997 C. Tague					*/
/*								*/
/* 	in calculation of S, min changed to max, bug fix	*/
/*								*/
/*	Nov 26. 1997	RAF					*/
/*	There was a bug in the way I implemented Norman's 	*/
/*	formula for diffuse penetration.  I should have had a 	*/
/*	factor of 0.5 in front of (PAI^0.7).			*/
/*								*/ 
/*--------------------------------------------------------------*/
#include "stdafx.h"
#include <stdio.h>

#include <math.h>
#include "CHESS.h"
#include "Constants.h"

double	compute_diffuse_radiative_PAR_fluxes(	int	verbose_flag,
											 double	*flux_down_ptr,                     
											 double	direct_flux,                     
											 double	extinction_coef,
											 double	gap_fraction,
											 double	lai,
											 double	theta_noon,
											 double	reflectance_canopy)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.						*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	
	double	flux_reflected;
	double	flux_transmitted;
	double	flux_absorbed;
	double	S;
	/*--------------------------------------------------------------*/
	/*	We assume that the reflectance_canopy is suitable for	*/
	/*	diffuse irradiance of the canopy.			*/
	/*--------------------------------------------------------------*/
	flux_reflected = *flux_down_ptr * reflectance_canopy;
	/*--------------------------------------------------------------*/
	/*	Add additional direct flux scattered into the diffuse	*/
	/*	stream.							*/
	/*								*/
	/*	We assume that the direct to diffuse ratio can be 	*/
	/*	approximated by the mean daily ratio and that we	*/
	/*	can use theta_noon instead of instantaneou theta.	*/
	/*	These assumptions may be ok since the direct to diffuse	*/
	/*	ratio is highest at theta_noon so theta_noon is a	*/
	/*	representative angle; also given that it is highest at	*/
	/*	theta noon, the mean daily value is likely to be close	*/
	/*	to theta noon.						*/
	/*								*/
	/*--------------------------------------------------------------*/
	if ( *flux_down_ptr != 0 ){
		S = 0.07 * ( direct_flux / *flux_down_ptr  ) *
			max(0,1.1- 0.1 * lai * (1- gap_fraction))  * exp( -1 * cos(theta_noon));
	}
	else{
		S = 0;
	}
	flux_absorbed = ( *flux_down_ptr - flux_reflected)
		* ( 1 - exp(-0.5 *  pow(lai*(1-gap_fraction),0.7)) + S);
	flux_transmitted = *flux_down_ptr -  flux_reflected - flux_absorbed ;
	*flux_down_ptr = flux_transmitted;

	return( flux_absorbed );
} /*end compute_diffuse_radiative_PAR_fluxes*/
