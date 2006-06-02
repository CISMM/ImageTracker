#pragma once
#include <vector>

/**
 * Sort implements sorting for a vector of Comparable
 * objects.  Code borrowed heavily from:
 * http://www.cs.fiu.edu/~weiss/dsaa_c++/code/Sort.h.
 * The sort procedure is called as:
 * Sort<TComparable>::mergeSort(std::vector<TComparable>& toSort), 
 * where the elements of the toSort vector are comparable--that is, 
 * they overload the binary comparison operators <, >, <=, >=, ==.
 */
template <class TComparable>
class Sort
{
public:
    typedef std::vector<TComparable> VectorType;

    /**
     * Sorts the elements of the given vector using
     * merge sort.  The vector is sorted in place--that is
     * the elements of the given vector are rearranged.
     */
    static void mergeSort(VectorType &toSort, bool ascending = true) 
    {
        std::vector<TComparable> tempSort(toSort.size());
        Sort::mergeSort(toSort, tempSort, 0, toSort.size()-1, ascending);
    }

private:
    /**
     * Recursive sort procedure--recurses until left and right
     * lists are small enough (one elememt), then starts merging
     * lists.
     */
    static void mergeSort(VectorType &orig, VectorType &tempSort, 
        int left, int right, bool ascending)
    {
        if (left < right)
        {
            int center = (left + right) / 2;
            Sort::mergeSort(orig, tempSort, left, center, ascending);
            Sort::mergeSort(orig, tempSort, center+1, right, ascending);
            merge(orig, tempSort, left, center+1, right, ascending);
        }
    }

    /**
     * Merges two sorted lists.
     */
    static void merge(VectorType &orig, VectorType &tempSort, 
        int leftInd, int rightInd, int rightEnd, bool ascending)
    {
        int leftEnd = rightInd - 1;
        int tempInd = leftInd;
        int numElements = rightEnd - leftInd + 1;

        while (leftInd <= leftEnd && rightInd <= rightEnd)
        {
            if ((orig[leftInd] <= orig[rightInd] && ascending) ||
                (orig[leftInd] >= orig[rightInd] && !ascending))
            {            
                tempSort[tempInd++] = orig[leftInd++];
            }
            else
            {
                tempSort[tempInd++] = orig[rightInd++];
            }
        }
        
        while (leftInd <= leftEnd)
        {
            tempSort[tempInd++] = orig[leftInd++];
        }
        while (rightInd <= rightEnd)
        {
            tempSort[tempInd++] = orig[rightInd++];
        }

        for (int i = 0; i < numElements; i++, rightEnd--)
        {
            orig[rightEnd] = tempSort[rightEnd];
        }
    }
};
