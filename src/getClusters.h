#include <Rcpp.h>
#include <stdio.h>
#include <math.h> 

using namespace Rcpp;

/**
 * Sets true (= "visited") to a path point indicated in xys_i.
 * @param path a reference to a LogicalMatrix that represents points that have been visited
 * and those that have not.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 */
inline void setVisited(LogicalMatrix &path, const IntegerVector &xys_i);

/**
 * Sets false (= "not visited") to a path point indicated in xys_i.
 * @param path a reference to a LogicalMatrix that represents points that have been visited
 * and those that have not.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 */
inline void removeVisited(LogicalMatrix &path, const IntegerVector &xys_i);
/**
 * Checks whether a coordinate given in xys_i is in bounds of the image field (i.e. (-1,0) is not).
 * @param img a reference to an image intensity matrix.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix. 
 * @return true if the point is in field, false otherwise.
 */
inline const bool inField(const NumericMatrix &img, const IntegerVector &xys_i);
/**
 * Checks if the coordinate given in xys_i has been visited yet (potentially by another expanding cluster)
 * @param path a reference to a LogicalMatrix that represents points that have been visited
 * and those that have not.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 * @return true if the point has been visited, false otherwise.
 */
inline const bool hasBeenVisited(LogicalMatrix &path, const IntegerVector &xys_i);
/**
 * Checks if the pixel/dot given in xys_i is brigh enough to be considered a part of the cell.
 * @param img a reference to an image intensity matrix.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 * @return true if the pixel is bright, false otherwise.
 */
inline const bool brightEnough(const NumericMatrix &img, const IntegerVector &xys_i);
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
                        const double width, const double var);
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
                      const IntegerVector &start, const double width, const double var);
/**
 * Returns a pixel/cell to the right.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 * @retrun new IntegerVector with coordintaes of the cell to the right.
 */
inline const IntegerVector * right(const IntegerVector * xys_i);
/**
 * Returns a pixel/cell to the left.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 * @retrun new IntegerVector with coordintaes of the cell to the left.
 */
inline const IntegerVector * left(const IntegerVector * xys_i);
/**
 * Returns a pixel/cell on top of the given one.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 * @retrun new IntegerVector with coordintaes of the cell on top.
 */
inline const IntegerVector * up(const IntegerVector * xys_i);
/**
 * Returns a pixel/cell beneath the given one.
 * @param xys_i reference to an integer vector of two coordinates (x,y), that indicate a position in either
 * intensity matrix, or path matrix.
 * @retrun new IntegerVector with coordintaes of the cell beneath.
 */
inline const IntegerVector * down(const IntegerVector * xys_i);

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
                       const double width, const double var);
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
                   const double width, const double var);
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
RcppExport SEXP getClusters(SEXP imgMtx, SEXP sortedXY, SEXP pathMtx, SEXP meanWidth, SEXP varWidth, SEXP minClusterArea);
