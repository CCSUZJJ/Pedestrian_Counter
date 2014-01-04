#ifndef CLOSEDINTERVAL_H
#define CLOSEDINTERVAL_H

// Template class implementing a closed interval [a,b]
// A closed interval is an interval that includes all of its limit points.
// If the endpoints of the interval are finite numbers a and b, then the interval {x:a<=x<=b} is denoted [a,b].
// If one of the endpoints is +/-infinity, then the interval still contains all of its limit points (although not all of its endpoints),
// so [a,infinity) and (-infinity,b] are also closed intervals, as is the interval (-infinity,infinity).

template<class T> class ClosedInterval
{
public:

    // Getters (made inline for fast-access)

    inline T GetStart() const { return Start; }
    inline T GetEnd() const { return End; }

    // Span = End-Start

    inline T GetSpan() const { return End-Start; }

    // Setters
    // Set Start = end == value

    void Set(T Value)
    {
        Start = Value;
        End = Value;
    }

    void Set(const ClosedInterval& IntervalOI)
    {
        *this = IntervalOI;
    }

    // Set both values (start and end)
    // Start = smallest value, end = largest value

    void Set(T Value1,T Value2)
    {
        if (Value1 <= Value2)
        {
            Start = Value1;
            End = Value2;
        } else
        {
            Start = Value2;
            End = Value1;
        }
    }

    // Reset -> Set(0,0)

    void Reset()
    {
        Set(0);
    }

    // Add a value. If the value is out of the current bounds,
    // then the interval is adjusted likewise

    void Update(T Value)
    {
        if (Value < Start) Start = Value;
        else if (Value > End) End = Value;
    }

    void Update(const ClosedInterval& IntervalOI)
    {
        if (IntervalOI.Start < Start) Start = IntervalOI.Start;
        if (IntervalOI.End > End) End = IntervalOI.End;
    }

    // Functions
    // InRange = Start <= Value <= End (exit true)

    bool IsInRange(T Value) const
    {
        return ((Value >= Start) && (Value <= End));
    }

    // 2 regions are overlapping (exit true) in case
    // (Start1 <= End2) && (Start2 <= End1)

    bool IsOverlapping(const ClosedInterval& IntervalOI) const
    {
        return ((IntervalOI.End >= Start) && (End >= IntervalOI.Start));
    }

    // Get the region of overlap (if any)
    // and return true in case of overlap, else return false (overlap is undefined)

    bool GetOverlap(const ClosedInterval& IntervalOI,ClosedInterval& Overlap) const
    {
        if (!IsOverlapping(IntervalOI)) return false;

        // Get max{start} and min{end}

        T OverlapStart = IntervalOI.Start;
        if (OverlapStart < Start) OverlapStart = Start;
        T OverlapEnd = IntervalOI.End;
        if (OverlapEnd > End) OverlapEnd = End;
        Overlap.Set(OverlapStart,OverlapEnd);
        return true;
    }

    // Return the number of pixels that are involved in the overlap

    unsigned int GetNumberOfOverlappingPixels(const ClosedInterval& IntervalOI) const
    {
        if (IntervalOI.End < Start) return 0;
        if (IntervalOI.Start > End) return 0;

        // Determine overlap

        T StartOverlap = (Start > IntervalOI.Start) ? Start : IntervalOI.Start;
        T Endoverlap = (End < IntervalOI.End) ? End : IntervalOI.End;

        // Number of pixels involved ?

        return Endoverlap - StartOverlap + 1;
    }

    // Operators

    bool operator==(const ClosedInterval& IntervalOI) const
    {
        return ((Start == IntervalOI.Start) && (End == IntervalOI.End));
    }

    bool operator!=(const ClosedInterval& IntervalOI) const
    {
        return ((Start != IntervalOI.Start) || (End != IntervalOI.End));
    }

    // Constructors

    ClosedInterval() : Start(0),End(0) {}
    ClosedInterval(T Value) : Start(Value),End(Value) {}
    ClosedInterval(T Value1,T Value2) {Set(Value1,Value2);}

    // Data (Start <= End)

protected :

    T Start;
    T End;
};

#endif // CLOSEDINTERVAL_H
