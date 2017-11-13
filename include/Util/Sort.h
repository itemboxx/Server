//---------------------------------------------------------------------------

#ifndef SortH
#define SortH
//#include "c:\Home\Util\Sort.h"
//---------------------------------------------------------------------------
template <typename T, class Compare> void  SortShell(int size, T *arr, Compare sort)
{
	for (int d = size / 2; d>0; d /= 2)
		for (int i = d; i<size; i++)
			for (int j = i; j >= d && sort(arr[j - d], arr[j]); j -= d)
			{
				T temp = arr[j - d];
				arr[j - d] = arr[j];
				arr[j] = temp;
			}
}
//---------------------------------------------------------------------------
template <typename T, class Compare> void  SortShell(int size, T *arr, Compare sort,int *ind)
{
	for (int i = 0; i<size; i++)
		ind[i] = i;
	for (int d = size / 2; d>0; d /= 2)
		for (int i = d; i<size; i++)
			for (int j = i; j >= d && sort(arr[j - d], arr[j]); j -= d)
			{
				T temp = arr[j - d];
				arr[j - d] = arr[j];
				arr[j] = temp;
				int tempI = ind[j - d];
				ind[j - d] = ind[j];
				ind[j] = tempI;
			}
}
//---------------------------------------------------------------------------
template <typename T> void  SortShellDef(int size, T *arr)
{
    for(int d=size/2;d>0;d/=2)
        for(int i=d;i<size;i++)
            for(int j=i;j>=d && arr[j-d]>arr[j];j-=d)
            {
                T temp=arr[j-d];
                arr[j-d]=arr[j];
                arr[j]=temp;
            }
}
//---------------------------------------------------------------------------
template <typename T> void  SortShellIndDef(int size,T *arr,int *ind)
{
    for(int i=0;i<size;i++)
        ind[i]=i;
    for(int d=size/2;d>0;d/=2)
        for(int i=d;i<size;i++)
            for(int j=i;j>=d && arr[j-d]>arr[j];j-=d)
            {
                T temp=arr[j-d];
                arr[j-d]=arr[j];
                arr[j]=temp;
                int tempI=ind[j-d];
                ind[j-d]=ind[j];
                ind[j]=tempI;
            }
}
//---------------------------------------------------------------------------
template <typename T> int  SearchInStraightArr(T &val, unsigned dim, T *arr)
{
	if (dim <= 1) return 0;
	int nb1 = 0, nb2 = dim - 1;
	while (nb1 + 1 != nb2)
	{
		int nb = (nb1 + nb2) / 2;
		if (val<arr[nb])
			nb2 = nb;
		else
			nb1 = nb;
	}
	return (val < arr[nb2]) ? nb1 : nb2;
}
//---------------------------------------------------------------------------
template <typename T> int  SearchInStraightArrForIntKey(int val, unsigned dim, T *arr)
{
	if (dim <= 1) return 0;
	int nb1 = 0, nb2 = dim - 1;
	while (nb1 + 1 != nb2)
	{
		int nb = (nb1 + nb2) / 2;
		if (val<arr[nb].Key())
			nb2 = nb;
		else
			nb1 = nb;
	}
	return (val < arr[nb2].Key()) ? nb1 : nb2;
}
//---------------------------------------------------------------------------
#endif
