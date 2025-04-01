#include <gtest/gtest.h>

#include "ZipperIterator.hpp"
#include "IndexedZipper.hpp"
#include "SparseArray.hpp"

// Define the Position component with x and y coordinates
struct Position
{
    float x, y; // x and y represent the 2D position of an entity
};

// Define the Velocity component with dx and dy for movement
struct Velocity
{
    float dx, dy; // dx and dy represent the velocity in x and y directions

    // Constructor to initialize dx and dy
    Velocity(float dx = 0.0f, float dy = 0.0f) : dx(dx), dy(dy) {}
};

// Test case to verify basic iteration using `Zipper`
TEST(ZipperTests, BasicIteration)
{
    // Step 1: Create SparseArrays for Position and Velocity
    ECS::SparseArray<Position> positions;
    ECS::SparseArray<Velocity> velocities;

    // Step 2: Add components to the SparseArrays
    // These simulate entities with both Position and Velocity components
    positions[0] = Position{1.0f, 2.0f}; // Entity 0 has Position (1.0, 2.0)
    positions[1] = Position{3.0f, 4.0f}; // Entity 1 has Position (3.0, 4.0)
    velocities[0] = Velocity{0.5f, 1.0f}; // Entity 0 has Velocity (0.5, 1.0)
    velocities[1] = Velocity{-0.5f, -1.0f}; // Entity 1 has Velocity (-0.5, -1.0)

    // Step 3: Use Zipper to iterate over both SparseArrays simultaneously
    size_t index = 0; // Keep track of the iteration count
    for (auto&& [pos, vel] : Zipper(positions, velocities)) {
        // Step 4: Verify that Position and Velocity values are correct for each entity
        if (index == 0) {
            EXPECT_EQ(pos->x, 1.0f); // Entity 0's x-coordinate
            EXPECT_EQ(pos->y, 2.0f); // Entity 0's y-coordinate
            EXPECT_EQ(vel->dx, 0.5f); // Entity 0's dx (velocity x)
            EXPECT_EQ(vel->dy, 1.0f); // Entity 0's dy (velocity y)
        } else if (index == 1) {
            EXPECT_EQ(pos->x, 3.0f); // Entity 1's x-coordinate
            EXPECT_EQ(pos->y, 4.0f); // Entity 1's y-coordinate
            EXPECT_EQ(vel->dx, -0.5f); // Entity 1's dx (velocity x)
            EXPECT_EQ(vel->dy, -1.0f); // Entity 1's dy (velocity y)
        }
        ++index; // Increment the iteration count
    }

    // Step 5: Ensure the number of iterations matches the number of valid components
    EXPECT_EQ(index, 2); // Only two entities had both components
}

// Test case to verify that `Zipper` skips entities with missing components
TEST(ZipperTests, SkipMissingComponents)
{
    // Step 1: Create SparseArrays for Position and Velocity
    ECS::SparseArray<Position> positions;
    ECS::SparseArray<Velocity> velocities;

    // Step 2: Add components to SparseArrays with some missing components
    positions[0] = Position{1.0f, 2.0f}; // Entity 0 has Position
    positions[2] = Position{5.0f, 6.0f}; // Entity 2 has Position
    velocities[0] = Velocity{0.5f, 1.0f}; // Entity 0 has Velocity
    velocities[1] = Velocity{-0.5f, -1.0f}; // Entity 1 has Velocity

    // Step 3: Use Zipper to iterate over entities with both Position and Velocity
    size_t index = 0; // Keep track of valid iterations
    for (auto&& [pos, vel] : Zipper(positions, velocities)) {
        // Step 4: Verify that only Entity 0 is iterated over
        EXPECT_EQ(index, 0); // Only the first entity has both components
        EXPECT_EQ(pos->x, 1.0f); // Entity 0's x-coordinate
        EXPECT_EQ(pos->y, 2.0f); // Entity 0's y-coordinate
        EXPECT_EQ(vel->dx, 0.5f); // Entity 0's dx
        EXPECT_EQ(vel->dy, 1.0f); // Entity 0's dy
        ++index; // Increment the iteration count
    }

    // Step 5: Verify that only one valid iteration occurred
    EXPECT_EQ(index, 1); // Only Entity 0 had both components
}

// Test case to verify basic iteration using `indexedZipper`
TEST(IndexedZipperTests, BasicIteration)
{
    // Step 1: Create SparseArrays for Position and Velocity
    ECS::SparseArray<Position> positions;
    ECS::SparseArray<Velocity> velocities;

    // Step 2: Add components to the SparseArrays
    positions[0] = Position{1.0f, 2.0f}; // Entity 0 has Position
    positions[1] = Position{3.0f, 4.0f}; // Entity 1 has Position
    velocities[0] = Velocity{0.5f, 1.0f}; // Entity 0 has Velocity
    velocities[1] = Velocity{-0.5f, -1.0f}; // Entity 1 has Velocity

    // Step 3: Use indexedZipper to iterate with index
    for (auto&& [i, pos, vel] : indexedZipper(positions, velocities)) {
        // Step 4: Verify that the index and component values are correct
        if (i == 0) {
            EXPECT_EQ(pos->x, 1.0f); // Entity 0's x-coordinate
            EXPECT_EQ(pos->y, 2.0f); // Entity 0's y-coordinate
            EXPECT_EQ(vel->dx, 0.5f); // Entity 0's dx
            EXPECT_EQ(vel->dy, 1.0f); // Entity 0's dy
        } else if (i == 1) {
            EXPECT_EQ(pos->x, 3.0f); // Entity 1's x-coordinate
            EXPECT_EQ(pos->y, 4.0f); // Entity 1's y-coordinate
            EXPECT_EQ(vel->dx, -0.5f); // Entity 1's dx
            EXPECT_EQ(vel->dy, -1.0f); // Entity 1's dy
        }
    }
}

// Test case to verify that `indexedZipper` skips missing components
TEST(IndexedZipperTests, SkipMissingComponents)
{
    // Step 1: Create SparseArrays for Position and Velocity
    ECS::SparseArray<Position> positions;
    ECS::SparseArray<Velocity> velocities;

    // Step 2: Add components to SparseArrays with missing entries
    positions[0] = Position{1.0f, 2.0f}; // Entity 0 has Position
    positions[2] = Position{5.0f, 6.0f}; // Entity 2 has Position
    velocities[0] = Velocity{0.5f, 1.0f}; // Entity 0 has Velocity
    velocities[1] = Velocity{-0.5f, -1.0f}; // Entity 1 has Velocity

    // Step 3: Use indexedZipper and verify valid iterations
    size_t iterationCount = 0; // Track valid iterations
    for (auto&& [i, pos, vel] : indexedZipper(positions, velocities)) {
        // Step 4: Verify that only Entity 0 is iterated over
        EXPECT_EQ(i, 0); // Only the first entity has both components
        EXPECT_EQ(pos->x, 1.0f); // Entity 0's x-coordinate
        EXPECT_EQ(pos->y, 2.0f); // Entity 0's y-coordinate
        EXPECT_EQ(vel->dx, 0.5f); // Entity 0's dx
        EXPECT_EQ(vel->dy, 1.0f); // Entity 0's dy
        ++iterationCount; // Increment the iteration count
    }

    // Step 5: Verify the number of iterations matches valid components
    EXPECT_EQ(iterationCount, 1); // Only Entity 0 had both components
}

// Test case to verify behavior with empty SparseArrays
TEST(ZipperTests, EmptySparseArrays)
{
    // Step 1: Create empty SparseArrays for Position and Velocity
    ECS::SparseArray<Position> positions;
    ECS::SparseArray<Velocity> velocities;

    // Step 2: Iterate using Zipper
    size_t iterationCount = 0; // Track number of iterations
    for (auto&& [pos, vel] : Zipper(positions, velocities)) {
        ++iterationCount; // This block should not execute
    }

    // Step 3: Verify no iterations occurred
    EXPECT_EQ(iterationCount, 0); // Empty arrays mean no iterations
}

// Test case to verify behavior when one SparseArray is empty
TEST(ZipperTests, OneEmptySparseArray)
{
    // Step 1: Create SparseArrays
    ECS::SparseArray<Position> positions;
    ECS::SparseArray<Velocity> velocities;

    // Step 2: Add components to only one SparseArray
    positions[0] = Position{1.0f, 2.0f}; // Only Position array has data

    // Step 3: Iterate using Zipper
    size_t iterationCount = 0; // Track number of iterations
    for (auto&& [pos, vel] : Zipper(positions, velocities)) {
        ++iterationCount; // This block should not execute
    }

    // Step 4: Verify no iterations occurred
    EXPECT_EQ(iterationCount, 0); // No iterations since Velocity array is empty
}