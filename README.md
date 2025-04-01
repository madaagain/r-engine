# R-TYPE

This project in the **Advanced C++** unit is about diving into networked video game development.

The goal is to build a multi-threaded server and a graphical client for the classic game R-Type, all powered by a game engine create from scratch.

During the first part of the project the goals are to design the game’s core architecture and delivering a functional prototype. In the second part, the prototype is going to be enhance, by exploring one of the three tracks available: :
1. **Advanced Architecture**  - Building a real game engine
2. **Advanced Networking** - Building a reliable network game
3. **Advanced Gameplay** - Building a Fun and complete video
game

---

## Project Overview

R-TYPE is the occasion to discover **game engine development**. Inspired by the R-Type franchise, the project involves to design and build:

- A **multi-threaded server** to manage game logic and networking.
- A **graphical client** for rendering and player interaction.
- An **extensible game engine** showcasing clean architecture, modularity, and advanced features.

---

## Features

- **Networked Multiplayer**: The game must be multiplayer.
- **Custom Game Engine**: Architected to decouple rendering, logic, and networking for reusability.
- **Cross-Platform Compatibility**: Runs on Linux and Windows.
- **Modular Design**: Choose and extend features with ease.

---

## Preliminaries

| **Component**       | **Details**                              |
|----------------------|------------------------------------------|
| **Languages**        | C++                                     |
| **Build System**     | CMake                                   |
| **Dependencies**     | Package managers (CPM in our case)    |
| **Platform**         | Linux, Windows                         |
| **Networking**       | UDP for real-time gameplay              |

---

## Quick Start

### Prerequisites

1. Install a C++ compiler (GCC is recommended). Follow this link for [Ubuntu](https://linuxconfig.org/how-to-install-g-the-c-compiler-on-ubuntu-20-04-lts-focal-fossa-linux).
2. Set up the package manager: [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake).
3. Clone the repository.

```bash
git clone https://github.com/your-repo/r-type.git
cd r-type
```

### Build and Run

**Linux:**
```bash
mkdir build && cd build
cmake ..
make
cd ..
./r-type_server
./r-type_client
```

**Windows:**

Since the process in really different for the window build & run, please check the detail documentation [here](https://own-7.gitbook.io/r-type/setup-cross-compilation/building-for-windows-and-linux#windows).

<br>

---

## Documentation

R-TYPE includes comprehensive documentation:
- **Developer Guide:** Learn about the architecture, subsystems, and best practices.
- **Protocol Specifications:** Detailed network communication protocols.
- **Advanced Topics:** Tutorials for extending and optimizing the engine.

<br>

Explore our documentation **[here](https://own-7.gitbook.io/r-type)**.
<br>

---

## Contributor

Yanis AMEDJKANE - [GitHub](https://github.com/Yanis-Am) <br>
Adam ZERROUK - [GitHub](https://github.com/madaagain) <br>
<br>

---
