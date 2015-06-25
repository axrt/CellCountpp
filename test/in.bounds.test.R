in.bounds(x = 1.5, leftBound = 0.3, rightBound = 7.1, inclusive = TRUE )
in.bounds(x = 10, leftBound = 0.3, rightBound = 7.1, inclusive = FALSE )

system.time(sapply(1:1e5, function(i){
  in.bounds(x = 1.5, leftBound = 0.3, rightBound = 7.1, inclusive = TRUE )
  }))

system.time(sapply(1:1e5,function(i){
  return(10>=0.3&&10<=7.1)
}))