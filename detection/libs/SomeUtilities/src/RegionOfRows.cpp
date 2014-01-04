#include "RegionOfRows.h"

namespace vipnt
{

// UnitTest-features : this function must always return true

bool RegionOfRows::IsValidCode() const
{
    return ((Rows.GetSpan()+1) == (unsigned int)(Columns.size()));
}

// Equality-operator : Check if two regions are equal

bool RegionOfRows::operator==(const RegionOfRows& Other) const
{
    if (!IsDefined()) return !Other.IsDefined();			// I'm not defined, we are equal if the other is not defined too

    // Both are defined

    const ClosedInterval<unsigned int>* OtherRows = Other.GetRows();
    if (*OtherRows != Rows) return false;	// Difference in rows => not equal

    // Same rows defined => check all columns

    const ClosedInterval<unsigned int>* OtherColumns = Other.GetColumns();
    const ClosedInterval<unsigned int>* MyColumns = GetColumns();

    unsigned int RowStart = Rows.GetStart();
    unsigned int RowEnd = Rows.GetEnd();
    for (unsigned int i=RowStart;i<=RowEnd;i++)
    {
        if (*OtherColumns++ != *MyColumns++) return false;
    }

    // Regions are equal

    return true;
}

// Is the given location defined by row & column in the region?
// Exit true if condition is valid

bool RegionOfRows::IsInRegion(unsigned int Column,unsigned int Row)const
{
    if (!IsDefined()) return false;			// Can't process an empty region

    // In range of rows ?

    if (!Rows.IsInRange(Row)) return false;	// Row is out of range

    // Row is in range, investigate the columns

    unsigned int RowOffset = Row - Rows.GetStart();

    // Column is in range ?

    return Columns[RowOffset].IsInRange(Column);
}

// Is the given location specified by row & column in the region?
// (Compensate for rounding-errors between the rows)
// Column-range for row-1/row+1 is tested
// Exit true if condition is valid

bool RegionOfRows::IsInRoundedOffRegion(unsigned int Column,unsigned int Row) const
{
    if (!IsDefined()) return false;			// Can't process an empty region

    // In range of rows ?

    if (!Rows.IsInRange(Row)) return false;	// Row is out of range

    // Row is in range, investigate the columns

    unsigned int RowOffset = Row - Rows.GetStart();

    // In range ?

    if (Columns[RowOffset].IsInRange(Column)) return true;

    // In range of previous row ?

    if (RowOffset > 0)
    {
        if (Columns[RowOffset-1].IsInRange(Column)) return true;
    }

    // In range of next row ?

    if (Row < Rows.GetEnd())
    {
        if (Columns[RowOffset+1].IsInRange(Column)) return true;
    }

    // not in region

    return false;
}


// Special GetVertices

unsigned int RegionOfRows::GetMostLeftColumn() const
{
    if (!IsDefined()) return 0;     // invalid returned result!

    // Start with the 1st row

    const ClosedInterval<unsigned int>* MyColumns = GetColumns();

    unsigned int NumberOfRows = Rows.GetSpan() + 1;
    unsigned int MostLeftColumn = MyColumns->GetStart();
    MyColumns++;

    for (unsigned int i=1; i < NumberOfRows;++i)
    {
        unsigned int Column = MyColumns->GetStart();
        if (Column < MostLeftColumn) MostLeftColumn = Column;
        MyColumns++;
    }

    return MostLeftColumn;
}

unsigned int RegionOfRows::GetMostRightColumn() const
{
    if (!IsDefined()) return 0;     // invalid returned result!

    // Start with the 1st row

    const ClosedInterval<unsigned int>* MyColumns = GetColumns();

    unsigned int NumberOfRows = Rows.GetSpan() + 1;
    unsigned int GetMostRightColumn = MyColumns->GetEnd();
    MyColumns++;

    for (unsigned int i=1; i < NumberOfRows;++i)
    {
        unsigned int Column = MyColumns->GetEnd();
        if (Column > GetMostRightColumn) GetMostRightColumn = Column;
        MyColumns++;
    }

    return GetMostRightColumn;
}

unsigned int RegionOfRows::GetTopRow() const
{
    if (!IsDefined()) return 0;
    return Rows.GetStart();
}

unsigned int RegionOfRows::GetBottomRow() const
{
    if (!IsDefined()) return 0;
    return Rows.GetEnd();
}

unsigned int RegionOfRows::GetNumberOfColumnsOnLargestRow() const
{
    if (!IsDefined()) return 0;
    unsigned int MaximumNumberofColumns = 0;
    const ClosedInterval<unsigned int>* MyColumns = GetColumns();
    unsigned int NumberOfRows = Rows.GetSpan() + 1;

    for (unsigned int i=0; i < NumberOfRows;++i)
    {
        unsigned int ColumnsOnRow = MyColumns->GetSpan() + 1;
        if (ColumnsOnRow > MaximumNumberofColumns)  MaximumNumberofColumns = ColumnsOnRow;

        MyColumns++;
    }

    return MaximumNumberofColumns;
}

bool RegionOfRows::GetIntersection(const RegionOfRows& RegionOI,RegionOfRows& Intersection) const
{
    Intersection.Reset();

    // Regions are defined ?

    if (!IsDefined()) return false;
    if (!RegionOI.IsDefined()) return false;

    // Regions are defined, do the have common rows ?

    const ClosedInterval<unsigned int>* Region1Rows = GetRows();
    const ClosedInterval<unsigned int>* Region2Rows = RegionOI.GetRows();
    ClosedInterval<unsigned int> RowsOI;
    if (!Region1Rows->GetOverlap(*Region2Rows,RowsOI)) return false;		// no rows in common
    unsigned int RowStart = RowsOI.GetStart();
    unsigned int RowEnd = RowsOI.GetEnd();

    const ClosedInterval<unsigned int>* Columns1 = GetColumns();
    const ClosedInterval<unsigned int>* Columns2 = RegionOI.GetColumns();

    // Adjust offets

    Columns1 += RowStart - GetFirstRow();
    Columns2 += RowStart - RegionOI.GetFirstRow();

    for (unsigned int Row = RowStart;Row <= RowEnd;Row++)
    {
        ClosedInterval<unsigned int> ColumnsOI;
        if (Columns1->GetOverlap(*Columns2,ColumnsOI))
        {
            // Rows and columns in common => intersection

            Intersection.Add(Row,ColumnsOI.GetStart(),ColumnsOI.GetEnd());
        } else if (Intersection.IsDefined()) break;			// Convex region !

        Columns1++;
        Columns2++;
    }

    // Any intersection ?

    return Intersection.IsDefined();
}

// Shift the region

void RegionOfRows::ShiftRegion(int RowDisplacement,int ColumnDisplacement)
{
    if (!IsDefined()) return;

    if (RowDisplacement != 0)
    {

        // Shift rows

        int StartRow = (int) Rows.GetStart();
        StartRow += RowDisplacement;

        if (StartRow < 0)
        {
            // Can't shift => invalidate !

            Reset();
            return;
        }

        int EndRow = (int) Rows.GetEnd();
        EndRow += RowDisplacement;
        Rows.Set((unsigned int)StartRow,(unsigned int)EndRow);
    }

    if (ColumnDisplacement != 0)
    {
        // Shift Columns

        ClosedInterval<unsigned int>* MyColumns = GetMyColumns();
        unsigned int NumberOfRows = Rows.GetSpan() + 1;

        for (unsigned int i=0; i < NumberOfRows;++i)
        {
            int StartColumn = (int) MyColumns->GetStart();
            StartColumn += ColumnDisplacement;

            if (StartColumn < 0)
            {
                // Can't shift => invalidate !

                Reset();
                return;
            }

            int EndColumn = (int) MyColumns->GetEnd();
            EndColumn += ColumnDisplacement;
            MyColumns->Set((unsigned int)StartColumn,(unsigned int)EndColumn);
            MyColumns++;
        }
    }
}

void RegionOfRows::Copy(const RegionOfRows& RegionOI)
{
    if (!RegionOI.IsDefined())
    {
        Reset();
        return;
    }

    // Copy Rowspan

    const ClosedInterval<unsigned int>* RowsOI = RegionOI.GetRows();
    Rows = *RowsOI;

    // Copy Columns

    unsigned int NumberOfRows = Rows.GetSpan() + 1;
    Columns.resize(NumberOfRows);
    ClosedInterval<unsigned int>* Columns1 = GetMyColumns();
    const ClosedInterval<unsigned int>* Columns2 = RegionOI.GetColumns();

    for (unsigned int i = 0;i < NumberOfRows;++i)
    {
        *Columns1++ = *Columns2++;
    }

    // Set Area

    Area = RegionOI.GetArea();
}

const ClosedInterval<unsigned int>* RegionOfRows::GetCenterRow(unsigned int& RowCenter) const
{
    if (!IsDefined()) return 0;
    unsigned StartRow = Rows.GetStart();
    RowCenter = (StartRow + Rows.GetEnd() + 1)/2;
    return &Columns[RowCenter - StartRow];
}

} // namespace vipnt
