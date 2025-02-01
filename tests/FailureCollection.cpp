#include "../src/data_structures/FailureCollection.hpp"
#include <iostream>

// A helper function to print the contents of FailureCollection
void printFailureCollection(const FailureCollection& fc)
{
    std::cout << "FailureCollection contents (nextUnitinializedPair=" 
              << fc.nextUnitinializedPair << "):\n";
    for (size_t i = 0; i < fc.nextUnitinializedPair; ++i)
    {
        FailureDimensions dim = fc.signedFailureRecord[i].first;
        bool sign = fc.signedFailureRecord[i].second;
        std::string dimStr;

        switch (dim)
        {
        case FailureDimensions::CorruptedGenesis:
            dimStr = "CorruptedGenesis";
            break;
        case FailureDimensions::CorruptedLink:
            dimStr = "CorruptedLink";
            break;
        default:
            dimStr = "Unknown";
            break;
        }

        std::cout << "  [" << i << "] " << dimStr 
                  << " => " << (sign ? "true" : "false") << "\n";
    }
}

int main()
{
    FailureCollection fc;

    // Test 1: Initially, nothing is added
    std::cout << "Test 1: No elements added yet\n";
    printFailureCollection(fc);
    std::cout << "Expect: nextUnitinializedPair=0\n\n";

    // Test 2: Add one pair
    std::cout << "Test 2: Add {CorruptedGenesis, true}\n";
    fc.recordFailure(FailureDimensions::CorruptedGenesis);
    printFailureCollection(fc);
    std::cout << "Expect: nextUnitinializedPair=1, element[0]={CorruptedGenesis, true}\n\n";

    // Test 3: Add another pair
    std::cout << "Test 3: Add {CorruptedLink, false}\n";
    fc.recordFailure(FailureDimensions::CorruptedLink);
    printFailureCollection(fc);
    std::cout << "Expect: nextUnitinializedPair=2, element[1]={CorruptedLink, false}\n\n";

    // Test 4: Attempt to add a third pair (array size is 2 => FailureDimensions::COUNT=2)
    // This should be silently ignored since the array is already full.
    std::cout << "Test 4: Try to add a third pair (should be ignored)\n";
    fc.recordFailure(FailureDimensions::CorruptedGenesis);
    printFailureCollection(fc);
    std::cout << "Expect: nextUnitinializedPair still=2, no extra element\n\n";

    return 0;
}


