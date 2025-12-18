#include <iostream>
#include <vector>
#include <string>
#include "ArcadiaEngine.h"

using namespace std;

extern "C" {
    PlayerTable* createPlayerTable();
    Leaderboard* createLeaderboard();
    AuctionTree* createAuctionTree();
}

void printHeader(string title) {
    cout << "\n========================================" << endl;
    cout << "  " << title << endl;
    cout << "========================================" << endl;
}

int main() {
    PlayerTable* players = createPlayerTable();
    Leaderboard* leaderboard = createLeaderboard();
    AuctionTree* auctions = createAuctionTree();
    InventorySystem inventory;
    WorldNavigator navigator;
    ServerKernel kernel;

    int choice = -1;

    while (choice != 0) {
        cout << "\n--- ARCADIA ENGINE SYSTEM MENU ---" << endl;
        cout << "1. Player Management (Hash Table)" << endl;
        cout << "2. Leaderboard (Skip List)" << endl;
        cout << "3. Auction House (Red-Black Tree)" << endl;
        cout << "4. Loot & Inventory (DP)" << endl;
        cout << "5. World Navigator (Graphs)" << endl;
        cout << "6. Server Kernel (Greedy)" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                printHeader("PLAYER MANAGEMENT");
                cout << "1. Insert/Update Player\n2. Search Player\nSelection: ";
                int sub; cin >> sub;
                if (sub == 1) {
                    int id; string name;
                    cout << "Enter ID and Name: "; cin >> id >> name;
                    players->insert(id, name);
                    cout << "Player updated." << endl;
                } else {
                    int id; cout << "Enter ID: "; cin >> id;
                    string res = players->search(id);
                    cout << (res == "" ? "Not Found" : "Found: " + res) << endl;
                }
                break;
            }
            case 2: {
                printHeader("LEADERBOARD (SKIP LIST)");
                cout << "1. Add Score\n2. Remove Player\n3. Get Top N\nSelection: ";
                int sub; cin >> sub;
                if (sub == 1) {
                    int id, score; cout << "Enter ID and Score: "; cin >> id >> score;
                    leaderboard->addScore(id, score);
                } else if (sub == 2) {
                    int id; cout << "Enter ID to remove: "; cin >> id;
                    leaderboard->removePlayer(id);
                } else {
                    int n; cout << "Enter N: "; cin >> n;
                    vector<int> top = leaderboard->getTopN(n);
                    cout << "Top Players: ";
                    for (int id : top) cout << id << " ";
                    cout << endl;
                }
                break;
            }
            case 3: {
                printHeader("AUCTION HOUSE (RBT)");
                cout << "1. List Item\n2. Remove/Buy Item\nSelection: ";
                int sub; cin >> sub;
                if (sub == 1) {
                    int id, price; cout << "Enter Item ID and Price: "; cin >> id >> price;
                    auctions->insertItem(id, price);
                } else {
                    int id; cout << "Enter Item ID to remove: "; cin >> id;
                    auctions->deleteItem(id);
                }
                break;
            }
            case 4: {
                printHeader("LOOT & INVENTORY (DP)");
                cout << "1. Optimize Loot Split (Partition)\n2. Maximize Value (Knapsack)\nSelection: ";
                int sub; cin >> sub;
                if (sub == 1) {
                    int n; cout << "Number of coins: "; cin >> n;
                    vector<int> coins(n);
                    cout << "Enter coin values: ";
                    for(int i=0; i<n; i++) cin >> coins[i];
                    cout << "Minimum difference: " << inventory.optimizeLootSplit(n, coins) << endl;
                } else {
                    int cap, n; cout << "Capacity and Item count: "; cin >> cap >> n;
                    vector<pair<int, int>> items(n);
                    cout << "Enter Weight and Value for each:\n";
                    for(int i=0; i<n; i++) cin >> items[i].first >> items[i].second;
                    cout << "Max Value: " << inventory.maximizeCarryValue(cap, items) << endl;
                }
                break;
            }
            case 5: {
                printHeader("WORLD NAVIGATOR (GRAPHS)");
                cout << "1. Check Path (BFS)\n2. Minimum Bribe (MST)\nSelection: ";
                int sub; cin >> sub;
                if (sub == 1) {
                    int n, e; cout << "Nodes and Edges count: "; cin >> n >> e;
                    vector<vector<int>> edges(e, vector<int>(2));
                    cout << "Enter edges (u v):\n";
                    for(int i=0; i<e; i++) cin >> edges[i][0] >> edges[i][1];
                    int s, d; cout << "Source and Dest: "; cin >> s >> d;
                    cout << (navigator.pathExists(n, edges, s, d) ? "Path Exists!" : "No Path.") << endl;
                } else {
                    int n, m; long long gr, sr;
                    cout << "Nodes, Edges, GoldRate, SilverRate: "; cin >> n >> m >> gr >> sr;
                    vector<vector<int>> roads(m, vector<int>(4));
                    cout << "Enter (u v gold silver):\n";
                    for(int i=0; i<m; i++) cin >> roads[i][0] >> roads[i][1] >> roads[i][2] >> roads[i][3];
                    cout << "Min Bribe Cost: " << navigator.minBribeCost(n, m, gr, sr, roads) << endl;
                }
                break;
            }
            case 6: {
                printHeader("SERVER KERNEL (GREEDY)");
                int n, cooldown;
                cout << "Number of tasks: "; cin >> n;
                vector<char> tasks(n);
                cout << "Enter tasks (e.g., A A A B B B): ";
                for(int i=0; i<n; i++) cin >> tasks[i];
                cout << "Cooldown period: "; cin >> cooldown;
                cout << "Total cycles: " << kernel.minIntervals(tasks, cooldown) << endl;
                break;
            }
        }
    }

    delete players;
    delete leaderboard;
    delete auctions;

    cout << "Exiting Arcadia Engine..." << endl;
    return 0;
}