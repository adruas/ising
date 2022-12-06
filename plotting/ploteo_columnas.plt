set terminal png
unset border
unset xtics
unset ytics
unset key
set size ratio 1
set title 'Valor +1 si es de color negro, valor -1 si es de color blanco'
do for [i=0:70] {
    set output sprintf('matriz%03.0f.png',i)
    plot 'columnas.txt' u 1:2 index i pt 5 ps 0.4 lc "black"
}
