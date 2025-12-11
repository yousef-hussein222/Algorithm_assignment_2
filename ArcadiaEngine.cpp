// ArcadiaEngine.cpp - STUDENT TEMPLATE
// TODO: Implement all the functions below according to the assignment requirements

#include "ArcadiaEngine.h"
#include <algorithm>
#include <queue>
#include <numeric>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>

using namespace std;

// =========================================================
// PART A: DATA STRUCTURES (Concrete Implementations)
// =========================================================

// --- 1. PlayerTable (Double Hashing) ---

class ConcretePlayerTable : public PlayerTable {
private:
    // TODO: Define your data structures here
    // Hint: You'll need a hash table with double hashing collision resolution
    const int TABLE_SIZE = 101;
    vector<pair<int, string>> table;
public:
    ConcretePlayerTable() {
        // TODO: Initialize your hash table
        table.resize(TABLE_SIZE);
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i].first = -1;
        }
    }

    int h1(const int key) const {
        return key % TABLE_SIZE;
    }

    static int h2(const int key) {
        return 7 - (key % 7);
    }

    void insert(int playerID, string name) override {
        // TODO: Implement double hashing insert
        // Remember to handle collisions using h1(key) + i * h2(key)

        for (int i = 0; i < TABLE_SIZE;i++) {
            const int idx = (h1(playerID) + i * h2(playerID)) % TABLE_SIZE;
            if (table[idx].first == -1) {
                table[idx].first = playerID;
                table[idx].second = name;
                return;
            }
        }
    }

    string search(int playerID) override {
        // TODO: Implement double hashing search
        // Return "" if player not found

        for (int i = 0;i < TABLE_SIZE; i++) {
            const int idx = (h1(playerID) + i * h2(playerID)) % TABLE_SIZE;
            if (table[idx].first == playerID) {
                return table[idx].second;
            }

            if (table[idx].first == -1) {
                break;
            }
        }

        return "";
    }
};

// --- 2. Leaderboard (Skip List) ---

class ConcreteLeaderboard : public Leaderboard {
private:
    // TODO: Define your skip list node structure and necessary variables
    // Hint: You'll need nodes with multiple forward pointers

public:
    ConcreteLeaderboard() {
        // TODO: Initialize your skip list
    }

    void addScore(int playerID, int score) override {
        // TODO: Implement skip list insertion
        // Remember to maintain descending order by score
    }

    void removePlayer(int playerID) override {
        // TODO: Implement skip list deletion
    }

    vector<int> getTopN(int n) override {
        // TODO: Return top N player IDs in descending score order
        return {};
    }
};

// --- 3. AuctionTree (Red-Black Tree) ---

class ConcreteAuctionTree : public AuctionTree {
private:
    // TODO: Define your Red-Black Tree node structure
    // Hint: Each node needs: id, price, color, left, right, parent pointers
    struct RBNode {
        int id,price;
        char color;
        RBNode* left,*right,*parent;
        RBNode(int id,int price,char color) {
            this->id = id;
            this->price = price;
            this->color = color;
            this->left = nullptr;
            this->right = nullptr;
            this->parent = nullptr;
        }
    };
    RBNode* root;
    // -------------------------------------- Helper Functions --------------------------------------
    void rightRotate(RBNode* x)
    {
        RBNode* y = x->left;
        if (!y) {
            return;
        }

        x->left = y->right;
        if (y->right) {
            y->right->parent = x;
        }

        y->parent = x->parent;
        if (!x->parent) {
            root = y;
        }
        else if (x == x->parent->left) {
            x->parent->left = y;
        }
        else {
            x->parent->right = y;
        }

        y->right = x;
        x->parent = y;
    }
    void leftRotate(RBNode* x)
    {
        RBNode* y = x->right;
        if(!y) {
            return;
        }

        x->right = y->left;
        if(y->left) {
            y->left->parent = x;
        }

        y->parent = x->parent;
        if(!x->parent) {
            root = y;
        }
        else if(x == x->parent->left) {
            x->parent->left = y;
        }
        else {
            x->parent->right = y;
        }

        y->left = x;
        x->parent = y;
    }
public:
    ConcreteAuctionTree() {
        root = nullptr;
    }

    void insertItem(int itemID, int price) override {
        if(!root) {
            auto newNode = new RBNode(itemID,price,'B');
            root = newNode;
        }
        else {
            auto newNode = new RBNode(itemID,price,'R');
            RBNode* curr = root;
            RBNode* prev = nullptr;
            while(curr != nullptr) {
                prev = curr;
                if(newNode->id < curr->id) {
                    curr = curr->left;
                }
                else if(newNode->id > curr->id) {
                    curr = curr->right;
                }
                else {
                    cout << "Not allowing duplicate IDs\n";
                    break;
                }
            }
            newNode->parent = prev;
            if(newNode->id < prev->id) {
                prev->left = newNode;
            }
            else {
                prev->right = newNode;
            }

            while(newNode != root && newNode->parent->color == 'R') {

                if(newNode->parent == newNode->parent->parent->left) {
                    RBNode* y = newNode->parent->parent->right;
                    if(y && y->color == 'R') {
                        newNode->parent->color = 'B';
                        y->color = 'B';
                        newNode->parent->parent->color = 'R';
                        newNode = newNode->parent->parent;
                    }
                    else {
                        if(newNode == newNode->parent->right) {
                            newNode = newNode->parent;
                            leftRotate(newNode);
                        }
                        newNode->parent->parent->color = 'R';
                        newNode->parent->color = 'B';
                        rightRotate(newNode->parent->parent);
                    }
                }
                else {
                    RBNode* y = newNode->parent->parent->left;
                    if(y && y->color == 'R') {
                        newNode->parent->color = 'B';
                        y->color = 'B';
                        newNode->parent->parent->color = 'R';
                        newNode = newNode->parent->parent;
                    }
                    else {
                        if(newNode == newNode->parent->left) {
                            newNode = newNode->parent;
                            rightRotate(newNode);
                        }
                        newNode->parent->parent->color = 'R';
                        newNode->parent->color = 'B';
                        leftRotate(newNode->parent->parent);
                    }
                }
            }
            root->color = 'B';
        }
    }

    void deleteItem(int itemID) override {
        // TODO: Implement Red-Black Tree deletion
        // This is complex - handle all cases carefully
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================

int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {
    // TODO: Implement partition problem using DP
    // Goal: Minimize |sum(subset1) - sum(subset2)|
    // Hint: Use subset sum DP to find closest sum to total/2
    return 0;
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>>& items) {
    int rows = (int)items.size() + 1;
    int cols = capacity + 1;
    vector<vector<int>>table(rows,vector<int>(cols,0));
    for(int i = 1;i< rows;i++) {
        for(int j = 1;j < cols;j++) {
            if(j >= items[i-1].first)
                table[i][j] = max(table[i-1][j],table[i-1][j - items[i-1].first] + items[i-1].second);
            else
                table[i][j] = table[i-1][j];
        }
    }
    return table[rows-1][cols-1];
}

long long InventorySystem::countStringPossibilities(string s) {
    // TODO: Implement string decoding DP
    // Rules: "uu" can be decoded as "w" or "uu"
    //        "nn" can be decoded as "m" or "nn"
    // Count total possible decodings
    return 0;
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool WorldNavigator::pathExists(int n, vector<vector<int>>& edges, int source, int dest) {
    if(n <= 0)
        return false;

    if (source < 0 || source >= n || dest < 0 || dest >= n)
        return false;

    if (source == dest)
        return true;

    vector<vector<int>>adjacentList(n);
    // convert edges to adjacent list
    for(auto& edge:edges) {
        int u = edge[0];
        int v = edge[1];

        adjacentList[u].push_back(v);
        adjacentList[v].push_back(u);
    }


    vector<bool>visited(n, false);
    queue<int>q;
    visited[source] = true;
    q.push(source);

    while(!q.empty()) {
        int currNode = q.front();
        q.pop();

        for(int node : adjacentList[currNode]) {
            if(!visited[node]) {
                visited[node] = true;
                if(node == dest) {
                    return true;
                }
                q.push(node);
            }
        }
    }
    return false;
}

long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                                       vector<vector<int>>& roadData) {
    // TODO: Implement Minimum Spanning Tree (Kruskal's or Prim's)
    // roadData[i] = {u, v, goldCost, silverCost}
    // Total cost = goldCost * goldRate + silverCost * silverRate
    // Return -1 if graph cannot be fully connected

    vector<vector<pair<long long, int>>> adj(n);

    for (auto &r : roadData) {
        int u = r[0], v = r[1];
        long long goldCost = r[2], silverCost = r[3];

        long long totalCost = goldCost * goldRate + silverCost * silverRate;

        adj[u].push_back({totalCost, v});
        adj[v].push_back({totalCost, u});
    }

    vector<bool> mst(n, false);
    priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<pair<long long, int>>> pq;
    pq.push({0, 0});
    long long totalCost = 0;
    int visited = 0;
    while (!pq.empty()) {
        long long w = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (mst[u]) continue;

        mst[u] = true;

        totalCost += w;
        visited++;

        for (auto &p : adj[u]) {
            int v = p.second;
            long long cost = p.first;
            if (!mst[v])
                pq.push({cost, v});
        }
    }

    if (visited == n) {
        return totalCost;
    }

    return -1;
}
string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>>& roads) {
    // TODO: Implement All-Pairs Shortest Path (Floyd-Warshall)
    // Sum all shortest distances between unique pairs (i < j)
    // Return the sum as a binary string
    // Hint: Handle large numbers carefully
    return "0";
}

// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================

int ServerKernel::minIntervals(vector<char>& tasks, int n) {
    // TODO: Implement task scheduler with cooling time
    // Same task must wait 'n' intervals before running again
    // Return minimum total intervals needed (including idle time)
    // Hint: Use greedy approach with frequency counting
    return 0;
}

// =========================================================
// FACTORY FUNCTIONS (Required for Testing)
// =========================================================

extern "C" {
PlayerTable* createPlayerTable() {
    return new ConcretePlayerTable();
}

Leaderboard* createLeaderboard() {
    return new ConcreteLeaderboard();
}

AuctionTree* createAuctionTree() {
    return new ConcreteAuctionTree();
}
}

