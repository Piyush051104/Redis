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
bool mincheck(ll val,ll n,vector<ll>&v){
    ll extra = 0;
    for(ll i = n - 1 ; i >= 1;i--){
        if(v[i] + extra < val){
            ll need = val  - (v[i] + extra);
            extra = -need;
        }
        else{
            extra = 0;
        }
    }

    return v[0] + extra >= val;
}
bool maxcheck(ll val,ll n,vector<ll>&v){
    ll extra = 0;
    for(ll i = 0 ;i < n - 1;i++){
        if(v[i] + extra > val){
            ll need = v[i] + extra - val;
            extra = need;
        }
        else extra = 0;
    }
    return v[n - 1] + extra <= val;
}
ll solve(){
    ll n;cin >> n;
    vector<ll>v(n);
    ll maxi = LLONG_MIN;
    ll mini = LLONG_MAX;
    loop(i,n){
        cin >> v[i];
        mini = min(mini,v[i]);
        maxi = max(maxi,v[i]);
    }

    ll st = mini ,end = maxi,minmaxpos = st;
    while(st <= end){
        ll mid = (st + end)/2;

        bool flag = mincheck(mid,n,v);
        if(flag){
            minmaxpos = mid;
            st = mid + 1;
        }
        else end = mid - 1;

    }

    st = mini,end = maxi;
    ll maximinpos = end;
    while(st <= end){
        ll mid = (st + end)/2;
        bool flag = maxcheck(mid,n,v);

        if(flag){
            maximinpos = mid;
            end = mid - 1;
        }
        else{
            st = mid + 1;
        }
    }

    return maximinpos - minmaxpos;



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