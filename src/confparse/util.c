/*
 *      confparse/util.c
 *      Copyright © 2008,2009 Martin Duquesnoy <xorg62@gmail.com>
 *      All rights reserved.
 *
 *      Redistribution and use in source and binary forms, with or without
 *      modification, are permitted provided that the following conditions are
 *      met:
 *
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following disclaimer
 *        in the documentation and/or other materials provided with the
 *        distribution.
 *      * Neither the name of the  nor the names of its
 *        contributors may be used to endorse or promote products derived from
 *        this software without specific prior written permission.
 *
 *      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *      "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *      LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *      A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *      OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *      SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *      LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *      DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *      THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *      (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *      OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "confparse.h"

char*
erase_delim_content(char *buf)
{
     int i, j;
     char *str, c;

     if(!buf)
          return NULL;

     str = _strdup(buf);

     for(i = 0; i < strlen(str); ++i)
          if((c = str[i]) == '"' || (c = str[i]) == '\'')
          {
               for(str[(j = i)] = ' '; str[j] && str[j] != c; str[j++] = ' ');
               str[j] = ' ';
          }

     return str;
}

/* Erase all content of all delimiters, put it
 * int str, and check if buf[p] is in an delimiter. */
Bool
is_in_delimiter(char *buf, int p)
{
     if(*(erase_delim_content(buf) + p) != buf[p])
          return True;

     return False;
}

char*
erase_sec_content(char *buf)
{
     int i, j;
     char *p, *str, *name, *ret;

     if(!buf)
          return NULL;

     str = erase_delim_content(buf);
     ret = _strdup(buf);

     for(i = 1, name = _strdup(str + i); strchr(str + i, SEC_DEL_S); ++i, name = _strdup(str + i))
     {
          for(; str[i] && str[i] != SEC_DEL_S; ++i);
          for(j = 0; str[i] && str[i - 1] != SEC_DEL_E; name[j++] = str[i++]);
          name[j] = '\0';

          if((p = strstr(str + i, name)))
               for(++i; i < strlen(ret) - strlen(p); ret[i++] = ' ');
          else
               return ret;
     }

     return ret;
}

/* To get the RIGHT name of an option; if option needed is
 * pwet and there is tagadapwettagada in the configuration,
 * with strstr(), the name will matchs */
char*
opt_srch(char *buf, char *opt)
{
     char *p, c1, c2;

     if(!buf || !opt)
          return NULL;

     if((p = strstr(erase_delim_content(buf), opt)))
     {
          c1 = *(p + strlen(opt));
          c2 = *(p - 1);

          if((c1 == ' ' || c1 == '=')
             && (c2 == ' ' || c2 == '\n' || c2 == '\t' || !c2))
               return _strdup(buf + (strlen(buf) - strlen(p)));
     }

     return NULL;
}

opt_type
str_to_opt(char *str)
{
     opt_type ret = null_opt_type;

     if(!strlen(str))
          return ret;

     /* Integer */
     ret.num = atoi(str);

     /* Float */
     sscanf(str, "%f", &ret.fnum);

     /* Boolean */
     if(strstr(str, "true") || strstr(str, "True")
        || strstr(str, "TRUE") || strstr(str, "1"))
          ret.bool = True;

     /* String */
     ret.str = _strdup(str);

     return ret;
}

char*
clean_value(char *str)
{
     int i;
     char c, *p;

     if(!str)
          return NULL;

     p = _strdup(str);

     /* Remove useless spaces */
     for(; *p == ' '; ++p);
     for(; *(p + strlen(p) - 1) == ' '; *(p + strlen(p) - 1) = '\0');

     /* For string delimiter (" or ') */
     if(((c = *p) == '"' || (c = *p) == '\'')
        && strchr(p + 1, c))
     {
          for(++p, i = 0; p[i] && p[i] != c; ++i);
          p[i] = '\0';
     }

     return p;
}
