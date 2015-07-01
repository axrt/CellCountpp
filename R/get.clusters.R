get.clusters<-function(img.mtx,sorted.x.y,intensity.cutoff,mean.width, var.width, min.cell.area=mean.width-var.width/2){
  
  path.mtx<-matrix(FALSE,nrow = nrow(img.mtx), ncol=ncol(img.mtx))
  img.mtx[img.mtx<intensity.cutoff]<-0
  return(.Call("getClusters", img.mtx, sorted.x.y, path.mtx, mean.width, var.width, min.cell.area, PACKAGE = 'CellCountpp'))
  
}