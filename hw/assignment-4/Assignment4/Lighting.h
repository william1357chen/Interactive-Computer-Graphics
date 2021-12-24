#include "Angel-yjc.h"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;


class Lighting {
public:
	
	color4 ambient;
	color4 diffuse;
	color4 specular;

	vec4 direction;

	point4 lightPos;  // In world frame
};