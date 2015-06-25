#' Use to check whether the given value falls inbetween the bounds.
#' 
#' @param \code{x} numeric value
#' @param \code{leftBound} left numeric value
#' @param \code{rightBound} right numeric value
#' @param \code{inclusive} should the comparison be inclusive (TRUE) or exclusive (FALSE)
#' @return whether or not the x value falls inbetween leftBound and rightBound
#' @examples
#' x<-2.7
#' leftBound<-1.7
#' rightBound<-3.14
#' in.bounds(x,leftBound,rightBound,inclusive=TRUE)
#' 
in.bounds<-function(x,leftBound, rightBound, inclusive=FALSE){
  if(inclusive){
    return(x>=leftBound&&x<=rightBound)
  }else{
    return(x>leftBound&&x<rightBound)
  }
}