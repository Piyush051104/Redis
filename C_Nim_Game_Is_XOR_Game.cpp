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
    ll cnt = 0;
    ll xorval = 0;
    loop(i,n){
        cin >> v[i];
        xorval = xorval ^ v[i];
        if(v[i] > 0) cnt++;
    }
    if(cnt <= 1) return 0;

    if(xorval == 0) return 1;
    sort(all(v));
    
    vector<ll>freq(31,0);
    
    for(ll i = 0 ;i < n;i++){
        ll val = v[i];
        ll st = 30;
        while(val > 0){
            ll last = val & 1;
            if(last == 1) freq[st]++;
            st--;
            val = val >> 1;
        }
    }
   
    ll ans = 0;
    for(ll i = 0 ;i < n;i++){
        ll val = v[i];
        ll st = 30;
        ll i1 = 32,i2 = 32;
        for(st = 30; st >= 0;st--){
            
            if(freq[st] % 2 == 0){
            
                val = val >> 1;
            }
            else{
                
                if(val % 2 == 0){
                    i1 = st;   
                }
                else{
                    i2 = st;
                }
                val = val >> 1;
            }
        }
        //if(i == 0) cout << i2 << " " << i1 <<" ";
        if(i2 < i1) ans++;
    }
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