10/31/05 - Leo Grady

This code requires installation of the (free) Graph Analysis Toolbox available at:
http://eslab.bu.edu/software/graphanalysis/

The random walker algorithm was introduced in the paper:
Leo Grady and Gareth Funka-Lea, "Multi-Label Image Segmentation for Medical Applications Based on Graph-Theoretic Electrical Potentials", in Proceedings of the 8th ECCV04, Workshop on Computer Vision Approaches to Medical Image Analysis and Mathematical Methods in Biomedical Image Analysis, p. 230-245, May 15th, 2004, Prague, Czech Republic, Springer-Verlag.
Available at: http://cns.bu.edu/~lgrady/grady2004multilabel.pdf

NOTE: The makeweights() function in the graph analysis toolbox differs from the weighting function published in the paper.  Therefore, a different parameter is used as the default than what is given in the paper.  In order to use the parameter values in the paper, the makeweights() function would have to be modified.  Although the results between these weighting functions to not behave much differently, the Gaussian weighting function was used in the paper for reasons of conformity with other graph-based literature (e.g., graph cuts).

The included script function random_walker_example.m gives a demonstrated usage of the function.