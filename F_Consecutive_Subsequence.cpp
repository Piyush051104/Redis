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
void solve(){
    ll n;cin >> n;
    vector<ll>v(n);
    loop(i,n) cin >> v[i];

    map<ll,ll>mp;
    vector<ll>cnt(n,1);
    for(ll i = 0;i < n;i++){
        ll val = v[i];
        if(mp.find(v[i] - 1) != mp.end()){
            ll a = mp[v[i] - 1];
            mp[v[i]] = a + 1;
            cnt[i] = a + 1;
        }
        else mp[v[i]] = 1;
    }

    ll ans = 1;
    ll val = v[0];
    for(ll i = 0 ;i < n;i++){
        if(cnt[i] > ans){
            ans = cnt[i];
            val = v[i];
        }
    }
    ll start = val - ans + 1;
    cout << ans << endl;
    for(ll i = 0 ;i < n;i++){
        if(v[i] == start){
            cout << i + 1  << " ";
            start++;
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