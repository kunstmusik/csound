/*
    dcblockr.c:

    Copyright (C) 1998 John ffitch

    This file is part of Csound.

    The Csound Library is free software; you can redistribute it
    and/or modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    Csound is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with Csound; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA
*/

/*******************************************/
/*  DC Blocking Filter                     */
/*  by Perry R. Cook, 1995-96              */
/*  This guy is very helpful in, uh,       */
/*  blocking DC.  Needed because a simple  */
/*  low-pass reflection filter allows DC   */
/*  to build up inside recursive           */
/*  structures.                            */
/*******************************************/

#include "csdl.h"
#include "dcblockr.h"

static int dcblockrset(CSOUND *csound, DCBlocker* p)
{
    p->outputs = 0.0;
    p->inputs = 0.0;
    p->gain = (double)*p->gg;
    if (p->gain == 0.0 || p->gain>=1.0 || p->gain<=-1.0)
      p->gain = 0.99;
    return OK;
}

static int dcblockr(CSOUND *csound, DCBlocker* p)
{
    MYFLT       *ar = p->ar;
    int         n, nsmps = csound->ksmps;
    double      gain = p->gain;
    double      outputs = p->outputs;
    double      inputs = p->inputs;
    MYFLT       *samp = p->in;

    for (n=0; n<nsmps; n++) {
      double sample = (double)samp[n];
      outputs = sample - inputs + (gain * outputs);
      inputs = sample;
      ar[n] = (MYFLT)outputs;
    }
    p->outputs = outputs;
    p->inputs = inputs;
    return OK;
}

#define S(x)    sizeof(x)

static OENTRY localops[] = {
{ "dcblock", S(DCBlocker), 5, "a", "ao", (SUBR)dcblockrset, NULL, (SUBR)dcblockr}
};

int dcblockr_init_(CSOUND *csound)
{
    return csound->AppendOpcodes(csound, &(localops[0]),
                                 (int) (sizeof(localops) / sizeof(OENTRY)));
}

