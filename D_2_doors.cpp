#include <iostream>
#include <vector>

using namespace std;

struct Constraint {
    int j;
    int x;
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, q;
    if (!(cin >> n >> q)) return 0;

    vector<int> a(n + 1, (1 << 30) - 1);
    vector<vector<Constraint>> adj(n + 1);

    for (int k = 0; k < q; ++k) {
        int i, j, x;
        cin >> i >> j >> x;
        adj[i].push_back({j, x});
        if (i != j) {
            adj[j].push_back({i, x});
        }
        
        a[i] &= x;
        a[j] &= x;
    }

    for (int i = 1; i <= n; ++i) {
        int req = 0;
        for (const auto& c : adj[i]) {
            if (c.j == i) {
                req |= c.x;
            } else {
                req |= (c.x & ~a[c.j]);
            }
        }
        a[i] = req;
    }

    for (int i = 1; i <= n; ++i) {
        cout << a[i] << (i == n ? "" : " ");
    }
    cout << "\n";

    return 0;
}