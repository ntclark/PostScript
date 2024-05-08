// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "job.h"

//
// On entry, pStart is positioned just past the beginning delimiter
//
   void job::parse(char *pszBeginDelimiter,char *pszEndDelimiter,char *pStart,char **ppEnd) {

   long n = 0;
   char *p = pStart;
   char *pBegin = pStart;
   bool twoCharDelimiter = ! ('\0' == pszEndDelimiter[1]);
   char *pClear[64];
   memset(pClear,0,64 * sizeof(char *));
   long clearCount = 0L;

   if ( twoCharDelimiter ) {

      while ( *p && ( n || ! ( *p == pszEndDelimiter[0] ) || ! ( *(p + 1) == pszEndDelimiter[1] ) ) ) {

         if ( 0x0A == *p || 0x0D == *p ) {
            while ( *p && ( 0x0A == *p || 0x0D == *p ) )
               p++;
            if ( '\0' == *p )
               break;
            if ( '%' == *p && '%' == *(p + 1) ) {
               pClear[clearCount] = p;
               parseStructureSpec(p + 2,&p);
               memset(pClear[clearCount],' ',p - pClear[clearCount]);
               clearCount++;
               continue;
            }
            if ( '%' == *p ) {
               pClear[clearCount] = p;
               parseComment(p + 1,&p);
               memset(pClear[clearCount],' ',p - pClear[clearCount]);
               clearCount++;
               continue;
            }
            p--;
         }

         if ( *p == pszBeginDelimiter[0] && *(p + 1) == pszBeginDelimiter[1] )
            n++;
         else if ( *p == pszEndDelimiter[0] && *(p + 1) == pszEndDelimiter[1] )
            n--;

         p++;

      }

   } else {

      while ( *p && ( n || ! ( *p == pszEndDelimiter[0] ) ) ) {

         if ( 0x0A == *p || 0x0D == *p ) {
            while ( *p && ( 0x0A == *p || 0x0D == *p ) )
               p++;
            if ( '\0' == *p )
               break;
            if ( '%' == *p && '%' == *(p + 1) ) {
               pClear[clearCount] = p;
               parseStructureSpec(p + 2,&p);
               memset(pClear[clearCount],' ',p - pClear[clearCount]);
               clearCount++;
               continue;
            }
            if ( '%' == *p ) {
               pClear[clearCount] = p;
               parseComment(p + 1,&p);
               memset(pClear[clearCount],' ',p - pClear[clearCount]);
               clearCount++;
               continue;
            }
            p--;
         }

         if ( *p == pszBeginDelimiter[0] )
            n++;
         else if ( *p == pszEndDelimiter[0] )
            n--;

         if ( *p == '\\' ) 
            p++;

         p++;

      }

   }

   *ppEnd = p;

   long totalMotion = 0;

   for ( long k = 0; k < clearCount; k++ ) {

      pClear[k] -= totalMotion;

      char *pSource = pClear[k];

      long cntSpaces = 0L;
      while ( ' ' == *pSource ) {
         pSource++;
         cntSpaces++;
      }

      long n = (long)((char *)(*ppEnd + 1) - pSource);
      char *pNew = new char[n];

      memcpy(pNew,pSource,n);

      memcpy(pClear[k],pNew,n);

      delete [] pNew;

      pNew = pClear[k] + n;
      while ( pNew <= *ppEnd ) {
         *pNew = ' ';
         pNew++;
      }

      totalMotion += cntSpaces;

   }

   *ppEnd -= totalMotion;

   //pPStoPDF -> queueLog((char *)pStart);

   return;
   }