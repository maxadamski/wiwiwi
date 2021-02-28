(define (domain test)
	(:requirements :adl)

	(:types nat square phase)

	(:constants
		zero one two three four - nat
		move helper action - phase
		cr sp t f ps st so za zb - square)

	(:predicates
		(succ ?x ?y - nat)

		(poziomo ?x ?y - square)
		(pionowo ?x ?y - square)

		(pomocnik-1 ?x - square)
		(pomocnik-2 ?x - square)

		(kupiec ?x - square) ; kupiec jest na polu x
		(faza ?x - phase)

		(tkaniny ?x - nat)
		(przyprawy ?x - nat)
		(owoce ?x - nat)
		(kawa ?x - nat)

		(rubin)
	)

	(:action move-1 ; rusza się o jedno pole pola
		:parameters (?from ?to ?faza)
		:precondition (and
			(kupiec ?from)
			(or (faza action) (faza move))
			(or (poziomo ?from ?to) (pionowo ?from ?to)
				(poziomo ?to ?from) (pionowo ?to ?from))
		)
		:effect (and
			(not (kupiec ?from))
			(kupiec ?to)
			(not (faza ?faza))
			(faza helper)
		)
	)

	(:action move-2 ; rusza się o dwa pola
		:parameters (?from ?to ?faza)
		:precondition (and
			(kupiec ?from)
			(or (faza action) (faza move))
			(exists (?x) (or
				(and (or (poziomo ?from ?x) (poziomo ?x ?from))
					 (or (poziomo ?x ?to) (poziomo ?to ?x))
				)
				(and (or (pionowo ?from ?x) (pionowo ?x ?from))
					 (or (pionowo ?x ?to) (pionowo ?to ?x))
				)
			))
		)
		:effect (and
			(not (kupiec ?from))
			(kupiec ?to)
			(not (faza ?faza))
			(faza helper)
		)
	)

	(:action move-f ; rusza do fontanny
		:parameters (?from ?where-p1 ?where-p2)
		:precondition (and
			(faza move)
			(pomocnik-1 ?where-p1)
			(pomocnik-2 ?where-p2)
		)
		:effect (and
			(not (kupiec ?from))
			(kupiec f)
			(not (pomocnik-1 ?where-p1))
			(not (pomocnik-2 ?where-p2))
			(not (faza move))
			(faza helper)
		)
	)

	(:action leave-helper-1
		:parameters (?square)
		:precondition (and
			(faza helper)
			(kupiec ?square)
			(not (pomocnik-2 ?square))
			(forall (?x) (not (pomocnik-1 ?x)))
		)
		:effect (and
			(pomocnik-1 ?square)
			(not (faza helper))
			(faza action)
		)
	)

	(:action leave-helper-2
		:parameters (?square)
		:precondition (and
			(faza helper)
			(kupiec ?square)
			(not (pomocnik-1 ?square))
			(forall (?x) (not (pomocnik-2 ?x)))
		)
		:effect (and
			(pomocnik-2 ?square)
			(not (faza helper))
			(faza action)
		)
	)

	(:action take-helper-1
		:parameters (?square)
		:precondition (and
			(faza helper)
			(kupiec ?square)
			(pomocnik-1 ?square)
		)
		:effect (and
			(not (pomocnik-1 ?square))
			(not (faza helper))
			(faza action)
		)
	)

	(:action take-helper-2
		:parameters (?square)
		:precondition (and
			(faza helper)
			(kupiec ?square)
			(pomocnik-2 ?square)
		)
		:effect (and
			(not (pomocnik-2 ?square))
			(not (faza helper))
			(faza action)
		)
	)

	(:action sklad-owoce ; daje 3 owoce
		:parameters (?num)
		:precondition (and
			(faza action)
			(kupiec so)
			(owoce ?num)
		)
		:effect (and
			(not (owoce ?num))
			(owoce three)
			(not (faza action))
			(faza move)
		)
	)

	(:action sklad-tkaniny ; daje 3 tkaniny
		:parameters (?num)
		:precondition (and
			(faza action)
			(kupiec st)
			(tkaniny ?num)
		)
		:effect (and
			(not (tkaniny ?num))
			(tkaniny three)
			(not (faza action))
			(faza move)
		)
	)

	(:action sklad-przyprawy ; daje 3 przyprawy
		:parameters (?num)
		:precondition (and
			(faza action)
			(kupiec sp)
			(przyprawy ?num)
		)
		:effect (and
			(not (przyprawy ?num))
			(przyprawy three)
			(not (faza action))
			(faza move)
		)
	)

	(:action czarny-rynek
		:parameters (
			 ?owoce-num ?tkaniny-num ?przyprawy-num
			 ?owoce-succ ?tkaniny-succ ?przyprawy-succ
		)
		:precondition (and
			(faza action)
			(kupiec cr)
			(owoce ?owoce-num)
			(tkaniny ?tkaniny-num)
			(przyprawy ?przyprawy-num)
			(succ ?owoce-num ?owoce-succ)
			(succ ?tkaniny-num ?tkaniny-succ)
			(succ ?przyprawy-num ?przyprawy-succ)
		)
		:effect (and
			(not (owoce ?owoce-num))
			(owoce ?owoce-succ)
			(not (tkaniny ?tkaniny-num))
			(tkaniny ?tkaniny-succ)
			(not (przyprawy ?przyprawy-num))
			(przyprawy ?przyprawy-succ)
			(not (faza action))
			(faza move)
		)
	)

	(:action targowisko-owoce ; sprzedaje na T 3 owoce i dostaje kawę
		:parameters (?kawa-num ?kawa-succ)
		:precondition (and
			(faza action)
			(kupiec t)
			(owoce three)
			(not (kawa three))
			(succ ?kawa-num ?kawa-succ)
			(kawa ?kawa-num)
		)
		:effect (and
			(not (owoce three))
			(owoce zero)
			(not (kawa ?kawa-num))
			(kawa ?kawa-succ)
			(not (faza action))
			(faza move)
		)
	)

	(:action targowisko-tkaniny ; sprzedaje na T 3 tkaniny i dostaje kawę
		:parameters (?kawa-num ?kawa-succ)
		:precondition (and
			(faza action)
			(kupiec t)
			(tkaniny three)
			(not (kawa three))
			(succ ?kawa-num ?kawa-succ)
			(kawa ?kawa-num)
		)
		:effect (and
			(not (tkaniny three))
			(tkaniny zero)
			(not (kawa ?kawa-num))
			(kawa ?kawa-succ)
			(not (faza action))
			(faza move)
		)
	)

	(:action targowisko-przyprawy ; sprzedaje na T 3 przyprawy i dostaje kawę
		:parameters (?kawa-num ?kawa-succ)
		:precondition (and
			(faza action)
			(kupiec t)
			(przyprawy three)
			(not (kawa three))
			(succ ?kawa-num ?kawa-succ)
			(kawa ?kawa-num)
		)
		:effect (and
			(not (przyprawy three))
			(przyprawy zero)
			(not (kawa ?kawa-num))
			(kawa ?kawa-succ)
			(not (faza action))
			(faza move)
		)
	)

	(:action fontanna ; sprzedaje na T 3 przyprawy i dostaje kawę
		:parameters (?where-p1 ?where-p2)
		:precondition (and
			(faza action)
			(kupiec f)
			(or 
				(and (pomocnik-1 ?where-p1) (pomocnik-2 ?where-p2))
				(pomocnik-1 ?where-p1)
				(pomocnik-2 ?where-p2)
			)
		)
		:effect (and
			(when (pomocnik-1 ?where-p1) (not (pomocnik-1 ?where-p1)))
			(when (pomocnik-2 ?where-p2) (not (pomocnik-2 ?where-p2)))

			(not (faza action))
			(faza move)
		)
	)

	(:action palac ; 2 kawy, 1 z innych
		:parameters ()
		:precondition (and
			(faza action)
			(kupiec ps)
			(or (kawa two) (kawa three))
			(or (tkaniny one) (tkaniny two) (tkaniny three))
			(or (owoce one) (owoce two) (owoce three))
			(or (przyprawy one) (przyprawy two) (przyprawy three))
		)
		:effect (and
			(rubin) 
		)
	)
)

