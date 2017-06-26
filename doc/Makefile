SOURCE = template

all: pdf

rebuild: clean-all pdf

pdf: $(SOURCE).tex
	pdflatex $(SOURCE)
	bibtex   $(SOURCE)
	pdflatex $(SOURCE)
	pdflatex $(SOURCE)
	mv $(SOURCE).pdf README.pdf

clean-all: clean
	rm -f README.pdf

clean:
	rm -f $(CLASS).cls $(CLASS).log $(SOURCE).acn $(SOURCE).acr $(SOURCE).alg $(SOURCE).aux $(SOURCE).bbl $(SOURCE).blg $(SOURCE).glg $(SOURCE).glo $(SOURCE).gls $(SOURCE).glsdefs $(SOURCE).idx $(SOURCE).ilg $(SOURCE).ind $(SOURCE).ist $(SOURCE).loa $(SOURCE).lof $(SOURCE).log $(SOURCE).lot $(SOURCE).out $(SOURCE).toc
