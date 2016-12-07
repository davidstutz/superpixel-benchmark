/*
Copyright 2011, Ming-Yu Liu

All Rights Reserved 

Permission to use, copy, modify, and distribute this software and 
its documentation for any non-commercial purpose is hereby granted 
without fee, provided that the above copyright notice appear in 
all copies and that both that copyright notice and this permission 
notice appear in supporting documentation, and that the name of 
the author not be used in advertising or publicity pertaining to 
distribution of the software without specific, written prior 
permission. 

THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
ANY PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR 
ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING 
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
*/
#ifndef _m_mfunctions_h_
#define _m_mfunctions_h_

class MMFunctions
{
public:

	// sort the array of elements in ra.
	// return the sorted arrary in ra.
	template <typename T> 
	inline static void Sort(T* ra, int nVec,int ascending=1);

	// sort the array of elements in ra.
	// return the sorted arrary in ra.
	// also return an arrary containing the origin orders of the sorted elements in ira.
	template <typename T> 
	inline static void ISort(T* ra, int nVec, int* ira,int ascending=1);

private:

	template <typename T> 
	inline static void Sort_Ascending(T* ra, int nVec);

	template <typename T> 
	inline static void Sort_Descending(T* ra, int nVec);

	template <typename T> 
	inline static void ISort_Ascending(T* ra, int nVec, int* ira);

	template <typename T> 
	inline static void ISort_Descending(T* ra, int nVec, int* ira);
};


template <typename T> void MMFunctions::Sort(T* ra, int nVec,int ascending)
{
	if(ascending)
		Sort_Ascending<T>(ra,nVec);
	else
		Sort_Descending<T>(ra,nVec);
}


template <typename T> void MMFunctions::Sort_Descending(T* ra, int nVec)
{
   unsigned long n, l, ir, i, j;
   n = nVec;
   T rra;
   
   if (n<2)
      return;
   l = (n>>1)+1;
   ir = n;
   for (;;)
   {
      if (l>1)
      {
         rra = ra[(--l)-1];
      }
      else
      {
         rra = ra[ir-1];
         ra[ir-1] = ra[1-1];
         if (--ir==1)
         {
            ra[1-1] = rra;
            break;
         }
      }
      i = l;
      j = l+l;
      while (j<=ir)
      {
         if (j<ir && ra[j-1]>ra[j+1-1])
            j++;
         if (rra>ra[j-1])
         {
            ra[i-1] = ra[j-1];
            i = j;
            j <<= 1;
         }
         else
            j = ir+1;
      }
      ra[i-1] = rra;
   }
}

template <typename T> void MMFunctions::Sort_Ascending(T* ra, int nVec)
{
   unsigned long n, l, ir, i, j;
   n = nVec;
   T rra;
   
   if (n<2)
      return;
   l = (n>>1)+1;
   ir = n;
   for (;;)
   {
      if (l>1)
      {
         rra = ra[(--l)-1];
      }
      else
      {
         rra = ra[ir-1];
         ra[ir-1] = ra[1-1];
         if (--ir==1)
         {
            ra[1-1] = rra;
            break;
         }
      }
      i = l;
      j = l+l;
      while (j<=ir)
      {
         if (j<ir && ra[j-1]<ra[j+1-1])
            j++;
         if (rra<ra[j-1])
         {
            ra[i-1] = ra[j-1];
            i = j;
            j <<= 1;
         }
         else
            j = ir+1;
      }
      ra[i-1] = rra;
   }
}



template <typename T> void MMFunctions::ISort(T* ra, int nVec, int* ira,int ascending)
{
	if(ascending)
		ISort_Ascending<T>(ra,nVec,ira);
	else
		ISort_Descending<T>(ra,nVec,ira);
}

template <typename T> void MMFunctions::ISort_Descending(T* ra, int nVec, int* ira)
{
   unsigned long n, l, ir, i, j;
   n = nVec;
   T rra;
   int irra;
   
   if (n<2)
      return;
   l = (n>>1)+1;
   ir = n;
   for (;;)
   {
      if (l>1)
      {
         irra = ira[(--l)-1];
         rra = ra[l-1];
      }
      else
      {
         irra = ira[ir-1];
         rra = ra[ir-1];

         ira[ir-1] = ira[1-1];
         ra[ir-1] = ra[1-1];

         if (--ir==1)
         {
            ira[1-1] = irra;
            ra[1-1] = rra;
            break;
         }
      }
      i = l;
      j = l+l;
      while (j<=ir)
      {
         if (j<ir && ra[j-1]>ra[j+1-1])
            j++;
         if (rra>ra[j-1])
         {
            ira[i-1] = ira[j-1];
            ra[i-1] = ra[j-1];

            i = j;
            j <<= 1;
         }
         else
            j = ir+1;
      }
      ira[i-1] = irra;
      ra[i-1] = rra;
   }
}

template <typename T> void MMFunctions::ISort_Ascending(T* ra, int nVec, int* ira)
{
   unsigned long n, l, ir, i, j;
   n = nVec;
   T rra;
   int irra;
   
   if (n<2)
      return;
   l = (n>>1)+1;
   ir = n;
   for (;;)
   {
      if (l>1)
      {
         irra = ira[(--l)-1];
         rra = ra[l-1];
      }
      else
      {
         irra = ira[ir-1];
         rra = ra[ir-1];

         ira[ir-1] = ira[1-1];
         ra[ir-1] = ra[1-1];

         if (--ir==1)
         {
            ira[1-1] = irra;
            ra[1-1] = rra;
            break;
         }
      }
      i = l;
      j = l+l;
      while (j<=ir)
      {
         if (j<ir && ra[j-1]<ra[j+1-1])
            j++;
         if (rra<ra[j-1])
         {
            ira[i-1] = ira[j-1];
            ra[i-1] = ra[j-1];

            i = j;
            j <<= 1;
         }
         else
            j = ir+1;
      }
      ira[i-1] = irra;
      ra[i-1] = rra;
   }
}

#endif