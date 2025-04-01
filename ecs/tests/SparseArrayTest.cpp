#include <gtest/gtest.h>
#include "SparseArray.hpp"

// Test to verify that a newly initialized SparseArray is empty
TEST(SparseArrayTest, InitializesEmpty)
{
    ECS::SparseArray<int> array; // Create a SparseArray of integers
    EXPECT_EQ(array.size(), 0); // The size of the array should be 0 initially
}

// Test to ensure that inserting an element at a specific position increases the size of the SparseArray
TEST(SparseArrayTest, InsertAtIncreasesSize)
{
    ECS::SparseArray<int> array; // Create a SparseArray of integers

    array.insertAt(5, 42); // Insert the value 42 at position 5
    EXPECT_EQ(array.size(), 6); // The size should be updated to accommodate index 5 (0-based index)
    EXPECT_EQ(array[5], 42); // Verify that the value at position 5 is 42
}

// Test to ensure emplaceAt constructs elements in place and correctly stores them
TEST(SparseArrayTest, EmplaceAtConstructsInPlace)
{
    ECS::SparseArray<std::string> array; // Create a SparseArray of strings

    array.emplaceAt(3, "Easy peasy Louis Poulouloubi"); // Emplace a string at position 3
    EXPECT_EQ(array[3], "Easy peasy Louis Poulouloubi"); // Verify the string is correctly stored at index 3
}

// Test to ensure that erasing an element from the SparseArray removes its value
TEST(SparseArrayTest, EraseRemovesElement)
{
    ECS::SparseArray<int> array; // Create a SparseArray of integers

    array.insertAt(1, 100); // Insert the value 100 at position 1
    array.erase(1); // Erase the element at position 1

    EXPECT_FALSE(array[1].has_value()); // Verify that the element at position 1 no longer has a value
}

// Test to check that iterators can be used to traverse elements in the SparseArray
TEST(SparseArrayTest, IteratorAccess)
{
    ECS::SparseArray<int> array; // Create a SparseArray of integers

    array.insertAt(0, 10); // Insert the value 10 at position 0
    array.insertAt(1, 20); // Insert the value 20 at position 1

    auto it = array.begin(); // Get an iterator to the beginning of the array
    EXPECT_EQ(*it, 10); // The first element should be 10
    ++it; // Move the iterator to the next element
    EXPECT_EQ(*it, 20); // The second element should be 20
}

// Test to ensure that getIndex correctly returns the index of a specific element
TEST(SparseArrayTest, GetIndexReturnsCorrectVal) {
    ECS::SparseArray<int> array; // Create a SparseArray of integers

    array.insertAt(4, 99); // Insert the value 99 at position 4
    EXPECT_EQ(array.getIndex(array[4]), 4); // Verify that getIndex correctly returns index 4 for the value
}
