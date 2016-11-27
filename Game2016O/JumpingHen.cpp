#include "stdafx.h"
#include "JumpingHen.h"


JumpingHen::JumpingHen()
{
}

void JumpingHen::move()
{
	d_x += incrementX;
	d_y += incrementY;
	if (d_z == 0) {
		d_z += 10;
	}else {
		d_z = 0;
	}

	traveledDistance += sqrt(incrementX*incrementX + incrementY*incrementY);
}


JumpingHen::~JumpingHen()
{
}
