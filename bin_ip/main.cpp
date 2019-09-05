#include <vector>
#include <iostream>
#include <cassert>
#include <set>
#include <map>

#include <ilcplex/ilocplex.h>

#include "../bt-maxsat/io.hpp"
#include "../bt-maxsat/phyl_mat.hpp"

#define F first
#define S second
using namespace triangulator;
using namespace std;

set<int> gametes(const vector<int>& a, const vector<int>& b) {
  assert(a.size() == b.size());
  int h1=0;
  int h2=0;
  int h3=0;
  int h4=0;
  for (int i=0;i<(int)a.size();i++){
    if (a[i]==0&&b[i]==0){
      h1=true;
    }
    if(a[i]==0&&b[i]==1){
      h2=true;
    }
    if(a[i]==1&&b[i]==0){
      h3=true;
    }
    if(a[i]==1&&b[i]==1){
      h4=true;
    }
  }
  set<int> r;
  if (h1) r.insert(0);
  if (h2) r.insert(1);
  if (h3) r.insert(2);
  if (h4) r.insert(3);
  return r;
}

vector<vector<int>> forbGamete(vector<vector<int>> binmat) {
  bool f=true;
  cout<<"applying forbGamete"<<endl;
  while (f){
    f=false;
    for (int i=0;i<(int)binmat.size();i++){
      for (int ii=i+1;ii<(int)binmat.size();ii++){
        auto gs = gametes(binmat[i], binmat[ii]);
        if (gs.size()==4) {
          cout<<"forbgamete unsat"<<endl;
          return {{-2}};
        }
        if (gs.size()<3) continue;
        int miss=0;
        for (int g=0;g<4;g++){
          if (gs.count(g)==0){
            miss=g;
          }
        }
        for (int j=0;j<(int)binmat[i].size();j++){
          if (miss==0){
            if (binmat[i][j]==0&&binmat[ii][j]==-1){
              binmat[ii][j]=1;
              f=true;
            }
            if (binmat[ii][j]==0&&binmat[i][j]==-1){
              binmat[i][j]=1;
              f=true;
            }
          } else if(miss==1){
            if (binmat[i][j]==0&&binmat[ii][j]==-1){
              binmat[ii][j]=0;
              f=true;
            }
            if (binmat[ii][j]==1&&binmat[i][j]==-1){
              binmat[i][j]=1;
              f=true;
            }
          } else if(miss==2){
            if (binmat[i][j]==1&&binmat[ii][j]==-1){
              binmat[ii][j]=1;
              f=true;
            }
            if (binmat[ii][j]==0&&binmat[i][j]==-1){
              binmat[i][j]=0;
              f=true;
            }
          } else if(miss==3){
            if (binmat[i][j]==1&&binmat[ii][j]==-1){
              binmat[ii][j]=0;
              f=true;
            }
            if (binmat[ii][j]==1&&binmat[i][j]==-1){
              binmat[i][j]=0;
              f=true;
            }
          } else{
            assert(0);
          }
        }
      }
    }
    if (f) {
      cout<<"found fg"<<endl;
    }
  }
  return binmat;
}

vector<vector<int>> charNest(vector<vector<int>> binmat) {
  for (int i=0;i<(int)binmat.size();i++){
    for (int ii=i+1;ii<(int)binmat.size();ii++){
      bool ok=true;
      for (int j=0;j<(int)binmat[i].size();j++){
        if (binmat[i][j] != binmat[ii][j] && binmat[i][j] != -1) {
          ok = false;
          break;
        }
      }
      if (ok) {
        cout<<"found charnest"<<endl;
        swap(binmat[i], binmat.back());
        binmat.pop_back();
        return charNest(binmat);
      }
    }
  }
  return binmat;
}

bool solveMD(vector<vector<int>> binmat) {
  vector<vector<int>> bad = {{-2}};
  binmat = forbGamete(binmat);
  if (binmat == bad) return false;

  binmat = charNest(binmat);

  IloEnv env;
  IloModel model(env);
  IloCplex cplex(model);
  cplex.setParam(IloCplex::Threads, 1);

  vector<vector<int>> yvar_id(binmat.size());
  IloNumVarArray Y_vars(env);
  int ycs = 0;

  for (int i=0;i<(int)binmat.size();i++){
    yvar_id[i].resize(binmat[i].size());
    for (int j=0;j<(int)binmat[i].size();j++){
      Y_vars.add(IloNumVar(env, 0, 1, ILOBOOL));
      yvar_id[i][j] = ycs++;
      if (binmat[i][j] == 0) {
        model.add(Y_vars[yvar_id[i][j]] <= 0);
      } else if (binmat[i][j] == 1) {
        model.add(Y_vars[yvar_id[i][j]] >= 1);
      }
    }
  }

  IloExpr obj(env);
  IloNumVarArray tcvs(env);
  int tcs=0;

  for (int i=0;i<(int)binmat.size();i++){
    for (int ii=i+1;ii<(int)binmat.size();ii++){
      IloNumVar tc(env, 0, 1, ILOBOOL);
      obj += tc;

      tcvs.add(tc);
      tcs++;

      auto gs = gametes(binmat[i], binmat[ii]);
      assert(gs.size() < 4);

      IloExpr bexp(env);
      for (int g=0;g<4;g++){
        if (gs.count(g) == 0) {
          IloNumVar bv(env, 0, 1, ILOBOOL);
          bexp += bv;

          for (int j=0;j<(int)binmat[i].size();j++){
            if (binmat[i][j]==-1 || binmat[ii][j]==-1){
              if (g==0){
                model.add(bv >= -Y_vars[yvar_id[i][j]] -Y_vars[yvar_id[ii][j]] + 1);
              } else if(g==1){
                model.add(bv >= -Y_vars[yvar_id[i][j]] +Y_vars[yvar_id[ii][j]]);
              } else if(g==2){
                model.add(bv >= Y_vars[yvar_id[i][j]] -Y_vars[yvar_id[ii][j]]);
              } else {
                model.add(bv >= Y_vars[yvar_id[i][j]] +Y_vars[yvar_id[ii][j]] -1);              }
            }
          }
        }
      }
      model.add((int)gs.size() + bexp <= 3 + tc);
    }
  }

  model.add(IloMinimize(env, obj));
  cplex.solve();
  assert(cplex.getStatus() == IloAlgorithm::Optimal);
  IloNumArray vals(env);
  cplex.getValues(vals, tcvs);

  for (int i=0;i<tcs;i++){
    if (vals[i] > 0.5){
      return false;
    }
  }
  return true;
}

vector<int> trySolveCr(vector<vector<int>> binmat, vector<vector<int>> childs, int th) {
  IloEnv env;
  IloModel model(env);
  IloCplex cplex(model);
  cplex.setParam(IloCplex::Threads, 1);

  IloNumVarArray D_vars(env);
  IloNumVarArray G_vars(env);

  for (int i=0;i<binmat.size();i++){
    D_vars.add(IloNumVar(env, 0, 1, ILOBOOL));
  }
  IloExpr obj(env);
  for (int i=0;i<(int)childs.size();i++){
    G_vars.add(IloNumVar(env, 0, 1, ILOBOOL));
    obj += G_vars[i];
    IloExpr chc(env);
    for (int x : childs[i]){
      chc += D_vars[x];
    }
    model.add((int)childs[i].size() * G_vars[i] >= chc);
  }
  model.add(IloMinimize(env, obj));

  vector<vector<int>> yvar_id(binmat.size());
  IloNumVarArray Y_vars(env);
  int ycs=0;
  for (int i=0;i<(int)binmat.size();i++){
    yvar_id[i].resize(binmat[i].size());
    for (int j=0;j<(int)binmat[i].size();j++){
      Y_vars.add(IloNumVar(env, 0, 1, ILOBOOL));
      yvar_id[i][j] = ycs++;
      if (binmat[i][j] == 0) {
        model.add(Y_vars[yvar_id[i][j]] <= 0);
      } else if (binmat[i][j] == 1) {
        model.add(Y_vars[yvar_id[i][j]] >= 1);
      }
    }
  }

  for (int i=0;i<(int)binmat.size();i++){
    for (int ii=i+1;ii<(int)binmat.size();ii++){
      auto gs = gametes(binmat[i], binmat[ii]);
      if ((int)gs.size() >= th) {
        if (gs.size() == 4) {
          model.add(D_vars[i] + D_vars[ii] >= 1);
        } else {
          IloNumVar tc(env, 0, 1, ILOBOOL);

          IloExpr bexp(env);
          for (int g=0;g<4;g++){
            if (gs.count(g) == 0) {
              IloNumVar bv(env, 0, 1, ILOBOOL);
              bexp += bv;

              for (int j=0;j<(int)binmat[i].size();j++){
                if (binmat[i][j]==-1 || binmat[ii][j]==-1){
                  if (g==0){
                    model.add(bv >= -Y_vars[yvar_id[i][j]] -Y_vars[yvar_id[ii][j]] + 1);
                  } else if(g==1){
                    model.add(bv >= -Y_vars[yvar_id[i][j]] +Y_vars[yvar_id[ii][j]]);
                  } else if(g==2){
                    model.add(bv >= Y_vars[yvar_id[i][j]] -Y_vars[yvar_id[ii][j]]);
                  } else {
                    model.add(bv >= Y_vars[yvar_id[i][j]] +Y_vars[yvar_id[ii][j]] -1);              }
                }
              }
            }
          }
          model.add((int)gs.size() + bexp <= 3 + tc);
          model.add(D_vars[i] + D_vars[ii] - tc >= 0);
        }
      }
    }
  }
  cplex.solve();
  assert(cplex.getStatus() == IloAlgorithm::Optimal);
  IloNumArray vals(env);
  cplex.getValues(vals, G_vars);
  vector<int> sol;
  vector<vector<int>> binmatver;
  for (int i=0;i<(int)childs.size();i++){
    if (vals[i] > 0.5) {
      sol.push_back(i);
    } else {
      for (int x : childs[i]) {
        binmatver.push_back(binmat[x]);
      }
    }
  }
  cout<<"solving MD "<<sol.size()<<endl;
  if (solveMD(binmatver)) {
    cout<<"good MD"<<endl;
    return sol;
  } else {
    cout<<"bad MD"<<endl;
    return {-1};
  }
}

int main(){
  Io io;
  PhylMat pm = io.ReadPhylMat(std::cin);

  vector<map<int, int>> ss(pm.m());
  for (int j=0;j<pm.m();j++){
    for (int i=0;i<pm.n();i++){
      if (pm.GetMat(i, j)>=0){
        ss[j][pm.GetMat(i, j)]++;
      }
    }
  }
  vector<vector<int>> binmat;
  vector<vector<int>> childs(pm.m());
  for (int j=0;j<pm.m();j++){
    for (auto s1:ss[j]){
      for (auto s2:ss[j]){
        if (s1.F < s2.F && s1.S >= 2 && s2.S >= 2) {
          childs[j].push_back(binmat.size());
          binmat.push_back(vector<int>(pm.n()));
          for (int i=0;i<pm.n();i++){
            if (pm.GetMat(i,j)==s1.F){
              binmat.back()[i]=0;
            } else if(pm.GetMat(i,j)==s2.F){
              binmat.back()[i]=1;
            } else {
              binmat.back()[i]=-1;
            }
          }
        }
      }
    }
  }

  for (int q=4;q>=0;q--){
    auto sol=trySolveCr(binmat, childs, q);
    vector<int> bad = {-1};
    if (sol != bad){
      cout<<"sol "<<sol.size()<<endl;
      for (int x : sol){
        cout<<x<<" ";
      }
      cout<<endl;
      break;
    }
  }
}