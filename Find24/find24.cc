#include <string>
#include <list>
#include <set>
#include <map>
#include <vector>
#include <iostream>
#include <assert.h>

class Rational {
public:
  Rational(int dividend, int divisor=1) : 
    dividend_(dividend), 
    divisor_(divisor)
  {
    assert(divisor != 0);
    normalize();
  }

  Rational operator + (const Rational& other) const
  {
    return Rational(
      dividend_*other.divisor_+other.dividend_*divisor_, 
      divisor_*other.divisor_);
  }

  Rational operator - (const Rational& other) const 
  {
    return Rational(
      dividend_*other.divisor_-other.dividend_*divisor_, 
      divisor_*other.divisor_);
  }

  Rational operator * (const Rational& other) const 
  {
    return Rational(dividend_*other.dividend_, divisor_*other.divisor_);
  }

  Rational operator / (const Rational& other) const 
  {
    return Rational(dividend_*other.divisor_, divisor_*other.dividend_);
  }

  bool operator == (const Rational& other) const
  {
    return (dividend_ == other.dividend_) && (divisor_ == other.divisor_);
  }

  bool operator < (const Rational& other) const
  {
    return (dividend_*other.divisor_-other.dividend_*divisor_ < 0);
  }

  int cmp(const Rational& other) const
  {
    return (dividend_*other.divisor_-other.dividend_*divisor_);
  }

  std::string toString() const {
    if (dividend_ == 0) return "0";
    std::string ret=std::to_string(dividend_);
    if (divisor_ != 1) {
      ret+="/";
      ret+=std::to_string(divisor_);
    }

    return ret;
  }

private:
  int dividend_;
  int divisor_;
  static int gdc(int left, int right)  {
    assert(right != 0);
    // making sure both parameters are non-negative
    if (left < 0) left=-left;
    if (right < 0) right=-right;

    if (left == 0) return right;

    // both left and right are positive numbers
    if (left < right) std::swap(left, right);

    while ( (left % right) != 0) {
      int tmp=left%right;
      left=right;
      right=tmp;
    }
    return right;
  }

  void normalize() {
    int x=gdc(dividend_, divisor_);
    dividend_/=x;
    divisor_/=x;
    // make sure divisor is always positive
    if (divisor_ < 0) {
      divisor_=-divisor_;
      dividend_=-dividend_;
    }
  }
};

enum ETYPE { LITERAL, ADDSUB, MULDIV };

class Expr {
public:
  virtual int cmp(const Expr& other) const = 0;
  virtual std::string toString(bool embed) const = 0;
  virtual Rational eval() const = 0;
  virtual ETYPE getType() const = 0;
  virtual ~Expr() { }
};

static int cmpExpr(const Expr* left, const Expr* right) {
  ETYPE ltype=left->getType();
  ETYPE rtype=right->getType();
  if (ltype != rtype) return ltype - rtype;
  return left->cmp(*right);
}

typedef std::list<const Expr*> ExprList;

static int compareExprList(const ExprList& left, const ExprList& right)
{
    auto it1=left.cbegin();
    auto it2=right.cbegin();
    while ( (it1 != left.cend()) && (it2 != right.cend()) ) {
      int ret=cmpExpr(*it1, *it2);
      if (ret != 0) return ret;
      ++it1;
      ++it2;
    }
    if (it1 != left.cend()) return 1;
    if (it2 != right.cend()) return -1;

    return 0;
}

static void addToList(ExprList& list, const Expr* expr) {
  auto it=list.begin();
  while ((it != list.end()) && (cmpExpr(*it, expr)<0))
    ++it;
  list.insert(it, expr);
}

// Assuming both lists are already sorted
static void mergeList(ExprList& to, const ExprList& from) {
  auto it_to=to.begin();
  for (auto& expr : from) {
    while ((it_to != to.end()) && (cmpExpr(*it_to, expr)<0))
      ++it_to;
    to.insert(it_to, expr);
  }
}

class Literal : public Expr {
public:
  Literal(int val) : value_(val) { }

  int cmp(const Expr& other) const {
    const Literal* expr=dynamic_cast<const Literal*>(&other);
    return value_ - expr->value_;
  }

  std::string toString(bool embed) const {
    return std::to_string(value_);
  }

  Rational eval() const {
    return Rational(value_);
  }

  ETYPE getType() const {
    return LITERAL;
  }

private:
  int value_;
};

class AddSub : public Expr {
public:
  AddSub(const Expr* left, const Expr* right, bool isSub) {
    if (left->getType() == ADDSUB) {
      const AddSub* expr=dynamic_cast<const AddSub*>(left);
      add_list_ = expr->add_list_;
      sub_list_ = expr->sub_list_;
    } else {
      add_list_.push_back(left);
    }

    if (right->getType() == ADDSUB) {
      const AddSub* expr=dynamic_cast<const AddSub*>(right);
      if (isSub) {
        mergeList(add_list_, expr->sub_list_);
        mergeList(sub_list_, expr->add_list_);
      } else {
        mergeList(add_list_, expr->add_list_);
        mergeList(sub_list_, expr->sub_list_);
      }
    } else {
      if (isSub) {
        addToList(sub_list_, right);
      } else {
        addToList(add_list_, right);
      }
    }
  }

  int cmp(const Expr& other) const {
    const AddSub* expr=dynamic_cast<const AddSub*>(&other);
    int ret=compareExprList(add_list_, expr->add_list_);
    if (ret != 0) return ret;
    return compareExprList(sub_list_, expr->sub_list_);
  }

  std::string toString(bool embed) const {
    auto it=add_list_.cbegin();
    assert(it != add_list_.cend());
    std::string ret;
    if (embed) {
      ret+="(";
    }
    ret+=(*it)->toString(true);

    while (++it != add_list_.cend()) {
      ret+="+";
      ret+=(*it)->toString(true);
    }

    for (auto& expr : sub_list_) {
      ret+="-";
      ret+=expr->toString(true);
    }

    if (embed) {
      ret+=")";
    }

    return ret;
  }

  Rational eval() const {
    Rational ret(0);

    for (auto& expr : add_list_) {
      ret=ret+expr->eval();
    }

    for (auto& expr : sub_list_) {
      ret=ret-expr->eval();
    }

    return ret;
  }

  ETYPE getType() const { return ADDSUB; }

private:
  ExprList add_list_;
  ExprList sub_list_;

};

class MulDiv : public Expr {
public:
  MulDiv(const Expr* left, const Expr* right, bool isDiv) {
    if (left->getType() == MULDIV) {
      const MulDiv* expr=dynamic_cast<const MulDiv*>(left);
      mul_list_ = expr->mul_list_;
      div_list_ = expr->div_list_;
    } else {
      mul_list_.push_back(left);
    }

    if (right->getType() == MULDIV) {
      const MulDiv* expr=dynamic_cast<const MulDiv*>(right);
      if (isDiv) {
        mergeList(mul_list_, expr->div_list_);
        mergeList(div_list_, expr->mul_list_);
      } else {
        mergeList(mul_list_, expr->mul_list_);
        mergeList(div_list_, expr->div_list_);
      }
    } else {
      if (isDiv) {
        addToList(div_list_, right);
      } else {
        addToList(mul_list_, right);
      }
    }
  }

  int cmp(const Expr& other) const {
    const MulDiv* expr=dynamic_cast<const MulDiv*>(&other);
    int ret=compareExprList(mul_list_, expr->mul_list_);
    if (ret != 0) return ret;
    return compareExprList(div_list_, expr->div_list_);
  }

  std::string toString(bool embed) const {
    auto it=mul_list_.cbegin();
    assert(it != mul_list_.cend());
    std::string ret=(*it)->toString(true);
    while (++it != mul_list_.cend()) {
      ret+="*";
      ret+=(*it)->toString(true);
    }
    for (auto& expr : div_list_) {
      ret+="/";
      ret+=expr->toString(true);
    }

    return ret;
  }

  Rational eval() const {
    Rational ret(1);
    for (auto& expr : mul_list_) {
      ret=ret*expr->eval();
    }

    for (auto& expr : div_list_) {
      ret=ret/expr->eval();
    }

    return ret;
  }

  ETYPE getType() const { return MULDIV; }

private:
  ExprList mul_list_;
  ExprList div_list_;
};

class ExprCmp {
public:
  bool operator () (const Expr* left, const Expr* right) const {
    return cmpExpr(left, right) < 0;
  }
};

// elems must be sorted in ascending order, could have duplicated elems
typedef std::vector<int> NumVec; 

template<typename Op>
void selectK(int n, int k, Op op)
{
  int selection[k];
  int index=0;
  selection[0]=-1;
  while (index>=0) {
    ++selection[index];
    int elemNeeded=k-index-1;
    if (selection[index] + elemNeeded >= n) {
      --index;
      continue;
    }

    for (int i=index+1; i<k; ++i) {
      selection[i]=selection[i-1]+1;
    }
    index=k-1;

    do {
      op(selection, k);
      ++selection[index];
    } while (selection[index] < n);
    --index;
  }
}

void splitVec(const NumVec& from, int sel[], int k, NumVec& s1, NumVec& s2)
{
  int index=0;
  for (int i=0; i<from.size(); ++i) {
    if (index>=k) {
      s2.push_back(from[i]);
    } else if (i<sel[index]) {
      s2.push_back(from[i]);
    } else { // i == sel[index]
      s1.push_back(from[i]);
      ++index;
    }
  }
}

typedef std::set<Expr*, ExprCmp> ExprSet;
typedef std::map<Rational, ExprSet> ValExprMap;
typedef std::map<NumVec, ValExprMap> SolutionMap;

// Although the name comes from the game find-24, this class is a general 
// solution that can find arithmatic expressions that would yield a specific
// target number (other than 24) using any number of positive integers 
// (instead of 4 integers between 1 to 13).
// It will find all possible solutions, but will not show duplicates under
// commutative or associative laws.
class Find24 {
public:
  Find24(int target, std::vector<int> elems) : 
    target_(target), elems_(elems)
  {
    std::sort(elems_.begin(), elems_.end());
  }

  void run(bool debug) {
    buildSolutionMap();
    auto it = solution_.find(elems_);
    if (it == solution_.end()) {
      std::cerr << "Oops, something is wrong!" << std::endl;
      return;
    }
    auto it2 = it->second.find(Rational(target_));
    if (it2 == it->second.end()) {
      std::cerr << "Oops, no solution found!" << std::endl;
      return;
    }

    for (auto& expr : it2->second) {
      std::cout << expr->toString(false) << "=" << expr->eval().toString() << std::endl;
    }

    std::cout << "Found " << it2->second.size() << " solutions" << std::endl;

    if (debug) {
      std::cout << "counters: loops=" << counters_.loops << ", exprs=" << 
        counters_.exprs << std::endl;
    }
  }

private:
  int target_;
  NumVec elems_;
  SolutionMap solution_;

  struct Counters {
    int loops;
    int exprs;
    Counters() : loops(0), exprs(0) { }
  } counters_;

  void addLiterals()
  {
    for (auto& elem : elems_) {
      NumVec key;
      key.push_back(elem);
      if (solution_.find(key) == solution_.end()) {
        // avoid duplicated literals
        ExprSet exprs;
        exprs.insert(new Literal(elem));
        ValExprMap value;
        value.insert(make_pair(Rational(elem), exprs));
        solution_.insert(make_pair(key, value));
      }
    }
  }

  class ValueBuilder {
  public:
    ValueBuilder(const NumVec& key, ValExprMap& value, SolutionMap& solution, 
      Counters& counters) 
      : key_(key), value_(value), solution_(solution), counters_(counters) { }
    void operator() (int* sel, int k) {
      NumVec s1, s2;
      splitVec(key_, sel, k, s1, s2);
      const ValExprMap& s1_vals=(solution_.find(s1))->second;
      const ValExprMap& s2_vals=(solution_.find(s2))->second;
      // neither s1_vals nor s2_vals should be empty
      for (auto& i : s1_vals) {
        for (auto& j : s2_vals) {
          doPlus(i, j);
          doMinus(i, j);
          doMinus(j, i);
          doMultiple(i, j);
          doDivision(i, j);
          doDivision(j, i);
        }
      }
    }

  private:
    const NumVec& key_;
    ValExprMap& value_;
    const SolutionMap& solution_;
    Counters& counters_;

    void doPlus(const ValExprMap::value_type& left, const ValExprMap::value_type& right)
    {
      Rational result=left.first + right.first;
      ExprSet& exprs=value_[result];
      for (auto& lexpr : left.second) {
        for (auto& rexpr : right.second) {
          ++counters_.loops;
          std::unique_ptr<Expr> expr(new AddSub(lexpr, rexpr, false));
          if (exprs.find(expr.get()) == exprs.end()) {
            ++counters_.exprs;
            exprs.insert(expr.release());
          }
        }
      }
    }

    void doMinus(const ValExprMap::value_type& left, const ValExprMap::value_type& right)
    {
      Rational result=left.first - right.first;
      if (result < 0) return;

      ExprSet& exprs=value_[result];
      for (auto& lexpr : left.second) {
        for (auto& rexpr : right.second) {
          ++counters_.loops;
          std::unique_ptr<Expr> expr(new AddSub(lexpr, rexpr, true));
          if (exprs.find(expr.get()) == exprs.end()) {
            ++counters_.exprs;
            exprs.insert(expr.release());
          }
        }
      }
    }

    void doMultiple(const ValExprMap::value_type& left, const ValExprMap::value_type& right)
    {
      Rational result=left.first * right.first;
      ExprSet& exprs=value_[result];
      for (auto& lexpr : left.second) {
        for (auto& rexpr : right.second) {
          ++counters_.loops;
          std::unique_ptr<Expr> expr(new MulDiv(lexpr, rexpr, false));
          if (exprs.find(expr.get()) == exprs.end()) {
            ++counters_.exprs;
            exprs.insert(expr.release());
          }
        }
      }
    }

    void doDivision(const ValExprMap::value_type& left, const ValExprMap::value_type& right)
    {
      if (right.first == 0) return;
      Rational result=left.first / right.first;
      ExprSet& exprs=value_[result];
      for (auto& lexpr : left.second) {
        for (auto& rexpr : right.second) {
          ++counters_.loops;
          std::unique_ptr<Expr> expr(new MulDiv(lexpr, rexpr, true));
          if (exprs.find(expr.get()) == exprs.end()) {
            ++counters_.exprs;
            exprs.insert(expr.release());
          }
        }
      }
    }
  };
  
  class SolutionBuilder {
  public:
    SolutionBuilder(Find24* parent) : p_(parent) { }
    void operator() (int* sel, int k) {
      NumVec key;
      for (int i=0; i<k; ++i) {
        key.push_back(p_->elems_[sel[i]]);
      }
      if (p_->solution_.find(key) == p_->solution_.end()) {
        ValExprMap value;
        ValueBuilder vb(key, value, p_->solution_, p_->counters_);
        for (int i=1; i<=key.size()/2; ++i) {
          selectK(key.size(), i, vb);
        }
        p_->solution_.insert(make_pair(key, value));
      }
    }

  private:
    Find24* p_;
  };

  void buildSolutionMap() {
    addLiterals();
    SolutionBuilder sb(this);
    for (int i=2; i<=elems_.size(); ++i) {
      selectK(elems_.size(), i, sb);
    }
  }
};

int main(int argc, char* argv[])
{
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <target> <n1> <n2> ... " << std::endl;
    return -1;
  }
  
  int target=atoi(argv[1]);
  if (target<=0) {
    std::cerr << "target must be a positive number" << std::endl;
    return -1;
  }

  std::vector<int> elems;
  for (int i=2; i<argc; ++i) {
    int elem=atoi(argv[i]);
    if (elem<=0) {
      std::cerr << "input must be positive number(s)" << std::endl;
      return -1;
    }
    elems.push_back(elem);
  }

  Find24 runner(target, elems);
  runner.run(true);

  return 0;
}
