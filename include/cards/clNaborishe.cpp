//---------------------------------------------------------------------------
//#include "stdafx.h"


#pragma hdrstop

#include <fcntl.h>
#include <io.h>
#include <sys\stat.h>
#include <share.h>
#include <afxwin.h>
#include "c:\home\card\clNaborishe.h"

int glNBSHMaskBit[]={1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192};
int glNBSHArraySuited[128] = {0,0,0,1,0,2,4,7,0,3,5,8,6,10,13,17,0,0,0,9,0,11,14,18,0,12,15,19,16,20,21,31,0,0,0,0,0,0,0,1,0,0,0,2,0,4,7,30,0,0,0,3,0,5,8,28,0,6,9,25,10,21,16,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,29,0,0,0,0,0,0,0,27,0,0,0,24,0,20,15,2,0,0,0,0,0,0,0,26,0,0,0,23,0,19,14,3,0,0,0,22,0,18,13,4,0,17,12,5,11,6,7,8};
int glNBSHArray67Suited[128] = {0,22,23,1,24,2,7,0,25,3,8,0,12,0,0,0,26,4,9,0,13,0,0,0,16,0,0,0,0,0,0,0,27,5,10,0,14,0,0,0,17,0,0,0,0,0,0,0,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,28,6,11,0,15,0,0,0,18,0,0,0,0,0,0,0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
std::vector <stNBSHProperty> glNBSHVect;
int *glNBSHArray = NULL;


//---------------------------------------------------------------------------
stNBSHProperty  NBSHGetProperty(int id)
{
	if (id <= 0 || id >= NBSH_MAX_SIZE)
		id = id;
	if (glNBSHArray[id] < 0 || glNBSHArray[id] >= NBSH_MAX_SIZE)
	{
		id = id;
		stNBSHProperty p0; return p0;
	}
    return glNBSHVect[glNBSHArray[id]];
}
//---------------------------------------------------------------------------
void  NBSHSetProperty(int id,stNBSHProperty prop)
{
    glNBSHVect[glNBSHArray[id]] = prop;
}
//---------------------------------------------------------------------------
void NBSHSaveVectorToFile()
{
	int handle; _sopen_s(&handle, NBSH_PATH_TO_VECTOR_FILE, O_RDWR | O_CREAT | O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	_write(handle, glNBSHArray, NBSH_MAX_SIZE*sizeof(int));
	int size = glNBSHVect.size();
	_write(handle, &size, sizeof(int));
	_write(handle, &glNBSHVect[0], size*sizeof(stNBSHProperty));
	stNBSHProperty ss; ss = glNBSHVect[0];
	_close(handle);

}
bool NBSHLoadVectorFromFile()
{
	int handle; _sopen_s(&handle, NBSH_PATH_TO_VECTOR_FILE, O_RDWR | O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if (handle == -1) return false;
	if (glNBSHArray != NULL)
		free(glNBSHArray);
	glNBSHArray = (int *)malloc(NBSH_MAX_SIZE*sizeof(int));
	_read(handle, glNBSHArray, NBSH_MAX_SIZE*sizeof(int));
	int size;
	_read(handle, &size, sizeof(int));
	glNBSHVect.resize(size);
	glNBSHVect.reserve(NBSH_MAX_SIZE);
	_read(handle, &glNBSHVect[0], size*sizeof(stNBSHProperty));
	stNBSHProperty ss; ss = glNBSHVect[0];
	_close(handle);
	return true;
}
void NBSHCloseGlobals()
{
	if (glNBSHArray != NULL)
		free(glNBSHArray);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
clNaborishe::clNaborishe()
{
    _id = 0;
}
//---------------------------------------------------------------------------
void clNaborishe::AddManyCards(clCard *cards, int cnCard)
{
	int startCnCard = 0;
	clCard cards0[10];
	if (_id != 0)
	{
		stNBSHProperty prop0; prop0 = NBSHGetProperty(_id);
		startCnCard = prop0._cnCard;
		memcpy(cards0, prop0._cards, prop0._cnCard*sizeof(clCard));
	}

	memcpy(cards0 + startCnCard, cards, cnCard*sizeof(clCard));
	for (int i = startCnCard; i < cnCard + startCnCard; i++)
	{
		AddCard(cards0[i]);
		ConvertSuit(cards0, cnCard + startCnCard);
	}
}
//---------------------------------------------------------------------------
void clNaborishe::ConvertSuit(clCard *cards,int cnCard)
{
	stNBSHProperty prop0; prop0 = NBSHGetProperty(_id);
//	clCard cards0[10];
//	memcpy(cards0, cards, prop0._cnCard);
	clCard *cardN3 = prop0._cards;

	int sumSuited[CN_CARD_SUIT] = { 0, 0, 0, 0 };
	int sumSuitedN[CN_CARD_SUIT] = { 0, 0, 0, 0 };
	int sumSuitedCnCard[CN_CARD_SUIT] = { 0, 0, 0, 0 };
	bool convTo[CN_CARD_SUIT] = { false, false, false, false };
	enSuit faceTo[4];

	int nb = 0;
	for (int i = 0; i < prop0._cnCard; i++)
	{
		sumSuited[cards[i].suit] += glNBSHMaskBit[cards[i].face];
		sumSuitedN[cardN3[i].suit] += glNBSHMaskBit[cardN3[i].face];
	}
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (sumSuited[i] == sumSuitedN[j] && !convTo[j])
			{
				convTo[j] = true;
				faceTo[i] = (enSuit)j; break;
			}
		}
	}
	for (int i = 0; i < cnCard; i++)
		cards[i].suit = faceTo[cards[i].suit];

}
//---------------------------------------------------------------------------
void clNaborishe::AddCard(clCard card)
{
    if(_id == 0)
    {
        _id+=glNBSHMaskBit[card.face];
//        _s1=card.suit;
    }
    else
    {
        stNBSHProperty prop0;
        prop0 = NBSHGetProperty(_id);
        int nb = 0;
        for(int i=0;i<card.face;i++)
            if(_id & glNBSHMaskBit[i])
                nb++;
        int pd = Paired();
        if(_id & glNBSHMaskBit[card.face])   // start paired
        {
            if(pd == 0)
            {
                pd = nb + 1;
            }
            else if(pd > 0 && pd <= 6)
            {
                if(pd == 1)
                {
                    if(nb == 0)
                        pd = 21;
                    else
                        pd = 6 + nb;
                }
                else if(pd == 2)
                {
                    if(nb == 0)
                        pd = 7;
                    else if(nb == 1)
                        pd = 22;
                    else
                        pd = 9 + nb;
                }
                else if(pd == 3)
                {
                    if(nb == 0)
                        pd = 8;
                    else if(nb == 1)
                        pd = 11;
                    else if(nb == 2)
                        pd = 23;
                    else
                        pd = 11 + nb;
                }
                else if(pd == 4)
                {
                    if(nb == 0)
                        pd = 9;
                    else if(nb == 1)
                        pd = 12;
                    else if(nb == 2)
                        pd = 14;
                    else  if(nb == 3)
                        pd = 24;
                    else if(nb == 4)
                        pd = 16;
                }
                else if(pd == 5)
                {
                    if(nb == 0)
                        pd = 10;
                    else if(nb == 1)
                        pd = 13;
                    else if(nb == 2)
                        pd = 15;
                    else  if(nb == 3)
                        pd = 16;
                    else if(nb == 4)
                        pd = 25;
                }

            }
            else if(pd > 6 && pd <= 16)
            {
                if(pd == 7)
                {
                    if(nb == 0)
                        pd = 26;
                    else if(nb == 1)
                        pd = 29;
                    else if(nb == 2)
                        pd = 17;
                    else if(nb == 3)
                        pd = 18;
                }
                else if(pd == 8)
                {
                    if(nb == 0)
                        pd = 27;
                    else if(nb == 1)
                        pd = 17;
                    else if(nb == 2)
                        pd = 32;
                    else if(nb == 3)
                        pd = 19;
                }
                else if(pd == 9)
                {
                    if(nb == 0)
                        pd = 28;
                    else if(nb == 1)
                        pd = 18;
                    else if(nb == 2)
                        pd = 19;
                    else if(nb == 3)
                        pd = 35;
                }
                else if(pd == 11)
                {
                    if(nb == 0)
                        pd = 17;
                    else if(nb == 1)
                        pd = 30;
                    else if(nb == 2)
                        pd = 33;
                    else if(nb == 3)
                        pd = 20;
                }
                else if(pd == 12)
                {
                    if(nb == 0)
                        pd = 18;
                    else if(nb == 1)
                        pd = 31;
                    else if(nb == 2)
                        pd = 20;
                    else if(nb == 3)
                        pd = 36;
                }
                else if(pd == 14)
                {
                    if(nb == 0)
                        pd = 19;
                    else if(nb == 1)
                        pd = 20;
                    else if(nb == 2)
                        pd = 34;
                    else if(nb == 3)
                        pd = 37;
                }
            }
            else if(pd > 16 && pd <= 20)
            {
                if(pd == 17)
                {
                    pd = 38 + nb;
                }
                else
                {
                    // ---
                }
            }
            else if(pd > 20 && pd <= 25)
            {
                if(pd == 21)
                {
                    if(nb == 0)
                        pd = 41;
                    else if(nb == 1)
                        pd = 26;
                    else if(nb == 2)
                        pd = 27;
                    else if(nb == 3)
                        pd = 28;
                }
                else if(pd == 22)
                {
                    if(nb == 0)
                        pd = 29;
                    else if(nb == 1)
                        pd = 42;
                    else if(nb == 2)
                        pd = 30;
                    else if(nb == 3)
                        pd = 31;
                }
                else if(pd == 23)
                {
                    if(nb == 0)
                        pd = 32;
                    else if(nb == 1)
                        pd = 33;
                    else if(nb == 2)
                        pd = 43;
                    else if(nb == 3)
                        pd = 34;
                }
                else if(pd == 24)
                {
                    if(nb == 0)
                        pd = 35;
                    else if(nb == 1)
                        pd = 36;
                    else if(nb == 2)
                        pd = 37;
                    else if(nb == 3)
                        pd = 44;
                }
                else
                {
                    // ---
                }
            }
            else if(pd > 25 && pd <= 37)
            {
                if(pd == 26)
                {
                    if(nb == 0)
                        pd = 45;
                    else if(nb == 1)
                        pd = 53;
                    else if(nb == 2)
                        pd = 38;
                }
                else if(pd == 27)
                {
                    if(nb == 0)
                        pd = 46;
                    else if(nb == 1)
                        pd = 38;
                    else if(nb == 2)
                        pd = 54;
                }
                else if(pd == 29)
                {
                    if(nb == 0)
                        pd = 53;
                    else if(nb == 1)
                        pd = 47;
                    else if(nb == 2)
                        pd = 39;
                }
                else if(pd == 30)
                {
                    if(nb == 0)
                        pd = 39;
                    else if(nb == 1)
                        pd = 48;
                    else if(nb == 2)
                        pd = 55;
                }
                else if(pd == 32)
                {
                    if(nb == 0)
                        pd = 54;
                    else if(nb == 1)
                        pd = 40;
                    else if(nb == 2)
                        pd = 49;
                }
                else if(pd == 33)
                {
                    if(nb == 0)
                        pd = 40;
                    else if(nb == 1)
                        pd = 55;
                    else if(nb == 2)
                        pd = 50;
                }
                else
                {
                    // ---
                }
            }
            else if(pd > 37 && pd <= 40)
            {
                // ---
            }
            else if(pd > 40 && pd <= 44)
            {
                if(pd == 41)
                {
                    if(nb > 0)
                        pd = 44 + nb;
                }
                else if(pd == 42)
                {
                    if(nb == 0)
                        pd = 47;
                    else if(nb == 2)
                        pd = 48;
                }
                else if(pd == 43)
                {
                    if(nb == 0)
                        pd = 49;
                    else if(nb == 2)
                        pd = 50;
                }
            }
            else if(pd > 44 && pd <= 50)
            {
                if(pd == 45)
                {
                    if(nb == 1)
                        pd = 51;
                }
                else if(pd == 47)
                {
                    if(nb == 0)
                        pd = 52;
                }
            }
            else if(pd > 50 && pd <= 52)
            {
                // ---
            }
            else if(pd > 52 && pd <= 55)
            {
                if(pd == 53)
                {
                    if(nb == 0)
                        pd = 51;
                    else if(nb == 1)
                        pd = 52;
                }
                else
                {
                    // ----
                }
            }
        }
        else
        {
            _id += glNBSHMaskBit[card.face];
            if(pd == 0)
            {

            }
            else if(pd > 0 && pd <= 6)
            {
                if(pd >= nb + 1)
                    pd++;
            }
            else if(pd > 6 && pd <= 16)
            {
                switch(pd){
                    case 7:
                        if(nb == 0)
                            pd = 11;
                        else if(nb == 1)
                            pd = 8;
                        break;
                    case 8:
                        if(nb == 0)
                            pd = 12;
                        else if(nb > 0 && nb <= 2)
                            pd = 9;
                        break;
                    case 9:
                        if(nb == 0)
                            pd = 13;
                        else if(nb > 0 && nb <= 3)
                            pd = 10;
                        break;
                    case 10: // не может такого быть
                        break;
                    case 11:
                        if(nb >= 0 && nb <= 1)
                            pd = 14;
                        else if(nb == 2)
                            pd = 12;
                        break;
                    case 12:
                        if(nb >= 0 && nb <= 1)
                            pd = 15;
                        else if(nb >= 2 && nb <= 3)
                            pd = 13;
                        break;
                    case 13:
                        break;
                    case 14:
                        if(nb >= 0 && nb <= 2)
                            pd = 16;
                        else if(nb == 3)
                            pd = 15;
                        break;
                    case 15:
                        break;
                    case 16:
                        break;

                }
            }
            else if(pd > 16 && pd <= 20)
            {
                if(pd == 17)
                {
                    switch(nb){
                        case 0:
                            pd = 20;break;
                        case 1:
                            pd = 19;break;
                        case 2:
                            pd = 18;break;
                    }
                }
                // else не может такого быть

            }
            else if(pd > 20 && pd <= 25)
            {
                if(pd >= nb + 21)
                    pd++;
            }
            else if(pd > 25 && pd <= 37)
            {
                if(pd == 26)
                {
                    if(nb == 0)
                        pd = 30;
                    else if(nb == 1)
                        pd = 27;
                }
                else if(pd == 27)
                {
                    if(nb == 0)
                        pd = 31;
                    else if(nb >= 1 && nb <= 2)
                        pd = 28;
                }
                else if(pd == 29)
                {
                    if(nb == 0)
                        pd = 33;
                    else if(nb == 1)
                        pd = 32;
                }
                else if (pd == 30)
                {
                    if(nb >= 0 && nb <= 1)
                        pd = 34;
                    else if(nb == 2)
                        pd = 31;
                }
                else if(pd == 32)
                {
                    if(nb ==0)
                        pd = 36;
                    else if(nb >= 0 && nb <= 1)
                        pd = 35;
                }
                else if(pd == 33)
                {
                    if(nb >= 0 && nb <= 1)
                        pd = 37;
                    else if(nb == 2)
                        pd = 36;
                }
            }
            else if(pd > 37 && pd <= 40)
            {
                // ---
            }
            else if(pd > 40 && pd <= 44)
            {
                if(pd >= nb + 41)
                    pd++;
            }
            else if(pd > 44 && pd <= 50)
            {
                if(pd == 45)
                {
                    if(nb == 0)
                        pd = 48;
                    else if(nb == 1)
                        pd = 46;
                }
                else if(pd == 47)
                {
                    if(nb == 0)
                        pd = 50;
                    else if(nb == 1)
                        pd = 49;
                }
            }
            else if(pd > 50 && pd <= 52)
            {
                // ------
            }
            else if(pd > 52 && pd <= 55)
            {
                if(pd == 53)
                {
                    if(nb == 0)
                        pd = 55;
                    else if(nb == 1)
                        pd = 54;
                }

            }
        } // end paired
        prop0._cards[prop0._cnCard++] = card;
        CardStraight(prop0._cnCard,prop0._cards);
        int sd = GetSuitedFromCard(prop0._cards,prop0._cnCard);
        SetPaired(pd);
        SetSuited(sd);

    }
}
int clNaborishe::GetSuitedFromCard(clCard *cards,int cnCard)
{
    bool conversely; conversely = (cnCard == 6 || cnCard == 7);
    int sumSuited[CN_CARD_SUIT] = {0,0,0,0};
    int sumSuitedCnCard[CN_CARD_SUIT] = {0,0,0,0};
    if(cnCard == 6)
        for(int i = 0;i < CN_CARD_SUIT;i++)
            sumSuited[i] = 63;
    else if(cnCard == 7)
        for(int i = 0;i < CN_CARD_SUIT;i++)
            sumSuited[i] = 127;
    int nb = 0;
    for(int i = 0;i < cnCard; i++)
    {
        if(conversely)
            sumSuited[cards[i].suit] -= glNBSHMaskBit[nb];
        else
            sumSuited[cards[i].suit] += glNBSHMaskBit[nb];
        sumSuitedCnCard[cards[i].suit]++;
        if(i != cnCard - 1 && cards[i].face != cards[i+1].face)
            nb++;
    }
    int index,maxC  = -1;
    bool isTwoPlusTwo = false;
    for(int i = 0;i < CN_CARD_SUIT;i++)
        if(sumSuitedCnCard[i] > maxC)
        {
            maxC = sumSuitedCnCard[i];
            index = i;
        }
        else if(cnCard == 4 && maxC == 2 && maxC == sumSuitedCnCard[i])
        {
            isTwoPlusTwo = true;
			if (glNBSHArraySuited[sumSuited[index]] > glNBSHArraySuited[sumSuited[i]])
				index = i;
        }
    if(maxC + 7 - cnCard < 5)
        return 0;
    if(maxC == 6 && cnCard == 6)
        return 29;
    if(maxC == 7)
        return 30;
    if(isTwoPlusTwo)
        return glNBSHArraySuited[sumSuited[index]] + 21;
    if(conversely)
        return glNBSHArray67Suited[sumSuited[index]];

    return glNBSHArraySuited[sumSuited[index]];
}
//---------------------------------------------------------------------------

//#pragma package(smart_init)
