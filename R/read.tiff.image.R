#'Reads a tiff image and returns a matrix (normalized also possible)
#'
#'@param \code{image.file} a path to the image file
#'@param \cpde{normalize} determines if the data be normalized by [0,1], 
#'default is TRUE and seems redundant as the image comes normalized anyways
#'@return a numeric matrix with values, roughly (but close enough) representing the "intensities" of pixels
#'@examples
#'image.file<-"inst/extradata//control_sample.tif"
#'img.mtx<-read.tiff.image(image.file = image.file)
#'head(img.mtx)
#'max(img.mtx)
#'min(img.mtx)
#'
read.tiff.image<-function(image.file,normalize=TRUE){
  
  #TODO imput checks
  if(!require("tiff")){
    install.packages("tiff")
  }
  
  im.data<-readTIFF(source = image.file)
  n.col<-ncol(im.data)
  im.data<-im.data[,,1]+im.data[,,2]+im.data[,,3]
  im.data<-matrix(im.data, ncol=n.col)
 
  if(normalize){
    if(!require("scales")){
      install.packages("scales")
    }
    im.data<-rescale(im.data, c(0,max(im.data)),c(0,1))
  }
  
  return(im.data)
}