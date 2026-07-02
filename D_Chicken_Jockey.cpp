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
ll find(ll in,ll n,vector<ll>&v,vector<ll>&dp){
    if(in == 1) return v[1];
    if(in <= 0) return 0;

    if(dp[in] != -1) return dp[in];

    ll ans = LLONG_MAX;

    ans = max(0LL,v[in] - (in - 1)) + v[in - 1] + find(in - 2,n,v,dp);

    ans = min(ans,v[in]  - 1 + find(in - 1,n,v,dp));

    dp[in] = ans;
    return ans;

}
ll solve(){
    ll n;cin >> n;
    vector<ll>v(n + 1);
    loop1(1, n + 1) cin >> v[i];
    vector<ll>dp(n + 2,-1);
    ll ans = find(n,n,v,dp);
    return ans;
}
int main(){
    ios_base::sync_with_stdio(false); cin.tie(nullptr);
    ll t; cin >> t;
    while(t--){
        ll ans = solve();
        cout << ans << endl;
    }
    return 0;
}