#include "clValidateServer.h"

clValidateServer glVSServer;

clValidateServer::clValidateServer()
{
	
}


clValidateServer::~clValidateServer()
{
	if (_base.size() > 0)
	{
		int nb = 0;
		int handle = -1;
		while (true)
		{

			clAnsiString as = "C:\\data\\AllCorrectSit";
			as += (clAnsiString)nb + (clAnsiString)".bin";
			if ((handle = _open(as.c_str(), O_BINARY | O_RDWR)) == -1)
			{
				handle = _open(as.c_str(), O_BINARY | O_RDWR | O_CREAT);
				_chmod(as.c_str(), S_IREAD | S_IWRITE);
				break;
			}
			else
				_close(handle);
			nb++;
		}
		int mapSize = _base.size();
		_write(handle, &mapSize, sizeof(int));
		for (auto it = _base.begin(); it != _base.end(); it++)
		{
			if (it->second.size() != 0)
			{
				size_t size = it->second.size();
				_write(handle, &size, sizeof(size_t));
				WriteBigBlock(handle, &it->second, it->second.size() * sizeof(clSitHoldem));
			}
		}
	}
}

enNbError Inheritance(clSitHoldem sitOld, clSitHoldem sitNew)
{
	int cnPl = sitOld._atrHH._cnPl;
	if (sitOld._atrHH._cnPl != sitNew._atrHH._cnPl)
		return ERR_CNPL;
	if (sitOld._atrHH._maxSeat != sitNew._atrHH._maxSeat)
		return ERR_MXSEAT;
	if (sitOld._atrHH._tableCost != sitNew._atrHH._tableCost)
		return ERR_TABLECOST;
	if (sitOld._atrHH._bb != sitNew._atrHH._bb)
		return ERR_BB;
	if (sitOld._atrHH._ante != sitNew._atrHH._ante)
		return ERR_ANTE;
	if (sitOld._atrHH._nbTrny != sitNew._atrHH._nbTrny)
		return ERR_NBTRNY;
	if (sitOld._atrHH._nbG != sitNew._atrHH._nbG)
		return ERR_NBG;

	// повторим после выполнения актов
	for (int i = 0; i < sitOld.CnCardTable(); i++)
	{
		if (sitOld._atrHH._cardTable[i] != sitOld._atrHH._cardTable[i])
			return enNbError(ERR_TABLE_CARD0 + i);
	}

	if (strcmp(sitOld._nameTable, sitNew._nameTable) != 0)
		return ERR_NAMETABLE;
	if (memcmp(sitOld._arrNbPos, sitNew._arrNbPos, cnPl*sizeof(BYTE)) != 0)
		return ERR_NBPOS;

	for (int i = 0; i < cnPl; i++)
	{
		tpSeat &plOld = sitOld._seat[i], &plNew = sitOld._seat[i];
		if (strcmp(plOld._name, plOld._name) != 0)
			return ERR_NAMEPLAYER;
		for (int j = 0; j < 2; j++)
		{
			if (plOld._cards[j] != plNew._cards[j])
				return enNbError(ERR_PL_CARDS0 + j);
		}

		if (plOld._money != plNew._money)
			return ERR_PL_MONEY;
		if (plOld._inPlay != plNew._inPlay)
			return ERR_PL_INPLAY;
		if (plOld._busy != plNew._busy)
			return ERR_PL_BUSY;
	}

	for (int i = 0; i < sitOld._arrAct._cnAct; i++)
	{
		if (sitOld._arrAct._act[i] != sitNew._arrAct._act[i])
			return ERR_OLD_ACT;
	}

	if (sitOld._arrAct._cnAct > sitNew._arrAct._cnAct)
		return ERR_CNACT;


	for (int i = sitOld._arrAct._cnAct; i < sitNew._arrAct._cnAct; i++)
	{
		try
		{
			sitOld.ExecAct(sitNew._arrAct._act[i]);
		}
		catch (...)
		{
			return ERR_EXACTACT;
		}
	}
	if (sitOld.CnCardTable() != sitNew.CnCardTable())
		return ERR_CNCARDTABLE;

	if (sitOld.PosHero() != sitNew.PosHero())
		return ERR_POS_HERO;

	//повторяем
	for (int i = 0; i < sitOld.CnCardTable(); i++)
	{
		if (sitOld._atrHH._cardTable[i].Correct())
			if (sitOld._atrHH._cardTable[i] != sitOld._atrHH._cardTable[i])
				return enNbError(ERR_TABLE_CARD0 + i);
	}

	if (sitOld.TotalPot() != sitNew.TotalPot())
		return ERR_POT;

	if (sitOld.CurNbAct() != sitNew.CurNbAct())
		return ERR_NBACT;

	if (sitOld.NameTrade() != sitNew.NameTrade())
		return ERR_NAMETRADE;
	if (sitOld.RoundTrade() != sitNew.RoundTrade())
		return ERR_NAMETRADE;
	if (sitOld.Indicator() != sitNew.Indicator())
		return ERR_INDICATOR;
	if (sitOld.CNPlayerInTrade() != sitNew.CNPlayerInTrade())
		return ERR_CNPLINTRADE;
	if (sitOld.CNPlayerIsCard() != sitNew.CNPlayerIsCard())
		return ERR_CNPLISCARD;


	for (int i = 0; i < cnPl; i++)
	{
		if (0 != memcmp(sitOld.PlayerAtr(i), sitNew.PlayerAtr(i), sizeof(tpPlAtr)))
			return ERR_SITPL;
	}
	return AllIsGood;
}
enNbError S_Check(clSitHoldem sit)
{
	int cnPl = sit._atrHH._cnPl;
	if (sit._atrHH._cnPl<2 || sit._atrHH._cnPl> CN_PLAYER)
		return ERR_CNPL;
	if (sit._atrHH._maxSeat<2 || sit._atrHH._maxSeat>CN_PLAYER)
		return ERR_MXSEAT;
	/*if (sit._atrHH._tableCost < 0)
		return ERR_TABLECOST;*/
	if (sit._atrHH._bb <= 0)
		return ERR_BB;
	if (sit._atrHH._ante < 0)
		return ERR_ANTE;
	if (sit.CurAct() > sit._arrAct._cnAct)
		return ERR_NBACT;
	//int maxMoney = sit.PlMoney(0) * 6;
	for (int i = 0; i < cnPl; i++)
	{
		if (sit._seat[i]._money < 0)//sit in play
			return ERR_PL_MONEY;
	}
	if (sit.PosHero() != -1)
		for (int j = 0; j < 2; j++)
		{
			if (!sit.PlayerHand(sit.PosHero())._cards[j].Correct())
			{

				int face, suit;
				face = sit.PlayerHand(sit.PosHero())._cards[j].face;
				suit = sit.PlayerHand(sit.PosHero())._cards[j].suit;
				glTrasser.WriteMessage((clAnsiString)"1.Карта");
				glTrasser.WriteMessage((clAnsiString)face + (clAnsiString)"  " + (clAnsiString)suit);
				return enNbError(ERR_PL_CARDS0 + j);
			}
		}
	try
	{
		sit.ExecHist(sit._arrAct._cnAct);
	}
	catch (...)
	{
		return ERR_EXACTACT;
	}

	//повторяем
	int cnCard = 0;
	switch (sit.NameTrade())
	{
		case TRADE_PREFLOP:
			cnCard = 0;
			break;
		case TRADE_FLOP:
			cnCard = 3;
			break;
		case TRADE_TURN:
			cnCard = 4;
			break;
		case TRADE_RIVER:
			cnCard = 5;
			break;
		default:
			break;
	}

	if (cnCard != sit.CnCardTable())
		return ERR_CNCARDTABLE;

	for (int i = 0; i < cnCard; i++)
	{
		if (!sit._atrHH._cardTable[i].Correct())
			return enNbError(ERR_TABLE_CARD0 + i);
	}

	if (sit._arrAct._cnAct < 0)
		return ERR_CNACT;
	if (sit.CurNbAct() != sit._arrAct._cnAct)
		return ERR_CNACT;
	if (sit.TotalPot() < 0)
		return ERR_POT;

	int nbActShow = -1;
	for (int i = 0; i < sit.CnAct(); i++)
		if (sit.GetAct(i)->_dis._act == ACT_SHOW)
		{
			nbActShow = i;
			break;
		}
	if (nbActShow != -1)
	{
		if (sit.CnAct() != 0)
			if (sit.PosHero() < 0)
				return ERR_POS_HERO;

		int nbActBB = -1, nbActDeal = -1;
		for (int i = 0; i < sit.CnAct(); i++)
		{
			if (sit.GetAct(i)->_dis._act == ACT_POSTBB)
				nbActBB = i;
			if (sit.GetAct(i)->_dis._act == ACT_DELCARD)
				nbActDeal = i;

			if (nbActBB != -1 && nbActDeal != -1)
				break;
		}
		if (nbActBB == -1)
			return ERR_BB;
		if (nbActDeal == -1)
			return ERR_DEAL;

	}
	if (!sit.EndedSit())
		for (int i = 0; i < sit.CnPlayer(); i++)
		{
			if (sit._seat[i]._money != sit.PlayerStack(i))
				return ERR_PLMONEY;
			if (sit.PlayerViewCard(i)) 
				for (int j = 0; j < 2; j++)
					if (!sit.PlayerHand(sit.PosHero())._cards[j].Correct())
					{
						
						int face, suit;
						face = sit.PlayerHand(sit.PosHero())._cards[j].face;
						suit = sit.PlayerHand(sit.PosHero())._cards[j].suit;
						glTrasser.WriteMessage((clAnsiString)"2.Карта");
						glTrasser.WriteMessage((clAnsiString)face +(clAnsiString)"  "+(clAnsiString)suit);
						return enNbError(ERR_PL_CARDS0 + j);
					}
		}
	return AllIsGood;
}
//bool clValidateServer::PlayerProgressCheck(clSitHoldem sitOld, clSitHoldem sit, tpDis dis)
//{
//	
//	int nbAct = sitOld.AddAct(sitOld.ViewPlayer(), dis._act, dis._money);
//	if (!(sitOld.GetAct(nbAct)->_dis == sit.GetAct(nbAct)->_dis))
//	{
//		clAnsiString ss = (clAnsiString)"Ошибка :" +
//			HistDisToString(sit.GetAct(nbAct)->_dis) +
//			(clAnsiString)" вместо " +
//			HistDisToString(sitOld.GetAct(nbAct)->_dis);
//		char c = '\n';
//		_write(trasser, &c, sizeof(char));
//		char *str = ss.c_str();
//		_write(trasser, str, strlen(str));
//		return false;
//	}
//	else
//	{
//		clAnsiString ss = (clAnsiString)"Верно :" +
//			HistDisToString(sit.GetAct(nbAct)->_dis) +
//			(clAnsiString)" сделал " +
//			HistDisToString(sitOld.GetAct(nbAct)->_dis);
//		char c = '\n';
//		_write(trasser, &c, sizeof(char));
//		char *str = ss.c_str();
//		_write(trasser, str, strlen(str));
//		return false;
//	}
//	return true;
//}
enNbError clValidateServer::Add(clSitHoldem sit)
{
	enNbError nbErr = S_Check(sit);
	if (nbErr != AllIsGood)
		return nbErr;
	int nbTable = sit.NbTrny();
	std::vector<clSitHoldem> & vector = _base[nbTable];
	if (vector.size() >= 1 && vector.back().NbGame() == sit.NbGame())
	{
		nbErr = Inheritance(vector.back(), sit);
		glTrasser.WriteMessage((clAnsiString)"Inheritance = " + (clAnsiString)nbErr);
	}
	vector.push_back(sit);
	return AllIsGood;
}