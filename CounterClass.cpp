//
//  CounterClass.cpp
//  CrapProject
//
//  Created by David Mason on 8/28/13.
//  Copyright (c) 2013 David Mason. All rights reserved.
//

#include "CounterClass.h"


CounterClass::CounterClass(string label) {

  Counter=0;
  CounterHist=new TH1F(label.c_str(),label.c_str(),1,-1,2);

}


CounterClass::~CounterClass() {
  
}

CounterClass& CounterClass::operator++() {  //overloading prefix ++ to both increment the integer bit and add 1.0 to the 1 bin hist.

  Counter++;
  CounterHist->Fill(1.0);
  
  return *this;
  
}

CounterClass CounterClass::operator++(int) { // overloading postfix ++ -- evidently argument isn't used for these sorts of things..
  CounterClass dummy=*this;
  
  ++(*this);
  
  return dummy;  // we're returning the saved version...  
  
}

float CounterClass::Val() {
  return Counter;
}

TH1F* CounterClass::Hist() {
  return CounterHist;
}

