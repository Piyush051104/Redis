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
ll dfs(ll node,ll par,ll n,ll &ans,vector<ll>&v,vector<vector<ll>>&graph){

    vector<ll>temp;
    ll add = 0;
    for(auto child:graph[node]){
        if(child != par){
            ll cnt = dfs(child,node,n,ans,v,graph);
            add += cnt;
            temp.push_back(cnt);
        }
    }

    temp.push_back(n - add - 1);
    ll ro = sqrt(v[node]);
    if(ro * ro != v[node]) return add + 1;
    ll sum = 0;

    ll pair = 0;
    ll trip = 0;
    for(auto i:temp){
        trip += (pair * i);
        pair += (sum * i);
        sum += i;
    }
    ans += pair;
    ans += trip;

    return add + 1;
    
}
ll solve(){
    ll n;cin >> n;
    vector<ll>v(n + 1);
    loop1(1,n + 1) cin >> v[i];
    vector<vector<ll>>graph(n + 1);
    loop(i,n - 1){
        ll a,b;cin >> a >> b;
        graph[a].push_back(b);
        graph[b].push_back(a);
    }

    ll ans = 0;
    dfs(1,-1,n,ans,v,graph);
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