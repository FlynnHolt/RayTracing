#ifndef INTERVAL_H
#define INTERVAL_H

class interval {
  public:
    double min, max;

    interval() : min(+infinity), max(-infinity) {}

    interval(double min, double max) : min(min), max(max) {}

    double size() const {
        return max - min;
    }

    bool contains(double x) const {
        return (x <= max) && (x >= min);
    }

    bool surrounds(double x) const {
        return (x < max) && (x > min);
    }

    double clamp(double x) const {
        // returns a clipped (clamped) value of x that is within [min,max].
        if (x < min) { return min; }
        if (x > max) { return max; }
        return x;
    }

    static const interval empty, universe;
};

const interval interval::empty    = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);

#endif