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

void results_show_alnout(FILE * fp,
                         struct hit * hits,
                         int hitcount,
                         char * query_head,
                         char * qsequence,
                         long qseqlen,
                         char * rc);

void results_show_blast6out_one(FILE * fp,
                                struct hit * hp,
                                char * query_head,
                                char * qsequence,
                                long qseqlen,
                                char * rc);

void results_show_uc_one(FILE * fp,
                         struct hit * hp,
                         char * query_head,
                         char * qsequence,
                         long qseqlen,
                         char * rc);

void results_show_userout_one(FILE * fp,
                              struct hit * hp,
                              char * query_head,
                              char * qsequence,
                              long qseqlen,
                              char * rc);

void results_show_fastapairs_one(FILE * fp,
                                 struct hit * hp,
                                 char * query_head,
                                 char * qsequence,
                                 long qseqlen,
                                 char * rc);
