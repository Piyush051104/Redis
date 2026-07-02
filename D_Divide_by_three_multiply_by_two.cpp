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
#define sum_vector(v) accumulate(v.begin(),v.end(),0LL)
#define min_value(v) *min_element(v.begin(),v.end())
#define max_value(v) *max_element(v.begin(),v.end())
ll powermod(ll x, ll y, ll p){ll res = 1;x = x % p;if (x == 0) return 0;while (y > 0){if (y & 1)res = (res*x) % p;y = y>>1;x = (x*x) % p;}return res;}
// clang-format off
template<typename T, typename V> bool ckmax(T &a, V b) { return (long long) a < (long long) b ? a = b, 1: 0; }
template<typename T, typename V> bool ckmin(T &a, V b) { return (long long) a > (long long) b ? a = b, 1: 0; }
template <typename T> ostream &operator<<(ostream &out, const vector<T> &v) { for (const auto &x : v) out << x << ' '; return out; }
template <typename T> istream &operator>>(istream &in, vector<T> &v) { for (auto &x : v) in >> x; return in; }
bool dfs(ll node,vector<ll>&ans,map<ll,ll>&mp,ll n){
    if(ans.size() == n) return true;
    if(node % 3 == 0 && mp.find(node/3) != mp.end() && mp[node/3] > 0){
        mp[node/3]--;
        ans.push_back(node/3);
        bool f1 = dfs(node/3,ans,mp,n);
        if(f1) return true;
        mp[node/3]++;
        ans.pop_back();
    }
    if(mp.find(node*2) != mp.end() && mp[node * 2] > 0){
        mp[node * 2]--;
        ans.push_back(node * 2);
        bool f2 = dfs(node * 2,ans,mp,n);
        if(f2) return true;
        mp[node * 2]++;
        ans.pop_back();
    }

    return false;
}
void solve(){
    ll n;cin >> n;
    vector<ll>v(n);
    map<ll,ll>mp;
    loop(i,n){
        cin >> v[i];
        mp[v[i]]++;
    }

    for(ll i = 0 ;i < n;i++){
        map<ll,ll>temp = mp;
        vector<ll>ans;
        ans.push_back(v[i]);
        ll val = v[i];
        bool find = dfs(v[i],ans,mp,n);
        if(find){
            for(auto &j:ans){
                cout << j << " ";
            }
            return;
        } 
        
    }



}
int main(){
    ios_base::sync_with_stdio(false); cin.tie(nullptr);
    ll t; t = 1;
    while(t--){
        solve();
    }
    return 0;
}