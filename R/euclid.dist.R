#'Calculates euclid distance between two numeric vectors of same length. Faster implementation.
#'
#'@param \code{Rx} vector 1, must be numeric
#'@param \code{Ry} vector 2, must be numeric
#'@return distance between the vectors
#'@examples
#'a<-rep(1,4)
#'b<-rep(2,4)
#'euclid.dist(Rx = a, Ry = b)
#'
euclid.dist <- function(Rx,Ry){
  if(length(Rx)!=length(Ry)){
    stop("Rx and Ry must be of the same length!")
  }
  if(!is.numeric(Rx)||!is.numeric(Ry)){
    stop("Rx and Ry must be numeric!")
  }
  return(.Call("euclidDist",Rx,Ry, PACKAGE = 'CellCountpp'))
}