#ifndef CIRCULARBUFFERSTORAGE_H
#define CIRCULARBUFFERSTORAGE_H

// Circular buffer template
// Data is stored round-robin into the buffer


template<class T> class CircularBuffer
{
public:

    // Constructor

    CircularBuffer()
        : Buffer(0),SizeBuffer(0),IndexStorage(0),BufferIsFull(false)
    {
    }

    // Destructor

    virtual ~CircularBuffer()
    {
        if (Buffer != 0) delete[] Buffer;
        Buffer = 0;
    }

    // Reset = deallocate buffer => no more buffer-capacity

    void Reset()
    {
        if (Buffer != 0) delete[] Buffer;
        Buffer = 0;
        SizeBuffer = 0;
        IndexStorage = 0;
        BufferIsFull = false;
    }

    // Set the amount of data to store. In case the buffer has been already
    // defined, the storage will be reset.
    //
    // Remark : Size of the storage may not exceed the maximum-positive-integer-value !
    // For this reason, the input-parameter is specified as int

    void SetStorage(int NumberOfDataToStore)
    {
        if (NumberOfDataToStore <= 0)
        {
            // Zero or negative size storagesize specified => no storage

            if (Buffer != 0)
            {
                delete[] Buffer;
            }

            Buffer = 0;
            SizeBuffer = 0;
        } // end no storage
        else
        {
            if (Buffer != 0)
            {
                if (NumberOfDataToStore != SizeBuffer)
                {
                    // Reallocate bufferspace

                    delete[] Buffer;
                    Buffer = new T[NumberOfDataToStore];
                }
            }
            else
            {
                // Allocate new buffer

                Buffer = new T[NumberOfDataToStore];
            }

            SizeBuffer = NumberOfDataToStore;
        }

        IndexStorage = 0;
        BufferIsFull = false;
    }

    // Add a value. Value is stored at the current storage-index,
    // after adding index is adjusted. Old data is overwritten and
    // becomes the newest data!

    void Add(T Value)
    {
        if (Buffer != 0)
        {
            Buffer[IndexStorage] = Value;
            if (++IndexStorage == SizeBuffer)
            {
                IndexStorage = 0;
                BufferIsFull = true;
            }
        }
    }

    //  Index = 0  : last added value
    //         -1  : last before added value
    //         aso.
    //
    //  Function returns an undefined value if no bufferspace has been allocated

    T Get(int Index) const
    {
        if (Index > 0) return 0;                    // Undefined index
        if (!BufferIsFull)
        {
            // Buffer not full yet

            if (Index <=  -IndexStorage) return 0;  // Undefined value
        }
        else
        {
            // Buffer is full

            if (Index <= -SizeBuffer)  return 0;    // Undefined index
        }

        int Offset = IndexStorage+Index-1;
        if (Offset < 0) Offset += SizeBuffer;
        return Buffer[Offset];
    }

    // Returns an array of data
    // Data is stored as [index-n][Index-(n-1)] ....
    //
    // The caller is responsible for the buffersize of the DataBuffer.
    //
    // The number of data stored is returned (0 = no data). It is also possible that
    // no sufficient data is available to store the required amount of data

    unsigned int GetDataTimeAscendantOrdered(int Index,T* DataBuffer,unsigned int NumberOfDataToGet) const
    {
        if (Index > 0) return 0;                    // Undefined index
        if (DataBuffer == 0) return 0;              // No destination-buffer
        if (NumberOfDataToGet == 0) return 0;       // Nothing to get
        int NumberOfAvailableData = (int)GetNumberOfAvailableData();
        if (Index <= -NumberOfAvailableData) return 0;        // Data not available

        // Checking of parameters done. Set control-vars
        // to retrieve data. Not that storage-pointer is at the next "free" location

        NumberOfAvailableData += Index;
        int Offset = IndexStorage+Index-1;
        if (Offset < 0) Offset += SizeBuffer;
        int NumberOfDataToSave = NumberOfAvailableData;

        if (NumberOfDataToSave > 0)
        {
            if (NumberOfDataToSave > (int)NumberOfDataToGet)
            {
                // Get less data than data available

                NumberOfDataToSave = NumberOfDataToGet;
            }

            Offset -= (NumberOfDataToSave-1);
            if (Offset < 0)
            {
               Offset += SizeBuffer;
            }

            // Start data-transfert

            T* StoragePointer = DataBuffer;
            const T* SourcePointer = &Buffer[Offset];

            for (int i=0;i<NumberOfDataToSave;++i)
            {
                *StoragePointer++ = *SourcePointer++;
                Offset++;
                if (Offset == SizeBuffer)
                {
                    Offset = 0;
                    SourcePointer = Buffer;
                }
            }          
        }

        return NumberOfDataToSave;
    }

    // Get the amount of available data saved

    unsigned int GetNumberOfAvailableData() const
    {
        if (Buffer == 0) return 0;
        if (BufferIsFull) return SizeBuffer;
        return IndexStorage;
    }

    // Get the size of allocated buffer

    unsigned int GetAllocatedBufferSize() const
    {
        return SizeBuffer;
    }

private :

    T* Buffer;                  // Array
    int SizeBuffer;             // Allocated buffer-size
    int IndexStorage;           // index in buffer where new entry will be stored
    bool BufferIsFull;          // True = buffer is full, wrapped around
};

#endif // CIRCULARBUFFERSTORAGE_H
