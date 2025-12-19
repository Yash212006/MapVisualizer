# 🗺️ Map Navigator – Qt Project

A **graph-based map visualizer** built using **Qt**, designed to simulate navigation between locations in a city.  
The application visually represents places as nodes and roads as edges, and allows users to **set start and end points** and **find routes** between them.

This project focuses on **graph data structures**, **pathfinding concepts**, and **interactive GUI development** using Qt.

---

## 📌 Features

- 🏙️ **City Map Visualization**
  - Locations such as *Central Park, City Hall, Airport, Hospital, University,* etc. are shown as nodes.
  - Roads are represented as edges connecting locations.

- 🔍 **Search Functionality**
  - Search for a place using the search bar at the top.

- 📍 **Set Start and End Locations**
  - Choose any location as the starting point.
  - Choose a destination location.

- 🚗 **Route Finding**
  - Computes and highlights the optimal route between selected locations.
  - The selected path is displayed using highlighted edges.

- 🔎 **Zoom Controls**
  - Zoom in, zoom out, and reset view.
  - Displays current zoom level and total number of locations.

- 🎨 **Interactive UI**
  - Clean and intuitive user interface.
  - Clearly labeled locations and visually distinct routes.

---

## 🧠 Core Concept

The map is modeled as a **graph**:

- **Vertices (Nodes):** City locations
- **Edges:** Roads between locations
- **Weights (optional):** Distance or cost between locations

The route-finding logic can be implemented using:
- Dijkstra’s Algorithm
- BFS / DFS
- A* Algorithm (extendable)

---

## 🖥️ Tech Stack

- **Programming Language:** C++
- **Framework:** Qt (Qt Widgets / Graphics View)
- **Concepts Used:**
  - Graphs & Pathfinding
  - Event-driven programming
  - GUI rendering
  - Object-Oriented Design

---



