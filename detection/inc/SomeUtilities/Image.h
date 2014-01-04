#ifndef _IMAGE_H_INCLUDED
#define _IMAGE_H_INCLUDED

// Lean image-template used for manipulating image-planes.
// The image has a layout of contiguous array of image-elements (pixels), such that
//     Offset = row-index * Columns + column-index with row-index [0 .. NRows-1] and columns-index [0 .. NColumns-1]
//
// As the emphasis lies on executionspeed, no boundaries are being checked. It is also not recommended
// to use most of the helper-features such as [][] when processingspeed is paramount.
//      Image[rowindex] return a pointer to the start of a row
//      Image[rowindex][columnindex] gives access to an element
//
// Limitations:
//		1. The access to arrays with constants isn't handled (one has to copy the array and pass the class as a constant class
//          downstream)
//      2. Basic image-manipulation-functions are deliberatedly left out (can be
//          bundeled into a static library)
//      3. The handling of an image with variable-size-elements will cause an invalid
//         mapping of the layout. T must only be assigned to objects of a constant predefined size!

namespace vipnt
{
template<class T> class Image
{
public:

    // Constructors (copy-constructor ain't allowed)

    Image() : m_NumberOfRows(0), m_NumberOfColumns(0),m_NumberOfElements(0),m_Data(0)  {}

    // Construct an image (image-values are undefined)
    explicit Image (unsigned int NumberOfRows, unsigned int NumberOfColumns)
    {
        this->m_NumberOfRows = NumberOfRows;                   // Number of rows (0 .. N)
        this->m_NumberOfColumns = NumberOfColumns;             // Number of columns (0 .. N)
        m_NumberOfElements = NumberOfRows * NumberOfColumns;   // Number of elements ( 0 .. N)
        if (m_NumberOfElements > 0)
        {
            m_Data = new T[m_NumberOfElements];
        }
        else
        {
            // Invalid arguments => Null-image

            m_NumberOfRows = 0;                   // Number of rows (0 .. N)
            m_NumberOfColumns = 0;                // Number of columns (0 .. N)
            m_NumberOfElements = 0;                     // Number of elements ( 0 .. N)
            m_Data = (T*)0;
        }
    }

    // Construct an image (image-values are set to the specified value)
    explicit Image(unsigned int NumberOfRows, unsigned int NumberOfColumns, T InitialValue)
    {
        this->m_NumberOfRows = NumberOfRows;                   // Number of rows (0 .. N)
        this->m_NumberOfColumns = NumberOfColumns;             // Number of columns (0 .. N)
        m_NumberOfElements = NumberOfRows * NumberOfColumns;   // Number of elements ( 0 .. N)
        if (m_NumberOfElements > 0)
        {
            m_Data = new T[m_NumberOfElements];
            Set(InitialValue);
        }
        else
        {
            // Invalid arguments => Null-image

            m_NumberOfRows = 0;                   // Number of rows (0 .. N)
            m_NumberOfColumns = 0;                // Number of columns (0 .. N)
            m_NumberOfElements = 0;                     // Number of elements ( 0 .. N)
            m_Data = (T*)0;
        }
    }

    // Construct a image (image-values are set to the contents from the specified array with the samse size)
    explicit Image(unsigned int NumberOfRows, unsigned int NumberOfColumns, const T *SourceData)
    {
        if (SourceData != (const T*)0)
        {
        this->m_NumberOfRows = NumberOfRows;                   // Number of rows (0 .. N)
        this->m_NumberOfColumns = NumberOfColumns;             // Number of columns (0 .. N)
        m_NumberOfElements = NumberOfRows * NumberOfColumns;   // Number of elements ( 0 .. N)
        if (m_NumberOfElements > 0)
        {
            m_Data = new T[m_NumberOfElements];
        }
        else m_Data = (T*)0;
        Set(SourceData);
        }
        else
        {
            // Invalid arguments => Null-image

            m_NumberOfRows = 0;                   // Number of rows (0 .. N)
            m_NumberOfColumns = 0;                // Number of columns (0 .. N)
            m_NumberOfElements = 0;                     // Number of elements ( 0 .. N)
            m_Data = (T*)0;
        }
    }

    // Destructor

    virtual ~Image()
    {
        if (m_Data != 0) delete[] m_Data;
    }

    // Main functions
    // GetSize returns the number of elements. An empty image
    // has no elements.

    inline unsigned int GetNumberOfElements() const { return m_NumberOfElements; }
    inline bool IsEmpty() const { return m_NumberOfElements==0;}

    // Get the number of rows (0 .. N)

    inline unsigned int GetNumberOfRows() const {return m_NumberOfRows;}

    // Get the number of columns (0 .. N)

    inline unsigned int GetNumberOfColumns() const {return m_NumberOfColumns;}

    // Access to data

    inline T* GetData() { return m_Data;}
    inline const T* GetData() const { return m_Data;}
    inline T* GetRow(unsigned int Row) { return &(m_Data[m_NumberOfColumns*Row]);}
    inline const T* GetRow(unsigned int Row) const { return &(m_Data[m_NumberOfColumns*Row]);}

    // Operator []

    inline T* operator[] (unsigned int Index) {return &(m_Data[m_NumberOfColumns*Index]);}
    inline const T* operator[] (unsigned int Index) const {return &(m_Data[m_NumberOfColumns*Index]);}

    // Reset -> make empty

    void Reset()
    {
        if (m_Data != 0)
        {
            delete[] m_Data;
        }

        m_Data = (T*)0;
        m_NumberOfRows = 0;
        m_NumberOfColumns = 0;
        m_NumberOfElements = 0;
    }

    // Set all elements to a specific value

    void Set(T Value)
    {
        T* Destination = m_Data;
        for(unsigned long i = 0; i < m_NumberOfElements; ++i)	*Destination++ = Value;
    }

    // Set to the contents from the specified array with the samse size)

    void Set(const T *SourceData)
    {
        T* Destination = m_Data;
        const T* Source = SourceData;
        for (unsigned int i = 0;i<m_NumberOfElements; ++i) *Destination++ = *Source++;
    }

    // If the current size matches the specified size, then dummy-operation,
    // else the current data is deleted and a new block is allocated

    void Resize(unsigned int NumberOfRows, unsigned int NumberOfColumns)
    {
        if ((this->m_NumberOfRows != NumberOfRows) || (this->m_NumberOfColumns != NumberOfColumns))
        {
            if (m_Data != 0)
            {
                delete[] m_Data;
            }
            this->m_NumberOfRows = NumberOfRows;                   // Number of rows (0 .. N)
            this->m_NumberOfColumns = NumberOfColumns;             // Number of columns (0 .. N)
            m_NumberOfElements = NumberOfRows * NumberOfColumns;   // Number of elements ( 0 .. N)
            if (m_NumberOfElements > 0)
            {
                m_Data = new T[m_NumberOfElements];
            }
            else m_Data = (T*)0;
        }
    }

    // If the current size matches the specified size, then set all the values to the specified value,
    // else the current data is deleted and a new block is allocated and set to the specified value

    void Resize(unsigned int NumberOfRows, unsigned int NumberOfColumns,T Value)
    {
        if ((this->m_NumberOfRows != NumberOfRows) || (this->m_NumberOfColumns != NumberOfColumns))
        {
            if (m_Data != 0)
            {
                delete[] m_Data;
            }
            this->m_NumberOfRows = NumberOfRows;                   // Number of rows (0 .. N)
            this->m_NumberOfColumns = NumberOfColumns;             // Number of columns (0 .. N)
            m_NumberOfElements = NumberOfRows * NumberOfColumns;   // Number of elements ( 0 .. N)
            if (m_NumberOfElements > 0)
            {
                m_Data = new T[m_NumberOfElements];
            }
            else m_Data = (T*)0;
        }

        // Set data

        Set(Value);
    }

    // Copy

    void Copy(const Image<T> &ImageOI)
    {
        // Set recipient to the appropriate size

        Resize(ImageOI.GetNumberOfRows(),ImageOI.GetNumberOfColumns());
        Set(ImageOI.GetData());
    }

    // Test-utilities

    bool MatchDimensions(const Image &ImageOI) const
    {
        return ((m_NumberOfRows == ImageOI.GetNumberOfRows())
                && (m_NumberOfColumns == ImageOI.GetNumberOfColumns()));
    }

private:

    unsigned int m_NumberOfRows;      // Number of rows (0 .. N)
    unsigned int m_NumberOfColumns;   // Number of columns (0 .. N)
    unsigned int m_NumberOfElements;  // Number of elements ( 0 .. N)
    T* m_Data;

    // Prevent copy-constructor

    Image(const Image<T>&);		// prevent copy_constructor
};

} // namespace vipnt

#endif //_IMAGE_H_INCLUDED
