#ifndef GAUSSIANDISTRIBUTION_H
#define GAUSSIANDISTRIBUTION_H


class GaussianDistribution
{
    public:
        GaussianDistribution(float mean, float deviation);
        virtual ~GaussianDistribution();
        float getProbability(float x);
    protected:

    private:
        float mean;
        float deviation;
};

#endif // GAUSSIANDISTRIBUTION_H
