%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% LateX class for Bachelor's & Master's Thesis %%
%% Copyleft by Dawid Weiss & Marta Szachniuk    %%
%% Faculty of Computing and Telecommunication   %%
%% Poznan University of Technology, 2020        %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% This class is UNOFFICIAL and NOT ENDORSED by the university.
% (c) 2006--2011 Dawid Weiss, Poznan University of Technology
% (c) 2012--2017 Przemyslaw Wesolek, Poznan University of Technology
% (c) 2020--     Marta Szachniuk, Poznan University of Technology

\NeedsTeXFormat{LaTeX2e}[1995/12/01]
\ProvidesClass{ppfcmthesis}

% Declare bachelor thesis switch.
\newif\if@ppbachelor\@ppbachelorfalse

%
% Declare default options and load base package (memoir).
%
\DeclareOption{bachelor}
   {\@ppbachelortrue}

\ExecuteOptions{a4paper,pl,11pt,oneside,final}
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{memoir}}
\ProcessOptions\relax
\LoadClass{memoir}

%
% Require additional packages
%
\RequirePackage{babel}
\newif\if@pppolish
\iflanguage{polish}{\@pppolishtrue}{\@pppolishfalse}

\if@pppolish
\selectlanguage{polish}
\RequirePackage{indentfirst}
% Force "Literatura" instead of "Bibliografia"
\addto\captionspolish{\renewcommand{\bibname}{Literatura}}   
\else
\selectlanguage{english}
\fi

\RequirePackage{graphicx}
\RequirePackage{setspace}
\RequirePackage{booktabs}

\ifpdf
	\DeclareGraphicsExtensions{.pdf,.png,.jpg}
\else
	\DeclareGraphicsExtensions{.eps}
\fi
\RequirePackage{fancyvrb}

%
% Configure Hyperref
%

\RequirePackage{url}
\RequirePackage{nameref}
\RequirePackage{varioref}
  
\ifpdf
        \usepackage[unicode,linktocpage,bookmarks]{hyperref}
\else
        \usepackage[dvips,unicode,linktocpage,bookmarks]{hyperref}
\fi
\hypersetup{
    pdffitwindow=true,
    bookmarksnumbered=true,
    bookmarksopen=true,
    colorlinks=true,
    pdfpagelayout=SinglePage,
    pdfpagemode=UseOutlines,
    pdfstartview=Fit,
    linkcolor=black,
    citecolor=black,
    anchorcolor=black,
    filecolor=black,
    menucolor=black,
    urlcolor=black,
    pdftitle={},
    pdfauthor={},
    pdfkeywords={}
}
\RequirePackage{memhfixc}

%
% Emulate setspace package for older versions of memoir.
%
\ifx\undefined\SingleSpacing
\RequirePackage{setspace}
\newcommand{\SingleSpacing}{\singlespacing}
\fi
\ifx\undefined\OnehalfSpacing
\newcommand{\OnehalfSpacing}{\onehalfspacing}
\fi

%--------------------------------------
% Strona tytułowa pracy dyplomowej
%--------------------------------------

\aliaspagestyle{title}{empty}
\date{}
\newcommand{\ppsupervisor}[1]{\def\@ppsupervisor{#1}}
\newcommand{\ppyear}[1]{\def\@ppyear{#1}}

\newcommand{\authortitle}[1]{\def\@authortitle{#1}}

\setlength{\droptitle}{0cm}

\preauthor{\begin{center}\large\@authortitle}
\postauthor{\end{center}}

\renewcommand{\maketitlehooka}{%
\begin{center}
\if@pppolish
  \includegraphics[width=\textwidth]{wit/sygnet-pp.eps} \\
WYDZIA{\L} INFORMATYKI I TELEKOMUNIKACJI\\
Instytut Informatyki
\else
  \includegraphics[width=\textwidth]{wit/sygnet-pp-eng.eps} \\
FACULTY OF COMPUTING AND TELECOMMUNICATION\\
Institute of Computing Science
\fi\end{center}}

\pretitle{\vspace{4cm}\begin{center}%
\if@pppolish%
  \if@ppbachelor 
     Praca dyplomowa licencjacka
   \else 
     Praca dyplomowa magisterska
   \fi
\else
\if@ppbachelor 
Bachelor's thesis
\else 
Master's thesis
\fi
\fi\par\vspace{1cm}\Large\bfseries\MakeUppercase}
\posttitle{\end{center}}

\newcommand{\album}[1]{\unskip ,~#1}
\renewcommand{\and}{\\}

\renewcommand{\maketitlehookb}{\vspace{1cm}}

\renewcommand{\maketitlehookc}{\vfill\begin{center}
\if@pppolish
Promotor\\\@ppsupervisor
\else
Supervisor\\\@ppsupervisor
\fi\end{center}
\vspace{2cm}\begin{center}POZNA\'N \@ppyear\if@pppolish\fi\end{center}
}

%--------------------------------------
% Ostatnia strona z prawami autorskimi
%--------------------------------------

\newcommand{\ppcolophon}{%
\renewcommand{\and}{\unskip{}, }%
\renewcommand{\album}[1]{\unskip}%
\cleardoublepage\thispagestyle{empty}\vspace*{\fill}%
\footnotesize\SingleSpacing\noindent%
\raisebox{-1.3cm}{\includegraphics[height=1.5cm]{wit/logo-pp}}%
\hspace{1.5em}%
\begin{minipage}[t]{11cm}%
\vglue 0pt\copyright{} \@ppyear\ \@author

\medskip\noindent%
\if@pppolish%
Instytut Informatyki, Wydzia{\l} Informatyki i Telekomunikacji\\%
Politechnika Pozna\'nska%
\else%
Poznan University of Technology\\Faculty of Computing and Telecommunication\\Institute of Computing Science
\fi

\medskip\noindent%
\if@pppolish%
Sk{\l}ad przy u{\.z}yciu systemu \LaTeX{} na platformie Overleaf.
\else%
Typeset using \LaTeX{}.
\fi

\end{minipage}
}

%--------------------------------------
% Strony tytułowe rozdziałów
%--------------------------------------

\makepagestyle{ppfcmthesis}
\makeevenfoot{ppfcmthesis}{}{}{}
\makeatletter
\newcommand{\@ppfcmthesismarks}{%
  \let\@mkboth\markboth
  \def\chaptermark##1{\markboth{##1}{##1}}    % left mark & right marks
  \def\sectionmark##1{\markright{%
    \ifnum \c@secnumdepth>\z@
      \thesection. \ %
    \fi
    ##1}}
  \def\tocmark{\markboth{\contentsname}{}}
  \def\lofmark{\markboth{\listfigurename}{}}
  \def\lotmark{\markboth{\listtablename}{}}
  \def\bibmark{\markboth{\bibname}{}}
  \def\indexmark{\markboth{\indexname}{}}
}
\makepsmarks{ppfcmthesis}{\@ppfcmthesismarks}
\makeatother
\makeevenhead{ppfcmthesis}{\thepage}{}{\normalfont\small\itshape\leftmark}
\makeoddhead{ppfcmthesis}{\normalfont\small\itshape\rightmark}{}{\thepage}

%--------------------------------------
% Strona ze spisem treści
%--------------------------------------

\setcounter{tocdepth}{3}
\maxtocdepth{subsubsection}
\maxsecnumdepth{subsection}

%
% Customize book style
%
% \tightlists
\pagestyle{ppfcmthesis}
\setstocksize{297mm}{210mm}
\settrimmedsize{297mm}{210mm}{*}
\settrims{0pt}{0cm}
\setlrmarginsandblock{3.5cm}{2.5cm}{*}
\setulmarginsandblock{2.5cm}{2.5cm}{*}
\setmarginnotes{6pt}{50pt}{\onelineskip}
\setheadfoot{\onelineskip}{2\onelineskip}
\setheaderspaces{*}{2\onelineskip}{*}
\OnehalfSpacing

\changecaptionwidth
\captionwidth{.9\linewidth}
\captionnamefont{\footnotesize\scshape}
\captiontitlefont{\footnotesize}

\newcommand{\fcmfcaption}[2][\@empty]{
\captionnamefont{\footnotesize\bfseries}%
\captiondelim{.~}
\captionstyle{}
\captiontitlefont{\footnotesize}
\ifx #1\@empty \caption{#2}\else \caption[#1]{#2}}

\newcommand{\fcmtcaption}[2][\@empty]{
\captionnamefont{\footnotesize\bfseries\hfill}%
\captiondelim{}
\captionstyle{\raggedleft\\}
\captiontitlefont{\footnotesize\normalfont}
\ifx #1\@empty \caption{#2}\else \caption[#1]{#2}}

\def\thesubsubsection {\thesubsection .\arabic{subsubsection}}

\checkandfixthelayout

%
% Macros 
%

% English
\newcommand{\termdef}[1]{\emph{#1}}
\newcommand{\acronym}[1]{\emph{\MakeUppercase{#1}}}

% Polish
\newcommand{\definicja}[1]{\termdef{#1}}
\newcommand{\akronim}[1]{\acronym{#1}}
\newcommand{\english}[1]{ang.~\emph{#1}}

% Table head
\newcolumntype{L}{c}\newcommand{\tabhead}[1]{\multicolumn{1}{L}{#1}}
