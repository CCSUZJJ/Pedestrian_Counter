#ifndef REGION_OF_ROWS_H_INCLUDE_
#define REGION_OF_ROWS_H_INCLUDE_

#include <vector>
#include "ClosedInterval.h"

namespace vipnt
{
// class RegionOfRows
//
//	Describes a region of pixels
//     Row(i)		****
//     Row(i+1)		  ****
//     Row(i+2)		 *******
//     ........	*******
//     Row(n)		******
//
//    Rows (where the region is defined) from Row.Start .. Row.End
//    Columns : For each defined row, the region is defined
//	            from StartColumn until EndColumn
//                   (Start & End are part of the region)
//
//    A region can be empty (@ construction)
//    A region can also consist of a single point
//
//  Note that the row,colums are related to this class and do not
//  necessarily coincide with the rows ans columns of a give matrix or image

class RegionOfRows
{
public :

    // Is the given location defined by row & column in the region?

    bool IsInRegion(unsigned int Column,unsigned int Row) const;

    // Is the given location specified by row & column in the region?
    // (Compensate for rounding-errors between the rows)
    // Column-range for row-1/row+1 is tested

    bool IsInRoundedOffRegion(unsigned int Column,unsigned int Row) const;

    // Get special vertices

    unsigned int GetMostLeftColumn() const;
    unsigned int GetMostRightColumn() const;
    unsigned int GetTopRow() const;
    unsigned int GetBottomRow() const;

    // BuildFunctions
    // Add entry : It is required to add the entries according to ascending rows

    void Add(unsigned int Row,unsigned int ColumnStart,unsigned int ColumnEnd)
    {
        if (!IsDefined()) Rows.Set(Row);
        else
        {
            // Ascending ?

            if (Row != (Rows.GetEnd()+1)) return;		// Invalid operation ... get out of here
        }

        Rows.Update(Row);
        Columns.push_back(ClosedInterval<unsigned int>(ColumnStart,ColumnEnd));
        Area += (ColumnEnd-ColumnStart)+1;
    }

    // Make the region undefined

    void Reset()
    {
        Rows.Set(0,0);
        Columns.clear();
        Area = 0;
    }

    // return true if the region is defined (exists), else false

    bool inline IsDefined() const {return !Columns.empty();}

    // Getters

    unsigned int inline GetArea() const {return Area;}

    // Get array of defined columns [Rowstart ... RowEnd]
    // Returns NULL in case of an empty region

    const ClosedInterval<unsigned int>* GetColumns() const
    {
        if (Columns.empty()) return 0;
        return &(*Columns.begin());
    }

    // Get a column-region for a specified row.
    // Will return NULL in case the row is not defined for the given region

    const ClosedInterval<unsigned int>* GetColumnRegion(unsigned int Row) const
    {
        if (Columns.empty()) return 0;
        if ((Row < Rows.GetStart()) || (Row > Rows.GetEnd())) return 0;
        return &Columns[Row - Rows.GetStart()];
    }

    // Get first region (if defined)

    const ClosedInterval<unsigned int>* GetFirstColumnRegion() const
    {
        if (Columns.empty()) return 0;
        return &Columns[0];
    }

    const ClosedInterval<unsigned int>* GetLastColumnRegion() const
    {
        if (Columns.empty()) return 0;
        return &Columns[Rows.GetSpan()];
    }

    // Get the defined rows of the region.
    // Will return NULL in case the region is empty

    const ClosedInterval<unsigned int>* GetRows() const
    {
        if (Columns.empty()) return 0;
        return &Rows;
    }

    // Fast getters

    inline unsigned int GetFirstRow() const {return Rows.GetStart();}
    inline unsigned int GetLastRow() const {return Rows.GetEnd();}
    inline unsigned int GetNumberOfRows() const {return Rows.GetSpan() + 1;}

    // Dimensions

    unsigned int GetNumberOfColumnsOnLargestRow() const;

    // UnitTest-features (this function must always return true)
    // Don't use in normal code (doesn't make sense)

    bool IsValidCode() const;

    // General functions

    bool GetIntersection(const RegionOfRows& RegionOI,RegionOfRows& Intersection) const;
    void Copy(const RegionOfRows& RegionOI);

    // Shift the region
    // Region is shifted over the specified displacement.

    void ShiftRegion(int RowDisplacement,int ColumnDisplacement);

    // Special functions

    const ClosedInterval<unsigned int>* GetCenterRow(unsigned int& RowCenter) const;

    // Operators

    bool operator==(const RegionOfRows& Other) const;
    bool operator!=(const RegionOfRows& Other) const {return !(*this == Other);}

    // Constructors

    RegionOfRows() : Area(0) {}

    // Class-members

private:

    unsigned int Area;				// Total number of pixels

    // For RowStart until RowEnd

    ClosedInterval<unsigned int> Rows;
    std::vector<ClosedInterval<unsigned int> > Columns;

    // Private functions

    ClosedInterval<unsigned int>* GetMyColumns() { return &(*Columns.begin()); }
};

} // namespace vipnt

#endif //REGION_OF_ROWS_H_INCLUDE_

