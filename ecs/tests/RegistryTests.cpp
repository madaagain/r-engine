#include <gtest/gtest.h>

#include "Entity.hpp"
#include "Registry.hpp"
#include "ZipperIterator.hpp"

struct Position
{
    float x, y;
};

struct Velocity
{
    float dx, dy;
    Velocity(float dx, float dy) : dx(dx), dy(dy) {}
};

// Test case to verify registering components in the registry
TEST(RegistryTests, RegisterComponent)
{
    ECS::Registry registry;

    // Test: Register the Position component type
    EXPECT_NO_THROW(registry.registerComponent<Position>());

    // Test: Register the Velocity component type
    EXPECT_NO_THROW(registry.registerComponent<Velocity>());

    // Test: Attempt to register the Position component type again
    // Expected: This should throw a runtime_error because the component type is already registered
    EXPECT_THROW(registry.registerComponent<Position>(), std::runtime_error);
}

// Test case to verify adding and retrieving components from the registry
TEST(RegistryTests, AddAndRetrieveComponents)
{
    ECS::Registry registry;

    // Register the Position component type
    registry.registerComponent<Position>();

    // Spawn a new entity
    ECS::Entity entity = registry.spawnEntity();

    // Add a Position component to the entity
    EXPECT_NO_THROW(registry.addComponent<Position>(entity, {1.0f, 2.0f}));


    // Retrieve the Position components from the registry
    auto& positions = registry.getComponents<Position>();

    // Test: Verify that the Position component was correctly added
    EXPECT_EQ(positions[entity]->x, 1.0f); // x should be 1.0
    EXPECT_EQ(positions[entity]->y, 2.0f); // y should be 2.0
}

// Test case to verify emplacing components directly in the registry
TEST(RegistryTests, EmplaceComponents)
{
    ECS::Registry registry;

    // Register the Velocity component type
    registry.registerComponent<Velocity>();

    // Spawn a new entity
    ECS::Entity entity = registry.spawnEntity();

    // Emplace a Velocity component directly into the registry
    EXPECT_NO_THROW(registry.emplaceComponent<Velocity>(entity, 1.5f, 2.5f));

    // Retrieve the Velocity components from the registry
    auto& velocities = registry.getComponents<Velocity>();

    // Test: Verify that the Velocity component was correctly emplaced
    EXPECT_EQ(velocities[entity]->dx, 1.5f); // dx should be 1.5
    EXPECT_EQ(velocities[entity]->dy, 2.5f); // dy should be 2.5
}

// Test case to verify removing components from the registry
TEST(RegistryTests, RemoveComponents)
{
    ECS::Registry registry;

    // Register the Position component type
    registry.registerComponent<Position>();

    // Spawn a new entity
    ECS::Entity entity = registry.spawnEntity();

    // Add a Position component to  {3.0f, 4.0f}ity, {3.0f, 4.0f});

    // Remove the Position component from the entity
    registry.removeComponent<Position>(entity);

    // Retrieve the Position components from the registry
    auto& positions = registry.getComponents<Position>();

    // Test: Verify that the Position component was removed
    EXPECT_FALSE(positions[entity].has_value()); // Component should no longer exist
}

// Test case to verify killing an entity removes all its components
TEST(RegistryTests, KillEntity)
{
    ECS::Registry registry;

    // Register the Position and Velocity component types
    registry.registerComponent<Position>();
    registry.registerComponent<Velocity>();

    // Spawn a new entity
    ECS::Entity entity = registry.spawnEntity();

    // Add Position and Velocity components to the entity
    registry.addComponent<Position>(entity, {5.0f, 6.0f});
    registry.emplaceComponent<Velocity>(entity, 1.0f, 2.0f);

    // Kill the entity, removing all associated components
    registry.killEntity(entity);

    // Retrieve the Position and Velocity components from the registry
    auto& positions = registry.getComponents<Position>();
    auto& velocities = registry.getComponents<Velocity>();

    // Test: Verify that both components were removed
    EXPECT_FALSE(positions[entity].has_value()); // Position should be removed
    EXPECT_FALSE(velocities[entity].has_value()); // Velocity should be removed
}

// Test case to verify that entity IDs are recycled after killing an entity
TEST(RegistryTests, RecycleEntityIDs)
{
    ECS::Registry registry;

    // Spawn two entities
    ECS::Entity entity1 = registry.spawnEntity();
    ECS::Entity entity2 = registry.spawnEntity();

    // Kill the first entity
    registry.killEntity(entity1);

    // Spawn a new entity, which should reuse the ID of the killed entity
    ECS::Entity newEntity = registry.spawnEntity();

    // Test: Verify that the new entity reuses the ID of the killed entity
    EXPECT_EQ(static_cast<size_t>(newEntity), static_cast<size_t>(entity1));
}

// Test case to verify behavior when accessing missing components
TEST(RegistryTests, MissingComponents)
{
    ECS::Registry registry;

    // Register the Position component type
    registry.registerComponent<Position>();

    // Spawn a new entity
    ECS::Entity entity = registry.spawnEntity();

    // Retrieve the Position components from the registry
    auto& positions = registry.getComponents<Position>();

    // Test: Verify that the entity does not have a Position component
    EXPECT_FALSE(positions[entity].has_value()); // No Position component should exist
}

TEST(RegistryTests, AddAndRunSystems)
{
    // Step 1: Create a registry to manage entities and components
    ECS::Registry registry;

    // Step 2: Register the `Position` and `Velocity` component types
    // Registering a component type allows the registry to track entities with these components
    registry.registerComponent<Position>();
    registry.registerComponent<Velocity>();

    // Step 3: Spawn two entities in the registry
    // Entities are unique identifiers representing objects in the ECS
    ECS::Entity entity1 = registry.spawnEntity();
    ECS::Entity entity2 = registry.spawnEntity();

    // Step 4: Add components to the entities
    // Components store the data (e.g., position and velocity) associated with entities
    // Adding a `Position` component to entity1 with an initial position of (0.0, 0.0)
    registry.addComponent<Position>(entity1, {0.0f, 0.0f});
    // Adding a `Velocity` component to entity1 with an initial velocity of (1.0, 1.0)
    registry.addComponent<Velocity>(entity1, {1.0f, 1.0f});
    // Adding a `Position` component to entity2 with an initial position of (5.0, 5.0)
    registry.addComponent<Position>(entity2, {5.0f, 5.0f});
    // Adding a `Velocity` component to entity2 with an initial velocity of (0.5, 0.5)
    registry.addComponent<Velocity>(entity2, {0.5f, 0.5f});

    // Step 5: Define a system that updates positions based on velocities
    // Systems define the behavior of entities with specific components
    auto positionSystem = [](ECS::Registry &registry,
                              ECS::SparseArray<Position> &positions,
                              ECS::SparseArray<Velocity> &velocities) {
        // Use the `Zipper` to iterate over entities with both Position and Velocity components
        for (auto &&[pos, vel] : Zipper(positions, velocities)) {
            // Check if both components exist for the current entity
            if (pos.has_value() && vel.has_value()) {
                // Update the position based on the velocity
                // Dereferencing `std::optional` using `->` to access the `x` and `y` members
                pos->x += vel->dx;
                pos->y += vel->dy;
            }
        }
    };

    // Step 6: Add the position system to the registry
    // This ensures the system will run when `runSystems()` is called
    registry.addSystem<Position, Velocity>(positionSystem);

    // Step 7: Execute all registered systems
    // This applies the behavior defined in `positionSystem` to the entities
    EXPECT_NO_THROW(registry.runSystems()); // Ensure no exception is thrown

    // Step 8: Verify that the positions of the entities were updated correctly
    auto &positions = registry.getComponents<Position>();

    // After one system run:
    // entity1's position should be updated to (1.0, 1.0)
    EXPECT_FLOAT_EQ(positions[entity1]->x, 1.0f);
    EXPECT_FLOAT_EQ(positions[entity1]->y, 1.0f);
    // entity2's position should be updated to (5.5, 5.5)
    EXPECT_FLOAT_EQ(positions[entity2]->x, 5.5f);
    EXPECT_FLOAT_EQ(positions[entity2]->y, 5.5f);

    // Step 9: Run the systems again
    // This applies the velocity again, further updating the positions
    registry.runSystems();

    // Step 10: Verify the positions after the second run
    // entity1's position should now be (2.0, 2.0)
    EXPECT_FLOAT_EQ(positions[entity1]->x, 2.0f);
    EXPECT_FLOAT_EQ(positions[entity1]->y, 2.0f);
    // entity2's position should now be (6.0, 6.0)
    EXPECT_FLOAT_EQ(positions[entity2]->x, 6.0f);
    EXPECT_FLOAT_EQ(positions[entity2]->y, 6.0f);
}

TEST(RegistryTests, SystemsWithNoMatchingEntities)
{
    // Step 1: Create a registry to manage entities and components
    ECS::Registry registry;

    // Step 2: Register the `Position` and `Velocity` component types
    registry.registerComponent<Position>();
    registry.registerComponent<Velocity>();

    // Step 3: Define a system that updates positions based on velocities
    // The system will be tested without any entities having both components
    auto positionSystem = [](ECS::Registry &registry,
                              ECS::SparseArray<Position> &positions,
                              ECS::SparseArray<Velocity> &velocities) {
        // Use the `Zipper` to iterate over entities with both Position and Velocity components
        for (auto &&[pos, vel] : Zipper(positions, velocities)) {
            // This block should not execute as no entity has both components
            if (pos.has_value() && vel.has_value()) {
                pos->x += vel->dx;
                pos->y += vel->dy;
            }
        }
    };

    // Step 4: Add the position system to the registry
    registry.addSystem<Position, Velocity>(positionSystem);

    // Step 5: Spawn a single entity with only the `Position` component
    ECS::Entity entity = registry.spawnEntity();
    registry.addComponent<Position>(entity, {10.0f, 20.0f});

    // Step 6: Run systems
    // The system should run without errors, but no position updates should occur
    EXPECT_NO_THROW(registry.runSystems());

    // Step 7: Verify that the position remains unchanged
    auto &positions = registry.getComponents<Position>();
    EXPECT_FLOAT_EQ(positions[entity]->x, 10.0f);
    EXPECT_FLOAT_EQ(positions[entity]->y, 20.0f);

    // Step 8: Verify that no unexpected errors occur when the system has no matching entities
    // Add another system that prints debug messages if executed
    auto debugSystem = [](ECS::Registry &registry,
                          ECS::SparseArray<Position> &positions,
                          ECS::SparseArray<Velocity> &velocities) {
        bool executed = false;
        for (auto &&[pos, vel] : Zipper(positions, velocities)) {
            executed = true;
        }
        EXPECT_FALSE(executed); // Ensure the system did not process any entities
    };

    // Add and run the debug system
    registry.addSystem<Position, Velocity>(debugSystem);
    EXPECT_NO_THROW(registry.runSystems());
}
