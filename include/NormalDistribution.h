#ifndef NORMALDISTRIBUTION_H
#define NORMALDISTRIBUTION_H

class NormalDistribution
{
  public:
    NormalDistribution(double m, double sd) : mean(m), stdDev(sd) {}
    double getMean() { return mean; }
    double getStdDev() { return stdDev; }
  private:
    double mean;
    double stdDev;
};

#endif
