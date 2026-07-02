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
bool solve(){
    ll n;cin >> n;string s; cin >> s;
    vector<ll>v(n + 1,0);
    for(ll i = 1 ;i <= n;i++){
        v[i] = s[i - 1] - '0';
    }

    ll sum = 0;
    ll onecnt = 0;
    for(ll i = 1; i <= n;i++){  
        if(v[i] == 1) onecnt++;
        else{
            sum += onecnt;
        }
    }

    if(sum & 1) return true;

    vector<ll>prefione(n + 1,0);
    vector<ll>suffixzero(n + 1,0);
    onecnt = 0;
    for(ll i = 1 ;i <= n;i++){
        prefione[i] = onecnt;
        if(v[i] == 1) onecnt++;
    }
    ll zerocnt = 0;
    for(ll i = n ; i>= 0;i--){
        if(v[i] == 0) zerocnt++;
        suffixzero[i] = zerocnt;
    }

    bool flag = false;
    vector<bool>check(n + 1,false);
    for(ll i = n ;i >= 0;i--){
        if(v[i] == 1 && (suffixzero[i] % 2 != 0) ){
            flag = true;
        }
        check[i] = flag;
    }

    for(ll i = 1; i <= n;i++){
        ll cnt = prefione[i] * suffixzero[i];
        if(cnt & 1) return true;

        if(check[i]) return true;
    }
    return false;


}
int main(){
    ios_base::sync_with_stdio(false); cin.tie(nullptr);
    ll t; cin >> t;
    while(t--){
        bool ans = solve();
        if(ans){
            cout << "Alice" << endl;
        }
        else{
            cout << "Bob" << endl;
        }
    }
    return 0;
}