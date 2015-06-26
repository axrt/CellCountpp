#include <Rcpp.h>
#include <stdio.h>
#include <math.h> 

using namespace Rcpp;

inline void setVisited(LogicalMatrix &path, const IntegerMatrix::Row &xys_i){
  path(xys_i[1],xys_i[2])=true;
}

inline void removeVisited(LogicalMatrix &path, const IntegerMatrix::Row &xys_i){
  path(xys_i[1],xys_i[2])=false;
}

inline bool inField(const NumericMatrix &img, const IntegerMatrix::Row &xys_i){
  return(xys_i[1]>0&&xys_i[1]<=img.ncol()&&xys_i[2]>0&&xys_i[2]<=img.nrow());
}

inline bool hasBeenVisited(LogicalMatrix &path, const IntegerMatrix::Row &xys_i){
  return path(xys_i[1],xys_i[2]);
}

inline bool brightEnough(const NumericMatrix &img, const IntegerMatrix::Row &xys_i){
  return(img(xys_i[1],xys_i[2])>0);
}

inline bool allChecks(const NumericMatrix &img, LogicalMatrix &path, const IntegerMatrix::Row &xys_i,
                               const IntegerMatrix::Row &start, const double width){
   const bool in_field=inField(img,xys_i);
   const bool has_been_visited=hasBeenVisited(path,xys_i);
                                 
}

inline bool closeEnough(const IntegerMatrix::Row &xys_i,const IntegerMatrix::Row &start, const double width){
  const double dist=sqrt(pow(xys_i[1]-start[1],2)+pow(xys_i[2]-start[2],2));
  return(dist<=width);
}

IntegerMatrix * checkNeighborhood(LogicalMatrix &path, IntegerMatrix::Row &xys_i, 
                                     NumericMatrix &output, IntegerMatrix::Row &startPoint, const double width){
  //check if an adjasent pixel,is withing the view field, has been visited, is bright enough and is within the possible range
  
  return new IntegerMatrix();
}

// [[Rcpp::export]]
RcppExport SEXP getClusters(SEXP imgMtx, SEXP sortedXY, SEXP pathMtx, SEXP meanWidth) {
   
   const NumericMatrix img(imgMtx);
         IntegerMatrix xys(sortedXY);
         LogicalMatrix path(pathMtx);
   const NumericVector widthV(meanWidth);
   const double width=widthV[0];
   
   for(int i=0; i<xys.ncol();i++){
     //check if the point has been visited
     const IntegerMatrix::Row currow=xys.row(i);
     if(!hasBeenVisited(path,currow)){
       //set visited
       setVisited(path,currow);
       //check if the neigbor
     }
   }
   
   
   return wrap(0);
}
