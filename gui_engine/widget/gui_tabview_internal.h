#include <stdio.h>
#include <math.h>

// Define a small tolerance for error
#define EPSILON 1e-6

void cubicBezier(float t, float x1, float y1, float x2, float y2, float *outX, float *outY)
{
    float x0 = 0.0f, y0 = 0.0f;
    float x3 = 1.0f, y3 = 1.0f;

    // Compute the x and y coordinates of the Bezier curve
    float x = (1 - t) * (1 - t) * (1 - t) * x0 +
              3 * (1 - t) * (1 - t) * t * x1 +
              3 * (1 - t) * t * t * x2 +
              t * t * t * x3;

    float y = (1 - t) * (1 - t) * (1 - t) * y0 +
              3 * (1 - t) * (1 - t) * t * y1 +
              3 * (1 - t) * t * t * y2 +
              t * t * t * y3;

    // Output the calculated coordinates
    *outX = x;
    *outY = y;
}

// Solve for the value of t given a target x
float solveForT(float targetX, float x1, float y1, float x2, float y2)
{
    float tLow = 0.0f;
    float tHigh = 1.0f;
    float tMid = 0;
    float x, y;

    // Binary search for the value of t
    while (tHigh - tLow > EPSILON)
    {
        tMid = (tLow + tHigh) / 2.0f;
        cubicBezier(tMid, x1, y1, x2, y2, &x, &y);

        if (fabs(x - targetX) < EPSILON)
        {
            return tMid;
        }
        else if (x < targetX)
        {
            tLow = tMid;
        }
        else
        {
            tHigh = tMid;
        }
    }

    return tMid;
}

// Get the y value for a given x
float getYForX(float targetX, float x1, float y1, float x2, float y2)
{
    float t = solveForT(targetX, x1, y1, x2, y2);
    float x, y;
    cubicBezier(t, x1, y1, x2, y2, &x, &y);

    return y;
}

// int main() {
//     float x1 = 0.1f, y1 = 0.2f;
//     float x2 = 0.9f, y2 = 0.8f;
//     float targetX = 0.5f;

//     // Call the function to calculate the y value
//     float y = getYForX(targetX, x1, y1, x2, y2);

//     // Print the result
//     printf("When x = %f, y = %f\n", targetX, y);

//     return 0;
// }
