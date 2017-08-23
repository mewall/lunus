/* LFILLPIXIM.C - Replace pixels marked as belonging to Bragg peaks in an input list with the average of their surrounding background values.
   
   Author: Mike Wall  
   Date: 5/12/94
   Version: 1.
   
   */

#include<mwmask.h>

int lfillpixim(DIFFIMAGE *imdiff)
{

    int
	curr_hkl,
	prev_hkl,
	minx,
	miny,
	maxx,
	maxy,
	hkl,
	x,
	y,
	i,
	j,
	bkg_count,
	line_count,
	return_value = 0;
    
    IMAGE_DATA_TYPE
	bkg_val,
	bkg_val_avg;

    long
	this_hkl_pos,
	next_hkl_pos;

    char
	*current_line;

    size_t
	MAX_LINE_SIZE = 80;


    current_line = (char*)calloc(MAX_LINE_SIZE, sizeof(char));
    prev_hkl = 0;

Reset:

    this_hkl_pos = ftell(imdiff->infile);
    fgets(current_line, MAX_LINE_SIZE, imdiff->infile);
    sscanf(current_line, "%d %d %d", &curr_hkl, &miny, &minx);
    maxy = miny;
    maxx = minx;
    line_count = 1;
    if (prev_hkl == curr_hkl) {
	printf("NOPE\n");
	return(1);
    }
    next_hkl_pos = ftell(imdiff->infile);

    // Read in punch list
    while(fgets(current_line, MAX_LINE_SIZE, imdiff->infile) != NULL) {
	sscanf(current_line, "%d %d %d", &hkl, &y, &x);
	if (hkl == curr_hkl) {
	    if (y < miny)
		miny = y;
	    if (y > maxy)
		maxy = y;
	    if (x > maxx)
		maxx = x;
	    next_hkl_pos = ftell(imdiff->infile);
	    line_count++;
	}
	else { // time to process the now-fully-read hkl
	    bkg_val = 0;
	    bkg_count = 0;
	    fseek(imdiff->infile, this_hkl_pos, SEEK_SET);
	    fgets(current_line, MAX_LINE_SIZE, imdiff->infile);
	    sscanf(current_line, "%d %d %d", &hkl, &y, &x);
	    if (minx > 0)
		minx = minx - 1;
	    if (miny > 0)
		miny = miny - 1;
	    if (maxx < imdiff->hpixels - 1)
		maxx = maxx + 1;
	    if (maxy < imdiff->vpixels - 1)
		maxy = maxy + 1;
	    for (i = minx; i <= maxx; i++) {
		for (j = miny; j <= miny; j++) {
		    if ((i == x) && (j == y)) {
			// this pixel gets punched, so we don't add to bkg
			// go to next punched pixel by reading next line
			fgets(current_line, MAX_LINE_SIZE, imdiff->infile);
			sscanf(current_line, "%d %d %d", &hkl, &y, &x);
		    }		
		    else {
			if (imdiff->image[j*imdiff->hpixels + i] != imdiff->ignore_tag) {
			    // this pixel needs to be added to background
			    bkg_val += imdiff->image[j*imdiff->hpixels + i];
			    bkg_count++;
			}
		    }
		}
	    }
	    // compute average background value
	    if (bkg_count > 0)
		bkg_val_avg = bkg_val / bkg_count;
	    else
		bkg_val_avg = imdiff->ignore_tag;

	    // actually do the punching
	    fseek(imdiff->infile, this_hkl_pos, SEEK_SET);
	    for (i = 0; i < line_count; i++) {
		fgets(current_line, MAX_LINE_SIZE, imdiff->infile);
		sscanf(current_line, "%d %d %d", &hkl, &y, &x);
		if (imdiff->image[y*imdiff->hpixels + x] != imdiff->ignore_tag) {
		    imdiff->image[y*imdiff->hpixels + x] = bkg_val_avg;
		    //imdiff->image[y*imdiff->hpixels + x] = imdiff->punch_tag;
		}
	    }

	    // start the next hkl
	    fseek(imdiff->infile, next_hkl_pos, SEEK_SET);
	    prev_hkl = curr_hkl;
	    goto Reset;
	}

    }

    free(current_line);

    return(return_value);
  /*  
  long i;
  short
    lpunch_return,
    r,
    c;
  
  
  for (i=0; i<imdiff->mask_count; i++) {
    r = imdiff->pos.r+imdiff->mask[i].r;
    c = imdiff->pos.c+imdiff->mask[i].c;
    if (!((r < 0) || (r > imdiff->vpixels) || (c < 0) ||       
	  (c > imdiff->hpixels))) {
      imdiff->image[r*imdiff->hpixels + c] = 
	imdiff->punch_tag;
      lpunch_return = 0;
    } else lpunch_return = 1;
  }
  return(lpunch_return);

  */
}
