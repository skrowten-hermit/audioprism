#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class constVecTest1
{
private:
    vector<int> vectortest;
    static vector<int> vecOp1();
public:
    constVecTest1(vector<int> vin);
    const vector<int> cvectorin;
    vector<int> vectorin;
    void vecOp2(vector<int> vin);
};

constVecTest1::constVecTest1(vector<int> vin) : vectortest(), cvectorin(vecOp1()), vectorin()
{
    vecOp2(vin);
}

// constVecTest1::constVecTest1(vector<int> vin)
// {
//   vectortest = vin;
//   vectorin = vin;
//   cvectorin = (const vector<int>) vectortest;

//   cout << "Public Vector (vectorin) from constructor constVecTest1() : " << endl;
//   for(unsigned int i=0; i < vin.size(); i++)
//     cout << vectorin.at(i) << ' ';
//   cout << endl;

//   cout << "Private Vector (vectortest) from constructor constVecTest1() : " << endl;
//   for(unsigned int i=0; i < vin.size(); i++)
//     cout << vectortest.at(i) << ' ';
//   cout << endl;

//   copy(vectortest.begin(), vectortest.end(), back_inserter(cvectorin));

//   cout << "Constant Vector (cvectorin) from constructor constVecTest1() : " << endl;
//   for(unsigned int i=0; i < vin.size(); i++)
//     cout << vectortest.at(i) << ' ';
//   cout << endl;
// }

vector<int> constVecTest1::vecOp1()
{
    vector<int> result = {1, 0, 1, 0, 1};
    return result;
}

void constVecTest1::vecOp2(vector<int> vin)
{
  vectortest = vin;
  vectorin = vectortest;
}

class constVecTest2
{
private:
    vector<int> vectortest;
    vector<int> cvectorin;
public:
    constVecTest2(vector<int> vin);
    const vector<int> &get_cvectorin() const { return cvectorin; }
    vector<int> vectorin;
    void vecOp1();
    void vecOp2(vector<int> vin);
};

constVecTest2::constVecTest2(vector<int> vin)
{
  vecOp1();
  vecOp2(vin);
}

void constVecTest2::vecOp1()
{
  vectortest = {1, 0, 1, 0, 1};
  // cvectorin = vectortest;
  copy(vectortest.begin(), vectortest.end(), back_inserter(cvectorin));
}

void constVecTest2::vecOp2(vector<int> vin)
{
  vectortest = vin;
  vectorin = vectortest;
}


int main()
{
  vector<int> input {1, 2, 3, 4, 5}, update {6, 7, 8, 9};
  vector<int> cvo, cvc;
  const vector<int> cvm;

  constVecTest1 cvt1(input);
  cvo = cvt1.vectorin;
  cvc = cvt1.cvectorin;
  // cvm = cvc;

  cout << "Public Vector (vectorin) from main() : " << endl;
  for(unsigned int i=0; i < cvo.size(); i++)
    cout << cvo.at(i) << ' ';
  cout << endl;

  cout << "Constant Vector (cvectorin) from main() : " << endl;
  for(unsigned int i=0; i < cvc.size(); i++)
    cout << cvc.at(i) << ' ';
  cout << endl;

  /*cout << "Copied Vector (from cvectorin) from main() : " << endl;
  for(unsigned int i=0; i < cvm.size(); i++)
    cout << cvm.at(i) << ' ';
  cout << endl;*/

  cvc = update;
  // cvt1.cvectorin = update;

  constVecTest2 cvt2(input);
  cvo = cvt2.vectorin;
  cvc = cvt2.get_cvectorin();
  // cvm = cvt2.get_cvectorin();
  cvc.push_back(11);

  cout << "Public Vector (vectorin) from main() : " << endl;
  for(unsigned int i=0; i < cvo.size(); i++)
    cout << cvo.at(i) << ' ';
  cout << endl;

  cout << "Constant Vector (cvectorin) from main() : " << endl;
  for(unsigned int i=0; i < cvc.size(); i++)
    cout << cvc.at(i) << ' ';
  cout << endl;

  cvc = update;

  /*cout << "Copied Vector (from cvectorin) from main() : " << endl;
  for(unsigned int i=0; i < cvm.size(); i++)
    cout << cvm.at(i) << ' ';
  cout << endl;*/

  return 0;
}