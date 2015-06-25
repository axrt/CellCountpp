#include <Rcpp.h>
#include <stdio.h>
using namespace Rcpp;

// [[Rcpp::export]]
RcppExport SEXP inBounds( SEXP x, SEXP leftBound, SEXP rightBound, SEXP inclusive) {
  
  const NumericVector xx(x);
  const NumericVector lb(leftBound);
  const NumericVector rb(rightBound);
  const LogicalVector incl(inclusive);

  if(incl[0]){
    return wrap(xx[0]>=lb[0]&&xx[0]<=rb[0]);
  }else{
    return wrap(xx[0]>lb[0]&&xx[0]<rb[0]);
  }
  
}
