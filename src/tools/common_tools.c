/*

Copyright (c) 2015, Elliptic Technologies Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#include "common_tools.h"

static unsigned char convertchar2hex(char a);
static int read_string(ELP_FILE **in, int const max_op_size, unsigned char *op_str);
static int read_long(ELP_FILE **in, unsigned long *u);

static unsigned char convertchar2hex(char a)
{
   char val;

   switch (a)
   {
      case 'A':
      case 'a': val = 0x0a; break;
      case 'B':
      case 'b': val = 0x0b; break;
      case 'C':
      case 'c': val = 0x0c; break;
      case 'D':
      case 'd': val = 0x0d; break;
      case 'E':
      case 'e': val = 0x0e; break;
      case 'F':
      case 'f': val = 0x0f; break;
      case '0': val = 0x00; break;
      case '1': val = 0x01; break;
      case '2': val = 0x02; break;
      case '3': val = 0x03; break;
      case '4': val = 0x04; break;
      case '5': val = 0x05; break;
      case '6': val = 0x06; break;
      case '7': val = 0x07; break;
      case '8': val = 0x08; break;
      default:  val = 0x09; break;
   }

   return val;
}

int read_bytes(ELP_FILE **in, unsigned int iplen, unsigned char *op, unsigned int *oplen)
{
   char a;
   char value;
   int  bitcnt;
   int  bvalid;

   *oplen   = 0;
   value    = 0;
   bitcnt   = 0;

   while(!ELP_FEOF(*in) && (*oplen < iplen))
   {
      ELP_FSCANF(*in, "%c", &a);

      if ( ((a >= 48) && (a <= 57)) ||
           ((a >= 65) && (a <= 70)) ||
           ((a >= 97) && (a <= 102)) )
      {
         bitcnt++;
         value <<= 4;
         value &= 0xf0;
         bvalid = 1;

         value |= convertchar2hex(a);
      }
      else
      {
         bvalid = 0;

         if (a == '/' || a == '[')
         {
           if ( bitcnt == 1 )
           {
              op[*oplen] = value;
              (*oplen)++;
           }

           ELP_FSEEK(*in, -1, SEEK_CUR);

           return -2;
         }
      }

      switch(bitcnt)
      {
         case 0: break;
         case 1:  if ( bvalid == 1 ) break;
         case 2:
            op[*oplen] = value;
            bitcnt = 0;
            value = 0;
            (*oplen)++;
            if ( (*oplen) > iplen )
            {
               printf("Len exceeds size of array\n");
               return -1;
            }
            break;
         default:
            bitcnt = 0;
            break;
      }
   }

   return 0;
}

int read_string(ELP_FILE **in, int const max_op_size, unsigned char *op_str)
{
   char sline[ELP_COMMONG_PARSING_LINE_SIZE];
   int  i = 0;
   char *start = 0;
   int line_length;

   if ( fgets(sline, ELP_COMMONG_PARSING_LINE_SIZE, *in) != NULL )
   {
     line_length = (int)strlen(sline);

     /* eat leading white space */
     while((sline[i] == WHITE_SPACE) && (i++ < line_length));

     if (i == line_length) return -1;

     start = &sline[i];

     i = line_length - 1;

     while((sline[i] == WHITE_SPACE) && ((&sline[i]) != start) && (i-- >= 0));

     if (i < 0) return -1;

     if (i < (line_length - 1)) i++;

     sline[i] = 0x00;

     strncpy((char *)op_str, start, max_op_size-1);
     op_str[max_op_size-1] = 0;

     return 0;
   }

   return -1;
}

int read_long(ELP_FILE **in, unsigned long *u)
{
   char const ZERO = 0x30;
   char const NINE = 0x39;
   char sline[ELP_COMMONG_PARSING_LINE_SIZE];
   int  i = 0;
   char *snum = 0;
   int line_length;

   if ( fgets(sline, ELP_COMMONG_PARSING_LINE_SIZE, *in) != NULL )
   {
     line_length = (int)strlen(sline);

     /* eat leading white space */
     while((sline[i] == WHITE_SPACE) && (i++ < line_length));

     if (i == line_length) return -1;

     snum = &sline[i];

     while(((sline[i] >= ZERO) && (sline[i] <= NINE)) && (i++ < line_length));

     if ( i < line_length )
     {
        sline[i] = 0x00;
     }
     else
     {
        sline[i - 1] = 0x00;
     }

     *u = (unsigned long)atoll(snum);

     return 0;
   }

   return -1;

}

int read_short(ELP_FILE **in, unsigned short *s)
{
   char const ZERO = 0x30;
   char const NINE = 0x39;
   char sline[ELP_COMMONG_PARSING_LINE_SIZE];
   int  i = 0;
   char *snum = 0;
   int line_length;
   volatile unsigned int val;

   if ( fgets(sline, ELP_COMMONG_PARSING_LINE_SIZE, *in) != NULL )
   {
     line_length = (int)strlen(sline);

     /* eat leading white space */
     while((sline[i] == WHITE_SPACE) && (i++ < line_length));

     if (i == line_length) return -1;

     snum = &sline[i];

     while(((sline[i] >= ZERO) && (sline[i] <= NINE)) && (i++ < line_length));

     if ( i < line_length )
     {
        sline[i] = 0x00;
     }
     else
     {
        sline[i - 1] = 0x00;
     }

     val = (unsigned int)atoi(snum);
     *s = (unsigned short)val;

     return 0;
   }

   return -1;
}

int read_byte(ELP_FILE **in, unsigned char *m)
{
   char const ZERO = 0x30;
   char const NINE = 0x39;
   char sline[ELP_COMMONG_PARSING_LINE_SIZE];
   int  i = 0;
   char *snum = 0;
   int line_length;
   unsigned int val;

   if ( fgets(sline, ELP_COMMONG_PARSING_LINE_SIZE, *in) != NULL )
   {
     line_length = (int)strlen(sline);

     /* eat leading white space */
     while((sline[i] == WHITE_SPACE) && (i++ < line_length));

     if (i == line_length) return -1;

     snum = &sline[i];

     while(((sline[i] >= ZERO) && (sline[i] <= NINE)) && (i++ < line_length));

     if ( i < line_length )
     {
        sline[i] = 0x00;
     }
     else
     {
        sline[i - 1] = 0x00;
     }

     val = (unsigned int)atoi(snum);
     *m = (unsigned char)val;

     return 0;
   }

   return -1;

}

int parse_config(tparse_config_info *ptr, int size, char* filename)
{
   char sline[ELP_COMMONG_PARSING_LINE_SIZE];
   char svar[ELP_COMMONG_PARSING_LINE_SIZE];
   int line_length;
   int i,k,s;
   unsigned int oplength;
   ELP_FILE *fptr;

   if ((fptr = ELP_FOPEN(filename,"r",fptr)) == NULL)
   {
      printf("unable to open file [%s]\n", filename);
      return -1;
   }

   while(!ELP_FEOF(fptr))
   {
      sline[0] = 0x00;
      i         = 0;

      if ( fgets(sline, sizeof(sline), fptr) != NULL )
      {
         line_length = (int)strlen(sline);

         /* eat leading white space */
         while((sline[i] == WHITE_SPACE) && (i++ < line_length));

         if (i == line_length)   continue;
         if (sline[i] != '[')    continue;
         if (++i == line_length) continue;

         /* find end */
         k = 0;
         do{
            svar[k++] = sline[i]; i++;
         }while((sline[i] != ']') && (sline[i] != WHITE_SPACE)  && (i < line_length));
         svar[k] = 0;

         if (i == line_length)   continue;
         if (sline[i] != ']')    continue;
         if (++i == line_length) continue;

         /* match in structure */
         for (s = 0; (s < size) && (strcmp(ptr[s].name, svar) != 0); s++);

         if (s == size) continue;

         oplength = 0;
         switch(ptr[s].type)
         {
            case ELP_CHAR_HEX:
               read_bytes(&fptr, ptr[s].size, ptr[s].m, &oplength);
               ptr[s].set = oplength;
               break;
            case ELP_CHAR_STRING:
               read_string(&fptr, ptr[s].size, ptr[s].m);
               ptr[s].set = SET;
               break;
            case ELP_BYTE:
               read_byte(&fptr, ptr[s].m);
               ptr[s].set = SET;
               break;
            case ELP_SHORT:
               read_short(&fptr, ptr[s].s);
               ptr[s].set = SET;
               break;
            default:
               read_long(&fptr, ptr[s].u);
               ptr[s].set = SET;
               break;
         };
      }
   }

   ELP_FCLOSE(fptr);

   return 0;
}

void dump(char* table_name, uint8_t* table, uint32_t table_size)
{
   uint32_t i;

   printf("[%s]\n", table_name);

   for(i = 0; i < table_size; i++)
   {
      printf("%.2x ", table[i]);

      if ((i > 0) && (((i+1)%16)==0))
      {
         printf("\n");
      }
   }

   printf("\n");
}

int32_t parse_key_file(char* key_file, uint8_t* kpkf, uint32_t* kpf_size, uint8_t* duk, uint32_t* duk_size, uint8_t* ivw, uint32_t* ivw_size)
{
   FILE* fptr;
   char sline[MAX_LIN_LENGTH];
   uint32_t i, j;
   uint8_t get_tag;
   uint8_t found_pkf;
   uint8_t found_duk;
   uint8_t found_ivw;
   uint32_t max_indices;

   if ((fptr = fopen(key_file, "rb")) == NULL)
   {
      printf("Unable to open KEY file %s\n", key_file);
      return -4;
   }

   found_pkf = 0;
   found_duk = 0;
   found_ivw = 0;

   *kpf_size = 0;
   *duk_size = 0;
   *ivw_size = 0;

   while(!feof(fptr))
   {
      get_tag   = 1;
      found_pkf = 0;
      found_duk = 0;
      found_ivw = 0;

      if ( fgets(sline, MAX_LIN_LENGTH, fptr) != 0 )
      {
         for(i=0; i<MAX_LIN_LENGTH; i++)
         {
            if ((sline[i] != WHITE_SPACE) && (sline[i] != EQUAL_SIGN))
            {
               if (get_tag == 1)
               {
                  /* get the tag name */
                  if ((sline[i] == 'P') && (sline[i+1] == 'K') && (sline[i+2] == 'f'))
                  {
                     found_pkf = 1;
                     *kpf_size = 16;
                  }
                  else if ((sline[i] == 'D') && (sline[i+1] == 'U') && (sline[i+2] == 'K'))
                  {
                     found_duk = 1;
                     *duk_size = 16;
                  }
                  else if ((sline[i] == 'I') && (sline[i+1] == 'V') && (sline[i+2] == 'w') && (sline[i+3] == 'm'))
                  {
                     found_ivw = 1;
                     *ivw_size = 16;
                  }

                  get_tag = 0;
               }
               else
               {
                  if ((sline[i] == '0') && (sline[i+1] == 'x'))
                  {

                     max_indices = 32;

                     if (found_ivw == 1)
                     {
                        max_indices = 24;
                     }

                     for (j = 0; j < max_indices; j++)
                     {

                        if (found_pkf == 1)
                        {
                           if ((j%2) == 0)
                           {
                              kpkf[j>>1] = convertchar2hex(sline[i+2 + j]);
                              kpkf[j>>1] <<= 4;
                           }
                           else
                           {
                              kpkf[j>>1] |= convertchar2hex(sline[i+2 + j]);
                           }
                        }
                        else if (found_duk == 1)
                        {
                           if ((j%2) == 0)
                           {
                              duk[j>>1] = convertchar2hex(sline[i+2 + j]);
                              duk[j>>1] <<= 4;
                           }
                           else
                           {
                              duk[j>>1] |= convertchar2hex(sline[i+2 + j]);
                           }
                        }
                        else if (found_ivw == 1)
                        {
                           if ((j%2) == 0)
                           {
                              ivw[j>>1] = convertchar2hex(sline[i+2 + j]);
                              ivw[j>>1] <<= 4;
                           }
                           else
                           {
                              ivw[j>>1] |= convertchar2hex(sline[i+2 + j]);
                           }
                        }
                     }

                  break;
                  } /* end if ((sline[i] == '0') && (sline[i+1] == 'x')) */
               }
            }
         }
      }
   }

   fclose(fptr);

   if ((*kpf_size != 16) && (*duk_size != 16) && (*ivw_size != 16))
   {
      return -5;
   }

   return 0;
}
