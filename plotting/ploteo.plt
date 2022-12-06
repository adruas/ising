set terminal png
set palette gray
unset border
unset xtics
unset ytics
set size ratio 1
set title 'Valor +1 si es de color blanco, valor -1 si es de color negro'
unset colorbox
do for [i=0:((1024*3)-1)] {
    set output sprintf('matrix%03.0f.png',i)
    plot 'matrices.txt' index i matrix with image
}
