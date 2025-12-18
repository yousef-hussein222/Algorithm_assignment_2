// ArcadiaEngine.cpp - STUDENT TEMPLATE

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
    // Hint: You'll need a hash table with double hashing collision resolution
    const int TABLE_SIZE = 101;
    struct Entry {
        int key;
        string value;
        bool isOccupied;
        bool isDeleted;

        Entry() : key(-1), value(""), isOccupied(false), isDeleted(false) {}
    };
    vector<Entry> table;
public:
    ConcretePlayerTable() {
        table.resize(TABLE_SIZE);
    }

    int h1(const int key) const {
        return key % TABLE_SIZE;
    }

    static int h2(const int key) {
        return 7 - (key % 7);
    }

    void insert(int playerID, string name) override {
        // Remember to handle collisions using h1(key) + i * h2(key)

        for (int i = 0; i < TABLE_SIZE;i++) {
            const int idx = (h1(playerID) + i * h2(playerID)) % TABLE_SIZE;
            if (!table[idx].isOccupied || table[idx].isDeleted) {
                table[idx].key = playerID;
                table[idx].value = name;
                table[idx].isOccupied = true;
                table[idx].isDeleted = false;
                return;
            }

            if (table[idx].key == playerID) {
                table[idx].value = name;
                return;
            }
        }

        throw runtime_error("Table is full");
    }

    string search(int playerID) override {
        // Return "" if player not found

        for (int i = 0;i < TABLE_SIZE; i++) {
            const int idx = (h1(playerID) + i * h2(playerID)) % TABLE_SIZE;
            if (!table[idx].isOccupied && !table[idx].isDeleted) {
                return "";
            }

            if (table[idx].isOccupied && !table[idx].isDeleted &&
                table[idx].key == playerID) {
                return table[idx].value;
            }
        }

        return "";
    }
};

// --- 2. Leaderboard (Skip List) ---

class ConcreteLeaderboard : public Leaderboard {
private:
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

    void fixInsert(RBNode* newNode) {
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

    // --------------------------------------------------------------
    RBNode* searchById(RBNode* node, int id) {
        if (!node) return nullptr;
        if (node->id == id) return node;
        RBNode* leftSearch = searchById(node->left, id);
        if (leftSearch) return leftSearch;
        return searchById(node->right, id);
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

    void transplant(RBNode* u, RBNode* v) {
        if (!u->parent) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        if (v) v->parent = u->parent;
    }

        // -------------------- delete fixup --------------------
    void deleteFixup(RBNode* x) {
        while (x != root && x && x->color == 'B') {
            if (x == x->parent->left) {
                RBNode* w = x->parent->right;  // sibling

                // Case 1: sibling is red
                if (w && w->color == 'R') {
                    w->color = 'B';
                    x->parent->color = 'R';
                    leftRotate(x->parent);
                    w = x->parent->right;
                }

                // Case 2: sibling is black with two black children
                if ((!w || !w->left || w->left->color == 'B') && (!w || !w->right || w->right->color == 'B')) {
                    if (w) w->color = 'R';
                    x = x->parent;
                } else {
                    // Case 3: sibling is black, left child is red, right child is black
                    if (!w || !w->right || w->right->color == 'B') {
                        if (w && w->left) w->left->color = 'B';
                        if (w) w->color = 'R';
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    // Case 4: sibling is black, right child is red
                    if (w) w->color = x->parent->color;
                    x->parent->color = 'B';
                    if (w && w->right) w->right->color = 'B';
                    leftRotate(x->parent);
                    x = root;
                }
            } else {  // Mirror cases when x is right child
                RBNode* w = x->parent->left;  // sibling

                // Case 1: sibling is red
                if (w && w->color == 'R') {
                    w->color = 'B';
                    x->parent->color = 'R';
                    rightRotate(x->parent);
                    w = x->parent->left;
                }

                // Case 2: sibling is black with two black children
                if ((!w || !w->right || w->right->color == 'B') && (!w || !w->left || w->left->color == 'B')) {
                    if (w) w->color = 'R';
                    x = x->parent;
                } else {
                    // Case 3: sibling is black, right child is red, left child is black
                    if (!w || !w->left || w->left->color == 'B') {
                        if (w && w->right) w->right->color = 'B';
                        if (w) w->color = 'R';
                        leftRotate(w);
                        w = x->parent->left;
                    }
                    // Case 4: sibling is black, left child is red
                    if (w) w->color = x->parent->color;
                    x->parent->color = 'B';
                    if (w && w->left) w->left->color = 'B';
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        if (x) x->color = 'B';
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
                if(newNode->price < curr->price) {
                    curr = curr->left;
                }
                else if(newNode->price > curr->price) {
                    curr = curr->right;
                }
                else {
                    if(newNode->id < curr->id) {
                        curr = curr->left;
                    }
                    else {
                        curr = curr->right;
                    }
                }
            }
            newNode->parent = prev;
            if(newNode->price < prev->price) {
                prev->left = newNode;
            }
            else if (newNode->price > prev->price){
                prev->right = newNode;
            }
            else {
                if(newNode->id < prev->id) {
                    prev->left = newNode;
                }
                else {
                    prev->right = newNode;
                }
            }
            fixInsert(newNode);
        }
    }

    void deleteItem(int itemID) override {
        RBNode* z = searchById(root, itemID);
        if (!z) return;

        RBNode* y = z;
        RBNode* x;
        char yOriginalColor = y->color;

        if (!z->left) {
            // Case 1: z has no left child
            x = z->right;
            transplant(z, z->right);
        } else if (!z->right) {
            // Case 2: z has no right child
            x = z->left;
            transplant(z, z->left);
        } else {
            // Case 3: z has two children
            // Find successor (minimum in right subtree)
            y = minimum(z->right);
            yOriginalColor = y->color;
            x = y->right;

            if (y->parent == z) {
                // Successor is direct child of z
                if (x) x->parent = y;
            } else {
                // Successor is not direct child of z
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }

            // Replace z with y
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }

        delete z;

        // Fix Red-Black Tree properties if a black node was removed
        if (yOriginalColor == 'B') {
            deleteFixup(x);
        }
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================

int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {
    if (n == 0) return 0;

    int Sum = 0;
    for (int i = 0; i < n; i++) {
        Sum += coins[i];
    }

    int target = Sum / 2;

    vector<vector<bool>> dp(n + 1);
    for (int i = 0; i <= n; i++) {
        dp[i] = vector<bool>(target + 1, false);
    }

    for (int i = 0; i <= n; i++) {
        dp[i][0] = true;
    }

    //fill dp table
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= target; j++) {
            if (coins[i - 1] <= j) {
                dp[i][j] = dp[i - 1][j] || dp[i - 1][j - coins[i - 1]];
            }
            else {
                dp[i][j] = dp[i - 1][j];
            }
        }
    }

    int bestSplit = 0;
    for (int j = target; j >= 0; j--) {
        if (dp[n][j]) {
            bestSplit = j;
            break;
        }
    }

    return Sum - 2 * bestSplit;
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
    int n = s.length();
    if (n == 0) return 1;
    vector<long long> dp(n + 1, 0);
    dp[0] = 1;
    if (n >= 1) dp[1] = 1;
    for (int i = 2; i <= n; i++) {
        dp[i] = dp[i - 1];
        char c1 = s[i - 2], c2 = s[i - 1];
        if ((c1 == 'u' && c2 == 'u') || (c1 == 'n' && c2 == 'n')) {
            dp[i] += dp[i - 2];
        }
    }
    return dp[n];
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
    if (tasks.empty()) return 0;
    map<char, int> freqMap;
    for (char task : tasks) {
        freqMap[task]++;
    }
    priority_queue<int> pq;
    for (auto const& [task, count] : freqMap) {
        pq.push(count);
    }
    int time = 0;
    while (!pq.empty()) {
        int cycle_time = 0;
        vector<int> temp_list;
        while (cycle_time <= n) {
            if (!pq.empty()) {
                int count = pq.top();
                pq.pop();
                if (count > 1) {
                    temp_list.push_back(count - 1);
                }
            }
            time++;
            cycle_time++;
            if (pq.empty() && temp_list.empty()) {
                break;
            }
        }
        for (int count : temp_list) {
            pq.push(count);
        }
    }
    return time;
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

