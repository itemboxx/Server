//---------------------------------------------------------------------------

#ifndef clWeightLevelH
#define clWeightLevelH
#include	"vector"
//---------------------------------------------------------------------------
struct tpWeightL
{
	float _weight, _level;
};

class clWeightLevel : public std::vector < tpWeightL >
{
public:
	bool	Load(int dim, float *arr);
	float	Level(int nb)		{ return (*this)[nb]._level + (*this)[nb]._weight / 2; }
};
//---------------------------------------------------------------------------
#endif
