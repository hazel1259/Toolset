#pragma once
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

class CMathLib
{
public:
    CMathLib(void);
    ~CMathLib(void);
    // given the two sides of a right triangle, return the opposite angle
    //

    void GetSides(double angle, double radius, double * x, double* y)
    {
        double opposite = sin(ToRadians(angle))*radius;
        double adjacent = cos(ToRadians(angle))*radius;
        *x = adjacent;
        *y = opposite;
    }
    double GetAngle(double rise, double run)
    {
        return atan(rise/run);
    }
    
    // given the adjacent angle and the run, return the hypoteneuse
    //    
    double GetHypoteneuseFromAdjacent(double theta, double run)
    {
        return run/cos(theta); 
    }
    
    // given the opposite angle and the run, return the hypoteneuse
    //    
    double GetHypoteneuseFromOpposite(double rho, double rise)
    {
        return rise/sin(rho); 
    }
    
    // given the two hypoteneuse of a right triangle and the opposite angle
    // get the adjacent side
    //
    double GetAdjacent(double hypoteneuse, double theta)
    {
        return hypoteneuse*cos(theta);
    }
    
    // given the two hypoteneuse of a right triangle and the opposite angle
    // get the opposite side
    //
    double GetOpposite(double hypoteneuse, double theta)
    {
        return hypoteneuse*sin(theta);
    }
    
    double ToRadians(double degrees)
    {
        return degrees*(M_PI/180);
    }
    
    double ToDegrees(double radians)
    {
        return radians*(180/M_PI);
    }

    double GetHypoteneuse(double rise, double run)
    {
        return sqrt(rise*rise + run*run);
    }
};
