// Campus Navigation & Landmark Explorer
// C++17 DSA-II Mini Project
//
// Core DSA-II concepts : Graph (adjacency list), BFS, Dijkstra, Min-Heap (own implementation)
// Supporting structures: Queue (BFS), Stack (path reconstruction), BFS spanning tree

#include <iostream>
#include <vector>
#include <string>
#include <queue>      // std::queue, used only by BFS
#include <stack>      // std::stack, used only for path reconstruction
#include <algorithm>
#include <iomanip>
#include <climits>
#include <cctype>

using namespace std;

// =====================================================================
// SECTION 1: GRAPH DATA (adjacency list)
// =====================================================================

struct Edge {
    int to;        // destination landmark index (0-based)
    int distance;  // walking distance in meters
};

vector<string> landmarkNames;
vector<vector<Edge>> campusGraph;

void initializeCampus() {
    landmarkNames = {
        "Main Gate", "Admin Block", "Library", "Computer Science Block",
        "Engineering Block", "Canteen", "Auditorium", "Student Activity Center",
        "Sports Ground", "Hostel", "Medical Center", "Parking Area"
    };

    campusGraph.assign(landmarkNames.size(), vector<Edge>());

    // Undirected edge: store it in both landmarks' lists
    auto addPath = [](int u, int v, int distance) {
        campusGraph[u].push_back({v, distance});
        campusGraph[v].push_back({u, distance});
    };

    addPath(0, 1, 120);
    addPath(0, 11, 80);
    addPath(0, 6, 350);
    addPath(1, 2, 90);
    addPath(1, 6, 180);
    addPath(1, 3, 220);
    addPath(1, 11, 150);
    addPath(2, 3, 110);
    addPath(2, 5, 140);
    addPath(3, 4, 100);
    addPath(3, 5, 160);
    addPath(4, 8, 200);
    addPath(4, 7, 130);
    addPath(5, 7, 90);
    addPath(5, 9, 170);
    addPath(6, 10, 150);
    addPath(6, 7, 210);
    addPath(7, 8, 140);
    addPath(7, 9, 120);
    addPath(8, 9, 190);
    addPath(9, 10, 160);
    addPath(10, 11, 240);
}

// =====================================================================
// SECTION 2: MIN-HEAP (own implementation, array-based)
// =====================================================================
// Rule: every parent is <= its children, so the smallest item is at index 0.
// For the item at index i:  parent = (i-1)/2,  left child = 2i+1,  right child = 2i+2
// Insert and remove both cost O(log n) because the tree height is about log n.

struct HeapItem {
    int dist;  // priority: distance from source (smaller = higher priority)
    int node;  // landmark index
};

class MinHeap {
private:
    vector<HeapItem> data;

    // After inserting at the bottom: swap upward while smaller than parent
    void heapifyUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (data[i].dist < data[parent].dist) {
                swap(data[i], data[parent]);
                i = parent;
            } else {
                break;
            }
        }
    }

    // After moving the last item to the top: swap downward with the smaller child
    void heapifyDown(int i) {
        int n = static_cast<int>(data.size());
        while (true) {
            int smallest = i;
            int left = 2 * i + 1;
            int right = 2 * i + 2;

            if (left < n && data[left].dist < data[smallest].dist) smallest = left;
            if (right < n && data[right].dist < data[smallest].dist) smallest = right;

            if (smallest == i) break;  // heap order restored
            swap(data[i], data[smallest]);
            i = smallest;
        }
    }

public:
    bool empty() const { return data.empty(); }

    void push(int dist, int node) {
        data.push_back({dist, node});                 // 1. add at the bottom
        heapifyUp(static_cast<int>(data.size()) - 1); // 2. bubble it up
    }

    HeapItem pop() {
        HeapItem smallest = data[0];  // 1. the minimum is always at the top
        data[0] = data.back();        // 2. move the last item to the top
        data.pop_back();
        if (!data.empty()) heapifyDown(0);  // 3. push it down to its place
        return smallest;
    }
};

// =====================================================================
// SECTION 3: INPUT AND HELPER FUNCTIONS
// =====================================================================

string toLowerCase(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(),
              [](unsigned char c) { return tolower(c); });
    return result;
}

void clearInputBuffer() {
    cin.clear();
    string dummy;
    getline(cin, dummy);
}

// Returns 0-based landmark index, or -1 if input is invalid
int getValidatedLandmark(const string& prompt) {
    int id = 0;
    cout << prompt;
    if (!(cin >> id)) {
        clearInputBuffer();
        cout << "[Error] Please enter a valid numerical ID.\n";
        return -1;
    }
    clearInputBuffer();

    if (id < 1 || id > static_cast<int>(landmarkNames.size())) {
        cout << "[Error] Landmark ID must be between 1 and " << landmarkNames.size() << ".\n";
        return -1;
    }
    return id - 1;
}

// "1 walkway" / "3 walkways"
string walkwaysText(size_t count) {
    return to_string(count) + (count == 1 ? " walkway" : " walkways");
}

int getEdgeDistance(int u, int v) {
    for (const auto& edge : campusGraph[u]) {
        if (edge.to == v) return edge.distance;
    }
    return 0;
}

int calculatePathDistance(const vector<int>& path) {
    int total = 0;
    for (size_t i = 0; i + 1 < path.size(); ++i) {
        total += getEdgeDistance(path[i], path[i + 1]);
    }
    return total;
}

void printInlinePath(const vector<int>& path) {
    for (size_t i = 0; i < path.size(); ++i) {
        cout << landmarkNames[path[i]];
        if (i + 1 < path.size()) cout << " -> ";
    }
    cout << "\n";
}

// =====================================================================
// SECTION 4: ALGORITHMS
// =====================================================================

// Dijkstra: shortest walking distance from source to every landmark.
// Uses our own MinHeap to pick the nearest unfinished landmark each time.
void dijkstra(int source, vector<int>& dist, vector<int>& parent) {
    int n = static_cast<int>(landmarkNames.size());
    dist.assign(n, INT_MAX);
    parent.assign(n, -1);

    MinHeap heap;
    dist[source] = 0;
    heap.push(0, source);

    while (!heap.empty()) {
        HeapItem current = heap.pop();   // nearest landmark not yet finalised
        int u = current.node;

        if (current.dist > dist[u]) continue;  // outdated entry, skip

        for (const auto& edge : campusGraph[u]) {
            int v = edge.to;
            int newDist = dist[u] + edge.distance;

            if (newDist < dist[v]) {     // found a shorter way to v
                dist[v] = newDist;
                parent[v] = u;
                heap.push(newDist, v);
            }
        }
    }
}

// BFS from start. Fills visit order, and parent[] (used for paths and the tree).
void bfs(int start, vector<int>& visitOrder, vector<int>& parent) {
    int n = static_cast<int>(landmarkNames.size());
    vector<bool> visited(n, false);
    parent.assign(n, -1);
    visitOrder.clear();

    queue<int> q;
    visited[start] = true;
    q.push(start);

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        visitOrder.push_back(u);

        for (const auto& edge : campusGraph[u]) {
            if (!visited[edge.to]) {
                visited[edge.to] = true;
                parent[edge.to] = u;
                q.push(edge.to);
            }
        }
    }
}

// Walk parent[] backwards from destination, push onto a stack,
// then pop to get the path in forward order (source -> destination).
vector<int> reconstructPathUsingStack(int source, int destination, const vector<int>& parent) {
    vector<int> path;
    if (source == destination) {
        path.push_back(source);
        return path;
    }
    if (parent[destination] == -1) return path;  // unreachable -> empty path

    stack<int> routeStack;
    int curr = destination;
    while (curr != -1) {
        routeStack.push(curr);
        if (curr == source) break;
        curr = parent[curr];
    }

    while (!routeStack.empty()) {
        path.push_back(routeStack.top());
        routeStack.pop();
    }
    return path;
}

// =====================================================================
// SECTION 5: MENU FEATURES
// =====================================================================

void displayCampusMap() {
    cout << "\n==================================================\n";
    cout << "                  CAMPUS MAP                      \n";
    cout << "==================================================\n";
    cout << "LANDMARKS:\n";
    for (size_t i = 0; i < landmarkNames.size(); ++i) {
        cout << "  [" << setw(2) << (i + 1) << "] " << landmarkNames[i] << "\n";
    }

    cout << "\nCONNECTED PATHWAYS:\n";
    cout << left << setw(30) << "From Landmark"
         << setw(30) << "To Landmark" << "Distance\n";
    cout << string(70, '-') << "\n";

    for (size_t u = 0; u < campusGraph.size(); ++u) {
        for (const auto& edge : campusGraph[u]) {
            if (static_cast<size_t>(edge.to) > u) {   // print each edge once
                cout << left << setw(30) << landmarkNames[u]
                     << setw(30) << landmarkNames[edge.to]
                     << edge.distance << " m\n";
            }
        }
    }
    cout << "==================================================\n\n";
}

void exploreCampusBFS(int startNode) {
    int n = static_cast<int>(landmarkNames.size());
    vector<int> visitOrder, parent;
    bfs(startNode, visitOrder, parent);

    cout << "\n==================================================\n";
    cout << "          BREADTH FIRST SEARCH (BFS) EXPLORATION  \n";
    cout << "==================================================\n";
    cout << "Starting Landmark: [" << (startNode + 1) << "] " << landmarkNames[startNode] << "\n\n";
    cout << "Traversal Order:\n";

    for (size_t i = 0; i < visitOrder.size(); ++i) {
        cout << "  Step " << setw(2) << (i + 1) << ": ["
             << setw(2) << (visitOrder[i] + 1) << "] "
             << landmarkNames[visitOrder[i]] << "\n";
    }

    cout << "\nTotal Reachable Landmarks: " << visitOrder.size() << " / " << n << "\n";
    if (static_cast<int>(visitOrder.size()) == n) {
        cout << "Campus Connectivity Status: Fully Connected Graph.\n";
    } else {
        cout << "Campus Connectivity Status: Disconnected Components Present.\n";
    }
    cout << "==================================================\n\n";
}

void findShortestPath() {
    cout << "\n--- Find Shortest Route ---\n";
    int source = getValidatedLandmark("Enter Source Landmark ID (1-12): ");
    if (source == -1) return;

    int destination = getValidatedLandmark("Enter Destination Landmark ID (1-12): ");
    if (destination == -1) return;

    if (source == destination) {
        cout << "\n[Info] Source and Destination are the same: " << landmarkNames[source] << ".\n";
        cout << "Total Walking Distance: 0 meters.\n\n";
        return;
    }

    vector<int> dist, parent;
    dijkstra(source, dist, parent);

    if (dist[destination] == INT_MAX) {
        cout << "\n[Error] No path exists between " << landmarkNames[source]
             << " and " << landmarkNames[destination] << ".\n\n";
        return;
    }

    vector<int> path = reconstructPathUsingStack(source, destination, parent);

    cout << "\n==================================================\n";
    cout << "            SHORTEST WALKING ROUTE                \n";
    cout << "==================================================\n";
    cout << "From: [" << (source + 1) << "] " << landmarkNames[source] << "\n";
    cout << "To:   [" << (destination + 1) << "] " << landmarkNames[destination] << "\n\n";
    cout << "ROUTE DETAILS:\n";

    for (size_t i = 0; i < path.size(); ++i) {
        cout << landmarkNames[path[i]] << "\n";
        if (i + 1 < path.size()) {
            cout << "   |\n";
            cout << "   | " << getEdgeDistance(path[i], path[i + 1]) << " m\n";
            cout << "   v\n";
        }
    }

    cout << "\nTotal Walking Distance: " << dist[destination] << " meters\n";
    cout << "Number of Walkways Traversed: " << (path.size() - 1) << "\n";
    cout << "==================================================\n\n";
}

void searchLandmark() {
    cout << "\n--- Search Campus Landmark ---\n";
    cout << "Enter search query (name or keyword): ";
    string query;
    getline(cin, query);

    // trim spaces at both ends
    while (!query.empty() && (query.front() == ' ' || query.front() == '\t')) query.erase(query.begin());
    while (!query.empty() && (query.back() == ' ' || query.back() == '\t')) query.pop_back();

    if (query.empty()) {
        cout << "[Error] Search query cannot be empty.\n\n";
        return;
    }

    string lowerQuery = toLowerCase(query);
    vector<int> matches;
    for (size_t i = 0; i < landmarkNames.size(); ++i) {
        if (toLowerCase(landmarkNames[i]).find(lowerQuery) != string::npos) {
            matches.push_back(static_cast<int>(i));
        }
    }

    cout << "\n==================================================\n";
    cout << "                 SEARCH RESULTS                   \n";
    cout << "==================================================\n";
    if (matches.empty()) {
        cout << "No landmarks matched the keyword: \"" << query << "\"\n";
    } else {
        cout << "Found " << matches.size() << " matching landmark(s):\n\n";
        for (int idx : matches) {
            cout << "Landmark ID [" << (idx + 1) << "]: " << landmarkNames[idx] << "\n";
            cout << "  Direct Connections (" << campusGraph[idx].size() << " pathways):\n";
            for (const auto& edge : campusGraph[idx]) {
                cout << "    -> " << landmarkNames[edge.to] << " (" << edge.distance << " m)\n";
            }
            cout << "\n";
        }
    }
    cout << "==================================================\n\n";
}

void displayConnections() {
    cout << "\n--- View Landmark Connections ---\n";
    int node = getValidatedLandmark("Enter Landmark ID (1-12): ");
    if (node == -1) return;

    cout << "\n==================================================\n";
    cout << "             LANDMARK CONNECTIONS                 \n";
    cout << "==================================================\n";
    cout << "Landmark: [" << (node + 1) << "] " << landmarkNames[node] << "\n";
    cout << "Degree (Total Direct Connections): " << campusGraph[node].size() << "\n\n";
    cout << left << setw(30) << "Directly Connected To" << "Walking Distance\n";
    cout << string(50, '-') << "\n";

    for (const auto& edge : campusGraph[node]) {
        cout << left << setw(30) << landmarkNames[edge.to] << edge.distance << " m\n";
    }
    cout << "==================================================\n\n";
}

// Recursive print of the BFS tree (a preorder walk of the tree)
void printHierarchyTree(int u, const vector<vector<int>>& children, const string& prefix) {
    for (size_t i = 0; i < children[u].size(); ++i) {
        bool isLast = (i + 1 == children[u].size());
        int child = children[u][i];
        cout << prefix << (isLast ? "└── " : "├── ")
             << "[" << (child + 1) << "] " << landmarkNames[child] << "\n";
        printHierarchyTree(child, children, prefix + (isLast ? "    " : "│   "));
    }
}

void showTraversalTree() {
    cout << "\n--- View Campus Traversal Tree ---\n";
    int start = getValidatedLandmark("Enter Root/Starting Landmark ID (1-12): ");
    if (start == -1) return;

    // Reuse bfs(): turn each node's parent into a children list
    vector<int> visitOrder, parent;
    bfs(start, visitOrder, parent);

    vector<vector<int>> children(landmarkNames.size());
    for (int v : visitOrder) {              // visitOrder keeps BFS discovery order
        if (parent[v] != -1) children[parent[v]].push_back(v);
    }

    cout << "\n==================================================\n";
    cout << "         CAMPUS BFS SPANNING TRAVERSAL TREE       \n";
    cout << "==================================================\n";
    cout << "[" << (start + 1) << "] " << landmarkNames[start] << "\n";
    printHierarchyTree(start, children, "");
    cout << "\nNote: Tree branches represent first discovery paths during BFS.\n";
    cout << "==================================================\n\n";
}

void compareRoutes() {
    cout << "\n--- Compare Routes (BFS vs Dijkstra) ---\n";
    int source = getValidatedLandmark("Enter Source Landmark ID (1-12): ");
    if (source == -1) return;

    int destination = getValidatedLandmark("Enter Destination Landmark ID (1-12): ");
    if (destination == -1) return;

    if (source == destination) {
        cout << "\n[Info] Source and Destination are identical (" << landmarkNames[source] << ").\n";
        cout << "Both routes have 0 walkways and 0 meters distance.\n\n";
        return;
    }

    // Route 1: BFS (fewest walkways, ignores distances)
    vector<int> visitOrder, bfsParent;
    bfs(source, visitOrder, bfsParent);
    vector<int> bfsPath = reconstructPathUsingStack(source, destination, bfsParent);

    // Route 2: Dijkstra (shortest total distance)
    vector<int> dist, dijkstraParent;
    dijkstra(source, dist, dijkstraParent);
    vector<int> dijkstraPath = reconstructPathUsingStack(source, destination, dijkstraParent);

    if (bfsPath.empty() || dijkstraPath.empty()) {
        cout << "\n[Error] No path exists between " << landmarkNames[source]
             << " and " << landmarkNames[destination] << ".\n\n";
        return;
    }

    int bfsDistance = calculatePathDistance(bfsPath);
    int dijkstraDistance = dist[destination];

    cout << "\n==================================================\n";
    cout << "                 ROUTE COMPARISON                 \n";
    cout << "==================================================\n";
    cout << "From: [" << (source + 1) << "] " << landmarkNames[source] << "\n";
    cout << "To:   [" << (destination + 1) << "] " << landmarkNames[destination] << "\n\n";

    cout << "[1] BREADTH-FIRST SEARCH (BFS) ROUTE:\n";
    cout << "  Optimization Goal : Minimum number of walkways (fewest hops)\n";
    cout << "  Walkways (Hops)   : " << (bfsPath.size() - 1) << "\n";
    cout << "  Total Distance    : " << bfsDistance << " meters\n";
    cout << "  Path              : ";
    printInlinePath(bfsPath);

    cout << "\n[2] DIJKSTRA'S ALGORITHM ROUTE:\n";
    cout << "  Optimization Goal : Minimum total walking distance (meters)\n";
    cout << "  Walkways (Hops)   : " << (dijkstraPath.size() - 1) << "\n";
    cout << "  Total Distance    : " << dijkstraDistance << " meters\n";
    cout << "  Path              : ";
    printInlinePath(dijkstraPath);

    cout << "\nKEY DSA TAKEAWAY:\n";
    if (bfsDistance > dijkstraDistance) {
        cout << "  -> BFS used " << walkwaysText(bfsPath.size() - 1)
             << " vs " << walkwaysText(dijkstraPath.size() - 1)
             << " for Dijkstra, but Dijkstra's route is " << (bfsDistance - dijkstraDistance)
             << " meters shorter because it accounts for edge weights!\n";
    } else {
        cout << "  -> For this pair, the route with the fewest walkways is also the shortest in distance.\n";
    }
    cout << "==================================================\n\n";
}

// =====================================================================
// SECTION 6: MENU AND MAIN
// =====================================================================

void displayMenu() {
    cout << "==================================================\n";
    cout << "       CAMPUS NAVIGATION & LANDMARK EXPLORER      \n";
    cout << "             C++ DSA MINI PROJECT                 \n";
    cout << "==================================================\n";
    cout << "1. View Campus Map\n";
    cout << "2. Explore Campus (BFS)\n";
    cout << "3. Find Shortest Route (Dijkstra)\n";
    cout << "4. Search Landmark\n";
    cout << "5. View Landmark Connections\n";
    cout << "6. View Campus Traversal Tree\n";
    cout << "7. Compare Routes\n";
    cout << "8. Exit\n";
    cout << "==================================================\n";
    cout << "Enter your choice: ";
}

int main() {
    initializeCampus();

    while (true) {
        displayMenu();
        int choice = 0;
        if (!(cin >> choice)) {
            if (cin.eof()) {       // input ended (e.g. piped test file): stop cleanly
                cout << "\n";
                return 0;
            }
            clearInputBuffer();
            cout << "\n[Error] Invalid input. Please enter a number between 1 and 8.\n\n";
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: displayCampusMap(); break;
            case 2: {
                int start = getValidatedLandmark("Enter Starting Landmark ID (1-12): ");
                if (start != -1) exploreCampusBFS(start);
                break;
            }
            case 3: findShortestPath(); break;
            case 4: searchLandmark(); break;
            case 5: displayConnections(); break;
            case 6: showTraversalTree(); break;
            case 7: compareRoutes(); break;
            case 8:
                cout << "\nThank you for using Campus Navigation & Landmark Explorer! Have a great day.\n";
                return 0;
            default:
                cout << "\n[Error] Invalid option. Please choose between 1 and 8.\n\n";
                break;
        }
    }
}