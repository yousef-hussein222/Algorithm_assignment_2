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
    struct  Node {
        int score;
        int PlayerID;
        int level ;
        Node ** forward;
        Node(int PID,int sc,int l ) {
            PlayerID =PID;
            score = sc;
            level = l;
            forward = new Node * [ l+  1];
            for(int i = 0; i<= l;i++) {
                forward[i] = nullptr;
            }
        }
        ~Node() {
            delete[] forward;
        }
    };
    int maxLevel;
    int currentLevel;
    Node * sentinel;
    int randomLevel() {
        int lvl = 0;
        while ((rand() % 2) && lvl < maxLevel) lvl++;
        return lvl;
    }

public:
    ConcreteLeaderboard(int maxL = 6) {
        srand((unsigned)time(nullptr));
        maxLevel = maxL;
        currentLevel = 0;
        sentinel = new Node(-1, INT_MAX, maxL);
    }

    //destructor
    ~ConcreteLeaderboard() {
        Node* cur = sentinel->forward[0];
        while (cur) {
            Node* nxt = cur->forward[0];
            delete cur;
            cur = nxt;
        }
        delete sentinel;
    }
    //helper function
    int checkIfPlayerExist(int id) {
        Node* cur = sentinel->forward[0];
        while (cur != nullptr) {
            if (cur->PlayerID == id)
                return cur->score;
            cur = cur->forward[0];
        }
        return -1;
    }
    void addScore(int playerID, int score) override {
        Node * cur = sentinel;
        vector<Node *>path(maxLevel + 1 ,nullptr);
        int checkPlayerExist = checkIfPlayerExist(playerID);

        if(checkPlayerExist !=-1) {
            removePlayer(playerID);
        }
        for(int lvl = maxLevel ;lvl>= 0 ; lvl --) {
            while (cur->forward[lvl] && (cur->forward[lvl]->score > score ||
       (cur->forward[lvl]->score == score && cur->forward[lvl]->PlayerID < playerID))) {
                cur = cur->forward[lvl] ;
       }
            path[lvl] = cur;
        }

        int numoflevel = randomLevel();

        Node * newNode =new Node(playerID,score,numoflevel);
        for(int i = 0; i<= numoflevel; i++) {
            newNode->forward[i] = path[i]->forward[i];
            path[i]->forward[i] = newNode;
        }
        if (numoflevel > currentLevel) currentLevel = numoflevel;
    }

    void removePlayer(int playerID) override {
        // Find the node by ID and the time is O(n)
        Node* target = sentinel->forward[0];
        while (target && target->PlayerID != playerID)
            target = target->forward[0];
        if (!target) {
            cout << "Not found any player with id: " << playerID << endl;
            return;
        }

        int score = target->score;
        vector<Node*> path(maxLevel + 1, nullptr);
        Node* cur = sentinel;
        //get the node and ist forward path
        for (int lvl = maxLevel; lvl >= 0; lvl--) {
            while (cur->forward[lvl] &&(cur->forward[lvl]->score > score ||(cur->forward[lvl]->score == score &&cur->forward[lvl]->PlayerID < playerID)))
            {
                cur = cur->forward[lvl];
            }
            path[lvl] = cur;
        }
        Node* realTarget = path[0]->forward[0];

        // delete node
        if (realTarget && realTarget->PlayerID == playerID) {
            for (int i = 0; i <= realTarget->level; i++) {
                if (path[i]->forward[i] == realTarget)
                    path[i]->forward[i] = realTarget->forward[i];
            }
            delete realTarget;
        }
        //fix the forward vector
        while (currentLevel > 0 && sentinel->forward[currentLevel] == nullptr)
            currentLevel--;
    }

    //Time o(n) => (revers and get the first n IDPlayers )
    vector<int> getTopN(int n) override {
        Node * cur = sentinel->forward[0];
        vector<int> topPlayers;
        while (cur != nullptr && (int)topPlayers.size() < n) {
            topPlayers.push_back(cur->PlayerID);
            cur = cur->forward[0];
        }
        return topPlayers;
    }
    //searching using Score take time o(log n)
    vector<int> searchUsingScore(int scr) {
        vector<int> playerIDs;
        Node* cur = sentinel;
        for (int lvl = maxLevel; lvl >= 0; --lvl) {
            while (cur->forward[lvl] && cur->forward[lvl]->score > scr) {
                cur = cur->forward[lvl];
            }
        }
        Node* node = cur->forward[0];
        while (node && node->score == scr) {
            playerIDs.push_back(node->PlayerID);
            node = node->forward[0];
        }
        return playerIDs;
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
    RBNode* search(RBNode* node, int id) {
        while (node) {
            if (id < node->id) node = node->left;
            else if (id > node->id) node = node->right;
            else return node;
        }
        return nullptr;
    }

    RBNode* minimum(RBNode* node) {
        while (node && node->left) node = node->left;
        return node;
    }

    RBNode* successor(RBNode* node) {
        if (!node) return nullptr;
        if (node->right) return minimum(node->right);
        RBNode* p = node->parent;
        while (p && node == p->right) {
            node = p;
            p = p->parent;
        }
        return p;
    }

        // -------------------- delete fixup --------------------
    void deleteFixup(RBNode* node) {
    while (node != root && node->color == 'B') {
        RBNode* parentNode = node->parent;

        if (parentNode && node == parentNode->left) {
            RBNode* siblingNode = parentNode->right;

            if (siblingNode && siblingNode->color == 'R') {
                // Case 1: sibling is red
                siblingNode->color = 'B';
                parentNode->color = 'R';
                leftRotate(parentNode);
                siblingNode = parentNode->right;
            }

            if ((!siblingNode || !siblingNode->left || siblingNode->left->color == 'B') &&
                (!siblingNode || !siblingNode->right || siblingNode->right->color == 'B')) {
                // Case 2: sibling black, both children black
                if (siblingNode) siblingNode->color = 'R';
                node = parentNode;
            } else {
                if (!siblingNode || !siblingNode->right || siblingNode->right->color == 'B') {
                    // Case 3: sibling black, left child red, right child black
                    if (siblingNode && siblingNode->left) siblingNode->left->color = 'B';
                    if (siblingNode) siblingNode->color = 'R';
                    if (siblingNode) rightRotate(siblingNode);
                    siblingNode = parentNode->right;
                }
                // Case 4: sibling black, right child red
                if (siblingNode) siblingNode->color = parentNode->color;
                parentNode->color = 'B';
                if (siblingNode && siblingNode->right) siblingNode->right->color = 'B';
                leftRotate(parentNode);
                node = root;
            }
        } else if (parentNode) { //symmetric: node is right child
            RBNode* siblingNode = parentNode->left;

            if (siblingNode && siblingNode->color == 'R') {
                // Case 1 mirror: sibling red
                siblingNode->color = 'B';
                parentNode->color = 'R';
                rightRotate(parentNode);
                siblingNode = parentNode->left;
            }

            if ((!siblingNode || !siblingNode->left || siblingNode->left->color == 'B') &&
                (!siblingNode || !siblingNode->right || siblingNode->right->color == 'B')) {
                // Case 2 mirror: sibling black, both children black
                if (siblingNode) siblingNode->color = 'R';
                node = parentNode;
            } else {
                if (!siblingNode || !siblingNode->left || siblingNode->left->color == 'B') {
                    // Case 3 mirror: sibling black, right child red, left child black
                    if (siblingNode && siblingNode->right) siblingNode->right->color = 'B';
                    if (siblingNode) siblingNode->color = 'R';
                    if (siblingNode) leftRotate(siblingNode);
                    siblingNode = parentNode->left;
                }
                // Case 4 mirror: sibling black, left child red
                if (siblingNode) siblingNode->color = parentNode->color;
                parentNode->color = 'B';
                if (siblingNode && siblingNode->left) siblingNode->left->color = 'B';
                rightRotate(parentNode);
                node = root;
            }
        }
    }
    if (node) node->color = 'B';
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
        RBNode* nodeToDelete = search(root, itemID);
        if (!nodeToDelete) return;

        RBNode* nodeToSplice = nodeToDelete;
        RBNode* childNode = nullptr;
        char originalColor = nodeToSplice->color;
        bool usedTemporaryNIL = false;

        if (nodeToDelete->left && nodeToDelete->right) {
            nodeToSplice = successor(nodeToDelete);
            nodeToDelete->id = nodeToSplice->id;
            nodeToDelete->price = nodeToSplice->price;
            originalColor = nodeToSplice->color;
        }

        childNode = nodeToSplice->left ? nodeToSplice->left : nodeToSplice->right;

        if (!childNode) {
            childNode = new RBNode(0, 0, 'B');
            usedTemporaryNIL = true;
        }

        if (!nodeToSplice->parent) root = childNode;
        else if (nodeToSplice == nodeToSplice->parent->left)
            nodeToSplice->parent->left = childNode;
        else
            nodeToSplice->parent->right = childNode;

        if (childNode) childNode->parent = nodeToSplice->parent;

        if (originalColor == 'B') deleteFixup(childNode);

        if (usedTemporaryNIL && childNode) {
            if (childNode->parent) {
                if (childNode == childNode->parent->left) childNode->parent->left = nullptr;
                else childNode->parent->right = nullptr;
            }
            delete childNode;
        }

        delete nodeToSplice;
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
    const long long INF = (1LL << 60);
    unsigned long long total = 0;
    string bits;
    vector<vector<long long>> dist(n, vector<long long>(n, INF));
    for(int i =0 ;i<n ; i++) {
        dist[i][i] = 0;
    }

    // fill the dist with roads inputs
    for(auto &road : roads) {
        int from = road[0];
        int to = road[1];
        long long newdist = road[2];
        dist[from][to] = min(dist[from][to],newdist);
    }

    //Floyd–Warshall
    for(int k = 0; k < n; k++) {
        for(int i = 0;i < n ;i++) {
            if (dist[i][k] == INF) continue;
            for(int j =0 ;j< n ; j++) {
                if (dist[k][j] == INF) continue;
                dist[i][j] = min(dist[i][j],dist[i][k]+ dist[k][j]);
            }
        }
    }
    // sum of unique
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (dist[i][j] < INF)
                total += dist[i][j];
        }
    }
    cout<<total<<endl;
    //convert to binarey
    if (total == 0) return "0";
    while (total > 0) {
        bits.push_back((total & 1) ? '1' : '0');
        total >>= 1;
    }
    reverse(bits.begin(), bits.end());
    return bits;
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

