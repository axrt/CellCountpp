get.clusters<-function(img.mtx,sorted.x.y,intensity.cutoff,mean.width){
  
  path.mtx<-matrix(FALSE,nrow = nrow(img.mtx))
  img.mtx[img.mtx<intensity.cutoff]<-0
  return(.Call("getClusters", img.mtx, sorted.x.y, path.mtx, mean.width, PACKAGE = 'CellCountpp'))
  
}