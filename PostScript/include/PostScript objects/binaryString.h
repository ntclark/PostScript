#pragma once

#include "string.h"

   class binaryString : public string {
   public:

      binaryString(job *pJob,char *pszContents,BYTE *pBinary,long dwLength);

      virtual ~binaryString();

      long length();

      virtual void put(long index,BYTE v);
      virtual BYTE get(long index);

      BYTE *getData();
      
   private:

      DWORD dwLength{0L};
      BYTE *pbData{NULL};

   };