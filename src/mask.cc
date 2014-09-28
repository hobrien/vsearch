/*
  Copyright (C) 2014 Torbjorn Rognes

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Contact: Torbjorn Rognes <torognes@ifi.uio.no>,
  Department of Informatics, University of Oslo,
  PO Box 1080 Blindern, NO-0316 Oslo, Norway
*/

#include "vsearch.h"

static int dust_word = 3;
static int dust_level = 20;
static int dust_window = 64;
static int dust_window2 = dust_window >> 1;
static int word_count = 1 << (dust_word << 1);
static int bitmask = word_count - 1;

int wo(int len, const char *s, int *beg, int *end)
{
  int l1 = len - dust_word + 1 - 5; /* smallest possible region is 8 */
  if (l1 < 0)
    return 0;
  
  int bestv = 0;
  int besti = 0;
  int bestj = 0;
  int counts[word_count];
  int words[dust_window];
  int word = 0;

  for (int j = 0; j < len; j++)
    {
      word <<= 2;
      word |= chrmap_2bit[(int)(s[j])];
      words[j] = word & bitmask;
    }

  for (int i=0; i < l1; i++)
    {
      memset(counts, 0, sizeof(counts));
      
      int sum = 0;
      
      for (int j = dust_word-1; j<len-i; j++)
        {
          word = words[i+j];
          int c = counts[word];
          if (c)
            {
              sum += c;
              int v = 10 * sum / j;
              
              if (v > bestv)
                {
                  bestv = v;
                  besti = i;
                  bestj = j;
                }
            }
          counts[word]++;
        }
    }
  
  *beg = besti;
  *end = besti + bestj;
  
  return bestv;
}

void dust(char * m, int len)
{
  int a, b;

  /* make a local copy of the original sequence */
  char * s = (char*) alloca(len);
  memcpy(s, m, len);

  for (int i=0; i < len; i += dust_window2)
    {
      int l = (len > i + dust_window) ? dust_window : len-i;
      int v = wo(l, s+i, &a, &b);
      
      if (v > dust_level)
        {
          if (opt_hardmask)
            for(int j=a+i; j<=b+i; j++)
              m[j] = 'N';
          else
            for(int j=a+i; j<=b+i; j++)
              m[j] = s[j] | 0x20;
          
          if (b < dust_window2)
            i += dust_window2 - b;
        }
    }
}

static pthread_t * pthread;
static pthread_attr_t attr;
static pthread_mutex_t mutex;
static int nextseq = 0;
static int seqcount = 0;

void * dust_all_worker(void * vp)
{
  while(1)
    {
      pthread_mutex_lock(&mutex);
      int seqno = nextseq;
      if (seqno < seqcount)
        {
          nextseq++;
          progress_update(seqno);
          pthread_mutex_unlock(&mutex);
          dust(db_getsequence(seqno), db_getsequencelen(seqno));
        }
      else
        {
          pthread_mutex_unlock(&mutex);
          break;
        }
    }
  return 0;
}

void dust_all()
{
  seqcount = db_getsequencecount();
  progress_init("Masking", seqcount);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  pthread = (pthread_t *) xmalloc(opt_threads * sizeof(pthread_t));

  for(int t=0; t<opt_threads; t++)
    if (pthread_create(pthread+t, &attr, dust_all_worker, (void*)(long)t))
      fatal("Cannot create thread");

  for(int t=0; t<opt_threads; t++)
    if (pthread_join(pthread[t], NULL))
      fatal("Cannot join thread");

  free(pthread);

  pthread_attr_destroy(&attr);

  progress_done();
}

void hardmask(char * seq, int len)
{
  /* convert all lower case letters in seq to N */
  
  for(int j=0; j<len; j++)
    if (seq[j] & 0x20)
      seq[j] = 'N';
}

void hardmask_all()
{
  for(unsigned long i=0; i<db_getsequencecount(); i++)
    hardmask(db_getsequence(i), db_getsequencelen(i));
}

void mask()
{
  FILE * fp_output = fopen(opt_output, "w");
  if (!fp_output)
    fatal("Unable to open mask output file for writing");

  db_read(opt_mask, opt_dbmask != MASK_SOFT);
  show_rusage();

  if (opt_dbmask == MASK_DUST)
    dust_all();
  if (opt_hardmask)
    hardmask_all();
  show_rusage();

  progress_init("Writing output", seqcount);
  for(int i=0; i<seqcount; i++)
    {
      db_fprint_fasta(fp_output, i);
      progress_update(i);
    }
  progress_done();
  show_rusage();

  db_free();
  fclose(fp_output);
}
