/**
 * @file
 * @section ALGORITHM DESCRIPTION
 * 
 * The code below performs the most computationally intense parts of the algorithm. The file contains a 
 * number of funcitons, do not make much sense taken alone.
 * Instead, they make the code in the algorithm more readable and coherent. The code still conatins some
 * object space allocations, which could be avoided, however, these seem unlikely to have much impact on the
 * performance.
 * 
 * The idea behind the algorithm stems from the fact that retinal cells on a confocal scan image form 
 * clusters of bright dots, which do not necessarily form a circle or even a homogenious 
 * field (can have a dark nuclei trace in the center, forming a donut-like shape). 
 * Adhering to conventions, we suppose that cells with nuclei (seen as darker dots on an image within a cell) 
 * are still considered "good enough" for counting, despite their frequently low fluorescence. 
 * This makes the identification process of both very bright and less brigth 
 * cells of different shapes in the same image rather challenging.
 * 
 * The algorithm starts with a normalized N*M (N=M for most cases) matrix (the image), where the intensity of
 * a given dot is in [0,1] range. The coordinates of all dots are sorted by intensity descending. A certain 
 * cutoff is chosen such that all dots with intensitites lower that the cutoff are cosidered "background" and
 * are converted to zero values. The algorithm starts with the top (brightest) dot. It goes in a recursive
 * search for dots around the "brighest": each pixel "looks up, down, left and right" and checks if its 
 * neighbors can be also cosidered for inclusion in the cluster (which ideally will cover and represent
 * a cell in the image). It (the current pixel) then passes control up the stack to the neighbor, 
 * which in turn checks its surroundings. It won't pass control to a pixel if:
 * - the pixel has 0 intensity (not intense enough = "background")
 * - is out of scope (goes out of reasonable field that a cell may occupy). This feature is controlled by the
 *   "width" and "var" parameters, which do not allow to expand out too far from the brighest dot
 * - the pixel does not actully belong to the image (i.e. (-1,-1) obviously does not)
 * - the pixel has not been visited by another expanding cluster (means that a pixel can not belong to two
 *   different cells at a time, which makes sense)
 * The cluster expansion ends when no other pixels can add their neighbors due to any of the above conditions
 * unsatisfaction. The number of pixels, identified as a "cluster" are saved in a separate matrix, each matrix
 * contains all (x,y) coordinates of a corresponding cell in the image.
 * Finally the algorithm ckecks if the area of a cluster (number of pixels/dots it includes) can be considered 
 * large enough to reasonably cover a cell. The clusters that are large enough are saved to a list structure.
 * Finally (but outside the scope of this C++ component), the coordinates of each cluster are passed to
 * a maximum likelihood algorithm, that finds a most probable center of the cell and allows to unambigously
 * mark it on an image.
 * 
 * Provided the well adjusted parameters, the algorithm may perform surprisingly well and cover >95% of the cells
 * with a very low false positive and false negative rates. The parameters should be adjusted for each individual
 * set/experiment group of images separately. GUI-based component tools of this packaage are aimed at helping the
 * user to adjust the parameters with ease and "feeling" of the optimal.
 * 
 * @author Alex Tuzhikov <alexander.tuzhikov@gmail.com>
 * @version 0.1
 */
#include "getClusters.h"

/**
 * Sets true (= "visited") to a path point indicated in xys_i.
 * @param path a reference to a LogicalMatrix that represents points that have been visited
 * and those that have not.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 */
inline void setVisited(LogicalMatrix &path, const IntegerVector &xys_i){
  path(xys_i[0]-1,xys_i[1]-1)=true;
}

/**
 * Sets false (= "not visited") to a path point indicated in xys_i.
 * @param path a reference to a LogicalMatrix that represents points that have been visited
 * and those that have not.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 */
inline void removeVisited(LogicalMatrix &path, const IntegerVector &xys_i){
  path(xys_i[0]-1,xys_i[1]-1)=false;
}

/**
 * Checks whether a coordinate given in xys_i is in bounds of the image field (i.e. (-1,0) is not).
 * @param img a reference to an image intensity matrix.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 * @return true if the point is in field, false otherwise.
 */
inline const bool inField(const NumericMatrix &img, const IntegerVector &xys_i){
  return(xys_i[0]-1>=0&&xys_i[0]-1<img.ncol()&&xys_i[1]-1>=0&&xys_i[1]-1<img.nrow());
}

/**
 * Checks if the coordinate given in xys_i has been visited yet (potentially by another expanding cluster)
 * @param path a reference to a LogicalMatrix that represents points that have been visited
 * and those that have not.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 * @return true if the point has been visited, false otherwise.
 */
inline const bool hasBeenVisited(LogicalMatrix &path, const IntegerVector &xys_i){
  return path(xys_i[0]-1,xys_i[1]-1);
}

/**
 * Checks if the pixel/dot given in xys_i is brigh enough to be considered a part of the cell.
 * In fact, this function just checks if the intesity of a pixel is greater than 0, as it is
 * assumed that the intesity matrix has already been trimmed to a background cutoff.
 * @param img a reference to an image intensity matrix.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 * @return true if the pixel is bright (intensity greater than zero in this case), false otherwise.
 */
inline const bool brightEnough(const NumericMatrix &img, const IntegerVector &xys_i){
  return(img(xys_i[0]-1,xys_i[1]-1)>0);
}

/**
 * Checks of a pixel/dot given in xys_i is close (Euclidean distance) from the starting point (the brighest
 * point in the cluster).
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 * @param start reference to the starting pixel/dot (the brighest one in the cluster).
 * @param width a diameter of a cluster (cell)
 * @param var variance value, which rougly estimates how much the cells may vary in diameter
 * @return true if the given pixel/dot can be considered as a part of a cell, false if
 * the pixel/dot lays outside the reasonable cell size.
 */
inline const bool closeEnough(const IntegerVector &xys_i,const IntegerVector &start, 
                                 const double width, const double var){
  const double dist=sqrt(pow(xys_i[0]-start[0],2)+pow(xys_i[1]-start[1],2));
  const double dist_var=dist+var/2;
  const double radius=(width+var)/2;
  return(dist_var<=radius);
}

/**
 * Convinience function, checks all 4 conditions for a pixel/dot to be considered a part of a cell.
 * @param img a reference to an image intensity matrix.
 * @param path a reference to a LogicalMatrix that represents points that have been visited
 * and those that have not.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 * @param start reference to the starting pixel/dot (the brighest one in the cluster).
 * @param width a diameter of a cluster (cell)
 * @param var variance value, which rougly estimates how much the cells may vary in diameter
 * @return true if the given pixel/dot can be considered as a part of a cell, false if
 * the pixel/dot lays outside the reasonable cell size.
 */
inline const bool allChecks(const NumericMatrix &img, LogicalMatrix &path, const IntegerVector &xys_i,
                               const IntegerVector &start, const double width, const double var){
   if(inField(img,xys_i)){
     const bool has_been_visited=hasBeenVisited(path,xys_i);
     const bool bright_enough=brightEnough(img, xys_i);
     const bool close_enough=closeEnough(xys_i, start, width, var);
     return(!has_been_visited&&bright_enough&&close_enough);
   }else{
     return false;
   }
}

/**
 * Returns a pixel/cell to the right. NOTE: returns a pointer to an IntegerVector created with "new",
 * thereby care must be taken to "delete" the pointer to free the memory.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * @retrun new IntegerVector with coordintaes of the cell to the right.
 */
inline const IntegerVector * right(const IntegerVector * xys_i){
  IntegerVector * neigh = new IntegerVector(2);
  (*neigh)[0]=(*xys_i)[0]+1;
  (*neigh)[1]=(*xys_i)[1];
  return neigh;
}

/**
 * Returns a pixel/cell to the left. NOTE: returns a pointer to an IntegerVector created with "new",
 * thereby care must be taken to "delete" the pointer to free the memory.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 * @retrun new IntegerVector with coordintaes of the cell to the left.
 */
inline const IntegerVector * left(const IntegerVector * xys_i){
  IntegerVector * neigh = new IntegerVector(2);
  (*neigh)[0]=(*xys_i)[0]-1;
  (*neigh)[1]=(*xys_i)[1];
  return neigh;
}

/**
 * Returns a pixel/cell on top. NOTE: returns a pointer to an IntegerVector created with "new",
 * thereby care must be taken to "delete" the pointer to free the memory.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 * @retrun new IntegerVector with coordintaes of the cell on top.
 */
inline const IntegerVector * up(const IntegerVector * xys_i){
 IntegerVector * neigh = new IntegerVector(2);
  (*neigh)[0]=(*xys_i)[0];
  (*neigh)[1]=(*xys_i)[1]+1;
  return neigh;
}

/**
 * Returns a pixel/cell beneath. NOTE: returns a pointer to an IntegerVector created with "new",
 * thereby care must be taken to "delete" the pointer to free the memory.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 * @retrun new IntegerVector with coordintaes of the cell beneath.
 */
inline const IntegerVector * down(const IntegerVector * xys_i){
  IntegerVector * neigh = new IntegerVector(2);
  (*neigh)[0]=(*xys_i)[0];
  (*neigh)[1]=(*xys_i)[1]-1;
  return neigh;
}

/**
 * Part one of two voids that recursively check the neighborhood and add the neighborhood pixels/dots to the cluster.
 * @param img a reference to an image intensity matrix.
 * @param path a reference to a LogicalMatrix that represents points that have been visited
 * and those that have not.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 * @param output a standard library vector of pointers to IntegerVector object, that represent (x,y) coordinates of
 * pixels/dots in the intensity or logic path matrices. One full vector forms a cluster -> cell in the image.
 * @param start reference to the starting pixel/dot (the brighest one in the cluster).
 * @param width a diameter of a cluster (cell)
 * @param var variance value, which rougly estimates how much the cells may vary in diameter
 * @return true if the given pixel/dot can be considered as a part of a cell, false if
 * the pixel/dot lays outside the reasonable cell size.
 */
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

/**
 * Part two of two voids that recursively check the neighborhood and add the neighborhood pixels/dots to the cluster.
 * @param img a reference to an image intensity matrix.
 * @param path a reference to a LogicalMatrix that represents points that have been visited
 * and those that have not.
 * @param neigh pointer to an integer vector of two coordinates (x,y), that indicate the position 
 * of a neigboring pixel/dot.
 * @param output a standard library vector of pointers to IntegerVector object, that represent (x,y) coordinates of
 * pixels/dots in the intensity or logic path matrices. One full vector forms a cluster -> cell in the image.
 * @param start reference to the starting pixel/dot (the brighest one in the cluster).
 * @param width a diameter of a cluster (cell)
 * @param var variance value, which rougly estimates how much the cells may vary in diameter
 * @return true if the given pixel/dot can be considered as a part of a cell, false if
 * the pixel/dot lays outside the reasonable cell size.
 */
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

/**
 * Rcpp export function, 
 * @param imgMtx an image intensity matrix.
 * @param sortedXY a matrix of (x,y) coordinates, sorted by the corresponding pixel intensity descending.
 * @param pathMtx a logical matrix that represents points that have been visited
 * and those that have not (all false in the begining understandably).
 * @param meanWidth a diameter of a cluster (cell).
 * @param varWidth variance value, which rougly estimates how much the cells may vary in diameter
 * @param minClusterArea is used if its value is lower than meanWidth-varWidth/2, that is, if the cells vary
 * in size so drastically (in case of ischemic shock for instance), that very small cells along with normal large
 * cells must be considered for counting.
 * @return and unwrapped (cuz standard type for R/Rcpp) List of cluster corrdinate matrices, each representin 
 * a different cell.
 */
// [[Rcpp::export]]
RcppExport SEXP getClusters(SEXP imgMtx, SEXP sortedXY, SEXP pathMtx, SEXP meanWidth, SEXP varWidth, SEXP minClusterArea) {
   
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
   const NumericVector mca(minClusterArea);
   Rcout<<"Minimum cluster area read.."<<std::endl;
   
   const double width=widthV[0];
   const double var=varV[0];
   const double minCA=mca[0];
   
   List outClusterList;
   const double area=3*pow((width)/2,2);
   const double lowMargin=3*pow((width-var)/2,2);
   const double highMargin=3*pow((width+var)/2,2);
   double minCellArea=0;
   if(lowMargin>minCA){
     minCellArea=minCA;
   }else{
     minCellArea=lowMargin;
   }
   
   Rcout<<"Area: "<<area<<"+/-"<<3*pow(var/2,2)<<std::endl;
   
   for(int i=0; i<xys.nrow();i++){
     //check if the point has been visited
     if(!hasBeenVisited(path,xys.row(i))){
       //set visited
       IntegerVector * currow=new IntegerVector(2);
       (*currow)[0]=xys.row(i)[0];
       (*currow)[1]=xys.row(i)[1];
       
       setVisited(path,xys.row(i));
       std::vector<const IntegerVector*> * outputTemplate =new std::vector<const IntegerVector*>();
       //check if the neigbor can be attached
       outputTemplate->push_back(currow);
       
       checkNeighborhood(img, path, currow, outputTemplate, xys.row(i), width, var);
       
       if(outputTemplate->size()<=highMargin&&outputTemplate->size()>=minCellArea){
         IntegerMatrix im(outputTemplate->size(),2);
         Rcout<<"Cluster saved with: "<<outputTemplate->size()<<" dots.."<<std::endl;
         
         for(int j=0;j<outputTemplate->size();j++){
          for(int k=0;k<2;k++){
           im(j,k)=(*outputTemplate->at(j))[k];
          }
         }
         
         outClusterList.push_back(im); 
       }
       
       while(!outputTemplate->empty()) delete outputTemplate->back(), outputTemplate->pop_back();
       delete outputTemplate;
     }
   }
   
   Rcout<<"Done parsing clusters.."<<std::endl;
   Rcout<<"Done assembling cluster list.."<<std::endl;

   return outClusterList;
}
