#include <bits/stdc++.h>
#define F first
#define S second
using namespace std;

int main(int argc, char** argv){
  if (argc != 6) {
    cout<<"args: n, m, k, r, miss"<<endl;
    return 0;
  }
  int n = stoi(argv[1]);
  int m = stoi(argv[2]);
  int k = stoi(argv[3]);
  double r = stod(argv[4]);
  double miss = stod(argv[5]);
  assert(n>1&&n<=10000);
  assert(m>1&&m<=10000);
  assert(k>1&&k<=10000);
  assert(r>=0&&r<=100);
  assert(miss>=0&&miss<=1);
  stringstream ss;
  ss<<"./msdir/ms "<<n<<" 1 -s "<<m*(k-1)<<" -r "<<r<<" 5000 > phyltemp";
  assert(system(ss.str().c_str())==0);
  ifstream in("phyltemp");
  string tmp;
  getline(in, tmp);
  getline(in, tmp);
  getline(in, tmp);
  getline(in, tmp);
  assert(tmp == "//");
  getline(in, tmp);
  stringstream tss(tmp);
  tss>>tmp;
  assert(tmp == "segsites:");
  tss>>tmp;
  assert(tmp == to_string(m*(k-1)));
  getline(in, tmp);
  vector<string> data(n);
  for (int i=0;i<n;i++){
    getline(in, data[i]);
    assert((int)data[i].size() == m*(k-1));
    for (char c:data[i]){
      assert(c=='0' || c=='1');
    }
  }
  in.close();
  cout<<n<<" "<<m<<endl;
  std::uniform_real_distribution<double> ur(0, 1);
  std::uniform_int_distribution<int> ui(0, k-1);
  std::mt19937 re(time(0));
  vector<map<string, int>> mm(m);
  vector<int> ass(m);
  for (int i=0;i<n;i++){
    for (int j=0;j<m;j++){
      string s;
      for (int x=0;x<k-1;x++){
        s+=data[i][j*(k-1)+x];
      }
      assert((int)s.size() == k-1);
      int t=-1;
      if (k==2){
        if (s[0]=='0') t=0;
        else t=1;
      } else{
        if (mm[j].count(s)){
          t=mm[j][s];
        } else if(ass[j]<k){
          t=ass[j];
          mm[j][s]=t;
          ass[j]++;
        } else{
          t=ui(re);
          mm[j][s]=t;
          assert(r>0.0001);
        }
      }
      assert(t>=0&&t<k);
      if (ur(re)<miss) {
        cout<<"?";
      } else {
        cout<<t;
      }
      if (j+1<m) cout<<" ";
    }
    cout<<endl;
  }
}