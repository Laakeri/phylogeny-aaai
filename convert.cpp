#include <bits/stdc++.h>
using namespace std;

int main(int argc, char** argv){
  assert(argc == 2);
  if (string(argv[1]) == "tophy") {
    int n,m;
    cin>>n>>m;
    cout<<n<<" "<<m<<endl;
    vector<int> ss(m);
    for (int i=0;i<m;i++){
      cin>>ss[i];
      assert(ss[i]>0);
    }
    for (int i=0;i<n;i++){
      for (int ii=0;ii<m;ii++){
        int x;
        cin>>x;
        assert(x<ss[ii]);
        assert(x>=-1);
        if (x==-1){
          cout<<"? ";
        } else{
          cout<<x<<" ";
        }
      }
      cout<<endl;
    }
  } else if (string(argv[1]) == "toraw") {
    int n,m;
    cin>>n>>m;
    vector<vector<int>> mat(n);
    for (int i=0;i<n;i++){
      mat[i].resize(m);
      for (int ii=0;ii<m;ii++){
        string t;
        cin>>t;
        if (t=="?"){
          mat[i][ii]=-1;
        }else{
          mat[i][ii]=stoi(t);
        }
      }
    }
    vector<int> ss(m);
    for (int i=0;i<m;i++){
      int f=0;
      map<int, int> mx;
      for (int ii=0;ii<n;ii++){
        if (mat[ii][i]>=0){
          if (!mx.count(mat[ii][i])){
            mx[mat[ii][i]]=f++;
          }
          mat[ii][i]=mx[mat[ii][i]];
        }
      }
      ss[i]=f;
    }
    int h=0;
    for (int i=0;i<m;i++){
      if (ss[i]>0){
        h++;
      }
    }
    assert(h>0);
    cout<<n<<endl<<h<<endl;
    for (int i=0;i<m;i++){
      if (ss[i]>0){
        cout<<ss[i]<<" ";
      }
    }
    cout<<endl;
    for (int i=0;i<n;i++){
      for (int ii=0;ii<m;ii++){
        if (ss[ii]>0){
          cout<<mat[i][ii]<<" ";
        }
      }
      cout<<endl;
    }
  } else if(string(argv[1]) == "normal") {
    int n,m;
    cin>>n>>m;
    vector<vector<int>> mat(n);
    for (int i=0;i<n;i++){
      mat[i].resize(m);
      for (int ii=0;ii<m;ii++){
        string t;
        cin>>t;
        if (t=="?"){
          mat[i][ii]=-1;
        }else{
          mat[i][ii]=stoi(t);
        }
      }
    }
    vector<int> ss(m);
    for (int i=0;i<m;i++){
      int f=0;
      map<int, int> mx;
      for (int ii=0;ii<n;ii++){
        if (mat[ii][i]>=0){
          if (!mx.count(mat[ii][i])){
            mx[mat[ii][i]]=f++;
          }
          mat[ii][i]=mx[mat[ii][i]];
        }
      }
      ss[i]=f;
    }
    int h=0;
    for (int i=0;i<m;i++){
      if (ss[i]>1){
        h++;
      }
    }
    assert(h>0);
    cout<<n<<" "<<h<<endl;
    for (int i=0;i<n;i++){
      for (int ii=0;ii<m;ii++){
        if (ss[ii]>1){
          if (mat[i][ii]>=0) cout<<mat[i][ii]<<" ";
          else {
            assert(mat[i][ii]==-1);
            cout<<"? ";
          }
        }
      }
      cout<<endl;
    }
  } else {
    abort();
  }
}
