/*
 * ConnectedPixelClusters.cpp
 *
 */

#include "ConnectedPixelClusters.h"

namespace vipnt
{

// Constructor

ConnectedPixelClusters::ConnectedPixelClusters()
{
    FreeObjectsList = 0;
    HeadClusterList = 0;
    TailClusterList = 0;
}

void ConnectedPixelClusters::ReleaseClusters()
{
    if (HeadClusterList != 0)
    {
        TailClusterList->List = FreeObjectsList;
        FreeObjectsList = HeadClusterList;
    }

    HeadClusterList = 0;
    TailClusterList = 0;
}

void ConnectedPixelClusters::ResetClusters()
{
    if (RowScans.size() > 0) ReleaseClusters();
    else Reset();
}

void ConnectedPixelClusters::Reset()
{
    FreeObjectsList = 0;
    HeadClusterList = 0;
    TailClusterList = 0;
    Clusters.resize(0);
    RowScans.resize(0);
}

// Configuration. For speed-optimization we use static bufferspaces (avoiding new-operators). Enter
// the number of records to be reserved. If the algo runs out of bufferspace, the function will stop
// generating clusters => bottom of image will not be processed
//
//  BufferSpaceRowScans = number of rowscans (worst case scenario is a blockpattern = columns/2 * rows)
//
//  BufferSpaceClusters = number of clusters (worst case scenario is columns*rows/4)

void ConnectedPixelClusters::Configure(unsigned int BufferSpaceRowScans, unsigned int BufferSpaceClusters)
{
    Reset();
    if ((BufferSpaceRowScans == 0) || (BufferSpaceClusters == 0)) return;   // Configuration-error
    RowScans.resize(BufferSpaceRowScans);

    FreeObjectsList = 0;
    Clusters.resize(BufferSpaceClusters);
    if (BufferSpaceClusters > 0)
    {
        FreeObjectsList = &Clusters.front();

        for(std::vector<Cluster>::iterator it = Clusters.begin();it != Clusters.end();++it)
        {
            if ((it+1) != Clusters.end()) it->List = &(*(it+1));
            else it->List = 0;
        }
    }

    HeadClusterList = 0;
    TailClusterList = 0;
}

// Get clusters for pixels that are 8-connected to each other. If enough memory has been
// provided (see Configure : BufferSpaceRowScan) then all rowscans will be available in the
// clusters returned. The general cluster-specs will always be ok (even if some rowscans
// are missing).

const ConnectedPixelClusters::Cluster* ConnectedPixelClusters::Get8ConnectedClusters(const vipnt::BikeFrame& Image,unsigned int MinimumRequiredPixelsInCluster)
{
    // bufferspace provided ?

    if (RowScans.size() == 0) return 0;

    // Reset buffer-layout

    ReleaseClusters();
    Cluster* HeadOpenObjectsList = 0;			// Clusters waiting to be closed
    Cluster* TailOpenObjectsList = 0;			// Clusters waiting to be closed

    unsigned int Rows = Image.GetHeight();
    unsigned int Columns = Image.GetWidth();

    // Determine pixelstatus and cluster all pixels set

    const unsigned char* Data = Image.GetData();

    // List of 'free' rowscans. First we use all defined records subsequently - some of them will
    // be 'released' (cluster too small).

    RowScan* FreeRowScanList = 0;					// List with free row-scan-objects
    unsigned int UsedRowScanIndex = 0;
    unsigned int MaximumRowScanIndex = RowScans.size();

    // Rowscans of the previous row

    Cluster* PreviousRowScans = 0;

    for (unsigned int Row=0;Row < Rows;Row++)
    {
        Cluster* LastRowScans = 0;
        Cluster* RowScanOI = 0;

        // Get regions with subsequent pixels set

        for (unsigned int Column=0;Column < Columns; Column++)
        {
            // PixelOI ?

            if (*Data++ == 0)
            {
                // Pixel not of interest. Close a running scan?

                if (RowScanOI != 0)
                {
                    RowScanOI->RightColumn = Column-1;
                    RowScanOI = 0;	// Close scan
                } // End close previous rowscan
            } else
            {
                // Pixel of interest

                if (RowScanOI == 0)
                {
                    RowScanOI = FreeObjectsList;
                    if (RowScanOI != 0)
                    {
                        FreeObjectsList = RowScanOI->List;
                        RowScanOI->List = LastRowScans;
                        LastRowScans = RowScanOI;

                        RowScanOI->LeftColumn = Column;
                        RowScanOI->ClusterReference = 0;			// No clusters involved
                    }
                }
            } // End pixelOI
        } // end for-loop columns

        // Close open runscan

        if (RowScanOI != 0) RowScanOI->RightColumn = Columns-1;

        // 8Connected to previous rowscans?
        //
        //  +----+       +---+
        //  |    |       |   |
        //  +----+       +---+
        //       +-------+
        //       |       |
        //       +-------+

        RowScanOI = LastRowScans;
        Cluster* PointerPreviousRowScan = PreviousRowScans;

        // Check if rowscans are 8-connected to previous rowscans. In case of clustering,
        // the rowscans are added to the cluster if the rowscan leaves the scope (= row-2)

        while (RowScanOI != 0)
        {
            // 8 Connected ?

            while (PointerPreviousRowScan != 0)
            {
                // Potential 8connection?

                if (RowScanOI->LeftColumn > (PointerPreviousRowScan->RightColumn+1)) break;		// Can't get an 8-connected cluster

                // 8 connected ?

                if (((PointerPreviousRowScan->RightColumn+1) >= RowScanOI->LeftColumn)
                        && ((RowScanOI->RightColumn+1) >= PointerPreviousRowScan->LeftColumn))
                {
                    // 8 connected !
                    // Create new cluster? Join an existing one or merge clusters ?

                    if (PointerPreviousRowScan->ClusterReference == 0)
                    {
                        // Previous rowscan not clustered yet

                        if (RowScanOI->ClusterReference == 0)
                        {
                            // None of us is part of cluster -> let's join ourselves into a cluster
                            // Add cluster in open-objects-list

                            PointerPreviousRowScan->ClusterReference = FreeObjectsList;
                            if (PointerPreviousRowScan->ClusterReference != 0)
                            {
                                FreeObjectsList = FreeObjectsList->List;
                                RowScanOI->ClusterReference = PointerPreviousRowScan->ClusterReference;

                                // Add in OpenObjectsList (pending clusters)

                                if (HeadOpenObjectsList == 0) HeadOpenObjectsList = PointerPreviousRowScan->ClusterReference;
                                else TailOpenObjectsList->List = PointerPreviousRowScan->ClusterReference;
                                TailOpenObjectsList = PointerPreviousRowScan->ClusterReference;
                                PointerPreviousRowScan->ClusterReference->List = 0;		// close list

                                // Setup cluster

                                PointerPreviousRowScan->ClusterReference->HeadRowScans = 0;
                                PointerPreviousRowScan->ClusterReference->TopRow = Row-1;
                                PointerPreviousRowScan->ClusterReference->BottomRow = Row;

                                if (RowScanOI->LeftColumn < PointerPreviousRowScan->LeftColumn) PointerPreviousRowScan->ClusterReference->LeftColumn = RowScanOI->LeftColumn;
                                else PointerPreviousRowScan->ClusterReference->LeftColumn = PointerPreviousRowScan->LeftColumn;
                                if (RowScanOI->RightColumn > PointerPreviousRowScan->RightColumn) PointerPreviousRowScan->ClusterReference->RightColumn = RowScanOI->RightColumn;
                                else PointerPreviousRowScan->ClusterReference->RightColumn = PointerPreviousRowScan->RightColumn;
                                PointerPreviousRowScan->ClusterReference->NumberOfPixels = (RowScanOI->RightColumn - RowScanOI->LeftColumn) + (PointerPreviousRowScan->RightColumn - PointerPreviousRowScan->LeftColumn) + 2;
                            }
                        } else
                        {
                            // Previous scan is not part of a cluster
                            // Add previous scan to my cluster. As I'm part of a cluster, the toprow of the cluster
                            // can't be greater than row-1 => not required to adjust rowspecs

                            PointerPreviousRowScan->ClusterReference = RowScanOI->ClusterReference;
                            if (PointerPreviousRowScan->LeftColumn < PointerPreviousRowScan->ClusterReference->LeftColumn) PointerPreviousRowScan->ClusterReference->LeftColumn = PointerPreviousRowScan->LeftColumn;
                            if (PointerPreviousRowScan->RightColumn > PointerPreviousRowScan->ClusterReference->RightColumn) PointerPreviousRowScan->ClusterReference->RightColumn = PointerPreviousRowScan->RightColumn;
                            PointerPreviousRowScan->ClusterReference->NumberOfPixels += (PointerPreviousRowScan->RightColumn - PointerPreviousRowScan->LeftColumn + 1);
                        }
                    } else
                    {
                        // Add or merge ?

                        PointerPreviousRowScan->ClusterReference->BottomRow = Row;
                        if (RowScanOI->ClusterReference == 0)
                        {
                            // Add myself to the existing cluster

                            RowScanOI->ClusterReference = PointerPreviousRowScan->ClusterReference;

                            if (RowScanOI->LeftColumn < PointerPreviousRowScan->ClusterReference->LeftColumn) PointerPreviousRowScan->ClusterReference->LeftColumn = RowScanOI->LeftColumn;
                            if (RowScanOI->RightColumn > PointerPreviousRowScan->ClusterReference->RightColumn) PointerPreviousRowScan->ClusterReference->RightColumn = RowScanOI->RightColumn;
                            PointerPreviousRowScan->ClusterReference->NumberOfPixels += (RowScanOI->RightColumn - RowScanOI->LeftColumn + 1);
                        } else
                        {
                            // Note that we are both clustered
                            // Merging clusters (if we are different clusters!)

                            if (RowScanOI->ClusterReference != PointerPreviousRowScan->ClusterReference)
                            {
                                Cluster* ClusterToRelease = RowScanOI->ClusterReference;
                                RowScanOI->ClusterReference = PointerPreviousRowScan->ClusterReference;

                                if (ClusterToRelease->TopRow < RowScanOI->ClusterReference->TopRow) RowScanOI->ClusterReference->TopRow = ClusterToRelease->TopRow;
                                if (ClusterToRelease->LeftColumn < PointerPreviousRowScan->ClusterReference->LeftColumn) PointerPreviousRowScan->ClusterReference->LeftColumn = ClusterToRelease->LeftColumn;
                                if (ClusterToRelease->RightColumn > PointerPreviousRowScan->ClusterReference->RightColumn) PointerPreviousRowScan->ClusterReference->RightColumn = ClusterToRelease->RightColumn;
                                PointerPreviousRowScan->ClusterReference->NumberOfPixels += ClusterToRelease->NumberOfPixels;

                                // If there already rowscans, then add all rowscans to the cluster

                                if (ClusterToRelease->HeadRowScans != 0)
                                {
                                    if (PointerPreviousRowScan->ClusterReference->HeadRowScans == 0) PointerPreviousRowScan->ClusterReference->HeadRowScans = ClusterToRelease->HeadRowScans;
                                    else PointerPreviousRowScan->ClusterReference->TailRowScans->List = ClusterToRelease->HeadRowScans;

                                    PointerPreviousRowScan->ClusterReference->TailRowScans = ClusterToRelease->TailRowScans;
                                }

                                // Adjust references to cluster to release

                                Cluster* ClusterOI = PreviousRowScans;
                                while (ClusterOI != 0)
                                {
                                    if (ClusterOI->ClusterReference == ClusterToRelease) ClusterOI->ClusterReference = RowScanOI->ClusterReference;
                                    ClusterOI = ClusterOI->List;
                                }

                                // Adjust references in current scan

                                ClusterOI = LastRowScans;
                                while (ClusterOI != RowScanOI)
                                {
                                    if (ClusterOI->ClusterReference == ClusterToRelease) ClusterOI->ClusterReference = RowScanOI->ClusterReference;
                                    ClusterOI = ClusterOI->List;
                                }

                                // Remove the cluster from the open clusters list

                                Cluster* PreviousCluster = 0;
                                ClusterOI = HeadOpenObjectsList;
                                while (ClusterOI != ClusterToRelease)
                                {
                                    PreviousCluster = ClusterOI;
                                    ClusterOI = ClusterOI->List;
                                }

                                if (PreviousCluster == 0) HeadOpenObjectsList = HeadOpenObjectsList->List;
                                else PreviousCluster->List = ClusterToRelease->List;

                                if (TailOpenObjectsList == ClusterToRelease)
                                {
                                    if (PreviousCluster == 0) TailOpenObjectsList = HeadOpenObjectsList;
                                    else TailOpenObjectsList = PreviousCluster;
                                }

                                // Finally, release the cluster.

                                ClusterToRelease->List = FreeObjectsList;
                                FreeObjectsList = ClusterToRelease;
                            } // End cluster-merge
                        } // end both scans are clustered
                    }
                } // end 8-connection

                // Release previous rowscan ?

                if (RowScanOI->LeftColumn >= PointerPreviousRowScan->LeftColumn) break;			// Next scans can't be 8-connected
                PointerPreviousRowScan = PointerPreviousRowScan->List;
            } //end while previous rowscans

            RowScanOI = RowScanOI->List;
        } // end while lastrowscans

        // Process previous rowscans (scans are going out of scope)

        while (PreviousRowScans != 0)
        {
            Cluster* ClusterToRelease = PreviousRowScans;
            PreviousRowScans = ClusterToRelease->List;

            // Clustered ?

            if (ClusterToRelease->ClusterReference != 0)
            {
                // Clustered (add my rowscan)

                if (ClusterToRelease->ClusterReference->BottomRow < (Row-1)) ClusterToRelease->ClusterReference->BottomRow = Row-1;

                RowScan* MyRowScan = 0;
                if (UsedRowScanIndex < MaximumRowScanIndex) MyRowScan = &RowScans[UsedRowScanIndex++];
                else
                {
                    // Use released rowscans

                    MyRowScan = FreeRowScanList;
                    if (MyRowScan != 0) FreeRowScanList = MyRowScan->List;
                }

                if (MyRowScan != 0)
                {
                    MyRowScan->List = 0;
                    MyRowScan->LeftColumn = ClusterToRelease->LeftColumn;
                    MyRowScan->RightColumn = ClusterToRelease->RightColumn;
                    MyRowScan->Row = Row-1;

                    if (ClusterToRelease->ClusterReference->HeadRowScans == 0) ClusterToRelease->ClusterReference->HeadRowScans = MyRowScan;
                    else ClusterToRelease->ClusterReference->TailRowScans->List = MyRowScan;

                    ClusterToRelease->ClusterReference->TailRowScans = MyRowScan;
                }

                // Release object

                ClusterToRelease->List = FreeObjectsList;
                FreeObjectsList = ClusterToRelease;
            } else
            {
                // Isolated rowscan
                // Consider as a cluster ?

                unsigned int PixelsInvolved = ClusterToRelease->RightColumn - ClusterToRelease->LeftColumn + 1;
                if (PixelsInvolved >= MinimumRequiredPixelsInCluster)
                {
                    // Consider isolated rowscan as a cluster
                    // Add myself to the clusters !
                    // Any free Rowscans ?

                    RowScan* MyRowScan = 0;
                    if (UsedRowScanIndex < MaximumRowScanIndex) MyRowScan = &RowScans[UsedRowScanIndex++];
                    else
                    {
                        // Use released rowscans

                        MyRowScan = FreeRowScanList;
                        if (MyRowScan != 0) FreeRowScanList = MyRowScan->List;
                    }

                    // Memory allocated ?

                    if (MyRowScan != 0)
                    {
                        MyRowScan->List = 0;
                        ClusterToRelease->HeadRowScans = MyRowScan;
                        ClusterToRelease->TailRowScans = MyRowScan;

                        MyRowScan->LeftColumn = ClusterToRelease->LeftColumn;
                        MyRowScan->RightColumn = ClusterToRelease->RightColumn;
                        MyRowScan->Row = Row-1;
                    }

                    ClusterToRelease->NumberOfPixels = PixelsInvolved;
                    ClusterToRelease->TopRow = Row-1;
                    ClusterToRelease->BottomRow = Row-1;

                    // Cluster = closed

                    if (HeadClusterList == 0) HeadClusterList = ClusterToRelease;
                    else TailClusterList->List = ClusterToRelease;

                    TailClusterList = ClusterToRelease;
                    ClusterToRelease->List = 0;
                } else
                {
                    ClusterToRelease->List = FreeObjectsList;
                    FreeObjectsList = ClusterToRelease;
                }
            }
        }

        // Release closed clusters ?

        Cluster* ClusterOI = HeadOpenObjectsList;
        while (ClusterOI != 0)
        {
            // Enough pixels ?

            if (ClusterOI->BottomRow == Row) break;
            HeadOpenObjectsList = ClusterOI->List;
            if (HeadOpenObjectsList == 0) TailOpenObjectsList = 0;

            // ObjectOI ?

            if (ClusterOI->NumberOfPixels >= MinimumRequiredPixelsInCluster)
            {
                if (HeadClusterList == 0) HeadClusterList = ClusterOI;
                else TailClusterList->List = ClusterOI;

                TailClusterList = ClusterOI;
                ClusterOI->List = 0;
            } else
            {
                if (ClusterOI->HeadRowScans != 0)
                {
                    ClusterOI->TailRowScans->List = FreeRowScanList;
                    FreeRowScanList	 = ClusterOI->HeadRowScans;
                }

                // 'release' cluster (Not of interest)

                ClusterOI->List = FreeObjectsList;
                FreeObjectsList = ClusterOI;
            }

            ClusterOI = HeadOpenObjectsList;
        } // end while release closed clusters

        // Start processing a new row

        PreviousRowScans = LastRowScans;
    } // end for-loop rows

    // Open rowscans ?

    while (PreviousRowScans != 0)
    {
        Cluster* ClusterToRelease = PreviousRowScans;
        PreviousRowScans = ClusterToRelease->List;

        // Clustered ?

        if (ClusterToRelease->ClusterReference != 0)
        {
            // Clustered (add my rowscan)

            RowScan* MyRowScan = 0;
            if (UsedRowScanIndex < MaximumRowScanIndex) MyRowScan = &RowScans[UsedRowScanIndex++];
            else
            {
                // Used releases rowscans

                MyRowScan = FreeRowScanList;
                if (MyRowScan != 0) FreeRowScanList = MyRowScan->List;
            }

            if (MyRowScan != 0)
            {
                MyRowScan->List = 0;
                MyRowScan->LeftColumn = ClusterToRelease->LeftColumn;
                MyRowScan->RightColumn = ClusterToRelease->RightColumn;
                MyRowScan->Row = Rows-1;

                if (ClusterToRelease->ClusterReference->HeadRowScans == 0) ClusterToRelease->ClusterReference->HeadRowScans = MyRowScan;
                else ClusterToRelease->ClusterReference->TailRowScans->List = MyRowScan;

                ClusterToRelease->ClusterReference->TailRowScans =  MyRowScan;
            }

            ClusterToRelease->List = FreeObjectsList;
            FreeObjectsList = ClusterToRelease;
        } else
        {
            // Isolated rowscan
            // Consider as a cluster ?

            unsigned int PixelsInvolved = ClusterToRelease->RightColumn - ClusterToRelease->LeftColumn + 1;
            if (PixelsInvolved >= MinimumRequiredPixelsInCluster)
            {
                // Consider isolated rowscan as a cluster
                // Add myself to the clusters !
                // Any free Rowscans ?

                RowScan* MyRowScan = 0;
                if (UsedRowScanIndex < MaximumRowScanIndex) MyRowScan = &RowScans[UsedRowScanIndex++];
                else
                {
                    // Used releases rowscans

                    MyRowScan = FreeRowScanList;
                    if (MyRowScan != 0) FreeRowScanList = MyRowScan->List;
                }

                // Memory allocated ?

                if (MyRowScan != 0)
                {
                    MyRowScan->List = 0;
                    MyRowScan->LeftColumn = ClusterToRelease->LeftColumn;
                    MyRowScan->RightColumn = ClusterToRelease->RightColumn;
                    MyRowScan->Row = Rows-1;

                    if (ClusterToRelease->HeadRowScans == 0) ClusterToRelease->HeadRowScans = MyRowScan;
                    else ClusterToRelease->TailRowScans->List = MyRowScan;

                    ClusterToRelease->TailRowScans =  MyRowScan;
                }

                ClusterToRelease->NumberOfPixels = PixelsInvolved;
                ClusterToRelease->TopRow = Rows-1;
                ClusterToRelease->BottomRow = Rows-1;

                // Cluster = closed

                if (HeadClusterList == 0) HeadClusterList = ClusterToRelease;
                else TailClusterList->List = ClusterToRelease;

                TailClusterList = ClusterToRelease;
                ClusterToRelease->List = 0;
            } else
            {
                ClusterToRelease->List = FreeObjectsList;
                FreeObjectsList = ClusterToRelease;
            }
        }
    }

    // Open clusters ?

    Cluster* ClusterOI = HeadOpenObjectsList;
    while (ClusterOI != 0)
    {
        HeadOpenObjectsList = ClusterOI->List;

        // Enough pixels ?

        if (ClusterOI->NumberOfPixels >= MinimumRequiredPixelsInCluster)
        {
            if (HeadClusterList == 0) HeadClusterList = ClusterOI;
            else TailClusterList->List = ClusterOI;

            TailClusterList = ClusterOI;
            ClusterOI->List = 0;
        } else
        {
            // 'release' cluster (Not of interest)

            ClusterOI->List = FreeObjectsList;
            FreeObjectsList = ClusterOI;
        }
        ClusterOI = HeadOpenObjectsList;
    }

    // Finally, return the clusters found

    return HeadClusterList;
}

const ConnectedPixelClusters::Cluster* ConnectedPixelClusters::GetClustersOfClosePixels(const vipnt::BikeFrame& Image,unsigned int MinimumDistance,unsigned int MinimumRequiredPixelsInCluster)
{
    // bufferspace provided ?

    if (RowScans.size() == 0) return 0;

    // Reset buffer-layout

    ReleaseClusters();
    Cluster* HeadOpenObjectsList = 0;			// Clusters waiting to be closed
    Cluster* TailOpenObjectsList = 0;			// Clusters waiting to be closed

    unsigned int Rows = Image.GetHeight();
    unsigned int Columns = Image.GetWidth();
    unsigned int WidthStep = Columns;

    // RowScan

    unsigned int LeftColumn = 0;
    unsigned int RightColumn = 0;
    bool RowScan = false;

    // Determine pixelstatus and cluster all pixels set

    unsigned char* RowData = (unsigned char*)Image.GetData();

    for (unsigned int Row=0;Row < Rows;Row++)
    {
        unsigned char* Data = RowData;

        // Get regions with subsequent pixels set

        for (unsigned int Column=0;Column < Columns; Column++)
        {
            // PixelOI ?

            if (*Data++ == 0)
            {
                // Pixel not of interest -> close rowscan ?

                if (RowScan)
                {
                    // close rowscan

                    ConnectedPixelClusters::Cluster* ClusterOI = 0;

                    // Search for potential candidates

                    ConnectedPixelClusters::Cluster* ClusterOI2 = HeadOpenObjectsList;
                    ConnectedPixelClusters::Cluster* PreviousCluster = 0;
                    while ( ClusterOI2 != 0)
                    {
                        ConnectedPixelClusters::Cluster* NextCluster = ClusterOI2->List;

                        // Close enough ?

                        unsigned int Distance = Row - ClusterOI2->BottomRow;
                        if (Distance > MinimumDistance)
                        {
                            // Cluster going out of scope. For speed-optimalization,
                            // add this cluster to the processed clusters-tree

                            if (PreviousCluster == 0) HeadOpenObjectsList = ClusterOI2->List;
                            else PreviousCluster->List = ClusterOI2->List;

                            if (TailOpenObjectsList == ClusterOI2)
                            {
                                if (PreviousCluster == 0) TailOpenObjectsList = HeadOpenObjectsList;
                                else TailOpenObjectsList = PreviousCluster;
                            }

                             // Add or recycle ?

                            if (ClusterOI2->NumberOfPixels >= MinimumRequiredPixelsInCluster)
                            {
                                if (HeadClusterList == 0) HeadClusterList = ClusterOI2;
                                else TailClusterList->List = ClusterOI2;

                                TailClusterList = ClusterOI2;
                                ClusterOI2->List = 0;
                             } // end cluster with required pixels
                            else
                            {
                                // 'release' cluster (Not of interest)

                                ClusterOI2->List = FreeObjectsList;
                                FreeObjectsList = ClusterOI2;
                            } // end released cluster
                        } // end release cluster (going out of scope)
                        else
                        {
                            // Row not out of scope

                            unsigned int ColumnOI = ClusterOI2->LeftColumn;
                            if (RightColumn <= ColumnOI) Distance += (ColumnOI - RightColumn);
                            else
                            {
                                ColumnOI = ClusterOI2->RightColumn;
                                if (ColumnOI <= LeftColumn) Distance += (LeftColumn - ColumnOI);
                            }

                            if (Distance <= MinimumDistance)
                            {
                                if (ClusterOI == 0)
                                {
                                    // first clusterOI

                                    PreviousCluster = ClusterOI2;
                                    ClusterOI = ClusterOI2;
                                }
                                else
                                {
                                    // Merge clusters ...

                                    ClusterOI->NumberOfPixels += ClusterOI2->NumberOfPixels;
                                    if (ClusterOI->TopRow > ClusterOI2->TopRow) ClusterOI->TopRow = ClusterOI2->TopRow;
                                    if (ClusterOI->LeftColumn > ClusterOI2->LeftColumn) ClusterOI->LeftColumn = ClusterOI2->LeftColumn;
                                    if (ClusterOI->RightColumn < ClusterOI2->RightColumn) ClusterOI->RightColumn = ClusterOI2->RightColumn;

                                    // 'release' cluster

                                    if (PreviousCluster == 0) HeadOpenObjectsList = ClusterOI2->List;
                                    else PreviousCluster->List = ClusterOI2->List;

                                    if (TailOpenObjectsList == ClusterOI2)
                                    {
                                        if (PreviousCluster == 0) TailOpenObjectsList = HeadOpenObjectsList;
                                        else TailOpenObjectsList = PreviousCluster;
                                    }

                                    ClusterOI2->List = FreeObjectsList;
                                    FreeObjectsList = ClusterOI2;
                                }
                            } // end close cluster
                            else PreviousCluster = ClusterOI2;
                            ClusterOI2 = NextCluster;
                        } // end potential cluster

                        ClusterOI2 = NextCluster;       // continue
                    } // end while-loop seraching for close clusters

                    if (ClusterOI == 0)
                    {
                        // Get a new cluster

                        ClusterOI = FreeObjectsList;
                        if (ClusterOI != 0)
                        {
                            FreeObjectsList = ClusterOI->List;

                            ClusterOI->NumberOfPixels = RightColumn-LeftColumn+1;
                            ClusterOI->LeftColumn = LeftColumn;
                            ClusterOI->RightColumn = RightColumn;
                            ClusterOI->TopRow = Row;
                            ClusterOI->BottomRow = Row;

                            if (HeadOpenObjectsList == 0) HeadOpenObjectsList = ClusterOI;
                            else TailOpenObjectsList->List = ClusterOI;

                            TailOpenObjectsList = ClusterOI;
                            ClusterOI->List = 0;
                        } // end new cluster
                    }
                    else
                    {
                        // update close cluster

                        ClusterOI->BottomRow = Row;
                        ClusterOI->NumberOfPixels += RightColumn-LeftColumn+1;
                        if (ClusterOI->RightColumn < (unsigned int)RightColumn) ClusterOI->RightColumn = RightColumn;
                        if (ClusterOI->LeftColumn > (unsigned int)LeftColumn) ClusterOI->LeftColumn = LeftColumn;
                    } // end update cluster

                    RowScan = false;
                }
            } // End pixel == 0
            else
            {
                if (RowScan) RightColumn++;
                else
                {
                    RowScan = true;
                    LeftColumn = Column;
                    RightColumn = Column;
                }
            } // end pixel != 0
        } // end for-loop columns

        RowScan = false;

        // Advance pointers in images

        RowData += WidthStep;
    } // end for-loop rows

    // Open clusters ?

    Cluster* ClusterOI = HeadOpenObjectsList;
    while (ClusterOI != 0)
    {
        HeadOpenObjectsList = ClusterOI->List;

        // Enough pixels ?

        if (ClusterOI->NumberOfPixels >= MinimumRequiredPixelsInCluster)
        {
            if (HeadClusterList == 0) HeadClusterList = ClusterOI;
            else TailClusterList->List = ClusterOI;

            TailClusterList = ClusterOI;
            ClusterOI->List = 0;
        } else
        {
            // 'release' cluster (Not of interest)

            ClusterOI->List = FreeObjectsList;
            FreeObjectsList = ClusterOI;
        }
        ClusterOI = HeadOpenObjectsList;
    }

    // Finally, return the clusters found

    return HeadClusterList;
}

} // namespace vipnt
