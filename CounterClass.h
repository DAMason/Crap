//
//  CounterClass.h
//  CrapProject
//
//  Created by David Mason on 8/28/13.
//  Copyright (c) 2013 David Mason. All rights reserved.
//

#ifndef __CrapProject__CounterClass__
#define __CrapProject__CounterClass__

#include <iostream>
#include <TH1F.h>

using namespace std;

#endif /* defined(__CrapProject__CounterClass__) */

class CounterClass {
  public:
  CounterClass(string) ;
  virtual ~CounterClass();
  CounterClass& operator++ ();  // prefix
  CounterClass operator++(int); // postifx
  float Val();
  TH1F* Hist();
  
  private:

  int Counter;
  TH1F* CounterHist;
};


