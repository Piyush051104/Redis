#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

class segtree {
public:
    vector<pair<ll, ll>> treemax, treemin;

    segtree(ll n) {
        treemax.resize(4 * n + 1);
        treemin.resize(4 * n + 1);
    }

    void build(ll node, ll l, ll r, vector<ll>& v) {
        if (l == r) {
            treemax[node] = {v[l], -l};
            treemin[node] = {v[l], l}; 
            return;
        }
        ll mid = (l + r) / 2;
        build(2 * node, l, mid, v);
        build(2 * node + 1, mid + 1, r, v);
        treemax[node] = max(treemax[2 * node], treemax[2 * node + 1]);
        treemin[node] = min(treemin[2 * node], treemin[2 * node + 1]);
    }

    void update(ll node, ll l, ll r, ll idx) {
        if (l == r) {
            treemax[node] = {LLONG_MIN, -LLONG_MAX};
            treemin[node] = {LLONG_MAX, LLONG_MAX}; 
            return;
        }
        ll mid = (l + r) / 2;
        if (idx <= mid) {
            update(2 * node, l, mid, idx);
        } else {
            update(2 * node + 1, mid + 1, r, idx);
        }
        treemax[node] = max(treemax[2 * node], treemax[2 * node + 1]);
        treemin[node] = min(treemin[2 * node], treemin[2 * node + 1]);
    }

    pair<ll, ll> findmax(ll node, ll l, ll r, ll lq, ll rq) {
        if (l > rq || r < lq) return {LLONG_MIN, -LLONG_MAX};
        if (l >= lq && r <= rq) return treemax[node];
        ll mid = (l + r) / 2;
        return max(findmax(2 * node, l, mid, lq, rq), findmax(2 * node + 1, mid + 1, r, lq, rq));
    }

    pair<ll, ll> findmin(ll node, ll l, ll r, ll lq, ll rq) {
        if (l > rq || r < lq) return {LLONG_MAX, LLONG_MAX};
        if (l >= lq && r <= rq) return treemin[node];
        ll mid = (l + r) / 2;
        return min(findmin(2 * node, l, mid, lq, rq), findmin(2 * node + 1, mid + 1, r, lq, rq));
    }
};

void solve() {
    ll n;
    cin >> n;
    vector<ll> v(n);
    map<ll, vector<ll>> mp;
    
    for (ll i = 0; i < n; i++) {
        cin >> v[i];
        mp[v[i]].push_back(i);
    }
    
    set<ll> last_occurrences;
    for (auto& p : mp) {
        last_occurrences.insert(p.second.back());
    }
    
    segtree sg(n);
    sg.build(1, 0, n - 1, v);
    
    vector<ll> ans;
    ll st = 0, cnt = 1;
    
    while (!last_occurrences.empty()) {
        ll end = *last_occurrences.begin();
        pair<ll, ll> best;
        ll val, idx;
        
        if (cnt & 1) {
            best = sg.findmax(1, 0, n - 1, st, end);
            val = best.first;
            idx = -best.second;
        } else {
            best = sg.findmin(1, 0, n - 1, st, end);
            val = best.first;
            idx = best.second;
        }
        
        ans.push_back(val);
        last_occurrences.erase(mp[val].back());
        
        for (auto &j : mp[val]) {
            sg.update(1, 0, n - 1, j);
        }
        
        st = idx + 1;
        cnt++;
    }
    
    cout << ans.size() << "\n";
    for (ll i = 0; i < ans.size(); i++) {
        cout << ans[i] << " ";
    }
    cout << "\n";
}

int main() {
    ios_base::sync_with_stdio(false); 
    cin.tie(nullptr);
    ll t; 
    cin >> t;
    while (t--) {
        solve();
    }
    return 0;
}