//---------------------------------------------------------------------------

#ifndef clNaborisheH
#define clNaborisheH

#include <vector>
#include <windows.system.h>
#include "c:\home\card\pokGame.h"

#define NBSH_MAX_SIZE    0x1000000

#define NBSH_CN_1	13 
#define NBSH_CN_2	169
#define NBSH_CN_3	1755
#define NBSH_CN_4	16432
#define NBSH_CN_5	42783

#define NBSH_CN_WITH_ALL_1	13 
#define NBSH_CN_WITH_ALL_2	(NBSH_CN_1 + NBSH_CN_2)
#define NBSH_CN_WITH_ALL_3	(NBSH_CN_1 + NBSH_CN_2 + NBSH_CN_3)
#define NBSH_CN_WITH_ALL_4	(NBSH_CN_1 + NBSH_CN_2 + NBSH_CN_3 + NBSH_CN_4)
#define NBSH_CN_WITH_ALL_5	(NBSH_CN_1 + NBSH_CN_2 + NBSH_CN_3 + NBSH_CN_4 + NBSH_CN_5)

#define NBSH_SUM_CN_CARD_3	(52*51*50/6)	// Проверено !

#define NBSH_PATH_TO_VECTOR_FILE "c:\\home\\bin\\nabor_vector.bin"

#define NBSH_MASK_SUITED         0xF80000
#define NBSH_MASK_SUIT_0         0x07FFFF
#define NBSH_MASK_PAIRED         0x07E000
#define NBSH_MASK_PAIR_0         0xF81FFF


struct stNBSHProperty
{
    int _id,_cnNabor;
    byte _cnCard;
    clCard _cards[7];
    clGamePoker _game;
};
class clNaborishe
{
public:
    clNaborishe();
    void AddCard(clCard card);
    void AddManyCards(clCard *cards,int cnCard);
    int     Paired()    {return (_id&NBSH_MASK_PAIRED)>>13;}
    int     Suited()    {return (_id&NBSH_MASK_SUITED)>>19;}

    void    SetPaired(int cn)   {_id=_id&NBSH_MASK_PAIR_0;_id+=cn<<13;}
    void    SetSuited(int cn)   {_id=_id&NBSH_MASK_SUIT_0;_id+=cn<<19;}

    int GetSuitedFromCard(clCard *cards,int cnCard);
    void    NulledSuit();
	void ConvertSuit(clCard *cards, int cnCard);
private:
public:
    int _id;
};
int  NBSHCnCard(int id);
stNBSHProperty  NBSHGetProperty(int id);
extern std::vector <stNBSHProperty> glNBSHVect;
extern int *glNBSHArray;
extern int glNBSHMaskBit[];

void NBSHSaveVectorToFile();
bool NBSHLoadVectorFromFile();
void NBSHCloseGlobals();

//---------------------------------------------------------------------------
#endif
