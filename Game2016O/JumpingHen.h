#include "Hen.h"
class JumpingHen :
	public Hen
{
public:
	JumpingHen();
	JumpingHen(int objectId, double scale, CMesh * mesh,
		ScenarioPosition *initialPostion, bool paint,
		ScenarioPosition *finalPostion,
		int totalSteps, int barnId);
	void move();
	virtual ~JumpingHen();
};
