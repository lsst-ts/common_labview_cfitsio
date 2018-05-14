/* fitsLib.c    11/02/2007      NOAO */

/*
 *  Developed 2002-2017 by the National Optical Astronomy Observatories(*)
 *
 * (*) Operated by the Association of Universities for Research in
 *     Astronomy, Inc. (AURA) under cooperative agreement with the
 *     National Science Foundation.
 */

/*
 * modification history
 * --------------------
 * 01a 05sep2002, fdb - Changed pointer type of naxes from int to long in
 *                      functions labfits_create_img, labfits_insert_img.
 *                      Conditional dll code depending on variable "LINUX".
 * 01b 22jan2004, rcr - Add labfits_open_file and labfits_read_pix.
 * 01c 17dec2004, rcr - Add labfits_read_card.
 * 02a 25may2011, rcr - Remove the deprecated code.
 * 03a 13jul2012, rcr - Mod labfits_open_file to pass address by reference.
 * 04a 25jul2017, rcr - Mod. 64 bit support. 
 */

/*----------------------------------------------------------------------
 * Functions from CFITSIO, v2.0	adapted for being used easily with Labview
 * The functios are just wrappers to the CFITSIO ones.
 *---------------------------------------------------------------------*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "fitsio.h"
#include "extcode.h"

#ifdef TIMEDEB
#include <time.h>
#endif

#define MAX_HEADER	24
#define OK			0
#define MAX_AMPS	4

#ifndef LINUX
__declspec (dllexport)
     int labfits_create_file (char *name, int *fd);
__declspec (dllexport)
     int labfits_write_img (unsigned int ref, int datatype, int firstel, int nelem,
			    unsigned int location);
__declspec (dllexport)
     int labfits_write_img_arr (unsigned int ref, int datatype, int firstel, int nelem,
				void *data_array);
__declspec (dllexport)
     int labffppri (unsigned int ref, int group, int firstel, int nelem,
		    short *data_array);
__declspec (dllexport)
     int labfits_create_img (unsigned int ref, int bitpix, int naxis, long *naxes);
__declspec (dllexport)
     int labfits_close_file (unsigned int ref);
__declspec (dllexport)
     int labfits_write_date (unsigned int ref);
__declspec (dllexport)
     int labfits_write_comment (unsigned int ref, char *comment);
__declspec (dllexport)
     int labfits_write_geom_single (unsigned int ref, char *Asec, char *Bsec,
				    char *Csec, char *Dsec, char *Tsec,
				    char *amplifier);
__declspec (dllexport)
     int labfits_write_geom_multiple (unsigned int ref, char *data, char *bias,
				      char *ccd, char *amp, char *trim,
				      char *det, char *ccdsize,
				      char *amplifier);
__declspec (dllexport)
     int labfits_write_key (unsigned int ref, int datatype, char *keyname, void *value,
			    char *comment);
__declspec (dllexport)
     int labfits_write_key_from_char (unsigned int ref, int datatype, char *keyname,
				      char *value, char *comment);
__declspec (dllexport)
     int labfits_create_hdu (unsigned int ref);
__declspec (dllexport)
     int labfits_insert_img (unsigned int ref, int bitpix, int naxis, long *naxes);
__declspec (dllexport)
     int labfits_movabs_hdu (unsigned int ref, int hdunum, int hdutype);
__declspec (dllexport)
     int labfits_movrel_hdu (unsigned int ref, int nmove, int *hdutype);
__declspec (dllexport)
     int write_extensions (unsigned int ref, void *buffer, int num_of_ext, int firstel,
			   int nelem, short bpp);
__declspec (dllexport)
     int writeSingleImageNotRoi (unsigned int ref, int nelem, int eleRead, void *buffer,
				 int *writeprog, unsigned int imptr,
				 short bpp);
__declspec (dllexport)
     int writeMultImageNotRoi (unsigned int ref, int nelem, int eleRead, void *buffer,
			       int *writeprog, unsigned int imptr, short bpp);
__declspec (dllexport)
     int writeMultImageRoi (unsigned int ref, int nelem, int *eleRead, void *buffer,
			    int *writeprog, unsigned int imptr, short bpp);
__declspec (dllexport)
     void fits_set_values (int unsc, int data, int hdu,
			   unsigned short *roiused);
#endif

/*----------------------------------------------------------------------
 * labfits_create_file - YES
 *
 * Creates fits file
 *---------------------------------------------------------------------*/
int labfits_create_file (char *name, intptr_t *ref)
{
  fitsfile *fptr;
  int stat = 0;

  fits_create_file (&fptr, name, &stat);
  if (stat == 0)
    *ref = (intptr_t) fptr;
  return (-stat);
}

/*----------------------------------------------------------------------
 * labfits_write_img_arr - YES
 *
 * Writes the data into the already created file. Uses a pointer as the
 * pointer to the data
 *---------------------------------------------------------------------*/
int labfits_write_img_arr (intptr_t ref, int datatype, int firstel,
			   int nelem, void *data_array)
{
  fitsfile *fptr;
  int stat = 0;

  if (ref <= 0)
    return (-EBADF);
  if (nelem <= 0)
    return (-EINVAL);
  fptr = (fitsfile *) ref;
  fits_write_img (fptr, datatype, firstel, nelem, data_array, &stat);
  return (-stat);
}



/*----------------------------------------------------------------------
 * labfits_create_img - YES
 *
 * Creates an image in the file (acording to the given parameters)
 *---------------------------------------------------------------------*/
int labfits_create_img (intptr_t ref, int bitpix, int naxis, long *naxes)
{
  fitsfile *fptr;
  int stat = 0;

  if (ref == 0)
    return (-EBADF);

  fptr = (fitsfile *) ref;
  fits_create_img (fptr, bitpix, naxis, naxes, &stat);
  return (-stat);
}

/*----------------------------------------------------------------------
 * labfits_open_file - YES
 * 
 * Open a file
 *---------------------------------------------------------------------*/
int labfits_open_file (char *filename, int mode, intptr_t *ref)
{
  fitsfile *fptr;
  int stat = 0;

  fits_open_file (&fptr, filename, mode, &stat);
  if (stat != 0)
    return (-stat);

  *ref = (intptr_t) fptr;

  return (0);
}

/*----------------------------------------------------------------------
 * labfits_read_card - YES
 *
 * Read all 80 bytes keyword record
 *---------------------------------------------------------------------*/
int labfits_read_card (intptr_t ref, char *keyname, char **bufData)
{
  fitsfile *fptr;
  char record[80];
  int stat = 0;

  if (ref <= 0)
    return (-EBADF);

  fptr = (fitsfile *) ref;
  fits_read_card (fptr, keyname, record, &stat);
  if (stat != 0)
    return (-stat);
  stat = NumericArrayResize (iB, 1, (UHandle *) & bufData, 80);
  if (!stat) {
    **(int **) bufData = 80;
    memcpy (*(char **) bufData + 4, record, 80 * sizeof (char));
  }
  return (-stat);
}

/*----------------------------------------------------------------------
 * labfits_get_img_size - YES
 *
 * Get image size
 *---------------------------------------------------------------------*/
int labfits_get_img_size (intptr_t ref, int **bufData)
{
  fitsfile *fptr;
  long naxes[2];
  int stat = 0, n[2];

  if (ref <= 0)
    return (-EBADF);

  fptr = (fitsfile *) ref;
  fits_get_img_size (fptr, 2, naxes, &stat);
  if (stat != 0)
    return (-stat);
  stat = NumericArrayResize (iL, 1, (UHandle *) & bufData, 2);
  if (!stat) {
    n[0] = (int) naxes[0];
    n[1] = (int) naxes[1];
    fprintf(stderr,"%d %d\n", n[0], n[1]);
    **(int **) bufData = 2;
    memcpy (*(char **) bufData + 4, n, 2 * sizeof (int));
  }
  return (-stat);
}

/*----------------------------------------------------------------------
 * labfits_read_img - YES
 *
 *---------------------------------------------------------------------*/
int labfits_read_img (intptr_t ref, float **bufData)
{
  fitsfile *fptr;
  long naxes[9], nelements;
  int i, naxis, anynul = 0, stat = 0;

  if (ref <= 0)
    return (-EBADF);

  fptr = (fitsfile *) ref;
  fits_get_img_dim (fptr, &naxis, &stat);
  if (stat != 0)
    return (-stat);
  fits_get_img_size (fptr, naxis, naxes, &stat);
  if (stat != 0)
    return (-stat);
  nelements = 1;
  for (i = 0; i < naxis; i++)
    nelements *= naxes[i];
  /* fS means single precision double */
  stat = NumericArrayResize (fS, 1, (UHandle *) & bufData, nelements);
  if (!stat) {
    **(int **) bufData = nelements;
    fits_read_img (fptr, TFLOAT, 1, nelements, 0,
		   *(char **) bufData + 4, &anynul, &stat);
  }
  return (-stat);
}

/*---------------------------------------------------------------------
 * labfits_close_file - YES
 *
 * Close file
 *---------------------------------------------------------------------*/
int labfits_close_file (intptr_t ref)
{
  fitsfile *fptr;
  int stat = 0;

  if (ref <= 0)
    return (-EBADF);

  fptr = (fitsfile *) ref;
  fits_close_file (fptr, &stat);
  return (-stat);
}

/*----------------------------------------------------------------------
 * labfits_write_geom_single - YES
 *
 * Add into the headers all the geometry keywords that correspons to a
 * single-extension fits file
 *---------------------------------------------------------------------*/
int labfits_write_geom_single (intptr_t ref, char *Asec, char *Bsec, char *Csec,
			       char *Dsec, char *Tsec, char *amplifier)
{
  fitsfile *fptr;
  int stat1 = 0, stat2 = 0, stat3 = 0, stat4 = 0, stat5 = 0;
  char keyname[10];

  if (ref <= 0)
    return (-EBADF);

  fptr = (fitsfile *) ref;

  sprintf (keyname, "ASEC%s", amplifier);
  fits_write_key (fptr, 16, keyname, Asec, "amplifier section", &stat1);
  sprintf (keyname, "BSEC%s", amplifier);
  fits_write_key (fptr, 16, keyname, Bsec, "bias section", &stat2);
  sprintf (keyname, "CSEC%s", amplifier);
  fits_write_key (fptr, 16, keyname, Csec, "ccd section", &stat3);
  sprintf (keyname, "DSEC%s", amplifier);
  fits_write_key (fptr, 16, keyname, Dsec, "data section", &stat4);
  sprintf (keyname, "TSEC%s", amplifier);
  fits_write_key (fptr, 16, keyname, Tsec, "trim section", &stat5);

  return (-(stat1 + stat2 + stat3 + stat4 + stat5));
}

/*----------------------------------------------------------------------
 * labfits_write_key_from_char - YES
 *
 *---------------------------------------------------------------------*/
int labfits_write_key_from_char (intptr_t ref, int datatype, char *keyname,
				 char *value, char *comment)
{
  fitsfile *fptr;
  int stat = 0;
  long l_val;
  unsigned long ul_val;
  float f_val;
  double d_val;
  short s_val;
  void *val;

#ifdef TIMEDEB
  clock_t beg, end;
  beg = clock ();
#endif
  if (ref <= 0)
    return (-EBADF);

  fptr = (fitsfile *) ref;

  switch (datatype) {
  case TSTRING:{
      val = (void *) value;
      break;
    }
  case TLONG:{
      l_val = atol (value);
      val = (void *) &l_val;
      break;
    }
  case TULONG:
  case TUSHORT:{
      ul_val = strtoul (value, NULL, 10);
      val = (void *) &ul_val;
      break;
    }
  case TFLOAT:{
      f_val = atof (value);
      val = (void *) &f_val;
      break;
    }
  case TDOUBLE:
  case TCOMPLEX:{
      d_val = strtod (value, NULL);
      val = (void *) &d_val;
      break;
    }
  case TSHORT:{
      s_val = atoi (value);
      val = (void *) &s_val;
      break;
    }
  default:{
      val = (void *) value;
    }
  }


  printf ("%s\n", keyname);
  fflush(stdout);
  fits_write_key (fptr, datatype, keyname, val, comment, &stat);
#ifdef TIMEDEB
  end = clock ();
  printf ("%s %ld\n", keyname, end - beg);
#endif
  return (-stat);
}

