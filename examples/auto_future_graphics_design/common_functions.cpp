#include "common_functions.h"
#include <math.h>
/*
x'=(x-a)cos¦Á+(y-b)sin¦Á+a
y'=-(x-a)sin¦Á+(y-b)cos¦Á+b
*/

ImVec2 rotate_point_by_zaxis(ImVec2& tar, float angle, ImVec2& basePoint)
{
	ImVec2 des;
	des.x = (tar.x - basePoint.x)*cos(angle) + (tar.y - basePoint.y)*sin(angle) + basePoint.x;
	des.y = -(tar.x - basePoint.x)*sin(angle) + (tar.y - basePoint.y)*cos(angle) + basePoint.y;
	return des;
}
