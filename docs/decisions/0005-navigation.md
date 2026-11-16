# Decision 0005: navigation scope

Feature #3 includes a location navigation graph and basic pathfinding.

- Locations are connected by explicit edges with positive integer travel
  costs in a documented unit (proposed world-seconds).
- Pathfinding is Dijkstra over the graph. Equal-cost alternatives resolve by
  lowest location ID, and neighbors iterate in ID order, so the same world
  state and query always return the same path.
- Unreachable targets return an explicit result.
- Spatial navigation (continuous space, obstacles, steering) is out of scope;
  it belongs with the future graphical/engine work.
