#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef long double ld;
typedef pair<long long, long long> pll;
typedef priority_queue<ll> pq;
typedef priority_queue<ll, vector<ll>, greater<ll>> pqr;
#define all(a) (a).begin(), (a).end()
#define fixed(n) fixed << setprecision(n)
#define MOD 1000000007
#define loop(i,n) for(ll i=0;i<n;i++)
#define loop1(a,n) for(ll i=a;i<n;i++)

ll dfs(ll node, ll par, vector<vector<ll>>& graph, vector<ll>& h, ll& ans) {
    vector<ll> child_h;
    for (auto& child : graph[node]) {
        if (child != par) {
            child_h.push_back(dfs(child, node, graph, h, ans));
        }
    }
    
    if (child_h.empty()) {
        h[node] = 0;
        ans += 1;
    } else if (child_h.size() == 1) {
        h[node] = child_h[0] + 1;
        ans += 1;
    } else {
        sort(child_h.rbegin(), child_h.rend());
        h[node] = child_h[0] + 1;
        ans += child_h[1] + 2; 
    }
    return h[node];
}

ll solve() {
    ll n; 
    cin >> n;
    vector<vector<ll>> graph(n + 1);
    loop(i, n - 1) {
        ll p; 
        cin >> p;
        ll u = i + 2;
        graph[p].push_back(u);
        graph[u].push_back(p);
    }
    vector<ll> h(n + 1, 0);
    ll ans = 0;
    dfs(1, -1, graph, h, ans);
    return ans;
}

int main() {
    ios_base::sync_with_stdio(false); cin.tie(nullptr);
    ll t; 
    cin >> t;
    while(t--) {
        cout << solve() << "\n";
    }
    return 0;
}