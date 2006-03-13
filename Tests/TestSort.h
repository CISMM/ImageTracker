#pragma once
#include "../TestSuite/Test.h"
#include <vector>

#include "../Logger.h"
#include "../Sort.h"

class Integer
{
private:
    int value;

public:
    Integer() { this->value = 0; }
    Integer(int val) { this->value = val; }

    bool operator<(Integer other)
    {
        return this->value < other.value;
    }
    bool operator<=(Integer other)
    {
        return this->value <= other.value;
    }
    bool operator>(Integer other)
    {
        return this->value > other.value;
    }
    bool operator>=(Integer other)
    {
        return this->value >= other.value;
    }
    bool operator==(Integer other)
    {
        return this->value == other.value;
    }

    int getValue() { return this->value; }
    void setValue(int val) { this->value = val; }
};

class TestSort :
    public TestSuite::Test
{
public:
    TestSort(void){}
    ~TestSort(void){}
    
    void run()
    {
        Logger::logInfo("Running TestSort...");
        this->testSortComparable();
        this->testSortInt();
        this->testSortDesc();
    }

    void testSortComparable()
    {
        int n = 10;

        typedef Sort<Integer> SortType;
        SortType::VectorType* intVec = new SortType::VectorType();
        for (int i = n-1; i >= 0; i--)
        {
            Integer anInt(i);
            intVec->push_back(anInt);
        }

        SortType::mergeSort(*intVec);

        int j = 0;
        for (SortType::VectorType::iterator it = intVec->begin();
            it != intVec->end(); it++)
        {
            test_((*it).getValue() == j);
			j++;
        }

        delete intVec;
    }

    void testSortInt()
    {
        int n = 10;

        typedef Sort<int> SortType;
        SortType::VectorType* intVec = new SortType::VectorType();
        for (int i = n; i >= 0; i--)
        {
            intVec->push_back(i);
        }

        SortType::mergeSort(*intVec);

        int j = 0;
        for (SortType::VectorType::iterator it = intVec->begin();
            it != intVec->end();
            it++)
        {
            test_(*it == j);
			j++;
        }

        delete intVec;
    }

    void testSortDesc()
    {
        int n = 10;

        typedef Sort<float> SortType;
        SortType::VectorType* floatVec = new SortType::VectorType();
        for (int i = 0; i < n; i++)
        {
            floatVec->push_back(i + 0.5);
        }

        SortType::mergeSort(*floatVec, false);
        
        float f = n - 0.5;
        for (SortType::VectorType::iterator it = floatVec->begin();
            it != floatVec->end(); it++, f--)
        {
            test_(*it == f);
        }

        delete floatVec;
    }
};

