rm *.aux
rm *.toc
rm *.lof
rm *.log
rm *.out

for VAR in 1 2 3 4 5
do
	pdflatex report.tex
done

