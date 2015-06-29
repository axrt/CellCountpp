#include <Rcpp.h>
#include <stdio.h>
#include <math.h> 

using namespace Rcpp;

inline void setVisited(LogicalMatrix &path, const IntegerVector &xys_i){
  path(xys_i[0]-1,xys_i[1]-1)=true;
}

inline void removeVisited(LogicalMatrix &path, const IntegerVector &xys_i){
  path(xys_i[0]-1,xys_i[1]-1)=false;
}

inline bool inField(const NumericMatrix &img, const IntegerVector &xys_i){
  return(xys_i[0]-1>=0&&xys_i[0]-1<img.ncol()&&xys_i[1]-1>=0&&xys_i[1]-1<img.nrow());
}

inline bool hasBeenVisited(LogicalMatrix &path, const IntegerVector &xys_i){
  return path(xys_i[0]-1,xys_i[1]-1);
}

inline bool brightEnough(const NumericMatrix &img, const IntegerVector &xys_i){
  return(img(xys_i[0]-1,xys_i[1]-1)>0);
}

inline bool closeEnough(const IntegerVector &xys_i,const IntegerVector &start, 
                                 const double width, const double var){
  const double dist=sqrt(pow(xys_i[0]-start[0],2)+pow(xys_i[1]-start[1],2));
  const double dist_var=dist+var;
  return(dist<=dist_var);
}

inline bool allChecks(const NumericMatrix &img, LogicalMatrix &path, const IntegerVector &xys_i,
                               const IntegerVector &start, const double width, const double var){
   const bool in_field=inField(img,xys_i);
   const bool has_been_visited=hasBeenVisited(path,xys_i);
   const bool bright_enough=brightEnough(img, xys_i);
   const bool close_enough=closeEnough(xys_i, start, width, var);
    
    return(in_field&&!has_been_visited&&bright_enough&&close_enough);
}

inline const IntegerVector * right(const IntegerVector * xys_i){
  IntegerVector * neigh = new IntegerVector(2);
  (*neigh)[0]=(*xys_i)[0]+1;
  (*neigh)[1]=(*xys_i)[1];
  return neigh;
}

inline const IntegerVector * left(const IntegerVector * xys_i){
  IntegerVector * neigh = new IntegerVector(2);
  (*neigh)[0]=(*xys_i)[0]-1;
  (*neigh)[1]=(*xys_i)[1];
  return neigh;
}

inline const IntegerVector * up(const IntegerVector * xys_i){
 IntegerVector * neigh = new IntegerVector(2);
  (*neigh)[0]=(*xys_i)[0];
  (*neigh)[1]=(*xys_i)[1]+1;
  return neigh;
}

inline const IntegerVector * down(const IntegerVector * xys_i){
  IntegerVector * neigh = new IntegerVector(2);
  (*neigh)[0]=(*xys_i)[0];
  (*neigh)[1]=(*xys_i)[1]-1;
  return neigh;
}

void checkNeighbor(const NumericMatrix &img,LogicalMatrix &path, const IntegerVector * neigh, 
                                     std::vector<const IntegerVector *> * output, const IntegerVector  &startPoint, 
                                     const double width, const double var);

void checkNeighborhood(const NumericMatrix &img,LogicalMatrix &path, const IntegerVector * xys_i, 
                                     std::vector<const IntegerVector *> * output, const IntegerVector  &startPoint, 
                                     const double width, const double var){
  //check if an adjasent pixel,is withing the view field, has been visited, is bright enough and is within the possible range
  //right
  
  const IntegerVector * neighRight=right(xys_i);
  checkNeighbor(img,path,neighRight,output,startPoint,width,var);
  
  const IntegerVector * neighUp=up(xys_i);
  checkNeighbor(img,path,neighUp,output,startPoint,width,var);
  
  const IntegerVector * neighLeft=left(xys_i);
  checkNeighbor(img,path,neighLeft,output,startPoint,width,var);
  
  const IntegerVector * neighDown=down(xys_i);
  checkNeighbor(img,path,neighDown,output,startPoint,width,var);
  
}

void checkNeighbor(const NumericMatrix &img,LogicalMatrix &path, const IntegerVector * neigh, 
                                     std::vector<const IntegerVector *> * output, const IntegerVector  &startPoint, 
                                     const double width, const double var){
  
  if(allChecks(img,path,*neigh,startPoint,width,var)){
    setVisited(path,*neigh);
    output->push_back(neigh);
    checkNeighborhood(img,path,neigh,output,startPoint,width,var);
  }else{
    delete neigh;
  }
}

// [[Rcpp::export]]
RcppExport SEXP getClusters(SEXP imgMtx, SEXP sortedXY, SEXP pathMtx, SEXP meanWidth, SEXP varWidth) {
   
   Rcout<<"Started cluster search.."<<std::endl;
   const NumericMatrix img(imgMtx);
   Rcout<<"Intensity matrix read.."<<std::endl;
         IntegerMatrix xys(sortedXY);
   Rcout<<"Intensity gradient read.."<<std::endl;
         LogicalMatrix path(pathMtx);
   Rcout<<"Path read.."<<std::endl;
   const NumericVector widthV(meanWidth);
   Rcout<<"Width read.."<<std::endl;
   const NumericVector varV(varWidth);
   Rcout<<"Variance read.."<<std::endl;
   const double width=widthV[0];
   const double var=varV[0];
   
   std::vector<IntegerMatrix*> outClusters;
   int counter=0; 
   
   
   for(int i=0; i<xys.nrow();i++){
     //check if the point has been visited
     if(!hasBeenVisited(path,xys.row(i))){
       //set visited
       const IntegerVector * currow=new IntegerVector(xys.row(i)[0],xys.row(i)[1]);
       setVisited(path,xys.row(i));
       std::vector<const IntegerVector*> * outputTemplate =new std::vector<const IntegerVector*>();
       //check if the neigbor can be attached
       outputTemplate->push_back(currow);
       
       checkNeighborhood(img, path, currow, outputTemplate, xys.row(i), width, var);
       
       //IntegerMatrix * im = new IntegerMatrix(outputTemplate->size(),2);
  
       //for(int i=0;i<im->nrow();i++){
         //im->row(i)=*outputTemplate->at(i);
       //}
       //Rcout<<"Another cluster of size: "<<im->nrow()<<std::endl;
       //outClusters.push_back(im); 
       
       while(!outputTemplate->empty()) delete outputTemplate->back(), outputTemplate->pop_back();
       delete outputTemplate;
     }
   }
   
   Rcout<<"Done parsing clusters.."<<std::endl;
   
   List outClusterList;
   for(int i=0;i<counter;i++){
     outClusterList[i]=*(outClusters[i]);
     delete outClusters[i];
   }

   Rcout<<"Done assembling cluster list.."<<std::endl;
   return outClusterList;
}
