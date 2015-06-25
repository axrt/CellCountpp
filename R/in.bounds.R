in.bounds<-function(x,leftBound, rightBound, inclusive=FALSE){
  return(.Call("inBounds", x, leftBound, rightBound, inclusive))
}