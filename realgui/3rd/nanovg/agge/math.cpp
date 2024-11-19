#include <agge/math.hpp>

#include <math.h>

namespace agge
{
template <>
inline float limits<float>::resolution()
{   return 1e-6f;   }

template <>
inline double limits<double>::resolution()
{   return 1e-15;   }


const real_t distance_epsilon = limits<real_t>::resolution();
const real_t pi = real_t(3.14159265359);


float sqrt(float x)
{   return nvg__sqrtf(x);   }

double sqrt(double x)
{   return nvg__sqrtf(x);   }

float sin(float a)
{   return nvg__sinf(a);    }

float cos(float a)
{   return nvg__cosf(a);    }
}
