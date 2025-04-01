

#include "Server.hpp"

std::atomic<bool> run(true);

void signalHandler(int signum)
{
    if (signum == SIGINT) {
        run = false;
    }
}

int main()
{
    std::signal(SIGINT, signalHandler);

    ECS::Registry registry;
    // Enregistrement des composants nécessaires
    // registry.registerComponent<Transformable>();
    // registry.registerComponent<Velocity>();
    // registry.registerComponent<Controllable>();

    // // Création d'une entité test AVANT la boucle principale
    // auto testEntity = registry.spawnEntity();
    // registry.addComponent<Transformable>(testEntity, Transformable{200.0f, 300.0f});
    // registry.addComponent<Velocity>(testEntity, Velocity{0.0f, 0.0f});

    Server server(8080, registry);

    if (!server.start()) {
        std::cerr << "Failed to start the server.\n";
        return 84;
    }

    std::cout << "Server is running. Press Ctrl+C to stop.\n";

    while (run) {
        server.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    server.stop();
    return 0;
}
