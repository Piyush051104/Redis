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
void solve(){
    ll n,k,q;cin >> n >> k >> q;
    vector<ll>v(n + 1);
    loop1(1,n + 1) cin >> v[i];
    for(ll i = 1 ;i <= n;i++){
        v[i] -= i;
    }
    vector<ll>freq(n + 1);
    multiset<ll>ms;
    map<ll,ll>mp;
    for(ll i = 1 ;i <= k;i++){
        if(mp.find(v[i]) == mp.end()){
            mp[v[i]]++;
            ms.insert(1);
        }
        else{
            ll cnt = mp[v[i]];
            auto it = ms.find(cnt);
            ms.erase(it);
            mp[v[i]]++;
            ms.insert(cnt + 1);
        }
    }
    auto it = prev(ms.end());
    freq[k] = *it;

    for(ll i = k + 1 ;i <= n;i++){
        ll prevcnt = mp[v[i - k]];
        auto ter = ms.find(prevcnt);
        ms.erase(ter);
        if(prevcnt - 1 > 0) ms.insert(prevcnt - 1);
        mp[v[i - k]]--;
        if(mp[v[i - k]] == 0) mp.erase(v[i - k]);

        if(mp.find(v[i]) == mp.end()){
            mp[v[i]]++;
            ms.insert(1);
        }
        else{
            ll cnt = mp[v[i]];
            auto er = ms.find(cnt);
            ms.erase(er);
            mp[v[i]]++;
            ms.insert(cnt + 1);
        }

        auto it1 = prev(ms.end());
        freq[i] = *it1;
    }

    for(ll i = 0 ;i < q;i++){
        ll l,r;cin >> l >> r;
        cout << k - freq[r] << endl;

    }
    return;



}
int main(){
    ios_base::sync_with_stdio(false); cin.tie(nullptr);
    ll t; cin >> t;
    while(t--){
        solve();
    }
    return 0;
}