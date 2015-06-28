#'Converts an image matrix (presumably from read.tiff.image()) to a 3-column matrix, which may be sorted by value decreasing
#'
#'@param \code{img.mtx} image matrix
#'@param \code{sort.by.intensity.val} indicates whether the matrix shall be sorted by the intensity value decreasing
#'@return a 3-column matrix with X,Y coordinates and a correspongin V intensity values
#'@examples
#'image.file<-"inst/extradata//control_sample.tif"
#'img.mtx<-read.tiff.image(image.file = image.file)
#'img.mtx.melt<-extract.x.y(img.mtx = img.mtx, sort.by.intensity.val = TRUE)
#'head(img.mtx.melt)
#'
extract.x.y<-function(img.mtx, sort.by.intensity.val=TRUE){
  if(!require("reshape2")){
    install.packages("reshape2")
  }
  img.mtx<-melt(img.mtx)
  colnames(img.mtx)<-c("X","Y","V")
  if(sort.by.intensity.val){
    img.mtx<-img.mtx[order(img.mtx$V,decreasing = TRUE),]
  }
  img.mtx<-img.mtx[,1:2]
  img.mtx$X<-as.integer(img.mtx[,1])
  img.mtx$Y<-as.integer(img.mtx[,2])
  return(as.matrix(img.mtx))
}