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
ll dfs(ll node,ll par,ll &cnt,ll val,vector<vector<ll>>&graph){
    ll maxi = 0;
   
    for(auto &child:graph[node]){
        if(child != par){
            maxi = max(maxi,dfs(child,node,cnt,val,graph));
        }
    }

    if(maxi >= val - 1){
        cnt++;
        return 0;
    }

    else{
        return 1 + maxi;
    }
}
bool check(ll val,ll n,ll k,vector<vector<ll>>&graph){

    ll ans = 0;
    dfs(1,-1,ans,val,graph);

    return ans <= k;

}
ll solve(){
    ll n,k;cin >> n >> k;
    vector<ll>v(n + 1);
    v[1] = -1;
    vector<vector<ll>>graph(n + 1);
    for(ll i = 2 ;i <= n;i++){
        ll a;cin >> a;
        v[i] = a;
        graph[a].push_back(i);
        graph[i].push_back(a);
    }

    ll st = 1, end = n,ans = end;
    while(st <= end){
        ll mid = (st + end)/2;

        bool flag = check(mid,n,k,graph);

        if(flag){
            ans = mid;
            end = mid  - 1;
        }
        else{
            st = mid + 1;
        }
    }
    return ans - 1;


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