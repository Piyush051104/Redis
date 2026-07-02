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
ll solve(){
    ll n;cin >> n;
    vector<ll>v(n);
    loop(i,n) cin >> v[i];

    vector<ll>needleft(n);
    vector<ll>needright(n);
    for(ll i = 0 ;i < n;i++){
        needleft[i] = v[i] + i;
        needright[i] = v[i] + (n - 1 - i);
    }

    vector<ll>suffix(n);
    suffix[n - 1] = needleft[n - 1]; 
    for(ll i = n - 2 ; i >= 0;i--){
        suffix[i] = max(suffix[i + 1],needleft[i]);
    }

    ll maxi = LLONG_MIN;

    ll ans = LLONG_MAX;
    for(ll i = 0 ;i < n;i++){
        
        ll val = v[i];
        val = max(val,maxi);
        if(i + 1 <= n - 1){
            val = max(val,suffix[i + 1]);
        }
        ans = min(ans,val);
        maxi = max(maxi,needright[i]);
    }

    return ans;
}
int main(){
    ios_base::sync_with_stdio(false); cin.tie(nullptr);
    ll t; t = 1;
    while(t--){
        ll ans = solve();
        cout << ans << endl;
    }
    return 0;
}