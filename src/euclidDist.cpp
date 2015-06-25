#include <Rcpp.h>
#include <math.h>  
using namespace Rcpp;

// [[Rcpp::export]]
RcppExport SEXP euclidDist(SEXP x, SEXP y) {
   
   const NumericVector xx(x);
   const NumericVector yy(y);
   
   double sum=0;
   for(int i=0;i<xx.size();i++){
     sum+=pow(xx[i]-yy[i],2);
   }
   
   return(wrap(sqrt(sum)));
}
