#ifndef CONNECTEDPIXELCLUSTERS_H_
#define CONNECTEDPIXELCLUSTERS_H_

#include <vector>
#include "BikeFrame.h"

namespace vipnt
{

class ConnectedPixelClusters
{
public:

	// Subsequentpixels on a row
	// All coordinates are image-coordinates (are always part of the image)

	class RowScan
	{
	public :

        // List of rowscans connected with each other

        RowScan* List;							// 0 = end of list

        // Image-coordinates

        unsigned int Row;						// row involved
        unsigned int LeftColumn;				// Bounding-rectangle
        unsigned int RightColumn;				// Bounding-rectangle

        // Functions

        inline unsigned int GetPixels() const {return RightColumn - LeftColumn + 1;}
	};

	// Cluster: a set of RowScans which are connected with each other
	// Layout (as seen by the caller)
	//	Cluster describes bounding-rectangle

	class Cluster
	{
	public :

        Cluster* List;                          // 0 = end of list		(List of objects(clusters)

        union
        {
            RowScan* HeadRowScans;              // List of rowscans 	(can be zero, eg. when GetClusters is called)
            Cluster* ClusterReference;          // Pointer to a cluster
        };

        RowScan* TailRowScans;                  // undefined var (internal usage)

        // Pixels involved

        unsigned int NumberOfPixels;

        // Bounding Rectangle

        unsigned int TopRow;					// Bounding-rectangle
        unsigned int BottomRow;				// Bounding-rectangle
        unsigned int LeftColumn;				// Bounding-rectangle
        unsigned int RightColumn;				// Bounding-rectangle

        // Functions on bounding-rectangle

        inline unsigned int GetHeight() const { return BottomRow - TopRow + 1; }
        inline unsigned int GetWidth() const { return RightColumn - LeftColumn + 1; }
        inline unsigned int GetCenterColumn() const { return (RightColumn + LeftColumn + 1)/2; }
        inline unsigned int GetCenterRow() const { return (TopRow + BottomRow + 1)/2; }
        inline unsigned int GetArea() const {return (BottomRow - TopRow + 1) * (RightColumn - LeftColumn + 1);}
	};

private:

        // Bufferspaces

        std::vector<RowScan> RowScans;
        std::vector<Cluster> Clusters;

        // Process-control

        Cluster* FreeObjectsList;			// Free record
        Cluster* HeadClusterList;			// ClusterList
        Cluster* TailClusterList;			// Last record has always a null-pointer

        // Internal function to reset the clusters (set them back to the free-objects-list)

        void ReleaseClusters();

public:

        // Public Functions

        void Reset();

        // Configuration. For speed-optimization we use static bufferspaces (avoiding new-operators). Enter
        // the number of records to be reserved. If the algo runs out of bufferspace, the function will stop

        void Configure(unsigned int BufferSpaceRowScans, unsigned int BufferSpaceClusters);
        const Cluster* GetClustersOfClosePixels(const vipnt::BikeFrame& Image,unsigned int MinimumDistance,unsigned int MinimumRequiredPixelsInCluster);
        const Cluster* Get8ConnectedClusters(const vipnt::BikeFrame& Image,unsigned int MinimumRequiredPixelsInCluster);
        const Cluster* GetClusters() const { return HeadClusterList; }

        // Test-utilities

        void ResetClusters();

        // Constructor/destructor

        ConnectedPixelClusters();
        virtual ~ConnectedPixelClusters() {}

private:

        // Prevent copy-constructor
        ConnectedPixelClusters(const ConnectedPixelClusters&);
};
} // namespace vipnt
#endif /* CONNECTEDPIXELCLUSTERS_H_ */
