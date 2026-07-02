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
bool check(ll val,vector<ll>&v){
    ll n = v.size();
    n--;
    vector<ll>dp(n + 2);
    dp[0] = 0;

    set<pair<ll,ll>>st;
    st.insert({0,0});
    ll sum = 0;
    ll end = 0;

    for(ll i = 1 ; i <= n;i++){
        sum += v[i - 1];
        while(sum > val){
            
            st.erase({dp[end],end});
            end++;
            sum -= v[end];
        }

        if(st.empty()){
            dp[i] = LLONG_MAX;
        }
        else{
            auto it = st.begin();
            ll val1 = it -> first;
            val1 += v[i];
            dp[i] = val1;
            st.insert({val1,i});
        }
        
    }

    bool fl = false;
    sum = 0;
    for(ll i = n;i >= 1;i--){
        
        if(sum <= val && dp[i] <= val) return true;
        sum += v[i];
    }
    return false;
    


}
ll solve(){
    ll n;cin >> n;
    vector<ll>v(n + 1);
    v[0] = 0;
    
    ll s = 0;
    loop1(1,n + 1){
        cin >> v[i];
        s += v[i];
    }

    ll st = 0,end = s,ans = end;
    while(st <= end){
        ll mid = (st + end)/2;

        bool flag = check(mid,v);
        if(flag){
            ans = mid;
            end = mid - 1;
        }
        else{
            st = mid + 1;
        }
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